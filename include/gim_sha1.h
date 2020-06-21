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

/*	Thanks for this code to Jasin Bushnaief */


#ifndef _GIM_SHA1_H_
#define _GIM_SHA1_H_


	#include <assert.h>
	#include <ctype.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdint.h>

	typedef struct _Sha1Ctx
	{
		uint8_t block[64];
		uint32_t h[5];
		uint64_t bytes;
		uint32_t cur;
	} Sha1Ctx;

	// don't just forward declare as we want to pass it around by value
	typedef struct _Sha1Digest
	{
		uint32_t digest[5];
	} Sha1Digest;

			
	class gim_sha1_obj {
		public:
			Sha1Ctx 	* Sha1Ctx_create	(void);
			void		Sha1Ctx_reset		(Sha1Ctx*);
			void		Sha1Ctx_write		(Sha1Ctx*, const void* msg, uint64_t bytes);
			Sha1Digest	Sha1Ctx_getDigest	(Sha1Ctx*);
			void		Sha1Ctx_release		(Sha1Ctx*);
			Sha1Digest	Sha1_get 			(const void * msg , uint64_t bytes);
			Sha1Digest	Sha1Digest_fromStr	(const char* src);
			void		Sha1Digest_toStr	(const Sha1Digest* digest, char* dst);
			
		private:
			void		processBlock		(Sha1Ctx * ctx);
			uint32_t	rotl32				(uint32_t x, int b);
			uint32_t	get32				(const void* p);
			uint32_t	f					(int t, uint32_t b, uint32_t c, uint32_t d);
	};
		

#endif //_GIM_SHA1_H_
