//
// gim_gkfp_file_data.cc
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


_gim_flag	gim_gkfp_obj::write_data( _gim_handler * out , _gim_gkp_flist * startlist ) {
	char			path[1024];
	char			message[256];
	_gim_gkp_flist	* currentlist = NULL;
	_gim_handler	* in = NULL;
	char	 		* file = NULL , tname[1024];
	_gim_gkp_chunk	chunk;
	//gim_interface->clear();
	for( currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link ) {
		if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
		if ( currentlist->type == REGFILE ) {
			gim_interface->data->dst_ratio = (float)( (float)currentlist->id / (float)endlist->id );
			sprintf( message , "Cur id : %d  |  end id : %d  |  dst_ratio %f" , currentlist->id , endlist->id , gim_interface->data->dst_ratio );
			gim_error->set( "gim_gkfp_file_data_obj::write" , message );
			sprintf( tname , "%s%s" , currentlist->path , currentlist->name ) ;
			__GIM_CLEAR( &chunk , 1 ,  _gim_gkp_chunk );
			if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			if ( header.crypt == __GIM_FOG )
				in = gim_file_manager->open( tname , __GIM_FILE_POINTER , __GIM_READ );
			if ( header.crypt == __GIM_SUN )
				in = gim_file_manager->open( tname , __GIM_FILE_MAPPING , __GIM_READ );
			if ( in == NULL ) {
				sprintf( message , "I cannot open file %s" , currentlist->name );
				gim_error->set( GIM_ERROR_FATAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
			}
			if ( header.crypt == __GIM_FOG ) {
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Loading" );
				file = gim_file_manager->load( in );
				if ( file == NULL ) 
					gim_error->set( GIM_ERROR_FATAL , "gim_file_data_obj::data" , "I cannot load file" , __GIM_NOT_OK );
				strcpy( chunk.id , gim_checksum->md5( file , in->size ) ) ;
				crypt->set_secondary( currentlist->name );
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Crypting" );
				crypt->advanced_password( file , in->size );
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Hashing" );
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
				strcpy( chunk.xhash , gim_checksum->md5( file , in->size ) );
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			}
			else {
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Hashing" );
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
				strcpy( chunk.id , gim_checksum->md5( in->fm , in->size ) ) ;
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			}
			char message[1024];
			sprintf( message , "writing chunk id - %d" , currentlist->id );
			gim_error->set( "gim_file_data_obj::data" , message );
			if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			gim_file_manager->flush( out , (char *)&chunk , sizeof( chunk ) );
			__GIM_CLEAR( message , 1024 , char );
			sprintf( message , "writing file - %s" , currentlist->name );
			gim_error->set( "gim_file_data_obj::data" , message );
			if ( header.crypt == __GIM_FOG ) {
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Flushing" );
				gim_file_manager->flush( out , file , in->size );
			}
			if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			if ( header.crypt == __GIM_SUN )
				gim_file_manager->flush( out , in->fm , in->size );
			gim_file_manager->close( in );
		}
	}
	return __GIM_OK;
}
	

_gim_flag	gim_gkfp_obj::read_data( _gim_map map , _gim_gkp_flist * startlist ) {
	_gim_gkp_flist 	* currentlist = NULL;
	_gim_gkp_chunk	chunk;
	_gim_buffer		data = NULL;
	int 			c;
	for( c = 0 , currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link , c++ ) {
		if ( currentlist->type == REGFILE ) {
			char message[256];
			gim_interface->data->sou_size = gim_interface->data->sou_position = c + 1;
			sprintf( message , "Reading GKP::%s " , currentlist->name );
			gim_error->set( "gim_file_data_obj::data" , message );
			__GIM_CLEAR( &chunk , 1 , chunk );
			memcpy( &chunk , map , sizeof( chunk ) );
			map += sizeof( chunk );
			currentlist->handle = map;
			data = (_gim_buffer)gim_memory->Alloc( currentlist->size , __GIM_MEM_FILE_DATA_R , __GIM_HIDE );
			memcpy( data , map , currentlist->size );
			map += currentlist->size;
			sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
			sprintf( gim_interface->data->item_dna_name , "Hashing" );
			if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			if ( header.crypt == __GIM_SUN ) {
				if ( Lexical.str_equal( chunk.id , gim_checksum->md5( data , currentlist->size ) ) == __GIM_NO ) {
					char message[256];
					sprintf( message , "GKP::%s checksum error" , currentlist->name );
					gim_error->set( GIM_ERROR_CRITICAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
				}
			}
			if ( header.crypt == __GIM_FOG ) {
				if ( Lexical.str_equal( chunk.xhash , gim_checksum->md5( data , currentlist->size ) ) == __GIM_NO ) {
					char message[256];
					sprintf( message , "GKP::%s checksum error" , currentlist->name );
					gim_error->set( GIM_ERROR_CRITICAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
				}
			}
			gim_memory->Free( data );
		}
	}
	return __GIM_OK;
}


_gim_flag	gim_gkfp_obj::extract_data( _gim_gkp_flist * startlist ) {
	_gim_gkp_flist 	* currentlist = NULL;
	_gim_gkp_chunk	chunk;
	_gim_buffer		data = NULL;
	char 			message[256] , tname[1024];
	char			TmpFileName[2048];
	gim_utils_obj	* util = new gim_utils_obj;
	char 			* path_tmp;
	int  			len;
	for( currentlist = startlist ; currentlist != NULL ; currentlist = currentlist->link ) {
		switch ( currentlist->type ) {
			case ROOT : {
				if ( strlen( new_root ) != 0 ) {
					//printf("Root - %s..." , new_root );
					mkdir( new_root , currentlist->stat.st_mode );
					//puts( "Created" );
				}
				else {
					//printf("Root - %s..." , currentlist->name );                           
					mkdir( currentlist->name , currentlist->stat.st_mode );
					//puts( "Created" );
				}
				break;
			}
			case REGFILE	:
			case EMPTY		: {
				_gim_handler * file;
				gim_interface->data->dst_ratio = (float)( (float)currentlist->id / (float)endlist->id );
				sprintf( message , "Cur id : %d  |  end id : %d  |  dst_ratio %f" , currentlist->id , endlist->id , gim_interface->data->dst_ratio );
				gim_error->set( "gim_gkfp_obj::extract_data" , message );
				sprintf( tname , "%s%s" , currentlist->path , currentlist->name ) ;
				if ( internal_gim->get_state() == __GIM_TH_STOP ) 
					return __GIM_NOT_OK;
				sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				if ( strlen( new_root ) != 0 ) {
					char m[2048];
					//printf( " tmpfilename before : %s\n after : %s\n" , TmpFileName , util->str_subtraction( TmpFileName , old_root ) );
					sprintf( m , "%s%s" , new_root , util->str_subtraction( TmpFileName , old_root ) );
					strcpy( TmpFileName , m );
//					printf( " tmpfilename : %s\n" , TmpFileName );
				}
				file = gim_file_manager->open( TmpFileName , __GIM_FILE_POINTER , __GIM_WRITE );
				if ( file == NULL ) {
					sprintf( message , "Cannot open %s for writing" , TmpFileName );
					gim_error->set( GIM_ERROR_FATAL , "gim_gkfp_obj::extract_data" , message , __GIM_ERROR );
				}
				if ( header.crypt == __GIM_SUN ) {
					gim_file_manager->flush( file , currentlist->handle , currentlist->size );
				}
				if ( header.crypt == __GIM_FOG ) {
					sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
					sprintf( gim_interface->data->item_dna_name , "Loading" );
					currentlist->buffer = (_gim_buffer)gim_memory->Alloc( currentlist->size * sizeof( char ) );
					memcpy( currentlist->buffer , currentlist->handle , currentlist->size );
					crypt->set_secondary( currentlist->name );
					sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
					sprintf( gim_interface->data->item_dna_name , "Crypting" );
					crypt->advanced_password( currentlist->buffer , currentlist->size );
					if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
					sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
					sprintf( gim_interface->data->item_dna_name , "Flushing" );
					if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
					gim_file_manager->flush( file , currentlist->buffer , currentlist->size );
					gim_memory->Unlock_and_free( currentlist->buffer );
				}
				gim_file_manager->close( file );
				chmod( TmpFileName , currentlist->stat.st_mode );
				break;
			}
			case REGNODE	: {
				//~ sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				//~ printf("Node  - CREATE  -  %s\n" , TmpFileName );
				sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				if ( strlen( new_root ) != 0 ) {
					char m[2048];
					//printf( " tmpfilename before : %s\n after : %s\n" , TmpFileName , util->str_subtraction( TmpFileName , old_root ) );
					sprintf( m , "%s%s/" , new_root , util->str_subtraction( TmpFileName , old_root ) );
					strcpy( TmpFileName , m );
					//printf( " tmpfilename : %s\n" , TmpFileName );
				}
//				sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				mkdir( TmpFileName , currentlist->stat.st_mode );
				break;
			}
			case LINK : {
				//~ sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				//~ printf("Link  - SKIPPED - %s\n" , TmpFileName );
				break;
			}
			case BLK : {
				break;
			}
			case FIFO : {
				break;
			}
			case CHR : {
				break;
			}
			case SOCK : {
				break;
			}
			case UNKNOWN : {
				break;
			}
			default: {
			}
		}
	}
	return __GIM_OK;
}
		
