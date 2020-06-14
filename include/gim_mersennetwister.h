//
// gim_mersennetwister.h
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

/*!	\file		gim_mersennetwister.h
	\version	2.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

/* Taken from https://github.com/mgallo96/MersenneTwister "Thank you" */

/* A C-program for MT19937: Integer version (1998/4/6)            */
/*  genrand() generates one pseudorandom unsigned integer (32bit) */
/* which is uniformly distributed among 0 to 2^32-1  for each     */
/* call. sgenrand(seed) set initial values to the working area    */
/* of 624 words. Before genrand(), sgenrand(seed) must be         */
/* called once. (seed is any 32-bit integer except for 0).        */
/*   Coded by Takuji Nishimura, considering the suggestions by    */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.              */

/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */ 
/* 02111-1307  USA                                                 */

/* Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.       */
/* When you use this, send an email to: matumoto@math.keio.ac.jp   */
/* with an appropriate reference to your work.                     */

/* REFERENCE                                                       */
/* M. Matsumoto and T. Nishimura,                                  */
/* "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform  */
/* Pseudo-Random Number Generator",                                */
/* ACM Transactions on Modeling and Computer Simulation,           */
/* Vol. 8, No. 1, January 1998, pp 3--30.                          */

				 
#ifndef MERSENNETWISTER_H
#define MERSENNETWISTER_H

	#include <stdio.h>
	#include <limits.h>
	#include <time.h>

	/* Period parameters */  
	#define MT_N 624
	#define MT_M 397
	#define MATRIX_A 0x9908b0df   /* constant vector a */
	#define UPPER_MASK 0x80000000 /* most significant w-r bits */
	#define LOWER_MASK 0x7fffffff /* least significant r bits */

	/* Tempering parameters */   
	#define TEMPERING_MASK_B 0x9d2c5680
	#define TEMPERING_MASK_C 0xefc60000
	#define TEMPERING_SHIFT_U(y)  (y >> 11)
	#define TEMPERING_SHIFT_S(y)  (y << 7)
	#define TEMPERING_SHIFT_T(y)  (y << 15)
	#define TEMPERING_SHIFT_L(y)  (y >> 18)


	/*! \class 		MTRand
		\brief		Mersenne Twister random number generator
		\author		Based on code by Makoto Matsumoto, Takuji Nishimura considering the suggestions by Topher Cooper and Marc Rieffel
		\author		Richard J. Wagner  - rjwagner@writeme.com
		\warning	This class is not documented by <b>gkript.org</b>.
	*/
	class MTRand {
	// Data
	public:
	

	protected:
		_gim_Ulong mt[MT_N];	/* the array for the state vector  */
		int mti=MT_N+1;			/* mti==MT_N+1 means mt[MT_N] is not initialized */

	//Methods
	public:
		MTRand( _gim_Ulong oneSeed );  // initialize with a simple _gim_Ulong
		MTRand();  // auto-initialize with /dev/urandom or time() and clock()
		
		// Access to 32-bit random numbers
		double rand();                          // real number in [0,1]
		double rand( const double &n );         // real number in [0,n]
		double randExc();                       // real number in [0,1)
		double randExc( const double &n );      // real number in [0,n)
		double randDblExc();                    // real number in (0,1)
		double randDblExc( const double &n  );   // real number in (0,n)

		_gim_Ulong randInt();                       // integer in [0,2^32-1]
		_gim_Ulong randInt( const _gim_Ulong &n );      // integer in [0,n] for n < 2^32
		double operator()() { return rand(); }  // same as rand()
		
		// Re-seeding functions with same behavior as initializers
		void seed( const _gim_Ulong oneSeed );
		void seed();
		
	protected:
		static _gim_Ulong hash( time_t t, clock_t c );
	};


	inline MTRand::MTRand( _gim_Ulong oneSeed ) {
		seed( oneSeed );
	}

	inline MTRand::MTRand() {
		seed();
	}

	inline double MTRand::rand() {
		return double(randInt()) * (1.0/4294967295.0);
	}

	inline double MTRand::rand( const double &n ) {
		return double(randInt()) * n;
	}

	inline double MTRand::randExc() {
		return double(randInt()) * (1.0/4294967296.0);
	}

	inline double MTRand::randExc( const double &n ) {
		return randExc() * n;
	}

	inline _gim_Ulong MTRand::randInt() {
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

	inline _gim_Ulong MTRand::randInt( const _gim_Ulong &n )
	{
		_gim_Ulong used = n;
		used |= used >> 1;
		used |= used >> 2;
		used |= used >> 4;
		used |= used >> 8;
		used |= used >> 16;
		
		_gim_Ulong i;
		do
			i = randInt() & used;  // toss unused bits to shorten search
		while ( i > n );
		return i;
	}


	inline void MTRand::seed( const _gim_Ulong oneSeed ) {
		/* setting initial seeds to mt[MT_N] using         */
		/* the generator Line 25 of Table 1 in          */
		/* [KNUTH 1981, The Art of Computer Programming */
		/*    Vol. 2 (2nd Ed.), pp102]                  */
		mt[0]= oneSeed & 0xffffffff;
		for (mti=1; mti<MT_N; mti++)
			mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
	}

	inline void MTRand::seed() {
		register bool success = true;
				
		FILE* urandom = fopen( "/dev/urandom", "rb" );
		if( urandom ) {
			_gim_Ulong s;
			success = fread( &s, sizeof(_gim_Ulong), 1, urandom );
			fclose(urandom);
			if( success ) 
				seed( s );
			else
				seed( hash( time(NULL), clock() ) );
		}
		else
			seed( hash( time(NULL), clock() ) );
	}

	inline _gim_Ulong MTRand::hash( time_t t, clock_t c )
	{
		// Get a _gim_Ulong from t and c
		// Better than _gim_Ulong(x) in case x is floating point in [0,1]
		// Based on code by Lawrence Kirby (fred@genesis.demon.co.uk)

		static _gim_Ulong differ = 0;  // guarantee time-based seeds will change

		_gim_Ulong h1 = 0;
		unsigned char *p = (unsigned char *) &t;
		for( size_t i = 0; i < sizeof(t); ++i )
		{
			h1 *= UCHAR_MAX + 2U;
			h1 += p[i];
		}
		_gim_Ulong h2 = 0;
		p = (unsigned char *) &c;
		for( size_t j = 0; j < sizeof(c); ++j )
		{
			h2 *= UCHAR_MAX + 2U;
			h2 += p[j];
		}
		return ( h1 + differ++ ) ^ h2;
	}

#endif
