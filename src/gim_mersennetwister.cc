//
// gim_md5.cc
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
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */


#include "../include/gim_base_header.h"
#include "../include/gim_custom_type.h" 
#include "../include/gim_checksum.h"
#include "../include/gim_mersennetwister.h"


MTRand::MTRand( _gim_Uint32 oneSeed ) {
	seed( oneSeed );
}


MTRand::MTRand() {
	seed();
}



#if ( _GIM_MT_ORIGINAL == __GIM_YES )

	_gim_Uint32	MTRand::rand() {
		return _gim_Uint32(randInt()) * (1.0/4294967295.0);
	}

#elif ( _GIM_MT_ORIGINAL == __GIM_NO )

	_gim_Uint32	MTRand::rand() {
		return randInt();
	}

#endif

_gim_Uint32	MTRand::rand( const _gim_Uint32 &n ) {
	return _gim_Uint32(randInt()) * n;
}


float 		MTRand::randFloat( float range ) {
	_gim_Uint32 fix = 0;
	float 		flt = 0.0;
	_gim_Uint32	dcm = 10000;
		
	flt = (float)( (float)randInt( (_gim_Uint32)( range * dcm ) ) / (float)dcm ); 
	return flt;
}



_gim_Uint32	MTRand::randInt( _gim_Uint32 range ) {
	return ( this->randInt() % range );
}


#if ( _GIM_MT_ORIGINAL == __GIM_YES )

	_gim_Uint32	MTRand::randInt() {
		unsigned long y;
		static unsigned long mag01[2]={0x0, MATRIX_A};
		/* mag01[x] = x * MATRIX_A  for x=0,1 */

		if (mti >= MT_N) { /* generate MT_N words at one time */
			int kk;

			if (mti == MT_N+1)   /* if sgenrand() has not been called, */
			    randInt(4357); /* a default initial seed is used   */

			for (kk=0;kk<MT_N-MT_M;kk++) {
			    y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			    mt[kk] = mt[kk+MT_M] ^ (y >> 1) ^ mag01[y & 0x1];
			}
			for (;kk<MT_N-1;kk++) {
			    y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			    mt[kk] = mt[kk+(MT_M-MT_N)] ^ (y >> 1) ^ mag01[y & 0x1];
			}
			y = (mt[MT_N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
			mt[MT_N-1] = mt[MT_M-1] ^ (y >> 1) ^ mag01[y & 0x1];

			mti = 0;
		}
	  
		y = mt[mti++];
		y ^= TEMPERING_SHIFT_U(y);
		y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
		y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
		y ^= TEMPERING_SHIFT_L(y);

		return y; 
	}

#elif ( _GIM_MT_ORIGINAL == __GIM_NO )						//	GIM RAND ROUTINE MT BASED

	_gim_Uint32	MTRand::randIntMT() {
		unsigned long y;
		static unsigned long mag01[2]={0x0, MATRIX_A};
		/* mag01[x] = x * MATRIX_A  for x=0,1 */

		if (mti >= MT_N) { /* generate MT_N words at one time */
			int kk;

			if (mti == MT_N+1)   /* if sgenrand() has not been called, */
			    randInt(4357); /* a default initial seed is used   */

			for (kk=0;kk<MT_N-MT_M;kk++) {
			    y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			    mt[kk] = mt[kk+MT_M] ^ (y >> 1) ^ mag01[y & 0x1];
			}
			for (;kk<MT_N-1;kk++) {
			    y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
			    mt[kk] = mt[kk+(MT_M-MT_N)] ^ (y >> 1) ^ mag01[y & 0x1];
			}
			y = (mt[MT_N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
			mt[MT_N-1] = mt[MT_M-1] ^ (y >> 1) ^ mag01[y & 0x1];

			mti = 0;
		}
	  
		y = mt[mti++];
		y ^= TEMPERING_SHIFT_U(y);
		y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
		y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
		y ^= TEMPERING_SHIFT_L(y);

		return y; 
	}


	_gim_Uint32	MTRand::randInt() {
	
		_gim_Uint32		LB = this->randIntMT();
		_gim_Uint32		HB = this->randIntMT();
		_gim_Uint32		UB = this->randIntMT();
		_gim_Uint32		MB = this->randIntMT();

		_gim_Uint32_VAL	ret;
		
		ret.LB = gim_checksum->chsum8( gim_checksum->sha512( &LB , 4 * sizeof( _gim_Uint8 ) ) ); 
		ret.HB = gim_checksum->chsum8( gim_checksum->sha512( &HB , 4 * sizeof( _gim_Uint8 ) ) );
		ret.UB = gim_checksum->chsum8( gim_checksum->sha512( &UB , 4 * sizeof( _gim_Uint8 ) ) );
		ret.MB = gim_checksum->chsum8( gim_checksum->sha512( &MB , 4 * sizeof( _gim_Uint8 ) ) );

		return ret.VAL; 
	}

#endif

_gim_Uint8	MTRand::randUInt8() {
	_gim_Uint8 res = 0;
	_gim_int32 t = 0;
	
	t = (_gim_int32)labs(randInt( 0xff ));
	res = (t % 0xff ) & 0xff;
	return res;
}


void MTRand::seed( const _gim_Uint32 oneSeed ) {
	/* setting initial seeds to mt[MT_N] using         */
	/* the generator Line 25 of Table 1 in          */
	/* [KNUTH 1981, The Art of Computer Programming */
	/*    Vol. 2 (2nd Ed.), pp102]                  */
	mt[0]= oneSeed & 0xffffffff;
	for (mti=1; mti<MT_N; mti++)
		mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}


void MTRand::seed() {
	register bool success = true;
			
	FILE* urandom = fopen( "/dev/urandom", "rb" );
	if( urandom ) {
		_gim_Uint32 s;
		success = fread( &s, sizeof(_gim_Uint32), 1, urandom );
		fclose(urandom);
		if( success ) 
			seed( s );
		else
			seed( hash( time(NULL), clock() ) );
	}
	else
		seed( hash( time(NULL), clock() ) );
}


_gim_Uint32 MTRand::hash( time_t t, clock_t c ) {
	// Get a _gim_Uint32 from t and c
	// Better than _gim_Uint32(x) in case x is floating point in [0,1]
	// Based on code by Lawrence Kirby (fred@genesis.demon.co.uk)

	static _gim_Uint32 differ = 0;  // guarantee time-based seeds will change

	_gim_Uint32 h1 = 0;
	unsigned char *p = (unsigned char *) &t;
	for( size_t i = 0; i < sizeof(t); ++i )
	{
		h1 *= UCHAR_MAX + 2U;
		h1 += p[i];
	}
	_gim_Uint32 h2 = 0;
	p = (unsigned char *) &c;
	for( size_t j = 0; j < sizeof(c); ++j )
	{
		h2 *= UCHAR_MAX + 2U;
		h2 += p[j];
	}
	return ( h1 + differ++ ) ^ h2;
}


_gim_flag	MTRand::stat_distr_over_100_percentage( _gim_Uint8 perc ) {
	if ( perc > 100 ) 
		return __GIM_ERROR;
	if ( ( randInt( 100 ) ) <= perc ) 
		return __GIM_YES;
	else
		return __GIM_NO;
}


_gim_flag	MTRand::stat_distr_over_100_percentage( float perc ) {
	if ( perc > 100.0 ) 
		return __GIM_ERROR;
	if ( ( randFloat( 100.0 ) ) <= perc ) 
		return __GIM_YES;
	else
		return __GIM_NO;
}

