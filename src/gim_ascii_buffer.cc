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
#include    "../include/gim_list.h"

_gim_flag  gim_ascii_file_obj::set_dimension( _gim_Uint32 size ) {
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


_gim_in_buffer *	gim_ascii_file_obj::is_in_buffer( const char * to_find ) {
	char * str = NULL;
	_gim_in_buffer * result = NULL;
	_gim_Uint32 dim_find = 0;
	
	dim_find = strlen( to_find );
	str = (char *)( gim_memory->Alloc( ++dim_find , __GIM_ASCII_BUFFER , __GIM_HIDE ) );
	strcpy( str , to_find );
	result = is_in_buffer( str );
	gim_memory->Free( str );
	return ( result );
}


_gim_in_buffer *	gim_ascii_file_obj::is_in_buffer( char * to_find ) {
	_gim_Uint32			dim_find = 0;
	_gim_Uint32			idx_search = 0;
	_gim_Uint32	*		idx_tmp = 0;
	char * 				buff = this->chrbuf;
	char *				str_to_find = NULL;
	_gim_in_buffer *	result = NULL;
	_gim_list *			list_occurrence = new _gim_list;
	
	gim_error->set( GIM_ERROR_MESSAGE , "gim_ascii_file_obj::is_in_buffer" , "Starting..." , __GIM_OK );
	if ( ( this->dimension > 0 ) && ( this->chrbuf != NULL ) ) {
		if( ( strlen( to_find ) > 0 ) && ( to_find != NULL ) ) {
			result = (_gim_in_buffer *)gim_memory->Alloc( sizeof( _gim_in_buffer ) , __GIM_ASCII_BUFFER , __GIM_HIDE );
			dim_find = strlen( to_find );
			str_to_find = (char *)gim_memory->Alloc( dim_find, __GIM_ASCII_BUFFER , __GIM_HIDE );
			strcpy( str_to_find , to_find );
			while( ( idx_search + dim_find ) <= this->dimension ) {
				if( *buff == to_find[ 0 ] ) {
					if( !( strncmp( buff , to_find , dim_find ) ) ) {
                        idx_tmp = (_gim_Uint32 *)gim_memory->Alloc( sizeof( _gim_Uint32 ) , __GIM_ASCII_BUFFER , __GIM_HIDE ); //bring out?
                        *idx_tmp = idx_search;
						list_occurrence->add_item( idx_tmp );
						buff += ( dim_find - 1 );
						idx_search += ( dim_find - 1 );
					}
				}
				idx_search++;
				buff++;
			}
			if( !( list_occurrence->items( ) ) ) {
				gim_error->set( GIM_ERROR_WARNING , "gim_ascii_file_obj::is_in_buffer" , "No occurrences found" , __GIM_ERROR );
			}
		}
		else {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::is_in_buffer" , "String to find error" , __GIM_ERROR );
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::is_in_buffer" , "Buffer error" , __GIM_ERROR );
	}
	result->class_id = 3;
	result->pattern = str_to_find;
	result->occurrence = list_occurrence;
	gim_error->set( GIM_ERROR_MESSAGE , "gim_ascii_file_obj::is_in_buffer" , "Done!" , __GIM_OK );
	return( result );
}


_gim_flag	gim_ascii_file_obj::in_buffer_subst( _gim_in_buffer * resulting , char * new_part ) {

	_gim_in_buffer *	subst = NULL;
	_gim_Uint32			new_part_dim = 0;
	_gim_Uint32			old_part_dim = 0;
	char *				str_to_sub = NULL;
	_gim_Uint32			id = 0;
	_gim_Uint32			idx = 0;
	_gim_Uint32			idx_sub = 0;
	char * 				buff = this->chrbuf;
	char *				subst_buff = NULL;
	char *				subst_buff_start = NULL;
	_gim_Uint32			subst_dim = 0;
	_gim_list *			list_tmp = new _gim_list;
	_gim_Uint32 *		tmp;
	_gim_Uint32	*		offset = 0;
	_gim_Uint32			it_num;
	
	gim_error->set( GIM_ERROR_MESSAGE , "gim_ascii_file_obj::in_buffer_subst" , "Starting..." , __GIM_OK );
	if ( ( this->dimension > 0 ) && ( this->chrbuf != NULL ) ) {
		if( resulting != NULL ) {
			if( ( new_part !=NULL ) && ( strlen( new_part ) ) )  {
				
				new_part_dim = strlen( new_part );
				old_part_dim = strlen( resulting->pattern );
				subst_dim = ( this->dimension + ( resulting->occurrence->items( ) * new_part_dim ) - ( resulting->occurrence->items( ) * old_part_dim ) );
				subst_buff = (char *)gim_memory->Alloc( ( subst_dim * sizeof( char ) ), __GIM_ASCII_BUFFER , __GIM_HIDE );
				subst_buff_start = subst_buff;
				subst = (_gim_in_buffer *)gim_memory->Alloc( sizeof( _gim_in_buffer ) , __GIM_ASCII_BUFFER , __GIM_HIDE );
				str_to_sub = (char *)gim_memory->Alloc( ( new_part_dim + 1 ), __GIM_ASCII_BUFFER , __GIM_HIDE );
				strcpy( str_to_sub , new_part );
				tmp = (_gim_Uint32 *)gim_memory->Alloc( sizeof( _gim_Uint32 ) , __GIM_ASCII_BUFFER , __GIM_HIDE ); //bring out?
				
				resulting->occurrence->rewind( );
				it_num = resulting->occurrence->items( );
				for( id = 1 ; id <= it_num ; id++ ) {
					offset = (_gim_Uint32 *)resulting->occurrence->get_item( id );
					while( idx < *offset ) {
						*( subst_buff++ ) = *( buff++ );
						idx++;
						idx_sub++;
					}
					*tmp = idx_sub;
					list_tmp->add_item( tmp );
					strncpy( subst_buff , new_part , new_part_dim );	
					idx += old_part_dim;
					idx_sub += new_part_dim;
					buff += old_part_dim;
					subst_buff += new_part_dim;
				}
				while( idx < this->dimension ) {
					*( subst_buff++ ) = *( buff++ );
					idx++;
				}
			}
			else {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::in_buffer_subst" , "String to substitute error" , __GIM_ERROR );
				return( __GIM_NOT_OK );
			}
		}
		else {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::in_buffer_subst" , "Occurrence data error" , __GIM_ERROR );
			return( __GIM_NOT_OK );
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::in_buffer_subst" , "Buffer error" , __GIM_ERROR );
		return( __GIM_NOT_OK );
	}
	//gim_memory->Unlock_and_free ( chrbuf ); );
	this->chrbuf = subst_buff_start;
	subst->class_id = 4;
	subst->pattern = str_to_sub;
	subst->occurrence = list_tmp;
	//gim_memory->Unlock_and_free ( resulting );
	*resulting = *subst;
	gim_error->set( GIM_ERROR_MESSAGE , "gim_ascii_file_obj::in_buffer_subst" , "Done!" , __GIM_OK );
	return( __GIM_OK );
}


char *	gim_ascii_file_obj::load( char * filename , char * new_buffer , _gim_flag overwrite ) {
	FILE *			fp = NULL;
	_gim_Uint32		flen = 0;
	_gim_Uint32		idx = 0;
	_gim_int32		character = 0;
	char *			buffer = NULL;
	
	if( ( fp = fopen( filename , "r" ) ) == NULL ){
		gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::load" , "Opening file error" , __GIM_ERROR );
		return( NULL );
	}
	fseek( fp , 0L , SEEK_END );
	flen = (_gim_Uint32)ftell( fp );
	fseek( fp , 0L , SEEK_SET );
	printf("Load: File len = \"%d\"\n",flen);
	if( !flen ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::load" , "File is empty" , __GIM_ERROR );
		return( NULL );
	}
	if( new_buffer == NULL ) {
		if( this->dimension == 0 ) {
			this->set_dimension( flen );	//ripristinare
			//this->set_dimension( 1024 );
			printf("Load: Dimension len = \"%d\"\n",this->dimension);
		}
		else {
			if( this->lenght( ) > 0 ) {
				if( overwrite == 0 ) {	//to define
					printf("Load: internal buffer from user lenght = \"%d\"\n", this->lenght( ) );
					gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::load" , "Internal buffer not empty, overwrite not permitted" , __GIM_ERROR );
					return( NULL );
				}
				else {
					if( this->dimension < flen ) {
						//TODO: here resize internal buffer, when ::Realloc() will be written!
						gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::load" , "Internal buffer re-allocation not yet implemented" , __GIM_ERROR );
						return( NULL );
					}
				}
			}
		}
		buffer = this->chrbuf;
	}
	else {
		if( strlen( new_buffer ) < flen ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::load" , "Buffer too small" , __GIM_ERROR );
			return( NULL );
		}
		printf("Load: New_Buffer len = \"%d\"\n",strlen( new_buffer ));
		buffer = new_buffer;
	}
	for( idx = 0; idx < flen ; idx++ ) {
		character = fgetc( fp );
		if( character == EOF ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_ascii_file_obj::load" , "Reading file error" , __GIM_ERROR );
			fclose( fp );
			return( NULL );
		}
		*( buffer + idx ) = (char)character;
		printf(" -%c- ", character);
		printf(" +%c+ ", *(buffer+idx));
	}
	puts("\nLoad: ok");
	printf( "Load: Buffer = \"%s\"\n" , buffer );
	printf("Load: Buffer pointer = \"%p\"\n", buffer );
	fclose( fp );
	return( buffer );
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



