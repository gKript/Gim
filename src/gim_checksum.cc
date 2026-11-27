//
// gim_checksum.cc
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

/*!     \file       gim_checksum.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include	"../include/gim_base_header.h"

#include	"../include/gim_checksum.h"
#include	"../include/gim_file.h"
#include	"../include/gim_md5.h"
#include	"../include/gim_sha1.h"
#include	"../include/gim_sha256.h"
#include	"../include/gim_sha512.h"

/* REVISED 2026-11-27, NOT COMPILED: the *_buf() functions are new and must be built and tested on another PC
   (test/gim_checksum_test.cc). The static-buffer functions are unchanged. chsum() and chsum8() were removed: no caller. */

gim_checksum_obj	* gim_checksum;

/* Lower-case hex of n bytes, with the terminating zero. Returns 2*n, or 0 when out is NULL or too small. */
static size_t gim_checksum_hex( const unsigned char * digest , size_t n , char * out , size_t size ) {
	static const char	hexchars[] = "0123456789abcdef";
	size_t				i;
	if ( out == NULL || size < ( 2 * n + 1 ) )
		return 0;
	for ( i = 0 ; i < n ; i++ ) {
		out[ 2 * i ]     = hexchars[ digest[ i ] >> 4 ];
		out[ 2 * i + 1 ] = hexchars[ digest[ i ] & 0x0f ];
	}
	out[ 2 * n ] = '\0';
	return 2 * n;
}

char *	gim_checksum_obj::md5( char * buffer , _gim_Uint32 size ) {
	char				digest[16];
	static char			hex[33];
	MD5_CTX				c;
	_gim_Uint8 			i;
	gim_md5_obj			md5;
	md5.MD5Init( &c );
	md5.MD5Update( &c , ( unsigned char * ) buffer , size );
	md5.MD5Final( (unsigned char *) digest , &c );
	for( i = 0 ; i < 16 ; i++ )
		snprintf( ( hex + ( 2 * i ) ) , 3 , "%02x" , ( unsigned char ) digest[i] );
	return hex;
}


char *	gim_checksum_obj::md5( char * file_name ) {
	char				digest[16];
	static char			hex[33];
	MD5_CTX				c;
	_gim_Uint8 			i = 0;
	_gim_handler		* hand = NULL;
	gim_md5_obj			md5;
	hand = gim_file_manager->open( file_name , __GIM_FILE_MAPPING , __GIM_READ );
	if ( hand != NULL) {
		md5.MD5Init( &c );
		md5.MD5Update( &c , ( unsigned char * )hand->fm , hand->size );
		md5.MD5Final( (unsigned char *) digest , &c );
		for( i = 0 ; i < 16 ; i++ )
			snprintf( ( hex + ( 2 * i ) ) , 3 , "%02x" , ( unsigned char ) digest[i] );
		gim_file_manager->close( hand );
		return hex;
	}
	gim_error->set( "gim_checksum_obj::md5" , 2 );
	return NULL;
}


char *	gim_checksum_obj::sha1( char * buffer , _gim_Uint32 size ) {
	static char			hex[41];
	gim_sha1_obj		sha1;

	Sha1Digest computed = sha1.Sha1_get( (void *)buffer, size );
	sha1.Sha1Digest_toStr( &computed , hex );

	return hex;
}


char *	gim_checksum_obj::sha1( char * file_name ) {
	static char			hex[41];
	_gim_buffer			buffer = NULL;
	gim_file_obj		file( file_name );
	gim_sha1_obj		sha1;
	if ( file.check() == __GIM_EXIST ) {
		buffer = file.load();
	}
	else {
		gim_error->set( "gim_checksum_obj::sha1" , 2 );
		return NULL;
	}
	Sha1Digest computed = sha1.Sha1_get( (void *)buffer, file.size() );
	sha1.Sha1Digest_toStr( &computed , hex );
	file.close();
	return hex;
}

char *	gim_checksum_obj::sha256( char * file_name ) {
	char				digest[32];
	static char			hex[65];
	SHA256_ctx			c;
	_gim_Uint8			i;
	_gim_buffer			buffer = NULL;
	gim_file_obj		file( file_name );
	gim_sha256_obj		sha256;
	if ( file.check() == __GIM_EXIST ) {
		buffer = file.load();
	}
	else {
		gim_error->set( "gim_checksum_obj::sha256" , 2 );
		return NULL;
	}
	sha256.SHA256_init( &c );
	sha256.SHA256_update( &c , (unsigned char *)buffer , file.size() );
	sha256.SHA256_final( &c , (unsigned char *)digest );
	for( i = 0 ; i < 32 ; i++ )
		snprintf( ( hex + ( 2 * i ) ) , 3 , "%02x" , (unsigned char)digest[i] );
	file.close();
	return hex;
}

char *	gim_checksum_obj::sha256( char * buffer , _gim_Uint32 size ) {
	char				digest[32];
	static char			hex[65];
	SHA256_ctx			c;
	_gim_Uint8			i;
	gim_sha256_obj		sha256;
	sha256.SHA256_init( &c );
	sha256.SHA256_update( &c , ( unsigned char * ) buffer , size );
	sha256.SHA256_final( &c , ( unsigned char * ) digest );
	for( i = 0 ; i < 32 ; i++ )
		snprintf( ( hex + ( 2 * i ) ) , 3 , "%02x" , ( unsigned char ) digest[i] );
	return hex;
}

char *	gim_checksum_obj::sha512( char * file_name ) {
	char				digest[64];
	static char			hex[129];
	SHA512_ctx			c;
	_gim_Uint8			i;
	_gim_buffer			buffer = NULL;
	gim_file_obj		file( file_name );
	gim_sha512_obj		sha512;
	if ( file.check() == __GIM_EXIST ) {
		buffer = file.load();
	}
	else {
		gim_error->set( "gim_checksum_obj::sha512" , 2 );
		return NULL;
	}
	sha512.SHA512_init( &c );
	sha512.SHA512_update( &c , ( const void * ) buffer , file.size() );
	sha512.SHA512_final( &c );
	sha512.SHA512_digest( &c , ( unsigned char * ) digest );
	for( i = 0 ; i < 64 ; i++ )
		snprintf( ( hex + ( 2 * i ) ) , 3 , "%02x" , ( unsigned char ) digest[i] );
	file.close();
	return hex;
}

char *	gim_checksum_obj::sha512( char * buffer , _gim_Uint32 size ) {
	char				digest[64];
	static char			hex[129];
	SHA512_ctx			c;
	_gim_Uint8			i;
	gim_sha512_obj		sha512;
	sha512.SHA512_init( &c );
	sha512.SHA512_update( &c , ( const void * ) buffer , size );
	sha512.SHA512_final( &c );
	sha512.SHA512_digest( &c , ( unsigned char * ) digest );
	for( i = 0 ; i < 64 ; i++ )
		snprintf( ( hex + ( 2 * i ) ) , 3 , "%02x" , ( unsigned char ) digest[i] );
	return hex;
}


/* ---- thread-safe forms: the output goes only into the buffer of the caller ---- */

size_t	gim_checksum_obj::md5_buf( char * out , size_t size , char * buffer , _gim_Uint32 len ) {
	unsigned char		digest[16];
	MD5_CTX				c;
	gim_md5_obj			md5;
	md5.MD5Init( &c );
	md5.MD5Update( &c , ( unsigned char * ) buffer , len );
	md5.MD5Final( digest , &c );
	return gim_checksum_hex( digest , 16 , out , size );
}

size_t	gim_checksum_obj::md5_file_buf( char * out , size_t size , char * file_name ) {
	size_t				n = 0;
	_gim_buffer			buffer = NULL;
	gim_file_obj		file( file_name );
	if ( file.check() != __GIM_EXIST ) {
		gim_error->set( "gim_checksum_obj::md5_file_buf" , 2 );
		return 0;
	}
	buffer = file.load();
	if ( buffer == NULL ) {
		file.close();
		return 0;
	}
	n = md5_buf( out , size , (char *) buffer , (_gim_Uint32) file.size() );
	file.close();
	return n;
}

size_t	gim_checksum_obj::sha1_buf( char * out , size_t size , char * buffer , _gim_Uint32 len ) {
	char				tmp[ 41 ];
	gim_sha1_obj		sha1;
	Sha1Digest			computed = sha1.Sha1_get( (void *) buffer , len );
	sha1.Sha1Digest_toStr( &computed , tmp );
	if ( out == NULL || size < sizeof( tmp ) )
		return 0;
	memcpy( out , tmp , sizeof( tmp ) );
	return sizeof( tmp ) - 1;
}

size_t	gim_checksum_obj::sha1_file_buf( char * out , size_t size , char * file_name ) {
	size_t				n = 0;
	_gim_buffer			buffer = NULL;
	gim_file_obj		file( file_name );
	if ( file.check() != __GIM_EXIST ) {
		gim_error->set( "gim_checksum_obj::sha1_file_buf" , 2 );
		return 0;
	}
	buffer = file.load();
	if ( buffer == NULL ) {
		file.close();
		return 0;
	}
	n = sha1_buf( out , size , (char *) buffer , (_gim_Uint32) file.size() );
	file.close();
	return n;
}

size_t	gim_checksum_obj::sha256_buf( char * out , size_t size , char * buffer , _gim_Uint32 len ) {
	unsigned char		digest[32];
	SHA256_ctx			c;
	gim_sha256_obj		sha256;
	sha256.SHA256_init( &c );
	sha256.SHA256_update( &c , ( unsigned char * ) buffer , len );
	sha256.SHA256_final( &c , digest );
	return gim_checksum_hex( digest , 32 , out , size );
}

size_t	gim_checksum_obj::sha256_file_buf( char * out , size_t size , char * file_name ) {
	size_t				n = 0;
	_gim_buffer			buffer = NULL;
	gim_file_obj		file( file_name );
	if ( file.check() != __GIM_EXIST ) {
		gim_error->set( "gim_checksum_obj::sha256_file_buf" , 2 );
		return 0;
	}
	buffer = file.load();
	if ( buffer == NULL ) {
		file.close();
		return 0;
	}
	n = sha256_buf( out , size , (char *) buffer , (_gim_Uint32) file.size() );
	file.close();
	return n;
}

size_t	gim_checksum_obj::sha512_buf( char * out , size_t size , char * buffer , _gim_Uint32 len ) {
	unsigned char		digest[64];
	SHA512_ctx			c;
	gim_sha512_obj		sha512;
	sha512.SHA512_init( &c );
	sha512.SHA512_update( &c , ( const void * ) buffer , len );
	sha512.SHA512_final( &c );
	sha512.SHA512_digest( &c , digest );
	return gim_checksum_hex( digest , 64 , out , size );
}

size_t	gim_checksum_obj::sha512_file_buf( char * out , size_t size , char * file_name ) {
	size_t				n = 0;
	_gim_buffer			buffer = NULL;
	gim_file_obj		file( file_name );
	if ( file.check() != __GIM_EXIST ) {
		gim_error->set( "gim_checksum_obj::sha512_file_buf" , 2 );
		return 0;
	}
	buffer = file.load();
	if ( buffer == NULL ) {
		file.close();
		return 0;
	}
	n = sha512_buf( out , size , (char *) buffer , (_gim_Uint32) file.size() );
	file.close();
	return n;
}
