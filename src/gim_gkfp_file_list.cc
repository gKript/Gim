//
//  gim_gkfp_file_list.cc
//
//  Gim - Generic Information Manager
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
//
// File: gim_gkfp_file_list_obj.cc
// Created by: AsYntote <asyntote@gkript.org>
// Created on: Sun Jun 24 16:40:17 2007
//

/*!     \file       gim_gkfp_file_list.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include "../include/gim.h"


void 		gim_gkfp_obj::linit( void ) {
	char	message[256];
	startlist = NULL;
	currentlist = NULL;
	endlist = NULL;
	sprintf( message , "New gim_gkfp_file_list_obj @ %p" , (void *)this );
	gim_error->set( "gim_gkfp_file_list_obj::init" , message );
}
			
void		gim_gkfp_obj::lrelease( void ) {
	if ( startlist != NULL ) 
		ldestroy();
}


_gim_gkp_flist *	gim_gkfp_obj::ladd( void ) {
	_gim_gkp_flist	* TmpList;
	TmpList = (_gim_gkp_flist *)gim_memory->Alloc( sizeof (_gim_gkp_flist) , __GIM_MEM_GKP_FLIST_ADD , __GIM_HIDE );
	if ( TmpList == NULL )
		gim_error->set( GIM_ERROR_FATAL , "gim_gkfp_file_list_obj::add" , "Allocation of a new file list member failed"  , __GIM_ERROR );
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


_gim_Uint32	gim_gkfp_obj::lmake( const char * dir_in , _gim_Uint16 level ) {
	/* the entries are joined to the directory name: it must end with a separator, otherwise they are never found */
	char			dir[ 1100 ];
	const char	* dir_name = dir;
	size_t			dir_len;
	snprintf( dir , sizeof( dir ) - 2 , "%s" , dir_in );
	dir_len = strlen( dir );
	if ( dir_len && dir[ dir_len - 1 ] != '/' && dir[ dir_len - 1 ] != '\\' )
		strcat( dir , "/" );
	struct dirent 	** eps;
	int				n , cnt;
	char			TmpFileName[1024];
	char 			message[256];
	n = scandir ( dir_name , &eps , NULL , alphasort );
	if ( n ) {
		if ( level == __GIM_ROOT ) {
			sprintf( message , "Adding root %s" , dir_in );
			gim_error->set( "gim_gkfp_file_list_obj::make" , message );
			currentlist = ladd();
			strcpy( currentlist->path , "" );
			strcpy( currentlist->name , dir_in );
			currentlist->size = 0;
			currentlist->type = ROOT;
			stat( dir_name , &( currentlist->stat ) );
		}
		gim_interface->data->sou_size += n;
		for ( cnt = 0 ; cnt < n ; cnt++ ) {
			struct stat tmp;
			sprintf( TmpFileName , "%s%s" , dir_name , eps[cnt]->d_name );
			if ( ( ! strcmp( "." , eps[cnt]->d_name ) ) || ( ! strcmp( ".." , eps[cnt]->d_name ) ) )
				continue;
			if ( stat( TmpFileName , &tmp ) )
				continue;
			switch ( eps[cnt]->d_type ) {
				case DT_REG : {
					__GIM_CLEAR( message , 256 , char );
					currentlist = ladd( );
					gim_interface->data->sou_position = endlist->id;
					sprintf( TmpFileName , "%s%s" , dir_name , eps[cnt]->d_name );
					__GIM_CLEAR( currentlist->name , 256 , char );
					strcpy( currentlist->name , eps[cnt]->d_name );
					strcpy( currentlist->path , dir_name );
					if ( gim_file_manager->length( TmpFileName ) != 0 ) {
						currentlist->type = REGFILE;
						currentlist->size = gim_file_manager->length( TmpFileName );
						if ( header.hashed_file == __GIM_YES )
							strcpy( currentlist->hash , gim_checksum->sha1( TmpFileName ) );
						sprintf( message , "Added regular file %s..." , eps[cnt]->d_name );
					}
					else {
						currentlist->type = EMPTY;
						currentlist->size = 0;
						sprintf( message , "Added empty regular file %s..." , eps[cnt]->d_name );
					}
					stat( TmpFileName , &( currentlist->stat ) );
					gim_error->set( "gim_gkfp_file_list_obj::make" , message );
					break;
				}
				case DT_DIR : {
					currentlist = ladd( );
					gim_interface->data->sou_position = endlist->id;
					sprintf( TmpFileName , "%s%s/" , dir_name , eps[cnt]->d_name );
					__GIM_CLEAR( currentlist->name , 256 , char );
					strcpy( currentlist->name , eps[cnt]->d_name );
					strcpy( currentlist->path , dir_name );
					currentlist->type = REGNODE;
					stat( TmpFileName , &( currentlist->stat ) );
					sprintf( message , "Added regular node %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_gkfp_file_list_obj::make" , message );
					sprintf( message , "%s" , TmpFileName );
					gim_error->set( "gim_gkfp_file_list_obj::make" , message );
					if ( header.recursive == __GIM_YES ) 
						lmake( TmpFileName , level + 1 );
					break;
				}
				case DT_LNK : {
					currentlist = ladd( );
					gim_interface->data->sou_position = endlist->id;
					sprintf( TmpFileName , "%s%s" , dir_name , eps[cnt]->d_name );
					strcpy( currentlist->name , eps[cnt]->d_name );
					strcpy( currentlist->path , dir_name );
					currentlist->type = LINK;
					lstat( TmpFileName , &( currentlist->stat ) );
					sprintf( message , "Added link %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_gkfp_file_list_obj::make" , message );
					break;
				}
				case DT_BLK : {
					sprintf( message , "Skipped Block file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_gkfp_file_list_obj::make" , message );
					break;
				}
				case DT_FIFO : {
					sprintf( message , "Skipped Fifo file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_gkfp_file_list_obj::make" , message );
					break;
				}
				case DT_CHR : {
					sprintf( message , "Skipped Character file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_gkfp_file_list_obj::make" , message );
					break;
				}
				case DT_SOCK : {
					sprintf( message , "Skipped Socket file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_gkfp_file_list_obj::make" , message );
					break;
				}
				case DT_UNKNOWN : {
					sprintf( message , "Skipped UNKNOWN file %s..." , eps[cnt]->d_name );
					gim_error->set( "gim_gkfp_file_list_obj::make" , message );
					break;
				}
			}
		}
	}
	if ( n <= 0 ) gim_error->set( GIM_ERROR_CRITICAL , "gim_gkfp_file_list_obj::make" , "Couldn't open the directory" , __GIM_ERROR );
	if ( n == 2 ) gim_error->set( GIM_ERROR_WARNING , "gim_gkfp_file_list_obj::make" , "The directory is empty" , __GIM_ERROR );
	if ( n >  2 ) {
		char message [256];
		sprintf( message , "File list maked : added %d members" , endlist->id + 1 );
		gim_error->set( "gim_gkfp_file_list_obj::make" , message );
	}
	header.members = endlist->id;
	return endlist->id;
}



void	gim_gkfp_obj::ldestroy( void ) {
	_gim_gkp_flist	* TmpList = NULL;
	_gim_Uint32		c = 0;
	if ( startlist != NULL ) {
		gim_error->set( "gim_gkfp_file_list_obj::destroy" , "Destroy file list..." );
		for( currentlist=startlist ; currentlist != NULL ; currentlist = TmpList , c++ ) {
			char message[256];
			if ( currentlist->id == 0 )
				sprintf( message , "Releasing ROOT GKP::%s" , currentlist->name );
			else
				sprintf( message , "Releasing file GKP::%s" , currentlist->name );
			gim_error->set( "gim_gkfp_file_list_obj::destroy" , message );
			TmpList = currentlist->link;
			if ( currentlist->buffer != NULL ) {
				if ( gim_memory->Free( currentlist->buffer ) != __GIM_OK ){
					gim_error->set( GIM_ERROR_CRITICAL , "gim_gkfp_file_list_obj::destroy" , "Some memory issue" , __GIM_ERROR );
				}
				else {
					gim_error->set( "gim_gkfp_file_list_obj::destroy" , "Buffer leaved succesfully" );
				}
			}
			if ( gim_memory->Free( currentlist ) != __GIM_OK )
				gim_error->set( GIM_ERROR_CRITICAL , "gim_gkfp_file_list_obj::destroy" , "Some memory issue" , __GIM_ERROR );
		}
		if ( c ) {
			char message[256];
			sprintf( message , "Destroyed %d members" , c );
			gim_error->set( "gim_gkfp_file_list_obj::destroy" , message );
		}
		startlist	= NULL;
		currentlist	= NULL;
		endlist		= NULL;
		gim_error->set( "gim_gkfp_file_list_obj::destroy" , "Done" );
	}
	else 
		gim_error->set( GIM_ERROR_CRITICAL , "gim_gkfp_file_list_obj::destroy" , "No file list" , __GIM_ERROR );
}


_gim_gkp_flist *	gim_gkfp_obj::lget( void ) {
	if ( startlist != NULL )
		return startlist;
	else {
		gim_error->set( GIM_ERROR_FATAL , "gim_gkfp_file_list_obj::get" , "No file list" , __GIM_ERROR );
		return NULL;
	}
}


void	gim_gkfp_obj::lwrite( _gim_handler * out ) {
	_gim_gkp_flist	FileList;
	for( currentlist = startlist ; currentlist != NULL ; currentlist = currentlist->link ) {
		strcpy( FileList.name , currentlist->name ) ;
		strcpy( FileList.path , currentlist->path ) ;
		FileList.size	=	currentlist->size;
		FileList.type	=	currentlist->type;
		FileList.id		=	currentlist->id;
		memcpy( &(FileList.stat) , &(currentlist->stat) , sizeof( struct stat ) );
		FileList.handle	=	NULL ;
		FileList.link	=	NULL ;
		gim_file_manager->flush( out , (char *)&FileList , sizeof( FileList ) );
	}
}


_gim_map 	gim_gkfp_obj::lread( _gim_map map ) {
	for( unsigned int c = 0 ; c <= header.members ; c++ ) {
		currentlist = ladd();
		memcpy( currentlist , map , sizeof( _gim_gkp_flist ) );
		map += sizeof( _gim_gkp_flist );
		currentlist->buffer	= NULL;
		currentlist->handle = NULL;
		if ( currentlist->type == ROOT ) {
			strcpy( old_root , currentlist->name );
			char message[256];
			sprintf( message , "New root found : %s" , old_root );
			gim_error->set( "gim_gkfp_file_list_obj::get" , message );
		}
		gim_interface->data->sou_position = c;
	}
	return map;
}


void	gim_gkfp_obj::lclear( void ) {
	ldestroy();
}


_gim_gkp_flist *	gim_gkfp_obj::lnext( _gim_gkp_flist * tmplist ) {
	if ( tmplist != NULL )
		return tmplist->link;
	else
		return NULL;
}
