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

	
#include	"../include/gim_base_header.h"

#include	"../include/gim_checksum.h"
#include	"../include/gim_file.h"
#include	"../include/gim_md5.h"
#include	"../include/gim_sha1.h"
#include	"../include/gim_sha256.h"
#include	"../include/gim_sha512.h"

gim_checksum_obj	* gim_checksum;

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
	char				digest[20];
	static char			hex[41];
	SHA1_CTX			c;
	gim_sha1_obj		sha1;
	_gim_Uint8			i;
	sha1.SHA1Init( &c );
	sha1.SHA1Update( &c , ( unsigned char * )buffer , size );
	sha1.SHA1Final( ( unsigned char * )digest , &c );
	for( i = 0 ; i < 20 ; i++ )
		snprintf( ( hex + ( 2 * i ) ) , 3 , "%02x" , ( unsigned char ) digest[i] );
	return hex;
}


char *	gim_checksum_obj::sha1( char * file_name ) {
	char				digest[20];
	static char			hex[41];
	SHA1_CTX			c;
	_gim_Uint8			i;
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
	sha1.SHA1Init( &c );
	sha1.SHA1Update( &c , ( unsigned char * ) buffer , file.size() );
	sha1.SHA1Final( ( unsigned char * ) digest , &c );
	for( i = 0 ; i < 20 ; i++ )
		snprintf( ( hex + ( 2 * i ) ) , 3 , "%02x" , (unsigned char)digest[i] );
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
