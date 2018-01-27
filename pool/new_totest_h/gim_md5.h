//
// gim_md5.h
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

/*!	\file		gim_md5.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		
	
	#ifndef _MD5_H
	#define _MD5_H
	
	typedef struct md5_context {
		unsigned long total[2];
		unsigned long state[4];
		unsigned char buffer[64];
	} MD5_CTX;
	
	class gim_md5_obj {
		public:
			void	MD5Init( MD5_CTX *ctx );
			void	MD5Update( MD5_CTX *ctx , unsigned char *input , int ilen );
			void	MD5Final( unsigned char output[16] , MD5_CTX *ctx );
	};
	
	//~ void	md5_csum( unsigned char *input, int ilen, unsigned char output[16] );
	//~ int		md5_file( char *path, unsigned char output[16] );
	//~ void	md5_hmac( unsigned char *key, int keylen, unsigned char *input, int ilen, unsigned char output[16] );
	//~ int		md5_self_test( void );

#endif /* md5.h */
