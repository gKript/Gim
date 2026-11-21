#!/usr/bin/env python3
"""
gim_ai_train.py - Gim side of the PicGIM AI model (SYNC/COMM_CONTRACT.md, sez. 4.4 - 4.6).

Reads a labelled dataset of raw sensor samples, trains a small fully connected
network in float, quantizes it to the integer format of pgim_ai_net.c, checks it
with a bit-exact integer simulator and, only if accepted, writes pgim_ai_model.h.

Stdlib only (no numpy). Exit codes: 0 accepted, 2 rejected by a check, 1 bad input.

Dataset (CSV, one header line):
    label,ch0,ch0_ok,ch1,ch1_ok,...
    label  : class index 0..K-1
    chN    : raw integer 0..65535, same units the PIC driver reads
    chN_ok : 1 = valid (PG_OK), 0 = sensor fault (PG_NOK)

Usage:
    python gim_ai_train.py --dataset data.csv --out pgim_ai_model.h [--report r.txt]
           [--out-bits 8|16] [--hidden 8,8] [--epochs 200] [--lr 0.05]
           [--test-frac 0.25] [--seed 1] [--tol 0.02] [--test-vectors 16]
    python gim_ai_train.py --synthetic-out synth.csv [--synthetic-n 600] [--seed 1]

Notes:
  - Channel ranges (in_min/in_max) come from the training split only.
  - The split is a plain shuffle, not stratified.
  - The integer normalization and layers follow pgim_ai_input.c and pgim_ai_net.c
    operation by operation (no rounding in the normalization, rounding + arithmetic
    shift in the layers, ReLU then saturation, first index wins on ties).
"""

import argparse
import csv
import datetime
import hashlib
import math
import random
import sys

TOOL_VERSION = "1.0"
MAX_CHANNELS = 16          # PGIM_AI_MAX_CHANNELS upper limit (pgim_ai_input.h)
INT31 = 2 ** 31


class ModelError(Exception):
    """A model that does not pass a contract check (not an input-format error)."""


def out_max_for(bits):
    if bits == 8:
        return 127
    if bits == 16:
        return 32767
    raise ValueError("out-bits must be 8 or 16")


# ---------------------------------------------------------------- dataset

def read_dataset(path):
    with open(path, "rb") as f:
        raw_bytes = f.read()
    digest = hashlib.sha256(raw_bytes).hexdigest()
    rows = list(csv.reader(raw_bytes.decode("utf-8").splitlines()))
    if not rows or rows[0][:1] != ["label"]:
        raise ValueError("header must start with 'label'")
    cols = rows[0][1:]
    if len(cols) % 2 != 0 or not cols:
        raise ValueError("after 'label' expect pairs <name>,<name>_ok")
    names = []
    for k in range(0, len(cols), 2):
        if cols[k + 1] != cols[k] + "_ok":
            raise ValueError("column %r must be followed by %r" % (cols[k], cols[k] + "_ok"))
        names.append(cols[k])
    n = len(names)
    if n > MAX_CHANNELS:
        raise ValueError("too many channels (max %d)" % MAX_CHANNELS)
    samples = []
    for line_no, r in enumerate(rows[1:], start=2):
        if not r or all(not c.strip() for c in r):
            continue
        if len(r) != 1 + 2 * n:
            raise ValueError("line %d: expected %d fields" % (line_no, 1 + 2 * n))
        label = int(r[0])
        raw = [int(r[1 + 2 * i]) for i in range(n)]
        ok = [int(r[2 + 2 * i]) for i in range(n)]
        if label < 0:
            raise ValueError("line %d: negative label" % line_no)
        for v in raw:
            if not 0 <= v <= 65535:
                raise ValueError("line %d: raw value %d out of 0..65535" % (line_no, v))
        for v in ok:
            if v not in (0, 1):
                raise ValueError("line %d: ok must be 0 or 1" % line_no)
        samples.append((raw, ok, label))
    if not samples:
        raise ValueError("dataset is empty")
    n_cls = max(s[2] for s in samples) + 1
    present = set(s[2] for s in samples)
    if len(present) != n_cls or n_cls < 2:
        raise ValueError("labels must be 0..K-1 with K >= 2, all present")
    return names, samples, n_cls, digest


def split(samples, test_frac, seed):
    idx = list(range(len(samples)))
    random.Random(seed).shuffle(idx)
    n_test = max(1, int(round(len(idx) * test_frac)))
    test = [samples[i] for i in idx[:n_test]]
    train = [samples[i] for i in idx[n_test:]]
    if not train:
        raise ValueError("test fraction leaves no training samples")
    return train, test


# ---------------------------------------------------------------- normalization (sez. 4.2)

def channel_configs(train, n, out_max):
    cfgs = []
    for c in range(n):
        vals = [raw[c] for raw, ok, _ in train if ok[c] == 1]
        if not vals:
            raise ModelError("channel %d: no valid training sample" % c)
        lo, hi = min(vals), max(vals)
        if hi <= lo:
            raise ModelError("channel %d: constant in training (in_min == in_max)" % c)
        span = hi - lo
        for shift in range(16, -1, -1):          # largest shift with scale <= 65535
            scale = -((-(2 * out_max) << shift) // span)   # ceil
            if scale <= 65535:
                break
        cfgs.append({"in_min": lo, "in_max": hi, "scale": scale, "shift": shift,
                     "fallback": 0, "enabled": 1})
    return cfgs


def normalize_int(cfgs, raw, ok, out_max):
    """Bit-exact copy of pg_ai_input_normalize (pgim_ai_input.c)."""
    out = []
    for c, cfg in enumerate(cfgs):
        if cfg["enabled"] != 1 or ok[c] != 1:
            out.append(cfg["fallback"])
            continue
        r = min(max(raw[c], cfg["in_min"]), cfg["in_max"])
        diff = r - cfg["in_min"]
        v = (diff * cfg["scale"]) >> cfg["shift"]
        if v > 2 * out_max:
            v = 2 * out_max
        out.append(v - out_max)
    return out


# ---------------------------------------------------------------- float training (sez. 4.4 passi 4)

class FLayer(object):
    def __init__(self, n_in, n_out, relu, rnd):
        self.n_in, self.n_out, self.relu = n_in, n_out, relu
        std = math.sqrt((2.0 if relu else 1.0) / n_in)
        self.W = [[rnd.gauss(0.0, std) for _ in range(n_in)] for _ in range(n_out)]
        self.B = [0.0] * n_out


def forward_train(layers, x):
    acts, zs = [x], []
    a = x
    last = len(layers) - 1
    for l, L in enumerate(layers):
        z = [L.B[o] + sum(L.W[o][i] * a[i] for i in range(L.n_in)) for o in range(L.n_out)]
        zs.append(z)
        a = [min(max(v, 0.0), 1.0) for v in z] if l < last else z   # hidden: ReLU saturated at 1
        acts.append(a)
    return acts, zs


def float_predict(layers, x):
    acts, _ = forward_train(layers, x)
    z = acts[-1]
    return max(range(len(z)), key=lambda i: z[i])


def train_float(layers, xs, ys, epochs, lr, rnd):
    order = list(range(len(xs)))
    last = len(layers) - 1
    for _ in range(epochs):
        rnd.shuffle(order)
        for k in order:
            acts, zs = forward_train(layers, xs[k])
            zl = zs[-1]
            m = max(zl)
            e = [math.exp(v - m) for v in zl]
            s = sum(e)
            g = [v / s for v in e]
            g[ys[k]] -= 1.0
            for l in range(last, -1, -1):
                L = layers[l]
                if l < last:
                    g = [g[o] if 0.0 < zs[l][o] < 1.0 else 0.0 for o in range(L.n_out)]
                a_prev = acts[l]
                g_prev = [sum(L.W[o][i] * g[o] for o in range(L.n_out)) for i in range(L.n_in)]
                for o in range(L.n_out):
                    go = g[o]
                    row = L.W[o]
                    for i in range(L.n_in):
                        row[i] -= lr * go * a_prev[i]
                    L.B[o] -= lr * go
                g = g_prev


# ---------------------------------------------------------------- quantization (sez. 4.4 passo 5)

class ILayer(object):
    def __init__(self, n_in, n_out, shift, relu, W, B):
        self.n_in, self.n_out, self.shift, self.relu = n_in, n_out, shift, relu
        self.W, self.B = W, B   # W[o][i] int, B[o] int (accumulator scale)


def quantize_layer(L, out_max):
    """Largest shift q (0..16) such that |w_int| <= OUT_MAX and the int32 bound of sez. 4.5 holds."""
    for q in range(16, -1, -1):
        scale = float(1 << q)
        W = [[int(round(w * scale)) for w in row] for row in L.W]
        B = [int(round(b * scale * out_max)) for b in L.B]
        if any(abs(w) > out_max for row in W for w in row):
            continue
        fits = all(sum(abs(w) for w in row) * out_max + abs(b) + (1 << q) < INT31
                   for row, b in zip(W, B))
        if fits:
            return ILayer(L.n_in, L.n_out, q, L.relu, W, B)
    raise ModelError("layer %dx%d cannot be represented in int%d without int32 overflow"
                     % (L.n_in, L.n_out, 16 if out_max > 127 else 8))


def scale_output_layer(layers, xs_train):
    """Scale the last layer so that its logits on the training set lie in [-1, 1].
    Positive scaling keeps the argmax, and it avoids saturating the integer scores."""
    m = 0.0
    for x in xs_train:
        _, zs = forward_train(layers, x)
        m = max(m, max(abs(v) for v in zs[-1]))
    if m <= 0.0:
        return
    L = layers[-1]
    L.W = [[w / m for w in row] for row in L.W]
    L.B = [b / m for b in L.B]


def check_overflow(il, out_max):
    """sez. 4.5: sum|w| * OUT_MAX + |b| < 2^31 for every neuron."""
    for o in range(il.n_out):
        bound = sum(abs(w) for w in il.W[o]) * out_max + abs(il.B[o]) + (1 << il.shift)
        if bound >= INT31:
            raise ModelError("neuron %d of layer may overflow int32 (bound %d)" % (o, bound))


def int_forward(ilayers, x, out_max):
    """Bit-exact copy of pg_ai_net_infer / pg_ai_net_layer (pgim_ai_net.c)."""
    a = x
    for il in ilayers:
        y = []
        for o in range(il.n_out):
            acc = il.B[o]
            row = il.W[o]
            for i in range(il.n_in):
                acc += row[i] * a[i]
            if il.shift > 0:
                acc += 1 << (il.shift - 1)
            acc >>= il.shift
            if il.relu and acc < 0:
                acc = 0
            if acc > out_max:
                acc = out_max
            if acc < -out_max:
                acc = -out_max
            y.append(acc)
        a = y
    scores = a
    cls = max(range(len(scores)), key=lambda i: scores[i])   # first index wins ties
    return cls, scores


# ---------------------------------------------------------------- header (sez. 4.4, output)

def fmt_list(values):
    return ", ".join(str(v) for v in values)


def write_header(path, meta, cfgs, ilayers, tests, out_bits, out_max):
    n_in = len(cfgs)
    n_cls = ilayers[-1].n_out
    widths = [n_in] + [il.n_out for il in ilayers]
    max_width = max(widths)
    L = []
    L.append("/*")
    L.append("\tFile name :\t\tpgim_ai_model.h")
    L.append("\tProject :\t\tPicGim")
    L.append("")
    L.append("\t\\file\t\tpgim_ai_model.h")
    L.append("\t\\brief\t\tGENERATED by Gim (scripts/gim_ai_train.py). Do not edit by hand.")
    L.append("\tgenerated  : %s" % meta["date"])
    L.append("\tgenerator  : gim_ai_train.py %s" % TOOL_VERSION)
    L.append("\tdataset    : sha256 %s" % meta["dataset_sha256"])
    L.append("\tnetwork    : %s" % meta["architecture"])
    L.append("\tseed/epochs: %d / %d, lr %s" % (meta["seed"], meta["epochs"], meta["lr"]))
    L.append("\tfloat acc  : %.4f   integer acc: %.4f   (test split)" % (meta["float_acc"], meta["int_acc"]))
    L.append("*/")
    L.append("")
    L.append("#ifndef _PGIM_AI_MODEL_H_")
    L.append("\t#define _PGIM_AI_MODEL_H_")
    L.append("")
    L.append('\t#include "pgim_ai_net.h"')
    L.append("")
    L.append("\t#if ( PGIM_AI_OUT_BITS != %d )" % out_bits)
    L.append('\t\t#error "model generated for %d bit output"' % out_bits)
    L.append("\t#endif")
    L.append("")
    L.append("\t#define PG_AI_NUM_INPUTS\t\t%d" % n_in)
    L.append("\t#define PG_AI_NUM_LAYERS\t\t%d" % len(ilayers))
    L.append("\t#define PG_AI_NUM_CLASSES\t\t%d" % n_cls)
    L.append("\t#define PG_AI_MAX_WIDTH\t\t%d" % max_width)
    L.append("\t#define PG_AI_NUM_TESTS\t\t%d" % len(tests))
    L.append("\t#define PG_AI_RAM_BYTES\t\t(2 * PG_AI_MAX_WIDTH * sizeof(pg_ai_out_t))")
    L.append("")
    L.append("\t//---[ Channel configuration ]---")
    L.append("\trom const pg_ai_channel_cfg PG_AI_CHANNELS[ PG_AI_NUM_INPUTS ] = {")
    for c, cfg in enumerate(cfgs):
        L.append("\t\t{ %d, %d, %d, %d, %d, %d },\t// ch%d" % (
            cfg["in_min"], cfg["in_max"], cfg["scale"], cfg["shift"], cfg["fallback"], cfg["enabled"], c))
    L.append("\t};")
    L.append("")
    for l, il in enumerate(ilayers):
        L.append("\t//---[ Layer %d ]---" % l)
        L.append("\trom const pg_ai_w_t PG_AI_L%d_W[ %d ] = { %s };" % (
            l, il.n_in * il.n_out, fmt_list(w for row in il.W for w in row)))
        L.append("\trom const _pg_int32 PG_AI_L%d_B[ %d ] = { %s };" % (l, il.n_out, fmt_list(il.B)))
    L.append("")
    L.append("\trom const pg_ai_layer PG_AI_LAYERS[ PG_AI_NUM_LAYERS ] = {")
    for l, il in enumerate(ilayers):
        comma = "," if l < len(ilayers) - 1 else ""
        L.append("\t\t{ %d, %d, PG_AI_L%d_W, PG_AI_L%d_B, %d, %s }%s" % (
            il.n_in, il.n_out, l, l, il.shift, "PG_YES" if il.relu else "PG_NO", comma))
    L.append("\t};")
    L.append("")
    L.append("\t//---[ Test vectors: raw input -> normalized input -> expected output ]---")
    L.append("\trom const _pg_Uint16 PG_AI_TEST_RAW[ PG_AI_NUM_TESTS ][ PG_AI_NUM_INPUTS ] = {")
    L.extend("\t{ %s }," % fmt_list(t["raw"]) for t in tests[:-1])
    L.append("\t{ %s } };" % fmt_list(tests[-1]["raw"]))
    L.append("\trom const _pg_Uint8 PG_AI_TEST_OK[ PG_AI_NUM_TESTS ][ PG_AI_NUM_INPUTS ] = {")
    L.extend("\t{ %s }," % fmt_list(t["ok"]) for t in tests[:-1])
    L.append("\t{ %s } };" % fmt_list(tests[-1]["ok"]))
    L.append("\trom const pg_ai_out_t PG_AI_TEST_X[ PG_AI_NUM_TESTS ][ PG_AI_NUM_INPUTS ] = {")
    L.extend("\t{ %s }," % fmt_list(t["x"]) for t in tests[:-1])
    L.append("\t{ %s } };" % fmt_list(tests[-1]["x"]))
    L.append("\trom const pg_ai_out_t PG_AI_TEST_SCORES[ PG_AI_NUM_TESTS ][ PG_AI_NUM_CLASSES ] = {")
    L.extend("\t{ %s }," % fmt_list(t["scores"]) for t in tests[:-1])
    L.append("\t{ %s } };" % fmt_list(tests[-1]["scores"]))
    L.append("\trom const _pg_Uint8 PG_AI_TEST_CLASS[ PG_AI_NUM_TESTS ] = { %s };" % fmt_list(t["cls"] for t in tests))
    L.append("")
    L.append("#endif")
    L.append("")
    with open(path, "w", newline="\n") as f:
        f.write("\n".join(L))


# ---------------------------------------------------------------- synthetic data (testing only)

def make_synthetic(path, n, seed, n_ch=8, n_cls=3):
    rnd = random.Random(seed)
    mid = [rnd.uniform(500, 3000) for _ in range(n_ch)]
    sigma = [rnd.uniform(40, 150) for _ in range(n_ch)]
    step = [s * rnd.choice([-1.0, 1.0]) * rnd.uniform(0.3, 1.2) for s in sigma]
    names = []
    for c in range(n_ch):
        names += ["ch%d" % c, "ch%d_ok" % c]
    with open(path, "w", newline="\n") as f:
        f.write(",".join(["label"] + names) + "\n")
        for _ in range(n):
            label = rnd.randrange(n_cls)
            fields = [str(label)]
            for c in range(n_ch):
                mu = mid[c] + (label - (n_cls - 1) / 2.0) * step[c]
                v = int(min(max(rnd.gauss(mu, sigma[c]), 0), 65535))
                ok = 0 if rnd.random() < 0.02 else 1
                fields += [str(v), str(ok)]
            f.write(",".join(fields) + "\n")


# ---------------------------------------------------------------- pipeline

def run(args):
    out_max = out_max_for(args.out_bits)
    hidden = [int(x) for x in args.hidden.split(",") if x.strip()]
    names, samples, n_cls, digest = read_dataset(args.dataset)
    n = len(names)
    rnd = random.Random(args.seed)
    train, test = split(samples, args.test_frac, args.seed)

    cfgs = channel_configs(train, n, out_max)
    xs_train = [[v / float(out_max) for v in normalize_int(cfgs, raw, ok, out_max)] for raw, ok, _ in train]
    ys_train = [y for _, _, y in train]
    test_int = [normalize_int(cfgs, raw, ok, out_max) for raw, ok, _ in test]
    ys_test = [y for _, _, y in test]
    xs_test = [[v / float(out_max) for v in x] for x in test_int]

    sizes = [n] + hidden + [n_cls]
    layers = []
    for l in range(len(sizes) - 1):
        layers.append(FLayer(sizes[l], sizes[l + 1], relu=(l < len(sizes) - 2), rnd=rnd))
    train_float(layers, xs_train, ys_train, args.epochs, args.lr, rnd)
    scale_output_layer(layers, xs_train)

    float_acc = sum(float_predict(layers, x) == y for x, y in zip(xs_test, ys_test)) / float(len(ys_test))

    ilayers = [quantize_layer(L, out_max) for L in layers]
    for il in ilayers:
        check_overflow(il, out_max)

    correct = 0
    for x, y in zip(test_int, ys_test):
        cls, _ = int_forward(ilayers, x, out_max)
        correct += (cls == y)
    int_acc = correct / float(len(ys_test))

    lines = ["channels: %d, classes: %d, train %d, test %d" % (n, n_cls, len(train), len(test))]
    for c, cfg in enumerate(cfgs):
        lines.append("  %s: in [%d, %d] scale %d shift %d" % (names[c], cfg["in_min"], cfg["in_max"], cfg["scale"], cfg["shift"]))
    for l, il in enumerate(ilayers):
        lines.append("  layer %d: %d -> %d, shift %d, relu %s" % (l, il.n_in, il.n_out, il.shift, il.relu))
    lines.append("float accuracy (test): %.4f" % float_acc)
    lines.append("integer accuracy (test): %.4f" % int_acc)
    accepted = int_acc >= float_acc - args.tol
    lines.append("tolerance %.4f -> %s" % (args.tol, "ACCEPTED" if accepted else "REJECTED"))
    report = "\n".join(lines) + "\n"
    sys.stdout.write(report)
    if args.report:
        with open(args.report, "w", newline="\n") as f:
            f.write(report)
    if not accepted:
        raise ModelError("integer accuracy %.4f below float %.4f minus tolerance" % (int_acc, float_acc))

    tests = []
    for raw, ok, _ in test[:args.test_vectors]:
        x = normalize_int(cfgs, raw, ok, out_max)
        cls, scores = int_forward(ilayers, x, out_max)
        tests.append({"raw": raw, "ok": ok, "x": x, "scores": scores, "cls": cls})

    meta = {
        "date": datetime.date.today().isoformat(),
        "dataset_sha256": digest,
        "architecture": " -> ".join(str(s) for s in sizes),
        "seed": args.seed, "epochs": args.epochs, "lr": args.lr,
        "float_acc": float_acc, "int_acc": int_acc,
    }
    write_header(args.out, meta, cfgs, ilayers, tests, args.out_bits, out_max)
    sys.stdout.write("written %s\n" % args.out)
    return 0


def main(argv):
    p = argparse.ArgumentParser(description="Train and export pgim_ai_model.h (Gim side of PicGIM AI).")
    p.add_argument("--dataset", help="input CSV (see module docstring)")
    p.add_argument("--out", help="output pgim_ai_model.h")
    p.add_argument("--report", help="optional text report file")
    p.add_argument("--out-bits", type=int, default=8, choices=[8, 16])
    p.add_argument("--hidden", default="8", help="hidden layer sizes, comma separated")
    p.add_argument("--epochs", type=int, default=200)
    p.add_argument("--lr", type=float, default=0.05)
    p.add_argument("--test-frac", type=float, default=0.25)
    p.add_argument("--seed", type=int, default=1)
    p.add_argument("--tol", type=float, default=0.02, help="max accuracy drop integer vs float")
    p.add_argument("--test-vectors", type=int, default=16)
    p.add_argument("--synthetic-out", help="write a synthetic dataset (testing) and exit")
    p.add_argument("--synthetic-n", type=int, default=600)
    args = p.parse_args(argv)

    if args.synthetic_out:
        make_synthetic(args.synthetic_out, args.synthetic_n, args.seed)
        sys.stdout.write("synthetic dataset written %s\n" % args.synthetic_out)
        return 0
    if not args.dataset or not args.out:
        p.error("--dataset and --out are required")
    try:
        return run(args)
    except ModelError as e:
        sys.stdout.write("REJECTED: %s\n" % e)
        return 2
    except (ValueError, OSError) as e:
        sys.stderr.write("input error: %s\n" % e)
        return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
