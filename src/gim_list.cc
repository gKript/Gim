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
#include    "../include/gim_list.h"


_gim_int32	gim_list_obj::size( void ) {
	return item_size;
}


void		gim_list_obj::size( _gim_int32 mem_size ) {
	item_size = mem_size;
}


_gim_flag	gim_list_obj::add_item( void *member ) {
	_gim_list_item *	temp_item;
	temp_item = (gim_list_item *)gim_memory->Alloc ( sizeof( gim_list_item ) , __GIM_LIST_ITEM , __GIM_HIDE );
	if ( temp_item == NULL ) {
		return __GIM_ERROR;
	}
	temp_item->index = ++item_counter;
	temp_item->item = member;
	temp_item->nextitem = NULL;
	if ( startlist == NULL ) {
		startlist = temp_item;
		currentlist = startlist;
//		puts("primo item");
	}
	else {
		endlist->nextitem = temp_item;
//		puts("ulteriore item");
	}
	endlist = temp_item;
	//gim_error->set( "gim_list_obj::add_item" , "New item allocated" );
	return __GIM_OK;
}


_gim_flag	gim_list_obj::del_item( _gim_Uint32 index ) {
	if ( index = 0 ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_list_obj::del_item" , "index cannot be 0" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}

	_gim_list_item *	tmplist = startlist;
	_gim_list_item *	beforelist = NULL;
	_gim_list_item *	afterlist = NULL;

	if( index == 0 ) {
		startlist = startlist->nextitem;
		currentlist = startlist;
		gim_error->set( "gim_list_obj::add_item" , "Item deleted" );
	}

	while ( tmplist != NULL ) {
		if ( tmplist->index == index ) {
			beforelist = tmplist->previtem;
			afterlist = tmplist->nextitem;
			beforelist->nextitem = afterlist;
			afterlist->previtem = beforelist;
			gim_memory->Unlock_and_free ( tmplist );
			item_counter--;
			gim_error->set( "gim_list_obj::add_item" , "Item found and deleted" );
			return __GIM_OK;
		}
		tmplist = tmplist->nextitem;
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_list_obj::del_item" , "Item NOT FOUND" , __GIM_ERROR );
	return __GIM_NOT_OK;
}


void		gim_list_obj::destroy_list( void ) {
	if ( startlist == NULL )
		gim_error->Set( GIM_ERROR_WARNING , "gim_list_obj::destroy_list" , "The list is already empty." );
	else {

		_gim_list_item *	temp_item = startlist;
		_gim_list_item *	temp_free;
		_gim_Uint16			It = 0 , Ls = 0;
	

		while ( temp_item != NULL ) {
			if ( temp_item->item != NULL ) {
				if ( gim_memory->Check ( temp_item->item ) == __GIM_EXIST ) {
					if ( gim_memory->Free ( temp_item->item ) == __GIM_NOT_OK )
						gim_error->set( GIM_ERROR_CRITICAL , "gim_list_obj::destroy_list" , "Item NOT found. You have to deallocate it before to destroy the list" , __GIM_ERROR );
					else
						It++;
				}
				else
					gim_error->Set( GIM_ERROR_WARNING , "gim_list_obj::destroy_list" , "Item not found" );
			}
			else 
				gim_error->Set( GIM_ERROR_WARNING , "gim_list_obj::destroy_list" , "Item was NULL" );
			temp_free = temp_item;
			temp_item = temp_item->nextitem;
			if ( gim_memory->Free ( temp_free ) == __GIM_NOT_OK )
				gim_error->set( GIM_ERROR_CRITICAL , "gim_list_obj::destroy_list" , "Some problem to deallocate a List item." , __GIM_ERROR );
			else
				Ls++;
		}
		gim_error->Set( GIM_ERROR_MESSAGE , "gim_list_obj::destroy_list" , "Deallocated : [%3d List items] [%3d Linked items]" , Ls , It );
	}
}


void * gim_list_obj::get_item( void ) {
	if ( currentlist == NULL ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_list_obj::next_item" , "Endlist reached" , __GIM_ERROR );
		return NULL;
	}
	return currentlist->item;
}


void *  gim_list_obj::get_item( _gim_Uint32 index ) {
	if ( index == 0 ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_list_obj::get_item" , "index cannot be 0" , __GIM_ERROR );
		return NULL;
	}

	_gim_list_item *	tmplist = startlist;

	while ( tmplist != NULL ) {
		if ( tmplist->index == index ) {
			//gim_error->set( "gim_list_obj::get_item" , "Item successfully obtained." );
			return tmplist->item;
		}
		tmplist = tmplist->nextitem;
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_list_obj::get_item" , "Item NOT FOUND" , __GIM_ERROR );
	return NULL;
}


_gim_flag   gim_list_obj::next_item( void ) {
	if ( currentlist == NULL ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_list_obj::next_item" , "Endlist reached" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	currentlist = currentlist->nextitem;
	//gim_error->set( "gim_list_obj::next_item" , "Next item set" );
	return __GIM_OK;
}


_gim_flag	gim_list_obj::rewind( void ) {
	if ( startlist == NULL ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_list_obj::rewind" , "The list is empty" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	currentlist = startlist;
	//gim_error->set( "gim_list_obj::rewind" , "Startlit set" );
	return __GIM_OK;
}	


_gim_flag  gim_list_obj::seek( _gim_Uint32 index ) {
	if ( index = 0 ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_list_obj::seek" , "index cannot be 0" , __GIM_ERROR );
		return NULL;
	}
	currentlist = startlist;
	while ( currentlist != NULL ) {
		if ( currentlist->index == index ) {
			gim_error->set( "gim_list_obj::seek" , "Item successfully sought." );
			return __GIM_OK;
		}
		currentlist = currentlist->nextitem;
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_list_obj::seek" , "Item NOT FOUND" , __GIM_ERROR );
	return NULL;
}


_gim_Uint32		gim_list_obj::items( void ) {
	return item_counter;
}


_gim_Uint32		gim_list_obj::get_id( void ) {
	return currentlist->index;
}


void	gim_list_obj::backup( void ) {
	savelist = currentlist;
}


void	gim_list_obj::restore( void ) {
	currentlist = savelist;
	savelist = NULL;
}

