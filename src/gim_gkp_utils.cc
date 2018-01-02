/***************************************************************************
 *            GKP_utils.cc
 *
 *  Thu May 11 19:34:45 2006
 *  Copyright  2006  AsYntote
 *  asyntote@gkript.org
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "../include/gim.h"

const char *	gim_utils_obj::dir_name_check( const char * dname ) {
	_gim_int16	len;
	static char	* res = NULL;
	len = strlen( dname );
	if ( dname[ len - 1 ] != '/' ) {
		res = strdup( dname );
		strcat( res , "/" );
		return res;
	}
	return dname;
}

char *	gim_utils_obj::del_path( char * str ) {
	for ( char * s2 = str ; *s2 != '\0' ; s2++ )
		if ( *s2 == '/' )
			str = s2;
	return ++str;
}


char *	gim_utils_obj::str_subtraction( char * main_str , char * to_subtract ) {
	int		c;
	int		l  = strlen( main_str );
	int		l1 = strlen( to_subtract );
	char	message[1024];
//	printf( "M:%s@(%d)  -  S:%s@(%d)\n" , main_str , strlen( main_str ) , to_subtract , strlen( to_subtract ) );
	if ( strlen( main_str ) <= strlen( to_subtract ) ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_utils_obj::str_subtraction" , "Main string cannot be <= the subtraction string" , __GIM_ERROR );
		return NULL;
	}
	for ( c = 0 ; c <= l1 ; c++ ) {
		if ( *(main_str+c)  != *(to_subtract+c) ) {
//			puts( main_str+c );
			return main_str+c;
		}
	}
	return NULL;
}

char *	gim_utils_obj::keep_path( char * str ) {
	int c;
	char *str2 = strdup( str );
	int l = strlen( str2 );
	for ( c = l-1 ; c > 0 ; c-- ) {
		if ( *(str2 + c)  == '/' ) {
			*(str2 + c + 1) = '\0';
			return str2;
		}
	}
	return str2;
}


char *	gim_utils_obj::pwd( void ) {
	static char curdir[1024];
	getcwd( curdir , sizeof( curdir ) );
	return curdir;
}

_gim_flag gim_utils_obj::isdir( const char * d ) {
	struct stat buf;
	if ( stat( d , &buf ) != 0 ) {
		gim_error->set( "gim_utils_obj::isdir" , 24 );
		return __GIM_ERROR;
	}
	if ( S_ISDIR( buf.st_mode ) ) {
		//~ gim_error->set( "gim_utils_obj::isdir" , 25 );
		return __GIM_YES;
	}
	else {
		//~ gim_error->set( "gim_utils_obj::isdir" , 26 );
		return __GIM_NO;
	}
}


_gim_flag gim_utils_obj::isregular( const char * d ) {
	struct stat buf;
	char message[2048];
	if (lstat( d , &buf ) != 0) {
		sprintf( message , "Some problem with stat/lstat : %s" , d );
		gim_error->set( GIM_ERROR_CRITICAL , "gim_utils_obj::isregular" , message , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( S_ISREG( buf.st_mode ) ) {
		//~ gim_error->set( "gim_utils_obj::isregular" , 27 );
		return __GIM_YES;
	}
	else {
		//~ gim_error->set( "gim_utils_obj::isregular" , 28 );
		return __GIM_NO;
	}
}


_gim_flag gim_utils_obj::islink( const char * d ) {
	struct stat buf;
	if ( lstat( d , &buf ) != 0) {
		gim_error->set( "gim_utils_obj::islink" , 24 );
		return __GIM_ERROR;
	}
	if ( S_ISLNK(buf.st_mode) ) {
		//~ gim_error->set( "gim_utils_obj::islink" , 29 );
		return __GIM_YES;
	}
	else {
		//~ gim_error->set( "gim_utils_obj::islink" , 30 );
		return __GIM_NO;
	}
}


_gim_flag gim_utils_obj::isdevice( const char * d ) {
	struct stat buf;
	if ( lstat( d , &buf ) != 0) {
		gim_error->set( "gim_utils_obj::isdevice" , 24 );
		return __GIM_ERROR;
	}
	if ( S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode) ) {
		//~ gim_error->set( "gim_utils_obj::isdevice" , 31 );
		return __GIM_YES;
	}
	else {
		//~ gim_error->set( "gim_utils_obj::isdevice" , 32 );
		return __GIM_NO;
	}
}
