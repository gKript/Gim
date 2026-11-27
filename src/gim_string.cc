//
//  gim_string.cc
//
//  Gim - Generic Information Manager
//  Version 3.0-0  -  Milestone 3.0
//
//  [http://www.gkript.org]
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
// Gim is developed with Anjuta version 1.2.4  - 3.18.2
//

/* REWRITTEN 2026-11-27. NOT COMPILED: verify with test/gim_string_test.cc before relying on it.
   Memory: the buffer comes from gim_memory. A new buffer is unlocked at once: the default lock of the
   configuration is LOCKED, and gim_memory::Free() refuses to free a locked buffer allocated by the caller
   OTHER, so without the unlock the buffer would never be freed. */

/*!     \file       gim_string.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include "../include/gim_string.h"


/*--- construction, copy, destruction ---*/

gim_string_obj::gim_string_obj( void ) : str( NULL ) , len( 0 ) , cap( 0 ) {
}


gim_string_obj::gim_string_obj( const char * to_set ) : str( NULL ) , len( 0 ) , cap( 0 ) {
	if ( to_set != NULL )
		append_n( to_set , (_gim_Uint32) strlen( to_set ) );
}


gim_string_obj::gim_string_obj( const gim_string_obj & other ) : str( NULL ) , len( 0 ) , cap( 0 ) {
	append_n( other.text() , other.len );
}


gim_string_obj::~gim_string_obj( void ) {
	if ( str != NULL && gim_memory != NULL )
		gim_memory->Free( str );
	str = NULL;
	len = 0;
	cap = 0;
}


gim_string_obj & gim_string_obj::operator=( const gim_string_obj & other ) {
	if ( this != &other ) {
		gim_string_obj tmp( other );		// copy first: other may be read while this one changes
		set( tmp.text() );
	}
	return *this;
}


gim_string_obj & gim_string_obj::operator=( const char * to_set ) {
	set( to_set );
	return *this;
}


_gim_flag gim_string_obj::operator==( const gim_string_obj & other ) const {
	return ( strcmp( text() , other.text() ) == 0 ) ? __GIM_YES : __GIM_NO;
}


_gim_flag gim_string_obj::operator!=( const gim_string_obj & other ) const {
	return ( strcmp( text() , other.text() ) != 0 ) ? __GIM_YES : __GIM_NO;
}


/*--- private helpers ---*/

const char * gim_string_obj::text( void ) const {
	return ( str != NULL ) ? str : "";
}


bool gim_string_obj::own( const char * t ) const {
	return ( str != NULL ) && ( t >= str ) && ( t < str + cap );
}


_gim_flag gim_string_obj::grow( _gim_Uint32 need ) {
	if ( need + 1 <= cap )
		return __GIM_OK;
	if ( gim_memory == NULL )
		return __GIM_NOT_OK;
	_gim_Uint32 ncap = ( cap > 0 ) ? cap : 16;
	while ( ncap < need + 1 ) {
		if ( ncap > 0x7FFFFFFFu )
			return __GIM_NOT_OK;
		ncap *= 2;
	}
	char * nstr = (char *) gim_memory->Alloc( ncap , __GIM_MEM_OTHER , __GIM_HIDE );
	if ( nstr == NULL )
		return __GIM_NOT_OK;
	gim_memory->Unlock( nstr );			// see the note at the top of this file
	if ( str != NULL ) {
		memcpy( nstr , str , len + 1 );
		gim_memory->Free( str );
	}
	else
		nstr[ 0 ] = '\0';
	str = nstr;
	cap = ncap;
	return __GIM_OK;
}


_gim_flag gim_string_obj::append_n( const char * t , _gim_Uint32 n ) {
	if ( t == NULL || n == 0 )
		return __GIM_OK;
	if ( own( t ) ) {				// t is inside our buffer: it may move when the buffer grows
		gim_string_obj tmp;
		if ( tmp.append_n( t , n ) != __GIM_OK )
			return __GIM_NOT_OK;
		return append_n( tmp.text() , n );
	}
	if ( grow( len + n ) != __GIM_OK )
		return __GIM_NOT_OK;
	memcpy( str + len , t , n );
	len += n;
	str[ len ] = '\0';
	return __GIM_OK;
}


/*--- set, clean, read the size ---*/

void gim_string_obj::set( const char * to_set ) {
	if ( to_set == NULL )
		to_set = "";
	if ( own( to_set ) ) {			// the new text comes from this text: copy it first
		gim_string_obj tmp( to_set );
		set( tmp.text() );
		return;
	}
	_gim_Uint32 n = (_gim_Uint32) strlen( to_set );
	if ( grow( n ) != __GIM_OK )
		return;						// no memory: the text is left as it was
	memcpy( str , to_set , n );
	str[ n ] = '\0';
	len = n;
}


void gim_string_obj::clean( void ) {
	len = 0;
	if ( str != NULL )
		str[ 0 ] = '\0';
}


char * gim_string_obj::c_str( void ) {
	if ( str == NULL ) {
		if ( grow( 0 ) != __GIM_OK )
			return (char *) "";
	}
	return str;
}


_gim_Uint32 gim_string_obj::size( void ) {
	return len;
}


_gim_Uint32 gim_string_obj::length( void ) {
	return len;
}


_gim_Uint32 gim_string_obj::capacity( void ) {
	return ( cap > 0 ) ? cap - 1 : 0;
}


_gim_flag gim_string_obj::reserve( _gim_Uint32 n ) {
	return grow( n );
}


_gim_flag gim_string_obj::empty( void ) {
	return ( len == 0 ) ? __GIM_YES : __GIM_NO;
}


/*--- append and format ---*/

void gim_string_obj::cat( const char * to_cat ) {
	if ( to_cat == NULL )
		return;
	append_n( to_cat , (_gim_Uint32) strlen( to_cat ) );
}


void gim_string_obj::cat_format( const char * fmt , ... ) {
	if ( fmt == NULL )
		return;
	va_list ap;
	va_start( ap , fmt );
	int n = vsnprintf( NULL , 0 , fmt , ap );
	va_end( ap );
	if ( n < 0 )
		return;
	if ( grow( len + (_gim_Uint32) n ) != __GIM_OK )
		return;
	va_start( ap , fmt );
	vsnprintf( str + len , (size_t) n + 1 , fmt , ap );
	va_end( ap );
	len += (_gim_Uint32) n;
}


void gim_string_obj::set_format( const char * fmt , ... ) {
	if ( fmt == NULL )
		return;
	va_list ap;
	va_start( ap , fmt );
	int n = vsnprintf( NULL , 0 , fmt , ap );
	va_end( ap );
	if ( n < 0 )
		return;
	gim_string_obj tmp;
	if ( tmp.grow( (_gim_Uint32) n ) != __GIM_OK )
		return;
	va_start( ap , fmt );
	vsnprintf( tmp.str , (size_t) n + 1 , fmt , ap );
	va_end( ap );
	tmp.len = (_gim_Uint32) n;
	set( tmp.text() );
}


void gim_string_obj::set_int( _gim_Llong v ) {
	char b[ 32 ];
	snprintf( b , sizeof( b ) , "%lld" , (long long) v );
	set( b );
}


void gim_string_obj::set_float( double v ) {
	char b[ 64 ];
	snprintf( b , sizeof( b ) , "%g" , v );
	set( b );
}


_gim_Llong gim_string_obj::to_int( void ) {
	return (_gim_Llong) strtoll( text() , NULL , 10 );
}


double gim_string_obj::to_float( void ) {
	return strtod( text() , NULL );
}


/*--- characters, insert, erase, parts ---*/

char gim_string_obj::at( _gim_Uint32 pos ) {
	if ( pos < len )
		return str[ pos ];
	return '\0';
}


void gim_string_obj::insert( _gim_Uint32 pos , const char * t ) {
	if ( t == NULL )
		return;
	if ( pos > len )
		pos = len;
	gim_string_obj tmp( t );		// copy first: t may point inside this text
	_gim_Uint32 n = tmp.len;
	if ( n == 0 )
		return;
	if ( grow( len + n ) != __GIM_OK )
		return;
	memmove( str + pos + n , str + pos , len - pos + 1 );
	memcpy( str + pos , tmp.text() , n );
	len += n;
}


void gim_string_obj::erase( _gim_Uint32 pos , _gim_Uint32 count ) {
	if ( pos >= len || count == 0 )
		return;
	if ( count > len - pos )
		count = len - pos;
	memmove( str + pos , str + pos + count , len - pos - count + 1 );
	len -= count;
}


gim_string_obj gim_string_obj::substr( _gim_Uint32 pos , _gim_Uint32 count ) {
	gim_string_obj r;
	if ( pos >= len )
		return r;
	if ( count > len - pos )
		count = len - pos;
	r.append_n( str + pos , count );
	return r;
}


static bool gim_string_is_blank( char c ) {
	return ( c == ' ' ) || ( c == '\t' ) || ( c == '\r' ) || ( c == '\n' ) || ( c == '\v' ) || ( c == '\f' );
}


void gim_string_obj::trim_left( void ) {
	_gim_Uint32 i = 0;
	while ( ( i < len ) && gim_string_is_blank( str[ i ] ) )
		i++;
	if ( i == 0 )
		return;
	memmove( str , str + i , len - i + 1 );
	len -= i;
}


void gim_string_obj::trim_right( void ) {
	while ( ( len > 0 ) && gim_string_is_blank( str[ len - 1 ] ) )
		len--;
	if ( str != NULL )
		str[ len ] = '\0';
}


void gim_string_obj::trim( void ) {
	trim_right();
	trim_left();
}


void gim_string_obj::upper( void ) {
	for ( _gim_Uint32 i = 0 ; i < len ; i++ )
		str[ i ] = (char) toupper( (unsigned char) str[ i ] );
}


void gim_string_obj::lower( void ) {
	for ( _gim_Uint32 i = 0 ; i < len ; i++ )
		str[ i ] = (char) tolower( (unsigned char) str[ i ] );
}


void gim_string_obj::replace( const char * old_text , const char * new_text ) {
	if ( ( old_text == NULL ) || ( old_text[ 0 ] == '\0' ) || ( str == NULL ) )
		return;
	if ( new_text == NULL )
		new_text = "";
	_gim_Uint32 n = (_gim_Uint32) strlen( old_text );
	_gim_Uint32 m = (_gim_Uint32) strlen( new_text );
	gim_string_obj result;
	_gim_Uint32 i = 0;
	while ( i < len ) {
		if ( ( n <= len - i ) && ( memcmp( str + i , old_text , n ) == 0 ) ) {
			result.append_n( new_text , m );
			i += n;
		}
		else {
			result.append_n( str + i , 1 );
			i++;
		}
	}
	*this = result;
}


_gim_Uint32 gim_string_obj::fields( char sep ) {
	if ( len == 0 )
		return 0;
	_gim_Uint32 count = 1;
	for ( _gim_Uint32 i = 0 ; i < len ; i++ )
		if ( str[ i ] == sep )
			count++;
	return count;
}


_gim_flag gim_string_obj::field( char sep , _gim_Uint32 n , gim_string_obj & out ) {
	if ( len == 0 ) {
		if ( n != 0 )
			return __GIM_NO;
		out.set( "" );
		return __GIM_YES;
	}
	_gim_Uint32 start = 0;
	_gim_Uint32 index = 0;
	for ( _gim_Uint32 i = 0 ; i <= len ; i++ ) {
		if ( ( i == len ) || ( str[ i ] == sep ) ) {
			if ( index == n ) {
				gim_string_obj tmp;
				tmp.append_n( str + start , i - start );	// tmp first: out may be this object
				out = tmp;
				return __GIM_YES;
			}
			index++;
			start = i + 1;
		}
	}
	return __GIM_NO;
}


/*--- prefixes, search ---*/

_gim_flag gim_string_obj::starts_with( const char * prefix ) {
	if ( prefix == NULL )
		return __GIM_NO;
	_gim_Uint32 n = (_gim_Uint32) strlen( prefix );
	if ( n > len )
		return __GIM_NO;
	return ( memcmp( text() , prefix , n ) == 0 ) ? __GIM_YES : __GIM_NO;
}


_gim_flag gim_string_obj::ends_with( const char * suffix ) {
	if ( suffix == NULL )
		return __GIM_NO;
	_gim_Uint32 n = (_gim_Uint32) strlen( suffix );
	if ( n > len )
		return __GIM_NO;
	return ( memcmp( text() + ( len - n ) , suffix , n ) == 0 ) ? __GIM_YES : __GIM_NO;
}


_gim_flag gim_string_obj::find( const char * to_find ) {
	return ( pos_of( to_find ) >= 0 ) ? __GIM_YES : __GIM_NO;
}


int gim_string_obj::pos_of( const char * to_find ) {
	if ( to_find == NULL )
		return -1;
	_gim_Uint32 n = (_gim_Uint32) strlen( to_find );
	if ( n == 0 )
		return 0;
	if ( n > len )
		return -1;
	for ( _gim_Uint32 i = 0 ; i + n <= len ; i++ )
		if ( memcmp( str + i , to_find , n ) == 0 )
			return (int) i;
	return -1;
}


int gim_string_obj::rpos_of( const char * to_find ) {
	if ( to_find == NULL )
		return -1;
	_gim_Uint32 n = (_gim_Uint32) strlen( to_find );
	if ( n == 0 )
		return (int) len;
	if ( n > len )
		return -1;
	_gim_Uint32 i = len - n + 1;
	while ( i > 0 ) {
		i--;
		if ( memcmp( str + i , to_find , n ) == 0 )
			return (int) i;
	}
	return -1;
}


char * gim_string_obj::left_find( char to_find ) {
	if ( str == NULL )
		return NULL;
	return strchr( str , to_find );
}


char * gim_string_obj::left_find( const char * to_find ) {
	if ( ( str == NULL ) || ( to_find == NULL ) )
		return NULL;
	return strstr( str , to_find );
}


char * gim_string_obj::left_find( const gim_string_obj & to_find ) {
	return left_find( to_find.text() );
}


char * gim_string_obj::right_find( char to_find ) {
	if ( str == NULL )
		return NULL;
	return strrchr( str , to_find );
}


char * gim_string_obj::right_find( const char * to_find ) {
	int p = rpos_of( to_find );
	if ( p < 0 )
		return NULL;
	return str + p;
}


char * gim_string_obj::right_find( const gim_string_obj & to_find ) {
	return right_find( to_find.text() );
}


/*--- comparison ---*/

int gim_string_obj::compare( const char * to_compare ) {
	if ( to_compare == NULL )
		to_compare = "";
	return strcmp( text() , to_compare );
}


int gim_string_obj::compare( const gim_string_obj & to_compare ) const {
	return strcmp( text() , to_compare.text() );
}


_gim_flag gim_string_obj::equal( const char * to_compare ) {
	return ( compare( to_compare ) == 0 ) ? __GIM_YES : __GIM_NO;
}


_gim_flag gim_string_obj::equal( const gim_string_obj & to_compare ) const {
	return ( compare( to_compare ) == 0 ) ? __GIM_YES : __GIM_NO;
}
