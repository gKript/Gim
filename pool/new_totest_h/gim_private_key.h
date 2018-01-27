//
// gim_private_key.h
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

/*!	\file		gim_private_key.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_PRVATE_KEY_OBJ_H_
#define _GIM_PRVATE_KEY_OBJ_H_

	#include "gim_base_header.h"
	#include "gim_lexical.h"
	#include "gim_checksum.h"
	#include "gim_mersennetwister.h"
	#include "gim_environment.h"

	/*!	\def 	PRE_LINE_KEY
		\brief	This is the separator for a key file
	*/
	#define PRE_LINE_KEY	"::--"

	/*!	\def	HKEY_SIZE_BIN
		\brief	This is the size of a key in binary
	*/
	#define HKEY_SIZE_BIN	( 512 )

	/*!	\def	HKEY_SIZE_ASC
		\brief	This is the size of a key in ascii
	*/
	#define	HKEY_SIZE_ASC	( 2 * HKEY_SIZE_BIN )

	/*!	\class	gim_private_key_obj
		\brief	This class automatically create, read, and check the private key. 
				In a future version it will be able to grant your personal key to other people and it will be able 
				to acquire personal key of other person to exchange data safetly.
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	1.0-0
		\date		2003-2007
	*/
	class gim_private_key_obj {
		public:
			/*!	\fn		void	grant( void )
				\todo	This method prepare and save to disk in gkp format the personal key ready to be gived to other people
			*/
			void	grant( void );

			/*!	\fn		void	acquire( void )
				\todo	This method read a key in gkp format and store the personal informations into the other keys path
			*/
			void	acquire( void );
		
		private:
			char 	key[ HKEY_SIZE_ASC ];
			char 	md5[33];
		
			void	create	( void );
			void	read	( void );
			void	write	( void );
		
			volatile _gim_flag	filled;
		
		public:
			/*!	the constructor
			*/
			inline gim_private_key_obj() {
				__GIM_CLEAR( key , HKEY_SIZE_ASC , char );
				filled = __GIM_NO;
				read();
			};
	};

	extern gim_private_key_obj	* gim_key;

#endif	// _GIM_PRVATE_KEY_OBJ_H_
