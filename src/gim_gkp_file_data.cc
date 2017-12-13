//
// gim_gkp_file_data.cc
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

void	gim_gkdp_obj::write_gkdp( _gim_handler * out ) {
	char			path[1024];
	_gim_handler	* in = NULL;
	char	 		* file = NULL , tname[1024];
	_gim_gkp_chunk	chunk;	
	if ( Lexical.str_equal( "ManualGKDP" , startlist->name ) != __GIM_YES ) {
		strcpy( path , startlist->name );
		for( currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link ) {
			sprintf( tname , "%s%s" , path , currentlist->name ) ;
			__GIM_CLEAR( &chunk , 1 ,  _gim_gkp_chunk );
			if ( header.crypt == __GIM_YES )
				in = gim_file_manager->open( tname , __GIM_FILE_POINTER , __GIM_READ );
			if ( header.crypt == __GIM_NO )
				in = gim_file_manager->open( tname , __GIM_FILE_MAPPING , __GIM_READ );
			if ( in == NULL ) {
				char message[256];
				sprintf( message , "I cannot open file %s" , currentlist->name );
				gim_error->set( GIM_ERROR_FATAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
			}
			if ( header.crypt == __GIM_YES ) {
				file = gim_file_manager->load( in );
				if ( file == NULL ) 
					gim_error->set( GIM_ERROR_FATAL , "gim_file_data_obj::data" , "I cannot load file" , __GIM_NOT_OK );
				strcpy( chunk.id , gim_checksum->md5( file , in->size ) ) ;
				crypt->set_secondary( currentlist->name );
				crypt->advanced_password( file , in->size );
				strcpy( chunk.xhash , gim_checksum->md5( file , in->size ) );
			}
			else {
				strcpy( chunk.id , gim_checksum->md5( in->fm , in->size ) ) ;
			}
			char message[1024];
			sprintf( message , "writing chunk id - %d" , currentlist->id );
			gim_error->set( "gim_file_data_obj::data" , message );
			gim_file_manager->flush( out , (char *)&chunk , sizeof( chunk ) );
			__GIM_CLEAR( message , 1024 , char );
			sprintf( message , "writing file - %s" , currentlist->name );
			gim_error->set( "gim_file_data_obj::data" , message );
			if ( header.crypt == __GIM_YES )
				gim_file_manager->flush( out , file , in->size );
			if ( header.crypt == __GIM_NO )
				gim_file_manager->flush( out , in->fm , in->size );
			gim_file_manager->close( in );
		}
	}
	else {
		gim_interface->clear();
		for( currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link ) {
			gim_interface->data->sou_ratio = (float)( ( (float)currentlist->id / (float)endlist->id ) / 100.00 );
			strcpy( gim_interface->data->item_sou_name , currentlist->name );
			__GIM_CLEAR( &chunk , 1 ,  _gim_gkp_chunk );
			strcpy( chunk.id , gim_checksum->md5( currentlist->handle , currentlist->size ) ) ;
			if ( header.crypt == __GIM_YES ) {
				crypt->set_secondary( currentlist->name );
				crypt->advanced_password( currentlist->handle , currentlist->size );
				strcpy( chunk.xhash , gim_checksum->md5( currentlist->handle , currentlist->size ) );
			}
			char message[1024];
			sprintf( message , "writing chunk id - %d" , currentlist->id );
			gim_error->set( "gim_file_data_obj::data" , message );
			gim_file_manager->flush( out , (char *)&chunk , sizeof( chunk ) );
			__GIM_CLEAR( message , 1024 , char );
			sprintf( message , "writing file - %s" , currentlist->name );
			gim_error->set( "gim_file_data_obj::data" , message );
			gim_file_manager->flush( out , currentlist->handle , currentlist->size );
		}
	}
}


void	gim_gkdp_obj::read_gkdp( _gim_map map ) {
	_gim_gkp_chunk	chunk;
	_gim_buffer		data = NULL;
	for( currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link ) {
		char message[256];
		sprintf( message , "Reading GKP::%s " , currentlist->name );
		gim_error->set( "gim_file_data_obj::data" , message );
		__GIM_CLEAR( &chunk , 1 , chunk );
		memcpy( &chunk , map , sizeof( chunk ) );
		map += sizeof( chunk );
		currentlist->handle = map;
		data = (_gim_buffer)gim_memory->Alloc( currentlist->size , __GIM_MEM_FILE_DATA_R , __GIM_HIDE );
		memcpy( data , map , currentlist->size );
		map += currentlist->size;
		if ( header.crypt == __GIM_NO ) {
			if ( Lexical.str_equal( chunk.id , gim_checksum->md5( data , currentlist->size ) ) == __GIM_NO ) {
				char message[256];
				sprintf( message , "GKP::%s checksum error" , currentlist->name );
				gim_error->set( GIM_ERROR_CRITICAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
			}
		}
		if ( header.crypt == __GIM_YES ) {
			if ( Lexical.str_equal( chunk.xhash , gim_checksum->md5( data , currentlist->size ) ) == __GIM_NO ) {
				char message[256];
				sprintf( message , "GKP::%s checksum error" , currentlist->name );
				gim_error->set( GIM_ERROR_CRITICAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
			}
		}
		gim_memory->Free( data );
	}
}

