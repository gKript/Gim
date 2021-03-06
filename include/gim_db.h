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
	#include "gim_list.h"
	#include "gim_lexical.h"
	#include "gim_checksum.h"


	class gim_db_obj {

		public:
			_gim_flag			set_name				( const char * dbname );
			char *				get_name				( void );
			_gim_flag			set_property			( _gim_flag property , _gim_flag value );
			_gim_flag			make_env				( void );
			_gim_flag			check					( void );
			_gim_flag			init					( void );
			_gim_flag			read					( const char * dbname );
			_gim_flag			exist_environment		( const char * dbname );
			_gim_flag			create_table			( const char * table_name );
			_gim_flag			table_exist				( char * table_name );
			_gim_db_table *		seek_table				( char * table_name );
			_gim_flag			add_field_to_table		( const char * table_name , const char * field_name , _gim_flag is_key , _gim_flag field_type );
			_gim_flag			add_field_to_table		( const char * table_name , const char * field_name , _gim_flag is_key , _gim_flag field_type , _gim_db_value * value );
			_gim_flag			init_from_gdbs			( const char * gdbs_name );
			_gim_flag			gdbs_line_syntax_check	( _gim_Uint8 NoT );
			_gim_int8			gdbs_tokenizer			( char * line );
			char * 				gdbs_getline			( void );
			_gim_flag			gdbs_open				( void );
			void				gdbs_close				( void );
			_gim_flag			gdbs_feof				( void );
			_gim_flag			gdbs_execute			( void );

		private:
			_gim_flag			read_tables				( void );
			_gim_flag			create_db				( const char * dbname );
			_gim_flag			add_table				( const char * table_name );
			_gim_flag			del_table				( const char * table_name );
			_gim_flag			add_field				( _gim_db_table * tbl , const char * field_name , _gim_flag is_key , _gim_flag field_type );
			_gim_flag			add_field				( _gim_db_table * tbl , const char * field_name , _gim_flag is_key , _gim_flag field_type , _gim_db_value * value );
//			_gim_flag			insert_record			( const char * table_name , _gim_db_record * record );
//			_gim_flag			delete_record			( const char * table_name , _gim_db_record * record );
			_gim_db_table *		get_table_handle		( const char * table_name );
			_gim_flag			command_parser			( const char * commad );
			_gim_flag			string_to_flag			( _gim_Uint8 index , _gim_flag context );
			_gim_flag			gim_db_version			( int maj , int min );
			_gim_flag			make_create_table		( char * table_name );
			_gim_flag			make_set_db				( _gim_flag value );
			_gim_flag			make_set_table			( _gim_flag value );
			_gim_flag			make_add_field			( _gim_Uint8 value , _gim_flag type , char * name );
			_gim_flag			make_set_field			( _gim_Uint8 value , _gim_flag type );
			char *				db_checksum				( char * data );

			_gim_db_main		* db;

			_gim_list			* gdbs_script;

			_gim_flag			GDBS_run;

			char				home[1024];
			char				name[128];
			char				d_name[1024];
			char				file_name[512];
			char				file_name_long[1024];
			char				gdbs_file_name[512];
			char				comment[1024];

			_gim_handler		* gdbs_file;
			volatile _gim_flag	gdbs_run;

			volatile _gim_flag	initiated;
			volatile _gim_flag	changed;

			SyntaxCoerence_obj  * syntax;
			_gim_lex			* lex;
			gim_checksum_obj	* cs;

			_gim_Uint32			gdbs_line_num;

		public:


			/*! gim_db_obj constructor
			*/
			inline gim_db_obj() {
				if ( strlen( gim_application_name ) != 0 ) {
					gim_error->set( "gim_db_obj::gim_db_obj" , "DB : Constructor start" );
					db = (_gim_db_main *)gim_memory->Alloc( sizeof( _gim_db_main ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
					strcpy( name			, "" );
					strcpy( comment			, "" );
					strcpy( file_name		, "" );
					strcpy( file_name_long	, "" );
					db->mode = GIM_DB_PERFORMANCE;
					db->type = GIM_DB_VOLATILE;
					db->tables_number = 0;
					strcpy( db->pin_table , "" );
					db->pin = __GIM_NO;
					db->conf = new _gim_prsr;
					db->tables = new _gim_list;
					cs = new gim_checksum_obj;
					lex = new _gim_lex;
					initiated	= __GIM_NO;
					changed		= __GIM_NO;
					syntax = (SyntaxCoerence_obj *)gim_memory->Alloc( sizeof( SyntaxCoerence_obj ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
					syntax->Title = __GIM_OFF;
					syntax->Section = __GIM_OFF;
					syntax->Feof = __GIM_OFF;
					gdbs_script  = new _gim_list;
					gdbs_run = __GIM_NOT_RUNNING;
					gim_error->set( "gim_db_obj::gim_db_obj" , "DB : Constructor end" );
				}
				else 
					gim_error->set( GIM_ERROR_FATAL , "gim_db_obj::gim_db_obj" , "DB : initialization error : use of db is not allowed without the application name" , __GIM_ERROR );
			};

			/*! gim_db_obj destructor
			*/
			inline ~gim_db_obj() {
				delete lex;
				delete cs;
				if ( strlen( name  ) ) {
					gim_error->Set( GIM_ERROR_MESSAGE ,  "gim_db_obj::~gim_db_obj" , "DB [%s] : start to shutting down..." , name );
				}
				else 
					gim_error->set( "gim_db_obj::~gim_db_obj" , "DB : start to shutting down..." );
				if ( ( db->type == GIM_DB_PERMANENT ) && ( changed == __GIM_YES ) ) {
					gim_error->set( "gim_db_obj::~gim_db_obj" , "DB : Something is changed in configuration. Updating" );
					db->conf->Write();
				}
				if ( db->tables->items() ) {
					gim_error->set( "gim_db_obj::~gim_db_obj" , "DB : Some allocated Table found. Deallocating" );
					db->tables->rewind();
					for ( _gim_int32 c = 0 ; c < db->tables_number ; c++ ) {
						_gim_db_table * Ttmp = (_gim_db_table *)db->tables->get_item();
						Ttmp->Tstruct->Down();
						Ttmp->Tdata->Down();
/*						if ( Ttmp->fields->items() )
							Ttmp->fields->destroy_list();
*/						delete Ttmp->fields;
						if ( Ttmp->records->items() ) {
							gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::~gim_db_obj" , "DB : Found some records [%d]. Deallocating..." , Ttmp->records->items() );
							for( _gim_int32 r = 1 ; r <= Ttmp->records->items() ; r++ ) {
								_gim_db_record * tr = (_gim_db_record *)Ttmp->records->get_item();
								tr->values->destroy_list();
								Ttmp->records->next_item();
							}
							Ttmp->records->destroy_list();
						}
						delete Ttmp->records;
						gim_memory->Free( Ttmp );
						db->tables->next_item();
					}
				}
				delete gdbs_script;
				gim_memory->Free ( syntax );
				delete db->tables;
				db->conf->Down();
				delete db->conf;
				gim_memory->Free( db );
				gim_error->set( "gim_db_obj::~gim_db_obj" , "DB : shutted down" );
			};
	};


#endif //_GIM_DB_OBJ_H_

