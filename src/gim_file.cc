//
// gim_file.cc
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

#include "../include/gim_file.h"

gim_file_obj			* gim_file;
gim_file_manager_obj	* gim_file_manager;
gim_utils_obj			gim_utils;


/*-----------------------------------/
 * gim_file_manager_obj		: BEGIN
 *----------------------------------*/
 
 
void	gim_file_manager_obj::init( void ) {
	gim_file_manager_list * L_init;
	startlist = NULL;
	currentlist = NULL;
	endlist = NULL;
	L_init = ( gim_file_manager_list * ) gim_memory->Alloc ( sizeof ( gim_file_manager_list ) , __GIM_MEM_FMAN_INIT , __GIM_HIDE );
	L_init->hand		= NULL;
	L_init->method		= 0;
	L_init->mode		= 0;

	L_init->next		= NULL;

	startlist		= L_init;
	currentlist		= startlist;
	endlist			= startlist;
	gim_error->set( "gim_file_manager_obj::init" , "Initialization completed succesfully" );
}


void	gim_file_manager_obj::release( void ) {
	gim_error->set( "gim_file_manager_obj::release" , 36 );
	release_item_list();
	gim_memory->Free( startlist );
	startlist = NULL;
	gim_error->set( "gim_file_manager_obj::release" , "Gim file manager is down" );
}



_gim_Uint32	gim_file_manager_obj::release_item_list( void ) {
	gim_file_manager_list *	Next;
	char message[256];
	_gim_Uint32	total = 0;
	gim_error->set( "gim_file_manager_obj::release_item_list" , 16 );
	for ( currentlist = startlist->next ; currentlist != NULL ; currentlist = Next ) {
		Next = currentlist->next;
		switch( currentlist->method ) {
			case __GIM_FILE_DESCRIPTOR : {
				::close( currentlist->hand->fd );
				break;
			}
			case __GIM_FILE_POINTER : {
				fclose( currentlist->hand->fp );
				break;
			}
			case __GIM_FILE_MAPPING : {
				::close( currentlist->hand->fd );
				munmap( currentlist->hand->fm , currentlist->hand->size );
				break;
			}
		}
		sprintf( message , "file [%s %d] open with %s in %s closed" , Lexical.string_trunc ( currentlist->hand->name , 30 ) , currentlist->hand->size , method_str( currentlist->method ) , mode_str( currentlist->mode ) );
		if ( currentlist->hand != NULL ) {
			if ( currentlist->hand->loadp != NULL ) {
				gim_memory->Free( currentlist->hand->loadp );
				gim_error->set( "gim_file_manager_obj::release_item_list" , "Unloaded succesfully" );
			}
			gim_memory->Free( currentlist->hand );
			gim_error->set( "gim_file_manager_obj::release_item_list" , "Handler released succesfully" );
		}
		gim_error->set( "gim_file_manager_obj::release_item_list" , message );
		gim_memory->Free( currentlist );
		total++;
	}
	sprintf( message , "Total element released [%d]" , total );
	gim_error->set( "gim_file_manager_obj::release_item_list" , message );
	currentlist = NULL;
	endlist = NULL;
	gim_error->set( "gim_file_manager_obj::release_item_list" , 34 );
	return total;
}



gim_file_manager_list *	gim_file_manager_obj::new_item( void ) {
	gim_file_manager_list * tmp;
	tmp = ( gim_file_manager_list * ) gim_memory->Alloc ( sizeof ( gim_file_manager_list ) , __GIM_MEM_FMAN_NEW , __GIM_HIDE  );
	tmp->next = NULL;
	tmp->hand = NULL;
	endlist->next = tmp;
	endlist = tmp;
	return tmp;
}


_gim_flag	gim_file_manager_obj::del_item( gim_file_manager_list * to_del ) {
	gim_file_manager_list * prec = startlist;
	if ( startlist == endlist ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_file_manager_obj::del_item" , "No element in list" , __GIM_ERROR );
	}
	else {
		for ( currentlist = startlist->next ; currentlist != NULL ; currentlist = currentlist->next ) {
			if ( currentlist == to_del ) {
				prec->next = currentlist->next;
				if ( currentlist->next == NULL )
					endlist = prec;
				char message[256];
				sprintf( message , "Item -> %s removed from itemlist" , Lexical.string_trunc ( currentlist->hand->name , 40 ) );
				gim_error->set( "gim_file_manager_obj::del_item" , message );
				gim_memory->Free( currentlist->hand );
				gim_memory->Free( currentlist );
				return __GIM_OK;
			}
			prec = currentlist;
		}
		gim_error->set( GIM_ERROR_WARNING , "gim_file_manager_obj::del_item" , "Item NOT found. Maybe closed twice" , __GIM_ERROR );
	}
	return __GIM_NOT_OK;
}


void	gim_file_manager_obj::open_fp( gim_file_manager_list * item , _gim_flag mode ) {
	switch( mode ) {
		case __GIM_READ : {
			item->hand->fp = fopen( item->hand->name , "rb" );
			break;
		}
		case __GIM_WRITE : {
			item->hand->fp = fopen( item->hand->name , "wb" );
			break;
		}
		case __GIM_APPEND : {
			item->hand->fp = fopen( item->hand->name , "ab" );
			break;
		}
		default : {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open_fp" , "Incorrect modality" , __GIM_ERROR );
		}
	}

}



void	gim_file_manager_obj::open_fm( gim_file_manager_list * item ) {
	item->hand->fd = ::open( item->hand->name , O_RDONLY );
	if ( item->hand->fd < 0 )
		gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open_fm" , "I cannot open" , __GIM_ERROR );
	else {
		item->hand->fm = ( char * ) mmap( NULL , item->st.st_size , PROT_READ , MAP_PRIVATE , item->hand->fd , 0 );
		if ( item->hand->fm == NULL )
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open_fm" , "I cannot open" , __GIM_ERROR );
	}	
}





void		gim_file_manager_obj::open_fd( gim_file_manager_list *	item , _gim_flag mode ) {
	switch( mode ) {
		case __GIM_READ : {
			item->hand->fd = ::open( item->hand->name , O_RDONLY );
			break;
		}
		case __GIM_WRITE : {
			item->hand->fd = ::open( item->hand->name , O_WRONLY );
			break;
		}
		case __GIM_APPEND : {
			item->hand->fd = ::open( item->hand->name , O_APPEND );
			break;
		}
		default : {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open_fd" , "Incorrect modality" , __GIM_ERROR );
			break;
		}
	}
}


void	gim_file_manager_obj::close( _gim_handler * hand ) {
	gim_file_manager_list *	item;
	//~ gim_error->set( "gim_file_manager_obj::close" , "Getting item pointer from handler" );
	item = gim_file_manager_obj::item( hand->name );
	if ( item != NULL ) {
		//~ gim_error->set( "gim_file_manager_obj::close" , "Item pointer found" );
		switch( item->method ) {
			case __GIM_FILE_DESCRIPTOR : {
				//~ gim_error->set( "gim_file_manager_obj::close" , "File desctiptor closing" );
				::close( item->hand->fd );
				gim_error->set( "gim_file_manager_obj::close" , "File closed" );
				break;
			}
			case __GIM_FILE_POINTER : {
				//~ gim_error->set( "gim_file_manager_obj::close" , "File pointer closing" );
				fclose( item->hand->fp );
				gim_error->set( "gim_file_manager_obj::close" , "File closed" );
				break;
			}
			case __GIM_FILE_MAPPING : {
				//~ gim_error->set( "gim_file_manager_obj::close" , "File mapping closing" );
				::close( item->hand->fd );
				munmap( item->hand->fm , item->hand->size );
				gim_error->set( "gim_file_manager_obj::close" , "File closed" );
				break;
			}
			default : {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::close" , "Unknown method" , __GIM_ERROR );
				break;
			}
		}
		if ( hand->loadp != NULL ) {
			//~ gim_error->set( "gim_file_manager_obj::close" , "Unloading buffer" );
			gim_memory->Free( item->hand->loadp );
			//~ item->hand->loadp = NULL;
			gim_error->set( "gim_file_manager_obj::close" , "Unloaded succesfully" );
		}
		//~ del_item( item );
		//~ gim_memory->Free( hand );
		//~ gim_error->set( "gim_file_manager_obj::close" , "Backup item handler" );
		//~ _gim_handler	* tmp = item->hand;
		//~ gim_error->set( "gim_file_manager_obj::close" , "Deleting item from list" );
		del_item( item );
		//~ gim_error->set( "gim_file_manager_obj::close" , "Item memory releasing" );
		//~ gim_memory->Free( tmp );

	}
	else
		gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::close" , "item not found. Maybe never opened or close twice" , __GIM_ERROR );
}



void	gim_file_manager_obj::close( const char * name ) {
	gim_file_manager_list *	item;
	gim_error->set( "gim_file_manager_obj::close" , "Getting item pointer from name" );
	item = gim_file_manager_obj::item( name );
	if ( item != NULL ) {
		gim_error->set( "gim_file_manager_obj::close" , "Item pointer found" );
		switch( item->method ) {
			case __GIM_FILE_DESCRIPTOR : {
				gim_error->set( "gim_file_manager_obj::close" , "File desctiptor closing" );
				::close( item->hand->fd );
				gim_error->set( "gim_file_manager_obj::close" , "File closed" );
				break;
			}
			case __GIM_FILE_POINTER : {
				gim_error->set( "gim_file_manager_obj::close" , "File pointer closing" );
				fclose( item->hand->fp );
				gim_error->set( "gim_file_manager_obj::close" , "File closed" );
				break;
			}
			case __GIM_FILE_MAPPING : {
				gim_error->set( "gim_file_manager_obj::close" , "File mapping closing" );
				::close( item->hand->fd );
				munmap( item->hand->fm , item->hand->size );
				gim_error->set( "gim_file_manager_obj::close" , "File closed" );
				break;
			}
			default : {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::close" , "Unknown method" , __GIM_NOT_OK );
				break;
			}
		}
		if ( item->hand->loadp != NULL ) {
			gim_error->set( "gim_file_manager_obj::close" , "Unloading buffer" );
			gim_memory->Free( item->hand->loadp );
			item->hand->loadp = NULL;
			gim_error->set( "gim_file_manager_obj::close" , "Unloaded succesfully" );
		}
		gim_error->set( "gim_file_manager_obj::close" , "Backup item handler" );
		_gim_handler	* tmp = item->hand;
		gim_error->set( "gim_file_manager_obj::close" , "Deleting item from list" );
		del_item( item );
		gim_error->set( "gim_file_manager_obj::close" , "Item memory releasing" );
		gim_memory->Free( tmp );
	}
	else
		gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::close" , "item not found. Maybe never opened or close twice" , __GIM_NOT_OK );
	gim_error->set( "gim_file_manager_obj::close" , "Operatin terminated" );
}



_gim_handler *	gim_file_manager_obj::open( const char * filename , _gim_flag method , _gim_flag mode ) {
	gim_file_manager_list *	item;
	char message[256];
	if ( exist( filename ) == __GIM_NO ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open" , "File not found" , __GIM_NOT_EXIST );
		return NULL;
	}
	if ( mode != __GIM_WRITE ) {
		if ( gim_utils.isregular( filename ) == __GIM_NO ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open" , "This file is not a regular file" , __GIM_NOT_EXIST );
			return NULL;
		}
	}
	sprintf( message , "Opening file [%s] in %s %s..." , Lexical.string_trunc ( filename , 30 ) , method_str( method ) , mode_str( mode ) );
	gim_error->set( "gim_file_manager_obj::open" , message );
	item = new_item();
	item->hand = (_gim_handler *)gim_memory->Alloc( sizeof( _gim_handler ) , __GIM_MEM_FMAN_OPEN , __GIM_HIDE );
	strcpy( item->hand->name , filename );
	if ( mode == __GIM_READ ) {
		stat( item->hand->name , &(item->st) );
		item->hand->size = item->st.st_size;
	}
	item->mode = mode;
	item->method = method;
	switch( method ) {
		case __GIM_FILE_DESCRIPTOR : {
			open_fd( item , mode );
			if ( item->hand->fd == 0 ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open" , "Some problem opening file" , __GIM_NOT_OK );
				del_item( item );
				return NULL;
			}
			break;
		}
		case __GIM_FILE_POINTER : {
			open_fp( item , mode );
			if ( item->hand->fp == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open" , "Some problem opening file" , __GIM_NOT_OK );
				del_item( item );
				return NULL;
			}
			break;
		}
		case __GIM_FILE_MAPPING : {
			if ( mode == __GIM_READ ) {
				open_fm( item );
				if ( item->hand->fm == NULL ) {
					gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open" , "Some problem opening file" , __GIM_NOT_OK );
					del_item( item );
					return NULL;
				}
			}
			else {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open" , "In file mapping method is possible to open a file in __GIM_READ only" , __GIM_NOT_OK );
				return NULL;
			}
			break;
		}
		default : {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::open" , "Incorrect method" , __GIM_NOT_OK );
			return NULL;
		}
	}
	return item->hand;
}


gim_file_manager_list *	gim_file_manager_obj::item( const char * filename ) {
	gim_file_manager_list * prec = startlist;
	if ( startlist == endlist ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_file_manager_obj::item" , "No element in list" , __GIM_NOT_OK );
	}
	else {
		for ( currentlist = startlist->next ; currentlist != NULL ; currentlist = currentlist->next ) {
			if ( ! ( strcmp( filename , currentlist->hand->name ) ) ) {
				return currentlist;
			}
			prec = currentlist;
		}
		char message[256];
		sprintf( message , "[%s] Item NOT found. Maybe already closed" , filename );
		gim_error->set( GIM_ERROR_WARNING , "gim_file_manager_obj::item" , message , __GIM_NOT_OK );
	}
	return NULL;
}


int		gim_file_manager_obj::length ( const char * fname ) {
	FILE * fid = fopen (fname, "r");
	if ( ! fid ) {
//		gim_error->set( GIM_ERROR_WARNING , "gim_file_manager_obj::length" , "File not found" , __GIM_NOT_EXIST );
		return __GIM_ERROR;
	}
	fseek (fid, 0L, SEEK_END);
	int flen = ftell( fid );
	fclose( fid );
	return flen;
}


_gim_flag	gim_file_manager_obj::exist ( const char * fname ) {
	if ( access( fname , F_OK ) == 0 )
		return __GIM_EXIST;
	else
		return __GIM_NOT_EXIST;
}


const char *		gim_file_manager_obj::mode_str( _gim_flag mode ) {
	switch ( mode ) {
		case __GIM_READ : {
			return "read";
			break;
		}
		case __GIM_WRITE : {
			return "write";
			break;
		}
		case __GIM_APPEND : {
			return "append";
			break;
		}
	}
	return NULL;
}


const char *			gim_file_manager_obj::method_str( _gim_flag method ) {
	switch ( method ) {
		case __GIM_FILE_DESCRIPTOR : {
			return "unbuffered";
			break;
		}
		case __GIM_FILE_POINTER : {
			return "buffered";
			break;
		}
		case __GIM_FILE_MAPPING : {
			return "file mapping";
			break;
		}
	}
	return NULL;
}


_gim_buffer		gim_file_manager_obj::load( _gim_handler * hand ) {
	gim_file_manager_list * tmp = NULL;
	tmp = item( hand->name );
	if ( tmp != NULL ) {
		if ( tmp->hand->loadp != NULL ) {
			if ( gim_memory->Check( hand->loadp ) == __GIM_EXIST ) {
				char message[256];
				sprintf( message , "File already loaded [0x%p]" , hand->loadp );
				gim_error->set( GIM_ERROR_WARNING , "gim_file_manager_obj::load" , message , __GIM_NOT_OK );
				return hand->loadp;
			}
			else 
				hand->loadp = NULL;
		}
		if ( tmp->mode == __GIM_READ ) {
			switch ( tmp->method ) {
				case __GIM_FILE_DESCRIPTOR : {
					hand->loadp = (char *)gim_memory->Alloc( hand->size * sizeof( char ) , __GIM_MEM_FMAN_LOAD , __GIM_HIDE );
					read( hand->fd , hand->loadp , hand->size );
					break;
				}
				case __GIM_FILE_POINTER : {
					hand->loadp = (char *)gim_memory->Alloc( hand->size * sizeof( char ) , __GIM_MEM_FMAN_LOAD , __GIM_HIDE );
					unsigned int tot = fread( hand->loadp , 1 , hand->size ,  hand->fp );
					if ( tot != hand->size ) {
						char message[256];
						sprintf( message , "Some problem with fread. Read %d/%d bytes" , tot , hand->size );
						gim_error->set( GIM_ERROR_CRITICAL , "gim_file_manager_obj::load" , message , __GIM_ERROR );
					}						
					break;
				}
				case __GIM_FILE_MAPPING : {
					hand->loadp = (char *)gim_memory->Alloc( hand->size * sizeof( char ) , __GIM_MEM_FMAN_LOAD , __GIM_HIDE );
					memcpy( hand->loadp , hand->fm , hand->size );
					break;
				}
			}
			gim_error->set( "gim_file_manager_obj::load" , "Loaded succesfully" );
		}
		else {
			if ( tmp->mode != __GIM_READ ) {
				gim_error->set( "gim_file_manager_obj::load" , "Load is possible in __GIM_READ only" );
			}
		}
		return hand->loadp;
	}
	return NULL;
}



_gim_flag	gim_file_manager_obj::flush( _gim_handler * hand , _gim_buffer data , int data_len ) {
	if ( data == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::flush" , "The source buffer cannot be NULL" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	gim_file_manager_list * tmp = NULL;
	tmp = item( hand->name );
	if ( tmp->mode == __GIM_WRITE ) {
		switch ( tmp->method ) {
			case __GIM_FILE_DESCRIPTOR : {
				write( hand->fd , data , data_len );
				break;
			}
			case __GIM_FILE_POINTER : {
				fwrite( data , data_len , 1 , hand->fp );
				break;
			}
			case __GIM_FILE_MAPPING : {
				gim_error->set( GIM_ERROR_WARNING , "gim_file_obj::flush" , "Flush is not possible in __GIM_FILE_MAPPING" , __GIM_ERROR );
				return __GIM_NOT_OK;
			}
		}
		gim_error->set( "gim_file_obj::flush" , "Flushed succesfully" );
		return __GIM_OK;
	}
	else 
		if ( tmp->mode != __GIM_WRITE ) {
			gim_error->set( GIM_ERROR_WARNING , "gim_file_obj::flush" , "Flush is possible in __GIM_WRITE only" , __GIM_ERROR );
		}
	return __GIM_NOT_OK;
}


_gim_flag			gim_file_manager_obj::DELETE( const char * fname ) {
	if ( exist( fname ) == __GIM_EXIST ) {
		char message[256];
		remove( fname );
		sprintf( message , "[%s] Deleted !!!" , fname );
		gim_error->set( "gim_file_manager_obj::DELETE" , message );
		return __GIM_OK;
	}
	return __GIM_NOT_OK;
}

/*---------------------------------/
 * gim_file_manager_obj		: END
 *--------------------------------*/


/*---------------------------/
 * gim_file_obj		: BEGIN
 *--------------------------*/


gim_file_obj::gim_file_obj( const char * filename ) {
	if ( gim_file_manager != NULL ) {
		handler = gim_file_manager->open( filename , __GIM_FILE_MAPPING , __GIM_READ );
		if ( handler != NULL ) {
			exist = __GIM_EXIST;
			handler->loadp = NULL;
			method = __GIM_FILE_MAPPING;
			mode = __GIM_READ;
			errors = __GIM_OK;
			gim_error->set( "gim_file_obj::gim_file_obj" , "End of constructor" );
		}
		else {
			errors = __GIM_ERROR;
			exist = __GIM_NOT_EXIST;
			char message[256];
			sprintf( message , "[%s] File not found" , filename );
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::gim_file_obj" , message , __GIM_ERROR );
		}
	}
	else
		gim_error->set( GIM_ERROR_FATAL , "gim_file_obj::gim_file_obj" , "Gim File Manager is down" , __GIM_ERROR );
}

gim_file_obj::gim_file_obj( const char * filename , _gim_flag Method ) {
	if ( gim_file_manager != NULL ) {
		handler = gim_file_manager->open( filename , Method , __GIM_READ );
		if ( handler != NULL ) {
			exist = __GIM_EXIST;
			handler->loadp = NULL;
			method = Method;
			mode = __GIM_READ;
			errors = __GIM_OK;
			gim_error->set( "gim_file_obj::gim_file_obj" , "End of constructor" );
		}
		else {
			errors = __GIM_ERROR;
			exist = __GIM_NOT_EXIST;
			char message[256];
			sprintf( message , "[%s] File not found" , filename );
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::gim_file_obj" , message , __GIM_ERROR );
		}
	}
	else
		gim_error->set( GIM_ERROR_FATAL , "gim_file_obj::gim_file_obj" , "Gim File Manager is down" , __GIM_ERROR );
}

gim_file_obj::gim_file_obj( const char * filename , _gim_flag Method , _gim_flag Mode ) {
	if ( gim_file_manager != NULL ) {
		handler = gim_file_manager->open( filename , Method , Mode );
		if ( handler != NULL ) {
			exist = __GIM_EXIST;
			handler->loadp = NULL;
			method = Method;
			mode = Mode;
			errors = __GIM_OK;
			gim_error->set( "gim_file_obj::gim_file_obj" , "End of constructor" );
		}
		else {
			errors = __GIM_ERROR;
			exist = __GIM_NOT_EXIST;
			char message[256];
			sprintf( message , "[%s] File not found" , filename );
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::gim_file_obj" , message , __GIM_ERROR );
		}
	}
	else
		gim_error->set( GIM_ERROR_FATAL , "gim_file_obj::gim_file_obj" , "Gim File Manager is down" , __GIM_ERROR );
}


char *	gim_file_obj::name( void ) {
	return handler->name;	
}

_gim_Uint32	gim_file_obj::size( void ) {
	return handler->size;
}


_gim_Uint32	gim_file_obj::size( _gim_Uint32 offset ) {
	return ( handler->size - offset );
}


struct stat *	gim_file_obj::get_stat( void ) {
	return NULL;
}


void	gim_file_obj::close( void ) {
	gim_file_manager->close( handler );
}


_gim_handler *	gim_file_obj::get_handler( void ) {
	return handler;
}


_gim_flag	gim_file_obj::check( void ) {
	return exist;
}


_gim_flag	gim_file_obj::error( void ) {
	return errors;
}


_gim_flag	gim_file_obj::type( void ) {
	if ( gim_utils.isdir( this->fname ) == __GIM_YES )
		return __GIM_REGULAR_NODE;
	if ( gim_utils.isregular( this->fname ) == __GIM_YES ) {
		if ( this->handler->size == 0 ) {
			return __GIM_EMPTY_FILE;
		}
		else {
			return __GIM_REGULAR_FILE;
		}
	}
	if ( gim_utils.islink( this->fname ) == __GIM_YES )
		return __GIM_LINK;
	if ( gim_utils.isdevice( this->fname ) == __GIM_YES )
		return __GIM_DEVICE;
	return __GIM_FILE_UNKNOWN;
}


_gim_buffer	gim_file_obj::load( void ) {
	if ( handler->loadp != NULL ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_file_obj::load" , "File already loaded" , __GIM_ERROR );
		return handler->loadp;
	}
	if ( ( mode == __GIM_READ ) && ( exist == __GIM_EXIST ) ) {
		switch ( method ) {
			case __GIM_FILE_DESCRIPTOR : {
				handler->loadp = (char *)gim_memory->Alloc( handler->size * sizeof( char ) , __GIM_MEM_FOBJ_LOAD , __GIM_HIDE );
				if ( handler->loadp == NULL ) return NULL;
				read( handler->fd , handler->loadp , handler->size );
				break;
			}
			case __GIM_FILE_POINTER : {
				handler->loadp = (char *)gim_memory->Alloc( handler->size * sizeof( char ) , __GIM_MEM_FOBJ_LOAD , __GIM_HIDE );
				if ( handler->loadp == NULL ) return NULL;
				fread( handler->loadp , handler->size , 1 , handler->fp );
				break;
			}
			case __GIM_FILE_MAPPING : {
				handler->loadp = (char *)gim_memory->Alloc( handler->size * sizeof( char ) , __GIM_MEM_FOBJ_LOAD , __GIM_HIDE );
				if ( handler->loadp == NULL ) return NULL;
				memcpy( handler->loadp , handler->fm , handler->size );
				break;
			}
		}
		gim_error->set( "gim_file_obj::load" , "Loaded succesfully"  );
	}
	else {
		if ( mode != __GIM_READ ) {
			gim_error->set( GIM_ERROR_WARNING , "gim_file_obj::load" , "Load is possible in __GIM_READ only" , __GIM_ERROR );
		}
		else if  ( exist == __GIM_EXIST ) {
			gim_error->set( GIM_ERROR_WARNING , "gim_file_obj::load" , "The file not exist" , __GIM_ERROR );
		}
	}
	f_buffer = handler->loadp;
	return handler->loadp;
}

_gim_flag	gim_file_obj::flush( char * data , int data_len ) {
	if ( ( mode == __GIM_WRITE ) && ( exist == __GIM_EXIST ) ) {
		switch ( method ) {
			case __GIM_FILE_DESCRIPTOR : {
				write( handler->fd , data , data_len );
				break;
			}
			case __GIM_FILE_POINTER : {
				fwrite( data , data_len , 1 , handler->fp );
				break;
			}
			case __GIM_FILE_MAPPING : {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::flush" , "Flush is not possible in __GIM_FILE_MAPPING" , __GIM_ERROR );
				return __GIM_NOT_OK;
			}
		}
		gim_error->set( "gim_file_obj::flush" , "Flushed succesfully" );
		return __GIM_OK;
	}
	else {
		if ( mode != __GIM_WRITE ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::flush" , "Flush is possible in __GIM_WRITE only" , __GIM_ERROR );
		}
		else if  ( exist == __GIM_EXIST ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::flush" , "The file not exist" , __GIM_ERROR );
		}
	}
	return __GIM_NOT_OK;
}


_gim_flag	gim_file_obj::put( const char * string ) {
	if ( ( mode == __GIM_WRITE ) && ( exist == __GIM_EXIST ) ) {
		switch ( method ) {
			case __GIM_FILE_DESCRIPTOR : {
				write( handler->fd , string , strlen( string ) );
				break;
			}
			case __GIM_FILE_POINTER : {
				fprintf( handler->fp , "%s\n" , string );
				break;
			}
			case __GIM_FILE_MAPPING : {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::puts" , "Puts is not possible in __GIM_FILE_MAPPING" , __GIM_ERROR );
				return __GIM_NOT_OK;
			}
		}
		return __GIM_OK;
	}
	else {
		if ( mode != __GIM_WRITE ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::puts" , "Puts is possible in __GIM_WRITE only" , __GIM_ERROR );
		}
		else if  ( exist == __GIM_EXIST ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::puts" , "The file not exist" , __GIM_ERROR );
		}
	}
	return __GIM_NOT_OK;
}


char *	gim_file_obj::load_gch( void ) {
	unsigned int len , c;
	for ( len = 0 ; getc ( handler->fp ) != EOF ; len++ ) ;
	rewind( handler->fp );
	handler->loadp = (char *)gim_memory->Alloc( len * sizeof( char ) , __GIM_MEM_FOBJ_LOAD_GCH /*, __GIM_HIDE*/ );
	handler->size = len;
	for ( c = 0 ; c < len ; c++ ) {
		*(handler->loadp + c) = getc( handler->fp );
	}
	return handler->loadp;
}



void		gim_file_obj::unload( void ) {
	if ( handler->loadp != NULL ) {
		if ( gim_memory->Free( handler->loadp ) == __GIM_OK ) 
			gim_error->set( "gim_file_obj::unload" , "Unloaded succesfully" );
		else
			gim_error->set( GIM_ERROR_CRITICAL , "gim_file_obj::unload" , "Some memory issue" , __GIM_ERROR );
	}
}


FILE *	gim_file_obj::fp( void ) {
	return handler->fp;
}

int		gim_file_obj::fd( void ) {
	return handler->fd;
}

char *	gim_file_obj::fm( void ) {
	return handler->fm;
}


_gim_buffer	gim_file_obj::buffer( void ) {
	return f_buffer;
}



/*-------------------------/
 * gim_file_obj		: END
 *------------------------*/
