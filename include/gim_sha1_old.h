//
// gim_sha1.h
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

/*!	\file		gim_sha1.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

/* #define LITTLE_ENDIAN * This should be #define'd if true. */
/* #define SHA1HANDSOFF * Copies data before messing with it. */

/* Hash a single 512-bit block. This is the core of the algorithm. */

#ifndef _GIM_SHA1_H_
#define _GIM_SHA1_H_
	
	#include <stdio.h>
	#include <string.h>
	
	typedef struct {
		unsigned int state[5];
		unsigned int count[2];
		unsigned char buffer[64];
	} SHA1_CTX;
	
	class gim_sha1_obj {
		public:
			void SHA1Transform( unsigned int state[5] , unsigned char buffer[64] );
			void SHA1Init( SHA1_CTX * context );
			void SHA1Update( SHA1_CTX * context , unsigned char * data , unsigned int len );
			void SHA1Final( unsigned char digest[20] , SHA1_CTX * context );
	};
	
	#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
	
	/* blk0() and blk() perform the initial expand. */
	/* I got the idea of expanding during the round function from SSLeay */
	#ifdef LITTLE_ENDIAN
	#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
		|(rol(block->l[i],8)&0x00FF00FF))
	#else
	#define blk0(i) block->l[i]
	#endif
	#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
		^block->l[(i+2)&15]^block->l[i&15],1))
	
	/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
	#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
	#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
	#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
	#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
	#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

#endif //_GIM_SHA1_H_
