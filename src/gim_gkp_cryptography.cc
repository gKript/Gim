//
// gim_gkp_cryptography.cc
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
#include <stdlib.h>

gim_cryptography_obj * gim_crypt;


gim_cryptography_obj::gim_cryptography_obj() {
	char message[256];
	IF_EXIST_KEY_IN_CONF( "crypt" , "iterations" ) {
		iter = gim_conf->GetKeyINT( "crypt" , "iterations" );
		sprintf( message , "Iterations setted to %d" , iter );
		gim_error->set( "gim_cryptography_obj::gim_cryptography_obj" , message );
	}
	else {
		gim_error->set( GIM_ERROR_WARNING , "gim_cryptography_obj::gim_cryptography_obj" , "Itarations not found in config file. Setted default 2 iteration" , __GIM_ERROR );
	}
}

_gim_buffer gim_cryptography_obj::simple_password( _gim_buffer buffer , _gim_Ulong size ) {
	_gim_Ulong	j=0;
	_gim_Ulong	c=0,len=0;
	_gim_rand	mt;
	calc();
	mt.seed( seed_main );
	len = (_gim_int8)strlen( password );
	for ( j = 0 ; j < size ; j++ ) {
		*(buffer+j) ^= password[c];
		*(buffer+j) ^= mt.randInt( 255 );
		++c < len ? c : c = 0;
	}
	gim_error->set( "gim_cryptography_obj::simple_password" , "Done" );
	return buffer;
}



_gim_buffer gim_cryptography_obj::advanced_password( _gim_buffer buffer , _gim_Ulong size ) {
	internal_gim->set_state( __GIM_TH_CRYPT );
	_gim_Ulong	y = 0 , j = 0;
	_gim_rand	mt;
	int	rnd[4];
	char message[64];
	calc();
	sprintf( gim_interface->data->item_sou_name , "Step 1/%d" , iter+1 );
	mt.seed( seed_main );
	gim_interface->data->sou_size = size;
	for ( j = 0 ; j < size ; j++ ) {
//		if ( internal_gim->get_state() != __GIM_TH_STOP ) {
			rnd[0] =  mt.randInt( GIM_SHA1_SIZE		- 1 );
			rnd[1] =  mt.randInt( GIM_MD5_SIZE		- 1 );
			rnd[2] =  mt.randInt( GIM_SHA256_SIZE	- 1 );
			rnd[3] =  mt.randInt( GIM_SHA512_SIZE	- 1 );
			*(buffer + j) ^= password_sha1  [ rnd[0] ];
			*(buffer + j) ^= password_md5   [ rnd[1] ];
			*(buffer + j) ^= password_sha256[ rnd[2] ];
			*(buffer + j) ^= password_sha512[ rnd[3] ];
			*(buffer + j) ^= mt.randInt( 255 );
			*(buffer + j) ^= mt.randInt( 255 );
			gim_interface->data->sou_ratio = (float)( (float)j / (float)size );
			gim_interface->data->sou_position = j;
/*		}
		else {
			gim_error->set( "gim_cryptography_obj::advanced_password" , "Stopped by user" );
			internal_gim->set_state( __GIM_TH_STOPPED );
			return NULL;
		}
*/	}
	for ( y = 0 ; y < iter ; y++ ) {
		sprintf( gim_interface->data->item_sou_name , "Step %d/%d" , y+2 , iter+1 );
		for ( j = 0 ; j < size ; j++ ) {
//			if ( internal_gim->get_state() != __GIM_TH_STOP ) {
				*(buffer + j) ^= mt.randInt( 255 );
				gim_interface->data->sou_ratio = (float)( (float)j / (float)size );
				gim_interface->data->sou_position = j;
/*			}
			else {
				gim_error->set( "gim_cryptography_obj::advanced_password" , "Stopped by user" );
				internal_gim->set_state( __GIM_TH_STOPPED );
				return NULL;
			}
*/		}
	}
	gim_error->set( "gim_cryptography_obj::advanced_password" , "Done" );
	internal_gim->set_state( __GIM_TH_IDLE );
	return buffer;
}

_gim_buffer gim_cryptography_obj::dna( _gim_buffer buffer , _gim_Ulong size , char * dna_file_name ) {
	_gim_buffer	dna;
	_gim_Ulong	c=0,j=0;
	_gim_file	dnafile( dna_file_name );
	if ( dnafile.check() == __GIM_EXIST ) {
		dna = dnafile.load();
		if ( dna ) {
			for ( j = 0 ; j < size ; j++ ) {
				*(buffer + j) ^= *(dna + c);
				++c < dnafile.size() ? c : c = 0;
			}
			dnafile.close();
			return buffer;
		}
	}	
	return NULL;
}


_gim_buffer gim_cryptography_obj::advanced_dna( _gim_buffer buffer , _gim_Ulong size , char * dna_file_name ) {
	_gim_buffer	dna;
	_gim_Ulong	j=0;
	_gim_file	dnafile( dna_file_name );
	_gim_rand	mt;
	calc();
	mt.seed( seed_main );
	if ( dnafile.check() == __GIM_EXIST ) {
		dna = dnafile.load();
		if ( dna ) {
			for ( j = 0 ; j < size ; j++ )
				*(buffer + j) ^= *(dna + mt.randInt( dnafile.size() ) );
			dnafile.close();
			return buffer;
		}
	}	
	return NULL;
}


//~ void		gim_cryptography_obj::init_password( gim_password_obj * passobj ) {
	//~ Password = passobj;
//~ }
