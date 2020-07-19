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
	
	
	#define	_GIM_MT_ORIGINAL	__GIM_NO	//	__GIM_YES means the original MT algo; __GIM_NO means the modified MT algo
	

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
			_gim_Uint32 		mt[ MT_N ];	/* the array for the state vector  */
			int 				mti = MT_N + 1;			/* mti==MT_N+1 means mt[MT_N] is not initialized */

		//Methods
		public:
			MTRand( _gim_Uint32 oneSeed );  // initialize with a simple _gim_Uint32
			MTRand();  // auto-initialize with /dev/urandom or time() and clock()
			
			// Access to 32-bit random numbers
			_gim_Uint32 		rand();									// real number in [0,1]
			_gim_Uint32 		rand( const _gim_Uint32 &n );			// real number in [0,n]

			float 				randFloat( float range );          		// integer in [0,range-1]

			_gim_Uint32 		randInt( _gim_Uint32 range );           // integer in [0,range-1]
			_gim_Uint32 		randInt();                       		// integer in [0,2^32-1]
			_gim_flag			stat_distr_over_100_percentage( _gim_Uint8 perc );
			_gim_flag			stat_distr_over_100_percentage( float perc );

			#if ( _GIM_MT_ORIGINAL == __GIM_NO )			
				_gim_Uint32 	randIntMT( const _gim_Uint32 &n );		// integer in [0,n] for n < 2^32
			#endif
			
			_gim_Uint8 		randUInt8();
			_gim_Uint32 		operator()() { return rand(); }			// same as rand()
			
			// Re-seeding functions with same behavior as initializers
			void 				seed( const _gim_Uint32 oneSeed );
			void 				seed();
			
		protected:
			static _gim_Uint32	hash( time_t t, clock_t c );

	};


#endif


