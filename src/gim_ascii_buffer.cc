//
//  gim_list.cc
//
//  Gim - Generic Information Manager
//  Version 2.4-1  -  Milestone 2.5
//  Creator - asyntote
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
// Gim is developed with Anjuta version 1.2.4  - 2.32.1.1
//


#include    "../include/gim_base_header.h"
#include    "../include/gim_ascii_buffer.h"
#include    "../include/gim_base_header.h"
#include    "../include/gim_list.h"
#include    "../include/gim_ascii_buffer.h"

_gim_flag  gim_ascii_file_obj::set_dimension( _gim_int32 size ) {
	if ( ( dimension == 0 ) && ( size > 0 ) )  {
		dimension = size;
		chrbuf = (char *)gim_memory->Alloc( sizeof( char ) * dimension , __GIM_ASCII_BUFFER , __GIM_HIDE );
		if ( chrbuf == NULL ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::set_dimension" , "Allocation of a new file list member failed"  , __GIM_ERROR );
			return __GIM_ERROR;
		}
		__GIM_VCLEAR( chrbuf , dimension , char , '\0' );
		return __GIM_OK;
	}
	gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::set_dimension" , "Buffer already defined." , __GIM_ERROR );
	gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::set_dimension" , "Please, use change_dimension method instead" , __GIM_ERROR );
	return __GIM_ERROR;
}


_gim_flag	gim_ascii_file_obj::append( char * to_append ) {
	this->seekf( __GIM_LAST );
	if ( ( dimension > 0 ) && ( chrbuf != NULL ) ) {
		strcat( chrbuf , to_append );
		this->seekf( __GIM_LAST );
		return __GIM_OK;
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::append" , "Buffer not defined" , __GIM_ERROR );
	return __GIM_ERROR;
}


/*_gim_flag	gim_ascii_file_obj::append( const char * to_append ) {
	this->seek( __GIM_LAST );
	if ( ( dimension > 0 ) && ( chrbuf != NULL ) ) {
		strcat( chrbuf , to_append );
		this->seek( __GIM_LAST );
		return __GIM_OK;
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::append" , "Buffer not defined" , __GIM_ERROR );
	return __GIM_ERROR;
}*/


_gim_flag	gim_ascii_file_obj::append( const char * format , ... ) {
	char	message[1024];
	_gim_flag   res;
	
	va_list	VAList;
	va_start( VAList , format );
	vsprintf( message , format , VAList );
	va_end( VAList );
	res = append( message );
	return res;
}


_gim_int32	gim_ascii_file_obj::lenght( void ) {
	_gim_Uint32 pos = 0;
	if ( ( dimension > 0 ) && ( chrbuf != NULL ) ) {
		while ( chrbuf[pos] != '\0' )
			pos++;
		return pos;
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::leght" , "Buffer not defined" , __GIM_ERROR );
	return __GIM_ERROR;
}


_gim_int32	gim_ascii_file_obj::lines( void ) {
	_gim_int32 c = 0 , res = 0;

	if ( ( dimension > 0 ) && ( chrbuf != NULL ) ) {
		while ( chrbuf[c] != '\0' ) {
			if ( chrbuf[c] == '\n' ) 
				res++;
		}
		return res;
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::lines" , "Buffer not defined" , __GIM_ERROR );
	return __GIM_ERROR;
}


_gim_list	*	gim_ascii_file_obj::is_in_buffer( const char * to_find ) {
	char str[512];
	strcpy( str , to_find );
	return ( is_in_buffer( str ) );
}


_gim_list	*	gim_ascii_file_obj::is_in_buffer( char * to_find ) {
	_gim_Uint32 dim_find = 0;
	_gim_Uint32	idx_search	= 0;
	_gim_Uint32	* idx_tmp;
	char * buff	= this->chrbuf;
	gim_error->set( GIM_ERROR_MESSAGE , "gim_ascii_file_obj::is_in_buffer" , "Starting" , __GIM_OK );
	_gim_list * occurrences = new _gim_list;
	if ( ( this->dimension > 0 ) && ( this->chrbuf != NULL ) ) {
		if( ( strlen( to_find ) > 0 ) && ( to_find != NULL ) ) {
			dim_find = strlen( to_find );
			while( ( idx_search + dim_find ) <= dimension ) {
				if( *buff == to_find[ 0 ] ) {
					if( !( strncmp( buff , to_find , dim_find ) ) ) {
                        idx_tmp = (_gim_Uint32 *)gim_memory->Alloc( sizeof( _gim_Uint32 ) , __GIM_ASCII_BUFFER , __GIM_HIDE );
                        *idx_tmp = idx_search;
						occurrences->add_item( idx_tmp );
						buff += dim_find-1;
						idx_search += dim_find-1;
					}
				}
				idx_search++;
				buff++;
			}
			if( !( occurrences->items( ) ) ) {
				gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::is_in_buffer" , "No occurrences found" , __GIM_ERROR );
			}
		}
		else {
			gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::is_in_buffer" , "String to find error" , __GIM_ERROR );
		}
	}
	else {
		gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::is_in_buffer" , "Buffer error" , __GIM_ERROR );
	}
	
	return( occurrences );
}



_gim_flag	gim_ascii_file_obj::in_buffer_subst( _gim_list * occurrence , char * new_part ) {


}



_gim_flag	gim_ascii_file_obj::seekf( _gim_flag position ) {
	if ( ( dimension > 0 ) && ( chrbuf != NULL ) ) {
		switch ( position ) {
			case __GIM_FIRST : {
				index = 0;
				return __GIM_OK;
			}
			case __GIM_LAST : {
				index = this->lenght();
				return __GIM_OK;
			}
			default : {
				gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::seekf" , "Position flag unknown" , __GIM_ERROR );
				return __GIM_ERROR;
			}
		}
	}
}


_gim_flag	gim_ascii_file_obj::seek( _gim_int32 position ) {
	_gim_int32 c = 0;
	if ( position > this->lenght() ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::seek" , "Requested position is beyond the end of buffer" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( ( dimension > 0 ) && ( chrbuf != NULL ) ) {
		while ( ( chrbuf[c] != '\0' ) || ( c == position  ) ) {
			index = c++;
		}
		if ( c == position ) 
			return __GIM_OK;
		else {
			gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::seek" , "Reached the end of buffer before the requested position" , __GIM_ERROR );
			return __GIM_ERROR;
		}
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::seek" , "Buffer not defined" , __GIM_ERROR );
	return __GIM_ERROR;
}


char *	gim_ascii_file_obj::get_buffer( void ) {
	if ( ( dimension > 0 ) && ( chrbuf != NULL ) )
		return chrbuf;
	gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::get_buffer" , "Buffer not defined" , __GIM_ERROR );
	return NULL;
}



