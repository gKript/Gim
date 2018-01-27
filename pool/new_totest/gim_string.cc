/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * gim2
 * Copyright (C) AsYntote 2007 <asyntote@gkript.org>
 * 
 * gim2 is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * gim2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with gim2.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include "../include/gim_string.h"


void		gim_string_obj::set			( char * to_set ) {
	gim_error->set( "gim_string_obj::set" , "Setting a new string value" );
	if ( strlen( data->str ) != 0 )
		strcpy( data->str_bkup , data->str );
	__GIM_CLEAR( data->str , 32768 , char );
	strcpy( data->str , to_set );
	data->str_len = strlen( data->str );
	gim_error->set( "gim_string_obj::set" , "Done" );
}


void		gim_string_obj::clean		( void ) {
	__GIM_CLEAR( data->str_bkup , 32768 , char );
	__GIM_CLEAR( data->str_bkup , 32768 , char );
	data->str_len = 0;
}


char *		gim_string_obj::c_str		( void ) {
	if ( strlen( data->str ) != 0 )
		return data->str;
	return NULL;
}


_gim_Uint16 gim_string_obj::size		( void ) {
	return data->str_len;
}


_gim_Uint16 gim_string_obj::length		( void ) {
	return data->str_len;
}


_gim_Uint16 gim_string_obj::capacity	( void ) {
	return 32768;
}


_gim_flag	gim_string_obj::empty		( void ) {
	if ( strlen( data->str ) != 0 )
		return __GIM_NO;
	return __GIM_YES;
}


void		gim_string_obj::cat			( char * to_cat ) {
	if ( strlen( data->str ) != 0 )
		strcat( data->str , to_cat );
}


_gim_flag	gim_string_obj::find		( char * to_find ) {
	if ( strstr( data->str , to_find ) != NULL )
		return __GIM_YES;
	return __GIM_NO;
}


char *		gim_string_obj::left_find	( char to_find ) {
	return NULL;
}


char *		gim_string_obj::left_find	( char * to_find ) {
	return NULL;
}


char *		gim_string_obj::left_find	( gim_string_obj to_find ) {
	return NULL;
}


char *		gim_string_obj::right_find	( char to_find ) {
	return NULL;
}


char *		gim_string_obj::right_find	( char * to_find ) {
	return NULL;
}


char *		gim_string_obj::right_find	( gim_string_obj to_find ) {
	return NULL;
}


_gim_flag	gim_string_obj::equal		( char * to_compare ) {
	
}


_gim_flag	gim_string_obj::equal		( gim_string_obj to_compare ) {
	
}


