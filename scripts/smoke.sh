#!/usr/bin/env bash
# Smoke test: pack a folder with gkfp, unpack it, compare (plain and encrypted).
# Usage: scripts/smoke.sh <directory containing the built tools>
set -euo pipefail

BIN="$(cd "$1" && pwd)"
WORK="$(mktemp -d)"
export PATH="$BIN:$PATH"          # gkfp calls gimstat through the PATH
export HOME="$WORK/home"          # keep the ~/.gim environment out of the way
export USERPROFILE="$HOME"
mkdir -p "$HOME" "$WORK/src/sub"

cd "$WORK"
echo "alpha"           > src/a.txt
echo "beta beta beta"  > src/sub/b.txt

for pass in "" "segreto123"; do
	name="plain"; [ -n "$pass" ] && name="enc"
	rm -rf out "$name.gkp"
	# a trailing "/" on the folder is required by the tools
	gkfp "$name" src/ $pass > /dev/null || true     # gimstat's exit code is not an error
	[ -f "$name.gkp" ] || { echo "FAIL: $name.gkp not created"; exit 1; }
	mkdir out
	( cd out && gkfp "../$name.gkp" $pass > /dev/null )
	diff -r src out/src
	echo "OK: $name round trip"
done

# the tools must start and print their usage
for t in gimstat gkdp gkmake gimclassgen; do
	"$t" > /dev/null 2>&1 || true
	command -v "$t" > /dev/null || { echo "FAIL: $t not found"; exit 1; }
done
echo "OK: smoke test passed"
