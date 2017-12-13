//
// gim_file_list.cc
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

void 		gim_gkdp_obj::init( void ) {
	char	message[256];
	startlist = NULL;
	currentlist = NULL;
	endlist = NULL;
	sprintf( message , "New gim_gkdp_obj @ 0x%x" , (unsigned long)this );
	gim_error->set( "gim_gkdp_obj::init" , message );
}
			
void		gim_gkdp_obj::release( void ) {
	if ( startlist != NULL ) 
		destroy();
}


_gim_gkp_flist *	gim_gkdp_obj::add( void ) {
	_gim_gkp_flist	* TmpList;
	TmpList = (_gim_gkp_flist *)gim_memory->Alloc( sizeof (_gim_gkp_flist) , __GIM_MEM_GKP_FLIST_ADD , __GIM_HIDE );
	if ( TmpList == NULL )
		gim_error->set( GIM_ERROR_FATAL , "gim_gkdp_obj::add" , "Allocation of a new file list member failed"  , __GIM_ERROR );
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



void		gim_gkdp_obj::add( const char * name , _gim_buffer data , int size ) {
	char message[256];
	currentlist = add();
	strcpy( currentlist->name , name );
	currentlist->size = size;
	currentlist->handle = data;
	sprintf( message , "Adding member %s..." , name );
	gim_error->set( "gim_gkdp_obj::add" , message );
}

void		gim_gkdp_obj::set_manual( void ) {
		currentlist = add();
		strcpy( currentlist->name , "ManualGKDP" );
		currentlist->size = 0;
		gim_error->set( "gim_gkdp_obj::set_manual" , "File list setted to manual. Added virtual root" );
}

_gim_Uint32	gim_gkdp_obj::make_gkdp( const char * dir_name ) {
	struct dirent 	** eps;
	int				n , cnt;
	char			TmpFileName[1024];
	n = scandir ( dir_name , &eps , NULL , alphasort );
	if ( n ) {
		char message[256];
		currentlist = add();
		strcpy( currentlist->name , dir_name );
		currentlist->size = 0;
		sprintf( message , "Adding root %s" , dir_name );
		gim_error->set( "gim_gkdp_obj::make_gkdp" , message );
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
					currentlist = add( );
					sprintf( TmpFileName , "%s%s" , dir_name , eps[cnt]->d_name );
					if ( gim_file_manager->length( TmpFileName ) != 0 ) {
						sprintf( message , "Adding regular file %s..." , eps[cnt]->d_name );
						__GIM_CLEAR( currentlist->name , 256 , char );
						strcpy( currentlist->name , eps[cnt]->d_name );
						currentlist->size = gim_file_manager->length( TmpFileName );
					}
					gim_error->set( "gim_gkdp_obj::make_gkdp" , message );
					break;
				}
				case DT_DIR : 
				case DT_LNK : 
				case DT_UNKNOWN :
				case DT_BLK :
				case DT_FIFO :
				case DT_CHR :
				case DT_SOCK :
				default :
					break;
			}
		}
	}
	if ( n <= 0 ) gim_error->set( GIM_ERROR_CRITICAL , "gim_gkdp_obj::make_gkdp" , "Couldn't open the directory" , __GIM_ERROR );
	if ( n == 2 ) gim_error->set( GIM_ERROR_WARNING , "gim_gkdp_obj::make_gkdp" , "The directory is empty" , __GIM_NOT_OK );
	if ( n >  2 ) {
		char message [256];
		sprintf( message , "File list maked : added %d members" , endlist->id + 1 );
		gim_error->set( "gim_gkdp_obj::make_gkdp" , message );	
	}
	header.members = endlist->id;
	return endlist->id;
}



void	gim_gkdp_obj::destroy( void ) {
	_gim_gkp_flist	* TmpList = NULL;
	_gim_Uint32		c = 0;
	if ( startlist != NULL ) {
		gim_error->set( "gim_gkdp_obj::destroy" , "Destroy file list..." );
		for( currentlist=startlist ; currentlist != NULL ; currentlist = TmpList , c++ ) {
			char message[256];
			if ( currentlist->id == 0 )
				sprintf( message , "Releasing ROOT GKP::%s" , currentlist->name );
			else
				sprintf( message , "Releasing file GKP::%s" , currentlist->name );
			gim_error->set( "gim_gkdp_obj::destroy" , message );
			TmpList = currentlist->link;
			if ( currentlist->buffer != NULL ) {
				if ( gim_memory->Free( currentlist->buffer ) != __GIM_OK ){
					gim_error->set( GIM_ERROR_CRITICAL , "gim_gkdp_obj::destroy" , "Some memory issue" , __GIM_ERROR );
				}
				else {
					gim_error->set( "gim_gkdp_obj::destroy" , "Buffer leaved succesfully" );
				}
			}
			if ( gim_memory->Free( currentlist ) != __GIM_OK )
				gim_error->set( GIM_ERROR_CRITICAL , "gim_gkdp_obj::destroy" , "Some memory issue" , __GIM_ERROR );
		}
		if ( c ) {
			char message[256];
			sprintf( message , "Destroyed %d members" , c );
			gim_error->set( "gim_gkdp_obj::destroy" , message );
		}
		startlist	= NULL;
		currentlist	= NULL;
		endlist		= NULL;
		gim_error->set( "gim_gkdp_obj::destroy" , "Done" );
	}
	else 
		gim_error->set( GIM_ERROR_WARNING , "gim_gkdp_obj::destroy" , "No memory list" , __GIM_ERROR );
}


_gim_gkp_flist *	gim_gkdp_obj::lget( void ) {
	if ( startlist != NULL )
		return startlist;
	else {
		gim_error->set( GIM_ERROR_FATAL , "gim_gkdp_obj::get" , "No file list" , __GIM_ERROR );
		return NULL;
	}
}


void	gim_gkdp_obj::lwrite( _gim_handler * out ) {
	_gim_gkp_flist	FileList;
	for( currentlist = startlist ; currentlist != NULL ; currentlist = currentlist->link ) {
		strcpy( FileList.name , currentlist->name ) ;
		FileList.size	=	currentlist->size;
		FileList.id		=	currentlist->id;
		FileList.handle	=	NULL ;
		FileList.link	=	NULL ;
		gim_file_manager->flush( out , (char *)&FileList , sizeof( FileList ) );
	}
}


_gim_map 	gim_gkdp_obj::lread( _gim_map map ) {
	char message[256];
	sprintf( message , "Reading file list : %d items" , header.members );
	gim_error->set( "gim_gkdp_obj::lread" , message );
	for( unsigned int c = 0 ; c <= header.members ; c++ ) {
		currentlist = add();
		memcpy( currentlist , map , sizeof( _gim_gkp_flist ) );
		map += sizeof( _gim_gkp_flist );
		currentlist->buffer	= NULL;
		currentlist->handle = NULL;
		sprintf( message , "Item %s succesfully readed" , currentlist->name );
		gim_error->set( "gim_gkdp_obj::lread" , message );
	}
	gim_error->set( "gim_gkdp_obj::lread" , "FileList succesfully readed" );
	return map;
}


void	gim_gkdp_obj::clear( void ) {
	destroy();
}


_gim_gkp_flist *	gim_gkdp_obj::next( _gim_gkp_flist * tmplist ) {
	if ( tmplist != NULL )
		return tmplist->link;
	else
		return NULL;
}
