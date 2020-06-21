//
// gim_sha1.cc
//
// Gim  -  Generic Information Manager static library
// Version 1.0-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%201.0?by=version]
//
/*#############################################################################

	Copyright (C) 2002, 2003 
	Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)

	This file is part of Gim library.

	Gim is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Gim is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Gim; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#############################################################################*/
//
// Gim is developed with Anjuta version 1.2.4
//


/*	Thanks for this code to Jasin Bushnaief */


#include "../include/gim_sha1.h"


uint32_t gim_sha1_obj::rotl32(uint32_t x, int b)
{
    return (x << b) | (x >> (32-b));
}

// switch endianness
uint32_t gim_sha1_obj::get32 (const void* p)
{
    const uint8_t *x = (const uint8_t*)p;
    return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}

uint32_t gim_sha1_obj::f (int t, uint32_t b, uint32_t c, uint32_t d)
{
    assert(0 <= t && t < 80);

    if (t < 20)
        return (b & c) | ((~b) & d);
    if (t < 40)
        return b ^ c ^ d;
    if (t < 60)
        return (b & c) | (b & d) | (c & d);
    //if (t < 80)
        return b ^ c ^ d;
}


void gim_sha1_obj::Sha1Ctx_reset (Sha1Ctx* ctx)
{
    ctx->h[0] = 0x67452301;
    ctx->h[1] = 0xefcdab89;
    ctx->h[2] = 0x98badcfe;
    ctx->h[3] = 0x10325476;
    ctx->h[4] = 0xc3d2e1f0;
    ctx->bytes = 0;
    ctx->cur = 0;
}

Sha1Ctx * gim_sha1_obj::Sha1Ctx_create (void)
{
    // TODO custom allocator support
    Sha1Ctx* ctx = (Sha1Ctx*)malloc(sizeof(Sha1Ctx));
    Sha1Ctx_reset(ctx);
    return ctx;
}

void gim_sha1_obj::Sha1Ctx_release (Sha1Ctx* ctx)
{
    free(ctx);
}

void gim_sha1_obj::processBlock (Sha1Ctx* ctx)
{
    static const uint32_t k[4] =
    {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };

    uint32_t w[16];
    uint32_t a = ctx->h[0];
    uint32_t b = ctx->h[1];
    uint32_t c = ctx->h[2];
    uint32_t d = ctx->h[3];
    uint32_t e = ctx->h[4];
    int t;

    for (t = 0; t < 16; t++)
        w[t] = get32(&((uint32_t*)ctx->block)[t]);

    for (t = 0; t < 80; t++)
    {
        int s = t & 0xf;
        uint32_t temp;
        if (t >= 16)
            w[s] = rotl32(w[(s + 13) & 0xf] ^ w[(s + 8) & 0xf] ^ w[(s + 2) & 0xf] ^ w[s], 1);

         temp = rotl32(a, 5) + f(t, b,c,d) + e + w[s] + k[t/20];

         e = d; d = c; c = rotl32(b, 30); b = a; a = temp;
    }

    ctx->h[0] += a;
    ctx->h[1] += b;
    ctx->h[2] += c;
    ctx->h[3] += d;
    ctx->h[4] += e;
}

void gim_sha1_obj::Sha1Ctx_write (Sha1Ctx* ctx, const void* msg, uint64_t bytes)
{
    ctx->bytes += bytes;

    const uint8_t* src = (const uint8_t*)msg;
    while (bytes--)
    {
        // TODO: could optimize the first and last few bytes, and then copy
        // 128 bit blocks with SIMD in between
        ctx->block[ctx->cur++] = *src++;
        if (ctx->cur == 64)
        {
            processBlock(ctx);
            ctx->cur = 0;
        }
    }
}

Sha1Digest gim_sha1_obj::Sha1Ctx_getDigest (Sha1Ctx* ctx)
{
    // append separator
    ctx->block[ctx->cur++] = 0x80;
    if (ctx->cur > 56)
    {
        // no space in block for the 64-bit message length, flush
        memset(&ctx->block[ctx->cur], 0, 64 - ctx->cur);
        processBlock(ctx);
        ctx->cur = 0;
    }

    memset(&ctx->block[ctx->cur], 0, 56 - ctx->cur);
    uint64_t bits = ctx->bytes * 8;

    // TODO a few instructions could be shaven
    ctx->block[56] = (uint8_t)(bits >> 56 & 0xff);
    ctx->block[57] = (uint8_t)(bits >> 48 & 0xff);
    ctx->block[58] = (uint8_t)(bits >> 40 & 0xff);
    ctx->block[59] = (uint8_t)(bits >> 32 & 0xff);
    ctx->block[60] = (uint8_t)(bits >> 24 & 0xff);
    ctx->block[61] = (uint8_t)(bits >> 16 & 0xff);
    ctx->block[62] = (uint8_t)(bits >> 8  & 0xff);
    ctx->block[63] = (uint8_t)(bits >> 0  & 0xff);
    processBlock(ctx);

    {
        Sha1Digest ret;
        int i;
        for (i = 0; i < 5; i++)
            ret.digest[i] = get32(&ctx->h[i]);
        Sha1Ctx_reset(ctx);
        return ret;
    }
}

Sha1Digest gim_sha1_obj::Sha1_get (const void* msg, uint64_t bytes)
{
    Sha1Ctx ctx;
    Sha1Ctx_reset(&ctx);
    Sha1Ctx_write(&ctx, msg, bytes);
    return Sha1Ctx_getDigest(&ctx);
}


Sha1Digest gim_sha1_obj::Sha1Digest_fromStr (const char* src)
{
    Sha1Digest d;
    int i;
    
    assert(src); // also, src must be at least 40 bytes
    for (i = 0; i < 20 && src[i]; i++)
    {
        // \todo just use atoi or something
        int c0 = tolower(*src++);
        int c1 = tolower(*src++);

        c0 = '0' <= c0 && c0 <= '9' ? c0 - '0' : ('a' <= c0 && c0 <= 'f' ? 0xa + c0 - 'a' : -1);
        c1 = '0' <= c1 && c1 <= '9' ? c1 - '0' : ('a' <= c1 && c1 <= 'f' ? 0xa + c1 - 'a' : -1);
        ((uint8_t*)d.digest)[i] = (uint8_t)((c0 << 4) | c1);
    }

    return d;
}

void gim_sha1_obj::Sha1Digest_toStr (const Sha1Digest* digest, char* dst)
{
    int i;
    assert(digest && dst); // dst must be at least 41 bytes (terminator)
    for (i = 0; i < 20; i++)
    {
        int c0 = ((uint8_t*)digest->digest)[i] >> 4;
        int c1 = ((uint8_t*)digest->digest)[i] & 0xf;

        assert(0 <= c0 && c0 <= 0xf);
        assert(0 <= c1 && c1 <= 0xf);
        c0 = c0 <= 9 ? '0' + c0 : 'a' + c0 - 0xa;
        c1 = c1 <= 9 ? '0' + c1 : 'a' + c1 - 0xa;

        *dst++ = (char)c0;
        *dst++ = (char)c1;
    }
    *dst = '\0';
}



















