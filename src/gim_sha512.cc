//
// gim_sha512.cc
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

/*
 * Implementation of SHA-512, based on Adam Back's sha-1 implementation.
 * Uses 32 bit words.
 */

#include "../include/gim_sha512.h"
#include <string.h>

typedef unsigned char u_int8_t;			/* 1-byte  (8-bits)  */
typedef unsigned short u_int16_t;			/* 1-byte  (8-bits)  */
typedef unsigned int u_int32_t;			/* 4-bytes (32-bits) */
typedef unsigned long long u_int64_t;	/* 8-bytes (64-bits) */

typedef u_int8_t	sha2_byte;			/* 8 bit type (1 byte)   */
typedef u_int16_t	sha2_doublebyte;	/* 16 bit type (2 bytes) */
typedef u_int32_t	sha2_word32;		/* 32 bit type (4 bytes) */
typedef u_int64_t	sha2_word64;		/* 64 bit type (8 bytes) */

//~ #define min( x, y ) ( ( x ) < ( y ) ? ( x ) : ( y ) )

//~ /* Rotate right by n bits; separate macros for high and low results */
//~ #define Sh(xh,xl,n) ( (n<32) ? ((xh>>n)|(xl<<(32-n))) : ((xh<<(64-n))|(xl>>(n-32))) )
//~ #define Sl(xh,xl,n) ( (n<32) ? ((xh<<(32-n))|(xl>>n)) : ((xh>>(n-32))|(xl<<(64-n))) )

//~ /* Shift right by n bits; separate macros for high and low results */
//~ #define Rh(xh,xl,n) ( (n<32) ? (xh>>n) : 0 )
//~ #define Rl(xh,xl,n) ( (n<32) ? ((xh<<(32-n))|(xl>>n)) : (xh>>(n-32)) )

//~ /* These can be done independently on high and low parts */
//~ #define Choice(x,y,z) ( ((x) & (y)) | (~(x) & (z)) )
//~ #define Maj(x,y,z) ( ( (x) & (y) ) | ( (x) & (z) ) | ( (y) & (z) ) )

//~ /* Sigma functions */
//~ #define SIG0h(xh,xl) ( Sh(xh,xl,28) ^ Sh(xh,xl,34) ^ Sh(xh,xl,39) )
//~ #define SIG0l(xh,xl) ( Sl(xh,xl,28) ^ Sl(xh,xl,34) ^ Sl(xh,xl,39) )
//~ #define SIG1h(xh,xl) ( Sh(xh,xl,14) ^ Sh(xh,xl,18) ^ Sh(xh,xl,41) )
//~ #define SIG1l(xh,xl) ( Sl(xh,xl,14) ^ Sl(xh,xl,18) ^ Sl(xh,xl,41) )
//~ #define sig0h(xh,xl) ( Sh(xh,xl, 1) ^ Sh(xh,xl, 8) ^ Rh(xh,xl, 7) )
//~ #define sig0l(xh,xl) ( Sl(xh,xl, 1) ^ Sl(xh,xl, 8) ^ Rl(xh,xl, 7) )
//~ #define sig1h(xh,xl) ( Sh(xh,xl,19) ^ Sh(xh,xl,61) ^ Rh(xh,xl, 6) )
//~ #define sig1l(xh,xl) ( Sl(xh,xl,19) ^ Sl(xh,xl,61) ^ Rl(xh,xl, 6) )

//~ /* Add with carry, x += y */
//~ #define ADDC(xh,xl,yh,yl) ( xl+=(yl), xh+=(yh)+(xl<(yl)) )

/* Stored as high, then low words */
static word32 K[] = {
	0x428a2f98, 0xd728ae22, 0x71374491, 0x23ef65cd,
	0xb5c0fbcf, 0xec4d3b2f, 0xe9b5dba5, 0x8189dbbc,
	0x3956c25b, 0xf348b538, 0x59f111f1, 0xb605d019,
	0x923f82a4, 0xaf194f9b, 0xab1c5ed5, 0xda6d8118,
	0xd807aa98, 0xa3030242, 0x12835b01, 0x45706fbe,
	0x243185be, 0x4ee4b28c, 0x550c7dc3, 0xd5ffb4e2,
	0x72be5d74, 0xf27b896f, 0x80deb1fe, 0x3b1696b1,
	0x9bdc06a7, 0x25c71235, 0xc19bf174, 0xcf692694,
	0xe49b69c1, 0x9ef14ad2, 0xefbe4786, 0x384f25e3,
	0x0fc19dc6, 0x8b8cd5b5, 0x240ca1cc, 0x77ac9c65,
	0x2de92c6f, 0x592b0275, 0x4a7484aa, 0x6ea6e483,
	0x5cb0a9dc, 0xbd41fbd4, 0x76f988da, 0x831153b5,
	0x983e5152, 0xee66dfab, 0xa831c66d, 0x2db43210,
	0xb00327c8, 0x98fb213f, 0xbf597fc7, 0xbeef0ee4,
	0xc6e00bf3, 0x3da88fc2, 0xd5a79147, 0x930aa725,
	0x06ca6351, 0xe003826f, 0x14292967, 0x0a0e6e70,
	0x27b70a85, 0x46d22ffc, 0x2e1b2138, 0x5c26c926,
	0x4d2c6dfc, 0x5ac42aed, 0x53380d13, 0x9d95b3df,
	0x650a7354, 0x8baf63de, 0x766a0abb, 0x3c77b2a8,
	0x81c2c92e, 0x47edaee6, 0x92722c85, 0x1482353b,
	0xa2bfe8a1, 0x4cf10364, 0xa81a664b, 0xbc423001,
	0xc24b8b70, 0xd0f89791, 0xc76c51a3, 0x0654be30,
	0xd192e819, 0xd6ef5218, 0xd6990624, 0x5565a910,
	0xf40e3585, 0x5771202a, 0x106aa070, 0x32bbd1b8,
	0x19a4c116, 0xb8d2d0c8, 0x1e376c08, 0x5141ab53,
	0x2748774c, 0xdf8eeb99, 0x34b0bcb5, 0xe19b48a8,
	0x391c0cb3, 0xc5c95a63, 0x4ed8aa4a, 0xe3418acb,
	0x5b9cca4f, 0x7763e373, 0x682e6ff3, 0xd6b2b8a3,
	0x748f82ee, 0x5defb2fc, 0x78a5636f, 0x43172f60,
	0x84c87814, 0xa1f0ab72, 0x8cc70208, 0x1a6439ec,
	0x90befffa, 0x23631e28, 0xa4506ceb, 0xde82bde9,
	0xbef9a3f7, 0xb2c67915, 0xc67178f2, 0xe372532b,
	0xca273ece, 0xea26619c, 0xd186b8c7, 0x21c0c207,
	0xeada7dd6, 0xcde0eb1e, 0xf57d4f7f, 0xee6ed178,
	0x06f067aa, 0x72176fba, 0x0a637dc5, 0xa2c898a6,
	0x113f9804, 0xbef90dae, 0x1b710b35, 0x131c471b,
	0x28db77f5, 0x23047d84, 0x32caab7b, 0x40c72493,
	0x3c9ebe0a, 0x15c9bebc, 0x431d67c4, 0x9c100d4c,
	0x4cc5d4be, 0xcb3e42b6, 0x597f299c, 0xfc657e2a,
	0x5fcb6fab, 0x3ad6faec, 0x6c44198c, 0x4a475817
};

#define H1h 0x6a09e667
#define H1l 0xf3bcc908
#define H2h 0xbb67ae85
#define H2l 0x84caa73b
#define H3h 0x3c6ef372
#define H3l 0xfe94f82b
#define H4h 0xa54ff53a
#define H4l 0x5f1d36f1
#define H5h 0x510e527f
#define H5l 0xade682d1
#define H6h 0x9b05688c
#define H6l 0x2b3e6c1f
#define H7h 0x1f83d9ab
#define H7l 0xfb41bd6b
#define H8h 0x5be0cd19
#define H8l 0x137e2179

word32 HH[ 16 ] = { H1h,H1l, H2h,H2l, H3h,H3l, H4h,H4l,H5h,H5l, H6h,H6l, H7h,H7l, H8h,H8l };

/* convert to big endian where needed */

static void convert_to_bigendian( void* data, int len ) {
/* test endianness */
	word32 test_value = 0x01;
	byte* test_as_bytes = (byte*) &test_value;
	int little_endian = test_as_bytes[ 0 ];
	word32 temp;
	byte* temp_as_bytes = (byte*) &temp;
	word32* data_as_words = (word32*) data;
	byte* data_as_bytes;
	int i;
	if ( little_endian ) {
		len /= 4;
		for ( i = 0; i < len; i++ ) {
		temp = data_as_words[ i ];
		data_as_bytes = (byte*) &( data_as_words[ i ] );
		data_as_bytes[ 0 ] = temp_as_bytes[ 3 ];
		data_as_bytes[ 1 ] = temp_as_bytes[ 2 ];
		data_as_bytes[ 2 ] = temp_as_bytes[ 1 ];
		data_as_bytes[ 3 ] = temp_as_bytes[ 0 ];
		}
	}
/* on big endian machines do nothing as the CPU representation
   automatically does the right thing for SHA1 */
}

void gim_sha512_obj::SHA512_init( SHA512_ctx* ctx ) {
	memcpy( ctx->H, HH, 16 * sizeof( word32 ) );
	ctx->lbits = 0;
	ctx->hbits = 0;
	ctx->mlen = 0;
}

static void SHA512_transform( SHA512_ctx* ctx ) {
	int t;
	word32 Ah = ctx->H[ 0 ];
	word32 Al = ctx->H[ 1 ];
	word32 Bh = ctx->H[ 2 ];
	word32 Bl = ctx->H[ 3 ];
	word32 Ch = ctx->H[ 4 ];
	word32 Cl = ctx->H[ 5 ];
	word32 Dh = ctx->H[ 6 ];
	word32 Dl = ctx->H[ 7 ];
	word32 Eh = ctx->H[ 8 ];
	word32 El = ctx->H[ 9 ];
	word32 Fh = ctx->H[ 10 ];
	word32 Fl = ctx->H[ 11 ];
	word32 Gh = ctx->H[ 12 ];
	word32 Gl = ctx->H[ 13 ];
	word32 Hh = ctx->H[ 14 ];
	word32 Hl = ctx->H[ 15 ];
	word32 T1h, T1l, T2h, T2l;
	word32 W[ 160 ]; /* Stored as hi, lo */
	memcpy( W, ctx->M, 32*sizeof(word32) );
	for ( t = 16; t < 80; t++ ) {
// W[ t ] = sig1(W[t-2]) + W[t-7] + sig0(W[t-15]) + W[t-16];
		T1h = sig1h(W[2*t-4],W[2*t-3]); T1l = sig1l(W[2*t-4],W[2*t-3]);
		ADDC( T1h, T1l, W[2*t-14], W[2*t-13] );
		ADDC( T1h, T1l, sig0h(W[2*t-30],W[2*t-29]), sig0l(W[2*t-30],W[2*t-29]) );
		ADDC( T1h, T1l, W[2*t-32], W[2*t-31] );
		W[2*t] = T1h; W[2*t+1] = T1l;
	}
	for ( t = 0; t < 80; t++ ) {
// T1 = H + SIG1(E) + Ch(E,F,G) + K[t] + W[t];
		T1h = Hh; T1l = Hl;
		ADDC( T1h, T1l, SIG1h(Eh,El), SIG1l(Eh,El) );
		ADDC( T1h, T1l, Choice(Eh,Fh,Gh), Choice(El,Fl,Gl));
		ADDC( T1h, T1l, K[2*t], K[2*t+1] );
		ADDC( T1h, T1l, W[2*t], W[2*t+1] );
// T2 = SIG0(A) + Maj(A,B,C);
		T2h = SIG0h(Ah,Al); T2l = SIG0l(Ah,Al);
		ADDC( T2h, T2l, Maj(Ah,Bh,Ch), Maj(Al,Bl,Cl) );
		Hh = Gh; Hl = Gl;
		Gh = Fh; Gl = Fl;
		Fh = Eh; Fl = El;
// E = D + T1;
		Eh = Dh; El = Dl;
		ADDC( Eh, El, T1h, T1l );
		Dh = Ch; Dl = Cl;
		Ch = Bh; Cl = Bl;
		Bh = Ah; Bl = Al;
// A = T1 + T2;
		Ah = T1h; Al = T1l;
		ADDC( Ah, Al, T2h, T2l );
	}
	ADDC( ctx->H[ 0], ctx->H[ 1], Ah, Al );
	ADDC( ctx->H[ 2], ctx->H[ 3], Bh, Bl );
	ADDC( ctx->H[ 4], ctx->H[ 5], Ch, Cl );
	ADDC( ctx->H[ 6], ctx->H[ 7], Dh, Dl );
	ADDC( ctx->H[ 8], ctx->H[ 9], Eh, El );
	ADDC( ctx->H[10], ctx->H[11], Fh, Fl );
	ADDC( ctx->H[12], ctx->H[13], Gh, Gl );
	ADDC( ctx->H[14], ctx->H[15], Hh, Hl );
}

void gim_sha512_obj::SHA512_update( SHA512_ctx* ctx, const void* vdata, word32 data_len ) {
	const byte* data = ( const byte * ) vdata;
	word32 use;
	word32 low_bits;
/* convert data_len to bits and add to the 128 bit word formed by lbits
   and hbits */
	ctx->hbits += data_len >> 29;
	low_bits = data_len << 3;
	ctx->lbits += low_bits;
	if ( ctx->lbits < low_bits ) { ctx->hbits++; }
/* deal with first block */
	use = min( 128 - ctx->mlen, data_len );
	memcpy( ctx->M + ctx->mlen, data, use );
	ctx->mlen += use;
	data_len -= use;
	data += use;
	while ( ctx->mlen == 128 ) {
		convert_to_bigendian( (word32*)ctx->M, 128 );
		SHA512_transform( ctx );
		use = min( 128, data_len );
		memcpy( ctx->M, data, use );
		ctx->mlen = use;
		data_len -= use;
		data += use; /* was missing */
	}
}

void gim_sha512_obj::SHA512_final( SHA512_ctx* ctx ) {
	if ( ctx->mlen < 128-16 ) {
		ctx->M[ ctx->mlen ] = 0x80; ctx->mlen++;
		memset( ctx->M + ctx->mlen, 0x00, 128-8 - ctx->mlen );
		convert_to_bigendian( ctx->M, 128-8 );
	}
	else {
		ctx->M[ ctx->mlen ] = 0x80;
		ctx->mlen++;
		memset( ctx->M + ctx->mlen, 0x00, 128 - ctx->mlen );
		convert_to_bigendian( ctx->M, 128 );
		SHA512_transform( ctx );
		memset( ctx->M, 0x00, 128-8 );
	}
	memcpy( ctx->M + 128-8, (void*)(&(ctx->hbits)), 8 );
	SHA512_transform( ctx );
}

void gim_sha512_obj::SHA512_digest( SHA512_ctx* ctx, byte* digest ) {
	if ( digest ) {
		memcpy( digest, ctx->H, 16 * sizeof( word32 ) );
		convert_to_bigendian( digest, 16 * sizeof( word32 ) );
	}
}
