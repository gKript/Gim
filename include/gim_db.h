//
// gim_db.h
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

/*!	\file		gim_db.h
	\version	2.5-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]

/*!	\class		gim_db_obj
	\brief		This class is an integrated and generic pourpose light database.
	\author		Danilo Zannoni  -  asyntote@gkript.org
	\version	2.5-0
	\date		2003-2008
*/

#ifndef _GIM_DB_OBJ_H_
#define _GIM_DB_OBJ_H_

	#include "gim_db_structures.h"

	class gim_db_obj {

		public:
			_gim_flag			set_name			( const char * dbname );
			char *				get_name			( void );
			_gim_flag			set_db_properities	( _gim_flag properities , _gim_flag value );
			_gim_flag			make_env			( void );
			_gim_flag			check				( void );
			_gim_flag			init				( void );
			_gim_flag			create_table		( const char * table_name );
			_gim_flag			add_field_to_table  ( const char * table_name , const char * field_name , _gim_flag is_key , _gim_flag field_type );
			_gim_flag			add_field_to_table  ( const char * table_name , const char * field_name , _gim_flag is_key , _gim_flag field_type , _gim_db_value * value );

		private:
			_gim_flag			create_db			( const char * dbname );
			_gim_flag			add_table			( const char * table_name );
			_gim_flag			del_table			( const char * table_name );
			_gim_flag			add_field			( _gim_db_table * tbl , const char * field_name , _gim_flag is_key , _gim_flag field_type );
			_gim_flag			add_field			( _gim_db_table * tbl , const char * field_name , _gim_flag is_key , _gim_flag field_type , _gim_db_value * value );
			_gim_flag			insert_record		( const char * table_name , _gim_db_record * record );
			_gim_flag			delete_record		( const char * table_name , _gim_db_record * record );
			_gim_db_table *		get_table_handle	( const char * table_name );
			_gim_flag			command_parser		( const char * commad );

			_gim_db_main		* db;

			char home[1024];
			char c_name[1024];
			char d_name[1024];

			volatile _gim_flag	initiated;
			volatile _gim_flag	changed;

			_gim_mempage		* memory;

		public:

			/*! gim_db_obj constructor
			*/
			inline gim_db_obj() {
				if ( strlen( gim_application_name ) != 0 ) {
					db = ( _gim_db_main * )gim_memory->Alloc( sizeof( _gim_db_main ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
					strcpy( db->name	, "" );
					strcpy( db->comment	, "" );
					set_db_properities( GIM_DB_VOLATILE , __GIM_YES );
					set_db_properities( GIM_DB_BALANCED , __GIM_YES );
					db->n_tables = 0;
					db->first_table = NULL;
					db->conf = new _gim_prsr;
					initiated	= __GIM_NO;
					changed		= __GIM_NO;
					gim_error->set( "gim_db_obj::gim_db_obj" , "DB : Constructor end" );
				}
				else 
					gim_error->set( GIM_ERROR_FATAL , "gim_db_obj::gim_db_obj" , "DB : initialization error : use of db is not allowed without the application name" , __GIM_ERROR );
			};


			/*! gim_db_obj constructor
			*/
			inline gim_db_obj( const char * dbname ) {
				if ( strlen( gim_application_name ) != 0 ) {
					char message[256];
					db = ( _gim_db_main * )gim_memory->Alloc( sizeof( _gim_db_main ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
					memory = new _gim_mempage;
					set_name( dbname );
					set_db_properities( GIM_DB_VOLATILE , __GIM_YES );
					set_db_properities( GIM_DB_BALANCED , __GIM_YES );
					db->n_tables = 0;
					db->first_table = NULL;
					db->conf = new _gim_prsr;
					initiated	= __GIM_NO;
					changed		= __GIM_NO;
					sprintf( message , "DB : [%s] Constructor end" , dbname );
					gim_error->set( "gim_db_obj::gim_db_obj" , message );
				}
				else 
					gim_error->set( GIM_ERROR_FATAL , "gim_db_obj::gim_db_obj" , "DB : initialization error : use of db is not allowed without the application name" , __GIM_ERROR );
			};


			/*! gim_db_obj constructor
			*/
			inline gim_db_obj( const char * dbname , _gim_flag type , _gim_flag mode ) {
				if ( strlen( gim_application_name ) != 0 ) {
					char message[256];
					sprintf( message , "DB : [%s] Constructor start" , dbname );
					gim_error->set( "gim_db_obj::gim_db_obj" , message );
					db = ( _gim_db_main * )gim_memory->Alloc( sizeof( _gim_db_main ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
					memory = new _gim_mempage;
					set_name( dbname );
					db->n_tables = 0;
					db->first_table = NULL;
					db->conf = new _gim_prsr;
					initiated	= __GIM_NO;
					changed		= __GIM_NO;
					db->type = type;
					db->mode = mode;
					if( check() == GIM_DB_NOT_EXIST )
						make_env();
					init();
					sprintf( message , "DB : [%s] Constructor end" , dbname );
					gim_error->set( "gim_db_obj::gim_db_obj" , message );
				}
				else 
					gim_error->set( GIM_ERROR_FATAL , "gim_db_obj::gim_db_obj" , "DB : initialization error : use of db is not allowed without the application name" , __GIM_ERROR );
			};


			/*! gim_db_obj destructor
			*/
			inline ~gim_db_obj() {
				char message[256];
				if ( strlen( db->name ) ) {
					sprintf( message , "DB [%s] : start to shutting down..." , db->name );
					gim_error->set( "gim_db_obj::~gim_db_obj" , message );
				}
				else {
					gim_error->set( "gim_db_obj::~gim_db_obj" , "DB : start to shutting down..." );
				}
				if ( ( db->type == GIM_DB_PERMANENT ) && ( changed == __GIM_YES ) ) {
					gim_error->set( "gim_db_obj::~gim_db_obj" , "DB : Something is changed in configuration. Updating" );
					db->conf->Write();
				}
				if ( db->first_table != NULL ) {
					gim_error->set( "gim_db_obj::~gim_db_obj" , "DB : Some allocated Table found. Deallocating" );
					_gim_db_table	* t_tbl = db->first_table;
					_gim_db_table	* n_tbl = NULL;
					for( ; t_tbl != NULL ; t_tbl = n_tbl ) {
						sprintf( message , "DB :   deallocating table [%s]" , t_tbl->name );
						gim_error->set( "gim_db_obj::~gim_db_obj" , message );
						n_tbl = t_tbl->next;
						gim_memory->Free( t_tbl );
					}
				}
				db->conf->Down();
				delete db->conf;
				gim_memory->Free( db );
				delete memory;
				gim_error->set( "gim_db_obj::~gim_db_obj" , "DB : shutted down" );
			};
	};


#endif //_GIM_DB_OBJ_H_

