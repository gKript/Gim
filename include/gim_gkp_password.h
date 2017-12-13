//
// gim_gkp_password.h
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

/*!	\file		gim_gkp_password.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_GKP_PASSWORD_H
#define _GIM_GKP_PASSWORD_H
		
	#include "gim_checksum.h"

	/*! \def	MIN_PASSWD_LEN
  		\brief	It defines the minimal length for the password
	*/
	#define	MIN_PASSWD_LEN			6
	
	/*! \def	GIM_PRIME_DATA_FNAME
  		\brief	It defines the filename for the prime number generated
	*/
	#define	GIM_PRIME_DATA_FNAME	"prime"

	/*! \class 		gim_password_obj
		\brief		It is the inner class used by Gim to manage the header of a Gkp file
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	1.0-0
		\date		2003-2007
	*/
	class gim_password_obj {
		public:
			/*! \fn		void set( char * passwd )
				\brief	This function is used to set a password, used by Gim when necessary
				\param	passwd	The password you want to set
			*/
			void				set( const char * passwd );

			/*! \fn		void set_secondary( char * passwd )
				\brief	This function is used to set a password, used by Gim when necessary
				\param	passwd	The secondary password you want to set
			*/
			void				set_secondary( const char * passwd );

			/*! \fn		void calc( void )
				\brief	This function is used to calc the password digest and the inner seeds 
			*/
			void				calc( void );

			/*! \fn		void recalc( void )
				\brief	This function is used to recalc the password digest and the inner seeds 
			*/
			void				recalc( void );
		
		private:

	friend	class				gim_cryptography_obj;
	friend	class				gim_cnc_encoding_obj;
	friend	class				gim_cnc_decoding_obj;
	friend	class				gim_cnc_alfabeto_obj;
			
			char				password[256];
			char				secondary[256];
			char	 			password_md5[GIM_MD5_SIZE];	
			char	 			password_sha1[GIM_SHA1_SIZE];	
			char	 			password_sha256[GIM_SHA256_SIZE];
			char	 			password_sha512[GIM_SHA512_SIZE];
			char	 			secondary_sha512[GIM_SHA512_SIZE];
			_gim_Ulong			seed_md5;
			_gim_Ulong			seed_sha1;
			_gim_Ulong			seed_sha256;
			_gim_Ulong			seed_sha512;
			_gim_Ulong			seed_secondary;
			_gim_Ulong			seed_main;
			
			int					Prime[2048];
			unsigned int		min_lenght;
			volatile _gim_flag	prime_filled;
		
			volatile _gim_flag	password_already_calc;
			
			_gim_flag			isPrime( _gim_Ulong n );
			void				prepare_prime( int size );
			
		public:	
			/*! the constructor
			*/
			gim_password_obj();
		};
		
	//~ extern gim_password_obj * gim_password;
	
#endif /* _GIM_GKP_PASSWORD_H */
