//
// gim_checksum.h
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

/*!	\file		gim_checksum.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/

/*! \class 		gim_checksum_obj
	\brief		A Gim class to digest everything
	\author		Danilo Zannoni  -  asyntote@gkript.org
	\version	1.0-0
	\date		2003-2007
*/

/*!	\def	GIM_MD5_SIZE
			The size of the md5 digest ascii string
	\def	GIM_SHA1_SIZE
			The size of the sha1 digest ascii string
	\def	GIM_SHA256_SIZE
			The size of the sha256 digest ascii string
	\def	GIM_SHA512_SIZE
			The size of the sha512 digest ascii string
*/



/*! \fn		char *	sha1( char * buffer , _gim_Uint32 size )
	\brief	Use this function to get an sha1 hash from a buffer
	\param	buffer	The pointer of the buffer<BR>
					\a buffer != NULL
	\param	size	It is the length of the buffer<BR>
					\a size > 0
	\return an ascii digest
*/


#ifndef		_GIM_CHECKSUM_H_
#define		_GIM_CHECKSUM_H_

	#define	CHECK_HARDWARE_PROPERTIES	__GIM_YES

	#include "gim_md5.h"
	#include "gim_sha1.h"
	#include "gim_sha256.h"
	#include "gim_sha512.h"

	#define	GIM_MD5_SIZE		33
	#define	GIM_SHA1_SIZE		65
	#define	GIM_SHA256_SIZE		65
	#define	GIM_SHA512_SIZE		129

	class gim_checksum_obj {
		public:
			/*! \fn		char *	md5( char * buffer , _gim_Uint32 size )
				\brief	Use this function to get an md5 hash from a buffer
				\param	buffer	The pointer of the buffer<BR>
								\a buffer != NULL
				\param	size	It describes the length of the buffer<BR>
								\a size > 0
				\return an ascii digest
			*/
			char *	md5( char * buffer , _gim_Uint32 size );

			/*! \fn		char *	md5( char * file_name )
				\brief	Use this function to get an md5 hash from a file
				\param	file_name	The file name you want to digest<BR>
									\a file_name != NULL
				\return an ascii md5 digest
			*/
			char *	md5( char * file_name );

			/*! \fn		char *	sha1( char * buffer , _gim_Uint32 size )
				\brief	Use this function to get an sha1 hash from a buffer
				\param	buffer	The pointer of the buffer<BR>
								\a buffer != NULL
				\param	size	It is the length of the buffer<BR>
								\a size > 0
				\return an ascii digest
			*/
			char *	sha1( char * buffer , _gim_Uint32 size );

			/*! \fn		char *	sha1( char * file_name )
				\brief	Use this function to get an sha1 hash from a file
				\param	file_name	The file name you want to digest<BR>
									\a file_name != NULL
				\return an ascii digest
			*/
			char *	sha1( char * file_name );
			
			/*! \fn		char *	sha256( char * buffer , _gim_Uint32 size )
				\brief	Use this function to get an sha256 hash from a buffer
				\param	buffer	The pointer of the buffer<BR>
								\a buffer != NULL
				\param	size	It describes the length of the buffer<BR>
								\a size > 0
				\return an ascii digest
			*/
			char *	sha256( char * buffer , _gim_Uint32 size );
			
			/*! \fn		char *	sha256( char * file_name )
				\brief	Use this function to get an sha256 hash from a file
				\param	file_name	The file name you want to digest<BR>
									\a file_name != NULL
				\return an ascii digest
			*/
			char *	sha256( char * file_name );
			
			/*! \fn		char *	sha512( char * buffer , _gim_Uint32 size )
				\brief	Use this function to get an sha512 hash from a buffer
				\param	buffer	The pointer of the buffer<BR>
								\a buffer != NULL
				\param	size	It describes the length of the buffer<BR>
								\a size > 0
				\return an ascii digest
			*/
			char *	sha512( char * buffer , _gim_Uint32 size );
			
			/*! \fn		char *	sha512( char * file_name )
				\brief	Use this function to get an sha512 hash from a file
				\param	file_name	The file name you want to digest<BR>
									\a file_name != NULL
				\return an ascii digest
			*/
			char *	sha512( char * file_name );
			
			/*! \fn		_gim_Uint32	chsum( char * digest )
				\brief	Use this function to get a checksum starting from a Digest
				\param	diget	The source Digest for the checksum<BR>
									\a digest != NULL
				\return the checksum value (Unsigned int 32bit)
			*/
			_gim_Uint32	chsum( char * digest );

			/*! \fn		_gim_Uint8	chsum8( char * digest )
				\brief	Use this function to get a 8 bit checksum starting from a Digest
				\param	diget	The source Digest for the checksum<BR>
									\a digest != NULL
				\return the checksum value (Unsigned int 32bit)
			*/
			_gim_Uint8	chsum8( char * digest );
			
			//! gim_checksum_obj constructor.
			/*!
				The gim_checksum_obj constructor make a control on the dimension of the standard type int
				and if the machine is big-endian or litle-endian.
			*/
			inline gim_checksum_obj() {
				#ifdef CHECK_HARDWARE_PROPERTIES
					char message[256];
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , "Checking architecture variable size..." );
					sprintf( message , "short     : %d bytes" , sizeof( short ) );
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , message );
					if ( sizeof( short ) != 2 ) {
						fprintf(stderr, "** Configuration error.  Setting for \"short\" is incorrect.\n");
						fprintf(stderr, "   This must be a 16 bit data type, but it\n");
						fprintf(stderr, "   is configured as a %d bit data type.\n", sizeof( short ) * 8);
						gim_error->set( GIM_ERROR_CRITICAL , "gim_checksum_obj::gim_checksum_obj" , "Hardware incompatibility. SHORT type is too long" , -1 );
					}
					else {
						gim_error->set( "gim_checksum_obj::gim_checksum_obj" , "short     : OK"  );
					}
					sprintf( message , "int       : %d bytes" , sizeof( _gim_int32 ) );
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , message );
					if ( sizeof( _gim_int32 ) != 4 ) {
						fprintf(stderr, "** Configuration error.  Setting for \"int\" is incorrect.\n");
						fprintf(stderr, "   This must be a 32 bit data type, but it\n");
						fprintf(stderr, "   is configured as a %d bit data type.\n", sizeof( int ) * 8);
						gim_error->set( GIM_ERROR_CRITICAL , "gim_checksum_obj::gim_checksum_obj" , "Hardware incompatibility. INT type is too long" , -1 );
					}
					else {
						gim_error->set( "gim_checksum_obj::gim_checksum_obj" , "int       : OK" );
					}
					sprintf( message , "long      : %d bytes" , sizeof( _gim_long ) );
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , message );
					if ( sizeof( _gim_long ) != 4 ) {
						fprintf(stderr, "** Configuration error.  Setting for \"long\" is incorrect.\n");
						fprintf(stderr, "   This must be a 32 bit data type, but it\n");
						fprintf(stderr, "   is configured as a %d bit data type.\n", sizeof( long ) * 8);
						gim_error->set( GIM_ERROR_CRITICAL , "gim_checksum_obj::gim_checksum_obj" , "Hardware incompatibility. LONG type is too long" , -1 );
					}
					else {
						gim_error->set( "gim_checksum_obj::gim_checksum_obj" , "long      : OK" );
					}
					sprintf( message , "long long : %d bytes" , sizeof( long long ) );
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , message );
					if ( sizeof( long long ) != 8 ) {
						fprintf(stderr, "** Configuration error.  Setting for \"long long\" is incorrect.\n");
						fprintf(stderr, "   This must be a 64 bit data type, but it\n");
						fprintf(stderr, "   is configured as a %d bit data type.\n", sizeof( long long ) * 8);
						gim_error->set( GIM_ERROR_CRITICAL , "gim_checksum_obj::gim_checksum_obj" , "Hardware incompatibility. LONG LONG type is too long" , -1 );
					}
					else {
						gim_error->set( "gim_checksum_obj::gim_checksum_obj" , "long long : OK" );
					}
					#ifndef HIGHFIRST
						{	
							int t = 0x12345678;
							if (*((char *) &t) != 0x78) {
								fprintf(stderr, "** Configuration error.  Setting for HIGHFIRST in file md5.h\n");
								fprintf(stderr, "   is incorrect.  This symbol has not been defined, yet this\n");
								fprintf(stderr, "   machine is a big-endian (most significant byte first in\n");
								fprintf(stderr, "   memory) architecture.  Please modify md5.h so HIGHFIRST is\n");
								fprintf(stderr, "   defined when building for this machine.\n");
								gim_error->set( GIM_ERROR_FATAL , "gim_checksum_obj::gim_checksum_obj" , "Hardware incompatibility." , -1 );
							}
						}
					#endif
				#endif
			}
		};
		
	extern gim_checksum_obj	* gim_checksum;

#endif //_GIM_CHECKSUM_H_
