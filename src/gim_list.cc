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
	if ( item_size == 0 )
		return __GIM_ERROR;
	temp_item = (gim_list_item *)gim_memory->Alloc ( sizeof( gim_list_item ) , __GIM_LIST_ITEM , __GIM_HIDE );
	if ( temp_item == NULL )
		return __GIM_ERROR;
	temp_item->index = item_counter++;
	temp_item->item = member;
	temp_item->previtem = endlist;
	temp_item->nextitem = NULL;
	if ( startlist == NULL ) {
		startlist = temp_item;
		currentlist = startlist;
	}
	else
		endlist->nextitem = temp_item;
	endlist = temp_item;
	gim_error->set( "gim_list_obj::gim_list_obj()" , "New item allocated" );
	return __GIM_OK;
}


void		gim_list_obj::destroy_list( void ) {
	_gim_list_item *	temp_item;
	_gim_list_item *	temp_free;
	if ( startlist == NULL )
		gim_error->Set( GIM_ERROR_WARNING , "gim_list_obj::destroy_list()" , "The list is already empty." );
	temp_item = startlist;
	while ( temp_item != NULL ) {
		if ( temp_item->item != NULL ) {
			if ( gim_memory->Check ( temp_item->item ) == __GIM_EXIST ) {
				gim_memory->Unlock_and_free ( temp_item->item );
				gim_error->set( "gim_list_obj::destroy_list()" , "Item found and deallocated succesfully" );
			}
			else
				gim_error->Set( GIM_ERROR_WARNING , "gim_list_obj::destroy_list()" , "Item not found" );
		}
		else 
			gim_error->Set( GIM_ERROR_WARNING , "gim_list_obj::destroy_list()" , "Item was NULL" );
		temp_free = temp_item;
		temp_item = temp_item->nextitem;
		gim_memory->Free ( temp_free );
		gim_error->set( "gim_list_obj::destroy_list()" , "Item list deallocated" );
	}
}

