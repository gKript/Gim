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

	Copyright (C) 2002-2026
	Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)

	This file is part of Gim library.

	Gim is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	Gim is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Gim.  If not, see <https://www.gnu.org/licenses/>.

#############################################################################*/
//
// Gim is developed with Anjuta version 1.2.4
//

/*!     \file       gim_checksum.h
		\version    1.0-0
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

/*! \class 		gim_checksum_obj
	\brief		A Gim class to digest everything
	\author		asyntote
	\version	1.0-0
	\date		2003-2026
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
				\brief	Use this function to get an md5 hash from a buffer.
						NOT thread-safe: the result is a static buffer. Use md5_buf().
				\param	buffer	The pointer of the buffer<BR>
								\a buffer != NULL
				\param	size	It describes the length of the buffer<BR>
								\a size > 0
				\return an ascii digest
			*/
			char *	md5( char * buffer , _gim_Uint32 size );

			/*! \fn		char *	md5( char * file_name )
				\brief	Use this function to get an md5 hash from a file.
						NOT thread-safe: the result is a static buffer. Use md5_file_buf().
				\param	file_name	The file name you want to digest<BR>
									\a file_name != NULL
				\return an ascii md5 digest, or NULL when the file does not exist
			*/
			char *	md5( char * file_name );

			/*! \fn		char *	sha1( char * buffer , _gim_Uint32 size )
				\brief	Use this function to get an sha1 hash from a buffer.
						NOT thread-safe: the result is a static buffer. Use sha1_buf().
				\param	buffer	The pointer of the buffer<BR>
								\a buffer != NULL
				\param	size	It is the length of the buffer<BR>
								\a size > 0
				\return an ascii digest
			*/
			char *	sha1( char * buffer , _gim_Uint32 size );

			/*! \fn		char *	sha1( char * file_name )
				\brief	Use this function to get an sha1 hash from a file.
						NOT thread-safe: the result is a static buffer. Use sha1_file_buf().
				\param	file_name	The file name you want to digest<BR>
									\a file_name != NULL
				\return an ascii digest, or NULL when the file does not exist
			*/
			char *	sha1( char * file_name );

			/*! \fn		char *	sha256( char * buffer , _gim_Uint32 size )
				\brief	Use this function to get an sha256 hash from a buffer.
						NOT thread-safe: the result is a static buffer. Use sha256_buf().
				\param	buffer	The pointer of the buffer<BR>
								\a buffer != NULL
				\param	size	It describes the length of the buffer<BR>
								\a size > 0
				\return an ascii digest
			*/
			char *	sha256( char * buffer , _gim_Uint32 size );

			/*! \fn		char *	sha256( char * file_name )
				\brief	Use this function to get an sha256 hash from a file.
						NOT thread-safe: the result is a static buffer. Use sha256_file_buf().
				\param	file_name	The file name you want to digest<BR>
									\a file_name != NULL
				\return an ascii digest, or NULL when the file does not exist
			*/
			char *	sha256( char * file_name );

			/*! \fn		char *	sha512( char * buffer , _gim_Uint32 size )
				\brief	Use this function to get an sha512 hash from a buffer.
						NOT thread-safe: the result is a static buffer. Use sha512_buf().
				\param	buffer	The pointer of the buffer<BR>
								\a buffer != NULL
				\param	size	It describes the length of the buffer<BR>
								\a size > 0
				\return an ascii digest
			*/
			char *	sha512( char * buffer , _gim_Uint32 size );

			/*! \fn		char *	sha512( char * file_name )
				\brief	Use this function to get an sha512 hash from a file.
						NOT thread-safe: the result is a static buffer. Use sha512_file_buf().
				\param	file_name	The file name you want to digest<BR>
									\a file_name != NULL
				\return an ascii digest, or NULL when the file does not exist
			*/
			char *	sha512( char * file_name );

			/*! \fn		size_t	md5_buf( char * out , size_t size , char * buffer , _gim_Uint32 len )
				\brief	Thread-safe md5 of a buffer: writes the ascii digest in out (GIM_MD5_SIZE characters are enough).
				\return	the number of characters written (32), 0 when out is NULL or too small
			*/
			size_t	md5_buf( char * out , size_t size , char * buffer , _gim_Uint32 len );

			/*! \fn		size_t	md5_file_buf( char * out , size_t size , char * file_name )
				\brief	Thread-safe md5 of a file: writes the ascii digest in out.
				\return	the number of characters written (32), 0 when the file does not exist or out is too small
			*/
			size_t	md5_file_buf( char * out , size_t size , char * file_name );

			/*! \fn		size_t	sha1_buf( char * out , size_t size , char * buffer , _gim_Uint32 len )
				\brief	Thread-safe sha1 of a buffer: writes the ascii digest in out (GIM_SHA1_SIZE characters are enough).
				\return	the number of characters written (40), 0 when out is NULL or too small
			*/
			size_t	sha1_buf( char * out , size_t size , char * buffer , _gim_Uint32 len );

			/*! \fn		size_t	sha1_file_buf( char * out , size_t size , char * file_name )
				\brief	Thread-safe sha1 of a file.
				\return	the number of characters written (40), 0 when the file does not exist or out is too small
			*/
			size_t	sha1_file_buf( char * out , size_t size , char * file_name );

			/*! \fn		size_t	sha256_buf( char * out , size_t size , char * buffer , _gim_Uint32 len )
				\brief	Thread-safe sha256 of a buffer (GIM_SHA256_SIZE characters are enough).
				\return	the number of characters written (64), 0 when out is NULL or too small
			*/
			size_t	sha256_buf( char * out , size_t size , char * buffer , _gim_Uint32 len );

			/*! \fn		size_t	sha256_file_buf( char * out , size_t size , char * file_name )
				\brief	Thread-safe sha256 of a file.
				\return	the number of characters written (64), 0 when the file does not exist or out is too small
			*/
			size_t	sha256_file_buf( char * out , size_t size , char * file_name );

			/*! \fn		size_t	sha512_buf( char * out , size_t size , char * buffer , _gim_Uint32 len )
				\brief	Thread-safe sha512 of a buffer (GIM_SHA512_SIZE characters are enough).
				\return	the number of characters written (128), 0 when out is NULL or too small
			*/
			size_t	sha512_buf( char * out , size_t size , char * buffer , _gim_Uint32 len );

			/*! \fn		size_t	sha512_file_buf( char * out , size_t size , char * file_name )
				\brief	Thread-safe sha512 of a file.
				\return	the number of characters written (128), 0 when the file does not exist or out is too small
			*/
			size_t	sha512_file_buf( char * out , size_t size , char * file_name );

			//! gim_checksum_obj constructor.
			/*!
				The gim_checksum_obj constructor make a control on the dimension of the standard type int
				and if the machine is big-endian or litle-endian.
			*/
			inline gim_checksum_obj() {
				#ifdef CHECK_HARDWARE_PROPERTIES
					char message[256];
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , "Checking architecture variable size..." );
					sprintf( message , "short     : %d bytes" , (int) sizeof( short ) );
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , message );
					if ( sizeof( short ) != 2 ) {
						fprintf(stderr, "** Configuration error.  Setting for \"short\" is incorrect.\n");
						fprintf(stderr, "   This must be a 16 bit data type, but it\n");
						fprintf(stderr, "   is configured as a %d bit data type.\n", (int) ( sizeof( short ) * 8 ) );
						gim_error->set( GIM_ERROR_CRITICAL , "gim_checksum_obj::gim_checksum_obj" , "Hardware incompatibility. SHORT type is too long" , -1 );
					}
					else {
						gim_error->set( "gim_checksum_obj::gim_checksum_obj" , "short     : OK"  );
					}
					sprintf( message , "int       : %d bytes" , (int) sizeof( _gim_int32 ) );
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , message );
					if ( sizeof( _gim_int32 ) != 4 ) {
						fprintf(stderr, "** Configuration error.  Setting for \"int\" is incorrect.\n");
						fprintf(stderr, "   This must be a 32 bit data type, but it\n");
						fprintf(stderr, "   is configured as a %d bit data type.\n", (int) ( sizeof( int ) * 8 ) );
						gim_error->set( GIM_ERROR_CRITICAL , "gim_checksum_obj::gim_checksum_obj" , "Hardware incompatibility. INT type is too long" , -1 );
					}
					else {
						gim_error->set( "gim_checksum_obj::gim_checksum_obj" , "int       : OK" );
					}
					sprintf( message , "long      : %d bytes" , (int) sizeof( _gim_long ) );
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , message );
					if ( sizeof( _gim_long ) != 4 ) {
						fprintf(stderr, "** Configuration error.  Setting for \"long\" is incorrect.\n");
						fprintf(stderr, "   This must be a 32 bit data type, but it\n");
						fprintf(stderr, "   is configured as a %d bit data type.\n", (int) ( sizeof( long ) * 8 ) );
						gim_error->set( GIM_ERROR_CRITICAL , "gim_checksum_obj::gim_checksum_obj" , "Hardware incompatibility. LONG type is too long" , -1 );
					}
					else {
						gim_error->set( "gim_checksum_obj::gim_checksum_obj" , "long      : OK" );
					}
					sprintf( message , "long long : %d bytes" , (int) sizeof( long long ) );
					gim_error->set( "gim_checksum_obj::gim_checksum_obj" , message );
					if ( sizeof( long long ) != 8 ) {
						fprintf(stderr, "** Configuration error.  Setting for \"long long\" is incorrect.\n");
						fprintf(stderr, "   This must be a 64 bit data type, but it\n");
						fprintf(stderr, "   is configured as a %d bit data type.\n", (int) ( sizeof( long long ) * 8 ) );
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


	/*!	\page	gimdoc_checksum	Digests
		\brief	Hashes (md5, sha1, sha256, sha512) of a buffer or of a file, as ascii hex.

		Class: \ref gim_checksum_obj. Header: gim_checksum.h. Status: REVISED 2026-11-27, NOT COMPILED: see SYNC/TODO_GIM.md.

		\section	gimdoc_checksum_what	What it is for

		Gim uses digests to compare files, to check the integrity of a key or of a record, and to name the results of the
		tools. Each algorithm comes in two forms: on a buffer already in memory, and on a file. The result is always the
		ascii hex of the digest, in lower case.

		\section	gimdoc_checksum_example	Example

		Digest of a text, written in a buffer owned by the caller (the thread-safe form):
		\code{.cpp}
		gim_checksum_obj ck;
		char out[ GIM_SHA256_SIZE ];
		char text[] = "abc";
		ck.sha256_buf( out , sizeof( out ) , text , 3 );     // "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
		\endcode

		Digest of a file:
		\code{.cpp}
		char out[ GIM_MD5_SIZE ];
		if ( ck.md5_file_buf( out , sizeof( out ) , (char *) "data.bin" ) == 0 )
			printf( "file not found\n" );
		\endcode

		\section	gimdoc_checksum_methods	The methods, by purpose

		| Purpose | Methods |
		|---------|---------|
		| Hash of a buffer, thread-safe | md5_buf(), sha1_buf(), sha256_buf(), sha512_buf() |
		| Hash of a file, thread-safe | md5_file_buf(), sha1_file_buf(), sha256_file_buf(), sha512_file_buf() |
		| Hash of a buffer, static result | md5(), sha1(), sha256(), sha512() with a buffer |
		| Hash of a file, static result | md5(), sha1(), sha256(), sha512() with a file name |

		\section	gimdoc_checksum_returns	Return values

		| Method | Returns |
		|--------|---------|
		| *_buf() | the number of characters written (32 for md5, 40 for sha1, 64 for sha256, 128 for sha512), 0 on error |
		| md5(), sha1(), sha256(), sha512() | the static ascii digest, or NULL when the file does not exist |

		\section	gimdoc_checksum_threads	Threads

		The methods without _buf return a static buffer shared by all the objects and all the threads: do not use them from
		more than one thread. The *_buf methods write only into the buffer of the caller and are safe to call from several
		threads, when the caller gives each thread its own output buffer.

		\section	gimdoc_checksum_warnings	Warnings and limits

		- The md5/sha1/sha256/sha512 functions on a file load the whole file in memory: big files need memory of the same size.
		- The size macros GIM_SHA1_SIZE (65) and the others include room for the terminating zero and some spare characters.
		- The constructor prints the size of the basic types, and aborts with GIM_ERROR_FATAL on a big-endian machine
		  when HIGHFIRST is not defined (NOT tested on a big-endian machine).
		- NOT verified with test vectors in this revision: see test/gim_checksum_test.cc and SYNC/TODO_GIM.md.
	*/

#endif //_GIM_CHECKSUM_H_
