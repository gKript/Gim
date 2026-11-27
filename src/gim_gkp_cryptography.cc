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

/*!     \file       gim_gkp_cryptography.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include "../include/gim_thread.h"
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



/*! \brief	Key stream of the gkp encryption: SHA-512 in counter mode.
	The key is derived from the password with GIM_CRYPT_KDF_ROUNDS rounds of SHA-512. The Mersenne Twister is not
	used: it is not a cryptographic generator. Each 64-byte block is SHA-512( key || nonce || pass || counter ).
	\warning	NOT VERIFIED with test vectors. The derivation is fast: it is not a password-hashing function
				(see SYNC/CRITTOGRAFIA_PIANO.md).
*/
#define GIM_CRYPT_KDF_ROUNDS	4096

/*! \brief	Derives the 64-byte key from the password: SHA-512 of the password, then GIM_CRYPT_KDF_ROUNDS rounds of SHA-512
			over key || password.
	\param	password	the password (NUL-terminated)
	\param	key			output: 64 bytes
*/
static void gim_crypt_key( const char * password , unsigned char key[ 64 ] ) {
	gim_sha512_obj	sha;
	SHA512_ctx		c;
	unsigned char	buf[ 64 + 256 ];
	int				r;
	size_t			len = strlen( password );
	sha.SHA512_init( &c );
	sha.SHA512_update( &c , (const void *) password , len );
	sha.SHA512_final( &c );
	sha.SHA512_digest( &c , key );
	for ( r = 0 ; r < GIM_CRYPT_KDF_ROUNDS ; r++ ) {
		memcpy( buf , key , 64 );
		memcpy( buf + 64 , password , len );
		sha.SHA512_init( &c );
		sha.SHA512_update( &c , (const void *) buf , 64 + len );
		sha.SHA512_final( &c );
		sha.SHA512_digest( &c , key );
	}
}

/*! \brief	One 64-byte block of the key stream: SHA-512( key || nonce || pass || counter ), with pass and counter as big-endian.
	\param	key			the 64-byte key from gim_crypt_key()
	\param	nonce		the nonce (may be empty: see the warnings of advanced_password_nonce())
	\param	pass		the pass number (0 to iter)
	\param	counter		the block number inside the pass
	\param	out			output: 64 bytes
*/
static void gim_crypt_block( const unsigned char key[ 64 ] , const char * nonce , unsigned int pass , unsigned long long counter , unsigned char out[ 64 ] ) {
	gim_sha512_obj	sha;
	SHA512_ctx		c;
	unsigned char	in[ 64 + 256 + 4 + 8 ];
	int				k;
	size_t			nlen = strlen( nonce );
	memcpy( in , key , 64 );
	memcpy( in + 64 , nonce , nlen );
	for ( k = 0 ; k < 4 ; k++ )
		in[ 64 + nlen + k ] = (unsigned char) ( pass >> ( 24 - 8 * k ) );
	for ( k = 0 ; k < 8 ; k++ )
		in[ 68 + nlen + k ] = (unsigned char) ( counter >> ( 56 - 8 * k ) );
	sha.SHA512_init( &c );
	sha.SHA512_update( &c , (const void *) in , 64 + nlen + 12 );
	sha.SHA512_final( &c );
	sha.SHA512_digest( &c , out );
}

void gim_cryptography_obj::derive_key( unsigned char key[ 64 ] ) const {
	gim_crypt_key( password , key );
}

unsigned int gim_cryptography_obj::iterations( void ) const {
	return (unsigned int) iter;
}

void gim_cryptography_obj::apply_stream( const unsigned char key[ 64 ] , unsigned int iter_n , const char * nonce , unsigned char * buffer , size_t size ) {
	unsigned int	pass , passes = iter_n + 1;
	unsigned char	ks[ 64 ];
	for ( pass = 0 ; pass < passes ; pass++ ) {
		size_t j = 0;
		unsigned long long blk = 0;
		while ( j < size ) {
			size_t k;
			gim_crypt_block( key , nonce , pass , blk , ks );
			for ( k = 0 ; k < 64 && j < size ; k++ , j++ )
				buffer[ j ] ^= ks[ k ];
			blk++;
		}
	}
}

void gim_cryptography_obj::apply_stream_parallel( const unsigned char key[ 64 ] , unsigned int iter_n , const char * nonce , unsigned char * buffer , size_t size , int threads , gim_crypt_progress_fn progress , void * ctx ) {
	size_t					nblk = ( size + 63 ) / 64;
	int						nt = threads;
	const size_t			total = size * ( (size_t) iter_n + 1 );
	std::atomic<size_t>		done( 0 );
	std::vector< gim_thread_obj * > workers;
	if ( nt > (int) nblk )
		nt = (int) nblk;
	if ( nt < 2 || size < ( (size_t) 1 << 20 ) ) {
		apply_stream( key , iter_n , nonce , buffer , size );
		if ( progress != NULL )
			progress( ctx , total , total );
		return;
	}
	std::atomic<size_t> * done_ptr = &done;
	for ( int t = 0 ; t < nt ; t++ ) {
		size_t blk_a = nblk * (size_t) t / (size_t) nt;
		size_t blk_b = nblk * (size_t) ( t + 1 ) / (size_t) nt;
		gim_thread_obj * w = new gim_thread_obj;
		w->start( [ key , iter_n , nonce , buffer , size , blk_a , blk_b , done_ptr ]( gim_thread_obj & ) {
			unsigned char ks[ 64 ];
			for ( unsigned int pass = 0 ; pass < iter_n + 1 ; pass++ )
				for ( size_t blk = blk_a ; blk < blk_b ; blk++ ) {
					size_t s = blk * 64 , e = s + 64 , j;
					if ( e > size )
						e = size;
					gim_crypt_block( key , nonce , pass , (unsigned long long) blk , ks );
					for ( j = s ; j < e ; j++ )
						buffer[ j ] ^= ks[ j - s ];
					done_ptr->fetch_add( e - s );
				}
		} );
		workers.push_back( w );
	}
	/* the thread that started the workers reports the progress until they end (it does not touch the workers' data) */
	if ( progress != NULL ) {
		bool running = true;
		while ( running ) {
			running = false;
			for ( size_t w = 0 ; w < workers.size() ; w++ )
				if ( workers[ w ]->is_running() == __GIM_YES ) {
					running = true;
					break;
				}
			progress( ctx , done.load() , total );
			if ( running )
				std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
		}
	}
	/* deleting a gim_thread_obj waits for its thread to end */
	for ( size_t w = 0 ; w < workers.size() ; w++ )
		delete workers[ w ];
	if ( progress != NULL )
		progress( ctx , total , total );
}

_gim_buffer gim_cryptography_obj::advanced_password( _gim_buffer buffer , _gim_Ulong size ) {
	return advanced_password_nonce( buffer , size , "" );
}

_gim_buffer gim_cryptography_obj::advanced_password_nonce( _gim_buffer buffer , _gim_Ulong size , const char * nonce ) {
	internal_gim->set_state( __GIM_TH_CRYPT );
	_gim_Ulong		j = 0 , blk = 0;
	unsigned int	pass , passes = iter + 1;
	unsigned char	key[ 64 ] , ks[ 64 ];
	calc();
	derive_key( key );
	for ( pass = 0 ; pass < passes ; pass++ ) {
		sprintf( gim_interface->data->item_sou_name , "Step %u/%u" , pass + 1 , passes );
		gim_interface->data->sou_size = size;
		for ( j = 0 , blk = 0 ; j < size ; blk++ ) {
			int k;
			gim_crypt_block( key , nonce , pass , (unsigned long long) blk , ks );
			for ( k = 0 ; k < 64 && j < size ; k++ , j++ )
				*( buffer + j ) ^= ks[ k ];
			gim_interface->data->sou_ratio = (float)( (float)j / (float)size );
			gim_interface->data->sou_position = j;
		}
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
