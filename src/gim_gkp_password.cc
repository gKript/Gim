//
// gim_gkp_password.cc
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

#include "../include/gim.h"

//~ gim_password_obj * gim_password;

gim_password_obj::gim_password_obj() {
	int size = 2048;
	volatile _gim_flag	changed = __GIM_NO;
	__GIM_CLEAR( password			, 256 				, char );
	__GIM_CLEAR( secondary			, 256 				, char );
	__GIM_CLEAR( password_md5		, GIM_MD5_SIZE		, char );
	__GIM_CLEAR( password_sha1		, GIM_SHA1_SIZE		, char );
	__GIM_CLEAR( password_sha256	, GIM_SHA256_SIZE	, char );
	__GIM_CLEAR( password_sha512	, GIM_SHA512_SIZE	, char );
	__GIM_CLEAR( secondary_sha512	, GIM_SHA512_SIZE	, char );
	seed_md5	= 0;
	seed_sha1	= 0;
	seed_sha256	= 0;
	seed_sha512	= 0;
	seed_secondary = 0;
	seed_main	= 0;
	password_already_calc = __GIM_NO;
	prime_filled = __GIM_NO;
	if ( gim_conf != NULL ) {
		if ( gim_conf->ExistSection( "prime" ) == __GIM_EXIST ) {
			if ( gim_conf->ExistKey( "prime" , "size" ) == __GIM_EXIST ) {
				size = gim_conf->GetKeyINT( "prime" , "size" );
				if ( ( size < 256 ) || ( size > 2048 ) ) {
					gim_error->set( GIM_ERROR_WARNING , "gim_password_obj::set" , "Prime size out of range. Setted 2048 per default" , __GIM_ERROR );
					size = 2048;
					gim_conf->ChangeKey( "prime" , "size" , 2048 );
					changed = __GIM_YES;
				}
			}
			else {
				size = 2048;
				gim_conf->AddKey( "prime" , "size" , 2048 );
				gim_error->set( "gim_password_obj::set" , "Setted 2048 per default" );
				changed = __GIM_YES;
			}
			if ( gim_conf->ExistKey( "prime" , "filename" ) == __GIM_NOT_EXIST ) {
				gim_conf->AddKey( "prime" , "filename" , "gim_prime.dat" );
				gim_error->set( "gim_password_obj::set" , "Setted [gim_prime.dat] per default" );
				changed = __GIM_YES;
			}
		}
		else {
			size = 2048;
			gim_conf->AddSection( "prime" );
			gim_conf->AddKey( "prime" , "size" , 2048 );
			gim_conf->AddKey( "prime" , "filename" , "prime" );
			gim_error->set( "gim_password_obj::set" , "prime::size setted 2048 per default" );
			changed = __GIM_YES;
		}
		if ( gim_conf->ExistSection( "password" ) == __GIM_EXIST ) {
			if ( gim_conf->ExistKey( "password" , "min_lenght" ) == __GIM_EXIST ) {
				min_lenght = gim_conf->GetKeyINT( "password" , "min_lenght" );
				if ( ( size < 1 ) || ( size > 256 ) ) {
					gim_error->set( GIM_ERROR_WARNING , "gim_password_obj::set" , "Min_lenght size out of range. Setted 6 per default" , __GIM_ERROR );
					min_lenght = 6;
					gim_conf->ChangeKey( "password" , "min_lenght" , 6 );
					changed = __GIM_YES;
				}
			}
			else {
				min_lenght = 6;
				gim_conf->AddKey( "password" , "min_lenght" , 6 );
				gim_error->set( "gim_password_obj::set" , "Password::min_lenght setted 6 per default" );
				changed = __GIM_YES;
			}
		}
		else {
			size = 2048;
			gim_conf->AddSection( "password" );
			gim_conf->AddKey( "password" , "min_lenght" , 6 );
			gim_error->set( "gim_password_obj::set" , "Password::min_lenght setted 6 per default" );
			changed = __GIM_YES;
		}
		if ( changed == __GIM_YES ) {
			gim_home->add_default_comment();
			gim_conf->Write();
		}
	}
	prepare_prime( size );
};



void		gim_password_obj::set( const char * passwd ) {
	if ( passwd == NULL )
		gim_error->set( GIM_ERROR_CRITICAL , "gim_password_obj::set" , "Null password. NO PASSWORD SETTED. Security issue?" , __GIM_ERROR );
	else {
		if ( strlen( passwd ) < min_lenght ) {
			gim_error->set( GIM_ERROR_WARNING , "gim_password_obj::set" , "Password very short. Security issue?" , __GIM_ERROR );
		}
		strcpy( password , passwd );
		gim_error->set( "gim_password_obj::set" , "New password setted" );
		recalc();
	}
}


void		gim_password_obj::set_secondary( const char * passwd ) {
	if ( ! strlen( password ) )
		gim_error->set( GIM_ERROR_CRITICAL , "gim_password_obj::set_secondary" , "Null password. NO PASSWORD SETTED. Security issue?" , __GIM_ERROR );
	else {
		strcpy( secondary , passwd );
		gim_error->set( "gim_password_obj::set_secondary" , "New secondary password setted" );
		recalc();
	}
}


_gim_flag	gim_password_obj::isPrime( _gim_Ulong n ) {
	_gim_Ulong	c;
	if ( n > 3 ) {
		for ( c = 2 ; c < n ; c+=1 ) { 
			if ( ! (n % c) ) 
				return __GIM_NO;
		}
	}
	return __GIM_YES;
}

 
void	gim_password_obj::calc( void ) {
	_gim_Uint8	c = 0 , t=0 ;
	gim_error->set( "gim_password_obj::calc" , "Calculating passworg digest and seed..." );
	if ( strlen( password ) == 0 )
		gim_error->set( GIM_ERROR_CRITICAL , "gim_password_obj::calc" , "No password setted. Security issue?" , __GIM_ERROR );
	if ( password_already_calc == __GIM_NO ) {
//		puts("Calcolo!!!");
		t	= (_gim_int8)strlen( password );
		seed_md5 	= 0 ;
		seed_sha1	= 0 ;
		seed_sha256	= 0 ;
		seed_sha512	= 0 ;
		seed_secondary = 0;
		seed_main	= 1 ;
	//	init - Digest
//		puts("Copio i digest nella struttura");
		strncpy(password_md5    , gim_checksum->md5   ( password			, (unsigned long)t - 1 ) , sizeof(password_md5) );
		strncpy(password_sha1   , gim_checksum->sha1  ( password_md5		, (unsigned long)strlen( password_md5	 ) - 1 ) , sizeof(password_sha1) );
		strncpy(password_sha256 , gim_checksum->sha256( password_sha1	, (unsigned long)strlen( password_sha1   ) - 1 ) , sizeof(password_sha256) );
		strncpy(password_sha512 , gim_checksum->sha512( password_sha256	, (unsigned long)strlen( password_sha256 ) - 1 ) , sizeof(password_sha512) );
//		puts("Fatto... mi occupo della secondary");
		if ( strlen( secondary ) )
			strcpy(secondary_sha512 , gim_checksum->sha512( secondary	, (unsigned long)strlen( secondary ) ) );
	//	init - Sha1		Seed
//		puts("Fatto... un po' di calcoli sui digest");
		for ( c = 0 ; c < GIM_SHA1_SIZE ; c++ )
			seed_sha1		+= password_sha1[c] * Prime[c];
	//	init - Md5		Seed	
		for ( c = 0 ; c < GIM_MD5_SIZE ; c++ )
			seed_md5		+= password_md5[c] * Prime[c];
	//	init - Sha256	Seed
		for ( c = 0 ; c < GIM_SHA256_SIZE ; c++ )
			seed_sha256	+= password_sha256[c] * Prime[c];
	//	init - Sha512	Seed
		for ( c = 0 ; c < GIM_SHA512_SIZE  ; c++ )
			seed_sha512	+= password_sha512[c]  * Prime[c];
		if ( strlen( secondary_sha512 ) ) {
			for ( c = 0 ; c < GIM_SHA512_SIZE  ; c++ )
				seed_secondary	+= secondary_sha512[c]  * Prime[c];
		}
//		puts("Fatto... altri calcoli");
		seed_main *= seed_md5;
		seed_main *= seed_sha1;
		seed_main *= seed_sha256;
		seed_main *= seed_sha512;
		if ( seed_secondary )
			seed_main *= seed_secondary;
//		puts("Fatto... sto uscendo");
		//	NO recalc anymore
		password_already_calc = __GIM_YES;
		char message[256];
		sprintf( message , "Calculated. main_seed is 0x%ld" , seed_main );
		gim_error->set( "gim_password_obj::calc" , message );
	}
}


void	gim_password_obj::recalc( void ) {
	gim_error->set( "gim_password_obj::recalc" , "Recalculating passworg digest and seed..." );
//	puts("__GIM_CLEAR( password_md5...");
	__GIM_CLEAR( password_md5		, GIM_MD5_SIZE , char );
//	puts("__GIM_CLEAR( password_sha1...");
	__GIM_CLEAR( password_sha1		, GIM_SHA1_SIZE , char );
//	puts("__GIM_CLEAR( password_sha256...");
	__GIM_CLEAR( password_sha256	, GIM_SHA256_SIZE , char );
//	puts("__GIM_CLEAR( password_sha512...");
	__GIM_CLEAR( password_sha512	, GIM_SHA512_SIZE , char );
//	puts("__GIM_CLEAR( secondary_sha512...");
	__GIM_CLEAR( secondary_sha512	, GIM_SHA512_SIZE , char );
	seed_md5	= 0;
	seed_sha1	= 0;
	seed_sha256	= 0;
	seed_sha512	= 0;
	seed_secondary = 0;
	seed_main	= 0;
	password_already_calc = __GIM_NO;
//	puts("Chiamo la calc");
	calc();
}


void	gim_password_obj::prepare_prime( int size ) {
	int c,j;
	gim_time_obj	t;
	_gim_handler	* hand;
	char message[256];
	char tmp[256];

	if ( ( size <= 2048 ) && ( size >= 256) ) {	
		__GIM_CLEAR( Prime , size , int );
		if ( env_data != NULL ) {
			sprintf( tmp , "%s%s" , env_data->general_home , gim_conf->GetKeySTR( "prime" , "filename" ) );
			if ( gim_file_manager->exist( tmp ) == __GIM_EXIST ) {
				hand = gim_file_manager->open( tmp , __GIM_FILE_MAPPING , __GIM_READ );
				memcpy( Prime , hand->fm , sizeof( int ) * size );
				gim_file_manager->close( hand );
			}
			else {
				gim_error->set( GIM_ERROR_WARNING , "gim_password_obj::prepare_prime" , "Prime data file not found. Regenarating prime array" , __GIM_ERROR );
				t.start_usec();
				for( j = 1 , c = 0 ; c < size ; j++ ) {
					if ( isPrime( j ) )
						Prime[c++] = j;
				}
				sprintf( message, "Prepared %d prime number in %s" , size , t.get_usec() );
				gim_error->set( "gim_password_obj::prepare_prime" , message );
				hand = gim_file_manager->open( tmp , __GIM_FILE_POINTER , __GIM_WRITE );
				fwrite( Prime , sizeof( int ) * size  , 1 , hand->fp );
				gim_file_manager->close( hand );
			}
			__GIM_CLEAR( message , 256 , char );
			for( c = 0 ; c < 20 ; c++ )
				sprintf( message, "%s%-3d" , message , Prime[c] );
			strcat( message , "..." );
			gim_error->set( "gim_password_obj::prepare_prime" , message );
		}
	}
}
