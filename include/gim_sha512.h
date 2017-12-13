//
// gim_sha512.h
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

/*!	\file		gim_sha512.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

/*
 * Implementation of SHA-512, based on Adam Back's sha-1 implementation.
 * Uses 32 bit words.
 */

#ifndef _GIM_SHA512_H_
#define _GIM_SHA512_H_

	#ifndef GIM_SHA256_H_
		#define byte unsigned char
		#define word32 unsigned int
	#endif
	
	struct SHA512_ctx {
		  word32 H[ 16 ];
		  word32 hbits, lbits;
		  byte M[ 256 ];
		  word32 mlen;
	};
	
	class gim_sha512_obj {
		public:
			void SHA512_init	( SHA512_ctx * );
			void SHA512_update	( SHA512_ctx * , const void * , word32 );
			void SHA512_final	( SHA512_ctx * );
			void SHA512_digest	( SHA512_ctx * , byte * );
	};
	
	
	#ifdef __cplusplus
	extern "C"
	{
	#endif
		
	#define min( x, y ) ( ( x ) < ( y ) ? ( x ) : ( y ) )

		/* Rotate right by n bits; separate macros for high and low results */
		#define Sh(xh,xl,n) ( (n<32) ? ((xh>>n)|(xl<<(32-n))) : ((xh<<(64-n))|(xl>>(n-32))) )
		#define Sl(xh,xl,n) ( (n<32) ? ((xh<<(32-n))|(xl>>n)) : ((xh>>(n-32))|(xl<<(64-n))) )
		
		/* Shift right by n bits; separate macros for high and low results */
		#define Rh(xh,xl,n) ( (n<32) ? (xh>>n) : 0 )
		#define Rl(xh,xl,n) ( (n<32) ? ((xh<<(32-n))|(xl>>n)) : (xh>>(n-32)) )
		
		/* These can be done independently on high and low parts */
		#define Choice(x,y,z) ( ((x) & (y)) | (~(x) & (z)) )
		#define Maj(x,y,z) ( ( (x) & (y) ) | ( (x) & (z) ) | ( (y) & (z) ) )
		
		/* Sigma functions */
		#define SIG0h(xh,xl) ( Sh(xh,xl,28) ^ Sh(xh,xl,34) ^ Sh(xh,xl,39) )
		#define SIG0l(xh,xl) ( Sl(xh,xl,28) ^ Sl(xh,xl,34) ^ Sl(xh,xl,39) )
		#define SIG1h(xh,xl) ( Sh(xh,xl,14) ^ Sh(xh,xl,18) ^ Sh(xh,xl,41) )
		#define SIG1l(xh,xl) ( Sl(xh,xl,14) ^ Sl(xh,xl,18) ^ Sl(xh,xl,41) )
		#define sig0h(xh,xl) ( Sh(xh,xl, 1) ^ Sh(xh,xl, 8) ^ Rh(xh,xl, 7) )
		#define sig0l(xh,xl) ( Sl(xh,xl, 1) ^ Sl(xh,xl, 8) ^ Rl(xh,xl, 7) )
		#define sig1h(xh,xl) ( Sh(xh,xl,19) ^ Sh(xh,xl,61) ^ Rh(xh,xl, 6) )
		#define sig1l(xh,xl) ( Sl(xh,xl,19) ^ Sl(xh,xl,61) ^ Rl(xh,xl, 6) )
		
		/* Add with carry, x += y */
		#define ADDC(xh,xl,yh,yl) ( xl+=(yl), xh+=(yh)+(xl<(yl)) )
		
	#ifdef __cplusplus
	}
	#endif

#endif //_GIM_SHA512_H_
