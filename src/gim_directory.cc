//
// gim_directory.cc
//
// Gim  -  Generic Information Manager static library
// Version 2.5-0
// AsYntote
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

/*!	\file		gim_directory.cc
	\version	2.5-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/

#include "../include/gim_base_header.h"
#include "../include/gim_directory.h"
#include "../include/gim_file.h"


gim_dir_file_list *		gim_directory_obj::add( void ) {
	gim_dir_file_list	* TmpList;
	TmpList = (gim_dir_file_list *)gim_memory->Alloc( sizeof (gim_dir_file_list) , __GIM_MEM_DIRECTORY_FLIST_ADD , __GIM_HIDE );
	if ( TmpList == NULL )
		gim_error->set( GIM_ERROR_FATAL , "gim_directory_obj::add" , "Allocation of a new file list member failed"  , __GIM_ERROR );
	if ( ! startlist ) {
		startlist		= TmpList;
		startlist->link	= NULL;
		endlist			= TmpList;
		TmpList->id		= 0;
	}
	else {
		endlist->link	= TmpList;
		TmpList->link	= NULL ;
		TmpList->id		= endlist->id + 1;
		endlist			= TmpList;
	}
	return TmpList;
}
		
_gim_Uint32				gim_directory_obj::p_make( const char * dir_name , _gim_Uint16 level ) {
	struct dirent 	** eps;
	int				n , cnt;
	char			TmpFileName[1024];
	char 			message[256];
//	sprintf( message , "Scanning %s..." , dir_name );
//	gim_error->set( "gim_directory_obj::make" , message );
	n = scandir ( dir_name , &eps , NULL , alphasort );
	if ( n ) {
		if ( level == __GIM_ROOT ) {
			strcpy( path , dir_name );
/*			sprintf( message , "Adding root %s" , dir_name );
			gim_error->set( "gim_directory_obj::make" , message );
			currentlist = ladd();
			strcpy( currentlist->path , "" );
			strcpy( currentlist->name , dir_name );
			currentlist->size = 0;
			currentlist->type = ROOT;
			stat( dir_name , &( currentlist->stat ) );
*/		}
		for ( cnt = 0 ; cnt < n ; cnt++ ) {
			struct stat tmp;
			sprintf( TmpFileName , "%s%s" , dir_name , eps[cnt]->d_name );
			if ( ( ! strcmp( "." , eps[cnt]->d_name ) ) || ( ! strcmp( ".." , eps[cnt]->d_name ) ) )
				continue;
			if ( ( *(eps[cnt]->d_name) == '.' ) && ( hide == __GIM_NO ) )
				continue;
			if ( stat( TmpFileName , &tmp ) )
				continue;
			switch ( eps[cnt]->d_type ) {
				case DT_REG : {
					__GIM_CLEAR( message , 256 , char );
					currentlist = add();
					sprintf( TmpFileName , "%s%s" , dir_name , eps[cnt]->d_name );
					__GIM_CLEAR( currentlist->name , 256 , char );
					strcpy( currentlist->name , eps[cnt]->d_name );
					strcpy( currentlist->path , dir_name );
					if ( gim_file_manager->length( TmpFileName ) != 0 ) {
						currentlist->type = REGFILE;
						currentlist->size = gim_file_manager->length( TmpFileName );
//						sprintf( message , "Added regular file %s..." , eps[cnt]->d_name );
					}
					else {
						currentlist->type = EMPTY;
						currentlist->size = 0;
//						sprintf( message , "Added empty regular file %s..." , eps[cnt]->d_name );
					}
					stat( TmpFileName , &( currentlist->stat ) );
//					gim_error->set( "gim_directory_obj::make" , message );
					break;
				}
				case DT_DIR : {
					currentlist = add( );
					sprintf( TmpFileName , "%s%s/" , dir_name , eps[cnt]->d_name );
					__GIM_CLEAR( currentlist->name , 256 , char );
					strcpy( currentlist->name , eps[cnt]->d_name );
					strcpy( currentlist->path , dir_name );
					currentlist->type = REGNODE;
					stat( TmpFileName , &( currentlist->stat ) );
/*					sprintf( message , "Added regular node %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_directory_obj::make" , message );
*/					if ( recursive == GIM_DIR_RECURSIVE ) 
						p_make( TmpFileName , level + 1 );
					break;
				}
				case DT_LNK : {
					currentlist = add( );
					sprintf( TmpFileName , "%s%s" , dir_name , eps[cnt]->d_name );
					strcpy( currentlist->name , eps[cnt]->d_name );
					strcpy( currentlist->path , dir_name );
					currentlist->type = LINK;
					lstat( TmpFileName , &( currentlist->stat ) );
/*					sprintf( message , "Added link %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_directory_obj::make" , message );
*/					break;
				}
				case DT_BLK : {
/*					sprintf( message , "Skipped Block file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_directory_obj::make" , message );
*/					break;
				}
				case DT_FIFO : {
/*					sprintf( message , "Skipped Fifo file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_directory_obj::make" , message );
*/					break;
				}
				case DT_CHR : {
/*					sprintf( message , "Skipped Character file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_directory_obj::make" , message );
*/					break;
				}
				case DT_SOCK : {
/*					sprintf( message , "Skipped Socket file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_directory_obj::make" , message );
*/					break;
				}
				case DT_UNKNOWN : {
/*					sprintf( message , "Skipped UNKNOWN file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_directory_obj::make" , message );
*/					break;
				}
			}
		}
	}
	if ( n <= 0 )
		gim_error->set( GIM_ERROR_WARNING , "gim_directory_obj::make" , "Couldn't open the directory" , __GIM_ERROR );
//	if ( n == 2 )
//		gim_error->set( "gim_directory_obj::make" , "The directory is empty" );
	if ( ( n >  2 ) && ( level == __GIM_ROOT ) ) {
		char message [256];
		sprintf( message , "items are %-8d : %s" , endlist->id + 1 , dir_name );
		gim_error->set( "gim_directory_obj::make" , message );
	}
	item = endlist->id;
	return endlist->id;
}

_gim_Uint32				gim_directory_obj::make( const char * dir_name ) {
	if ( strlen( dir_name ) == 0 ) {
		gim_error->set( "gim_directory_obj::make" , "" );
	}
	p_make( dir_name , __GIM_ROOT );
	reset_position();
}


void	gim_directory_obj::destroy( void ) {
	gim_dir_file_list	* TmpList = NULL;
	_gim_Uint32			c = 0;
	if ( startlist != NULL ) {
		gim_error->set( "gim_directory_obj::destroy" , "Destroying file list..." );
		for( currentlist=startlist ; currentlist != NULL ; currentlist = TmpList , c++ ) {
			char message[256];
/*			sprintf( message , "Releasing file GKP::%s" , currentlist->name );
			gim_error->set( "gim_directory_obj::destroy" , message );
*/			TmpList = currentlist->link;
			if ( gim_memory->Free( currentlist ) != __GIM_OK )
				gim_error->set( GIM_ERROR_CRITICAL , "gim_directory_obj::destroy" , "Some memory issue" , __GIM_ERROR );
		}
		if ( c ) {
			char message[256];
			sprintf( message , "Destroyed %d members" , c );
			gim_error->set( "gim_directory_obj::destroy" , message );
		}
		startlist	= NULL;
		currentlist	= NULL;
		endlist		= NULL;
		gim_error->set( "gim_directory_obj::destroy" , "Done" );
	}
	else 
		gim_error->set( GIM_ERROR_WARNING , "gim_directory_obj::destroy" , "No memory list" , __GIM_ERROR );
}


_gim_flag	gim_directory_obj::recursion( _gim_flag rec ) {
	if ( rec == __GIM_YES ) {
		recursive = GIM_DIR_RECURSIVE;
		return rec;
	}
	if ( rec == __GIM_NO ) {
		recursive = GIM_DIR_NOT_RECURSIVE;
		return rec;
	}
	return __GIM_ERROR;
}


void	gim_directory_obj::also_hide( _gim_flag _hide ) {
	if ( ( _hide == __GIM_YES ) || ( _hide == __GIM_NO ) ) {
		hide = _hide;
	}
}

_gim_Uint32	gim_directory_obj::refresh( void ) {
	_gim_Uint32		items;
	if ( ! startlist ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_directory_obj::refresh" , "Startlist is NULL : error" , __GIM_ERROR );
		return 0;
	}
	gim_error->set( "gim_directory_obj::refresh" , "Start to refresh" );
	destroy();
	items = p_make( path , __GIM_ROOT );
	gim_error->set( "gim_directory_obj::refresh" , "Refreshed" );
	reset_position();
	return items;
}

void		gim_directory_obj::reset_position( void ) {
	currentlist = startlist;
	gim_error->set( "gim_directory_obj::reset_position" , "Current position setted to startlist" );
}


_gim_dir_item *	gim_directory_obj::get_item( _gim_flag inc ) {
	char message[256];
	if ( ! startlist ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_directory_obj::get_item" , "Startlist is NULL : error" , __GIM_ERROR );
		return NULL;
	}
	if ( ! currentlist ) {
		gim_error->set( "gim_directory_obj::get_item" , "Endlist reached" );
		return NULL;
	}
	_gim_dir_item *	res = (_gim_dir_item *)gim_memory->Alloc( sizeof (gim_dir_item) , __GIM_MEM_DIRECTORY_ITEM , __GIM_HIDE );
	if ( ! res ) {
		gim_error->set( GIM_ERROR_FATAL , "gim_directory_obj::get_item" , "Allocation of a new item failed"  , __GIM_ERROR );
		return NULL;
	}
	sprintf( res->name , "%s%s" , currentlist->path , currentlist->name );
	res->size = currentlist->size;
	res->type = currentlist->type;
	res->id = currentlist->id;
	if ( inc == GIM_DIR_INCREMENT )
		currentlist = currentlist->link;
//	sprintf( message , "Got item id [%d]" , res->id );
//	gim_error->set( "gim_directory_obj::get_item" , message );
	return res;
}

_gim_flag		gim_directory_obj::free_item( _gim_dir_item * to_free ) {
	return gim_memory->Free( to_free );	
}


_gim_dir_item *	gim_directory_obj::get_item_id( _gim_Uint32 id ) {
	if ( id > item ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_directory_obj::get_item" , "ERROR : Id more high than item" , __GIM_ERROR );
		return NULL;
	}
	currentlist = startlist;
	while ( currentlist != NULL ) {
		if ( currentlist->id == id ) {
			char message[256];
			_gim_dir_item *	res = (_gim_dir_item *)gim_memory->Alloc( sizeof (gim_dir_item) , __GIM_MEM_DIRECTORY_ITEM , __GIM_HIDE );
			if ( ! res ) {
				gim_error->set( GIM_ERROR_FATAL , "gim_directory_obj::get_item" , "ERROR : Allocation of a new item failed"  , __GIM_ERROR );
				return NULL;
			}
			sprintf( res->name , "%s%s" , currentlist->path , currentlist->name );
			res->size = currentlist->size;
			res->type = currentlist->type;
			res->id = currentlist->id;
//			sprintf( message , "Got item id [%d]" , res->id );
//			gim_error->set( "gim_directory_obj::get_item" , message );
			return res;
		}
		currentlist = currentlist->link;
	}
	gim_error->set( GIM_ERROR_CRITICAL , "gim_directory_obj::get_item" , "ERROR : Item not found" , __GIM_ERROR );
	return NULL;
}
