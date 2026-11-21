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
*/

/*!	\class		gim_db_obj
	\brief		This class is an integrated and generic pourpose light database.
	\author		Danilo Zannoni  -  asyntote@gkript.org
	\version	2.5-0
	\date		2003-2008

	\todo Shell "select": 'or' in the where clause (precedence to decide: standard or strict left-to-right).
	\todo Shell "select": comparison between two fields (e.g. temperatura > media_10).
	\todo Shell "select": 'like' with wildcards; 'in (...)' lists.
	\todo Shell "select": with 'show', the fields keep the order written by the user; check that a repeated field works.
	\todo Values are single tokens in the shell: no embedded double quotes, no escapes.
	\todo Default table type in PERMANENT databases: VOLATILE today, decide whether PERMANENT is better.
	\todo GDBS scripts: a failing script can leave a half-created database; add a pre-validation pass or a rollback.
	\todo Records and tables cannot be updated or deleted yet (no 'update' / 'delete' commands).
	\todo Select is a linear scan that re-reads every key as text: add an index if tables grow large.
	\todo Check whether the buffers returned by GetKeySTR() and GetSectionName() are shared or must be freed.
	\todo FLOAT values are stored with 5 decimals: confirm that this precision is enough.
	\todo Automated tests: there is no test suite yet, all checks have been done by hand through gimshell.
	\todo Gim parser warns "I cannot read this file" when a data file is empty: silence it or treat empty files as valid.
*/

#ifndef _GIM_DB_OBJ_H_
#define _GIM_DB_OBJ_H_

	#include "gim_db_structures.h"
	#include "gim_list.h"
	#include "gim_lexical.h"
	#include "gim_checksum.h"


	class gim_db_obj {

		public:
			/*! Sets the name of the database. The name is used for the environment folder and the configuration files.
				\param dbname	name of the database
				\return		__GIM_OK on success
			*/
			_gim_flag			set_name				( const char * dbname );
			/*! Returns the name of the database.
				\return		pointer to the name (owned by the object)
			*/
			char *				get_name				( void );
			/*! Returns the number of tables in the database.
				\return		number of tables
			*/
			_gim_Uint8			tables_count			( void );
			/*! Returns a table given its position.
				\param index	position of the table, starting from 1
				\return		handle of the table, or NULL when the index is out of range
			*/
			_gim_db_table *		table_at				( _gim_Uint8 index );
			/*! Adds one record to a table. Every value is checked against the type of its field.
				For PERMANENT databases the table data is written to disk.
				\param table_name	name of the table
				\param n			number of (field, value) pairs
				\param fields		array of n field names
				\param values		array of n values, as text
				\return		__GIM_OK on success, __GIM_ERROR otherwise (the reason is logged)
			*/
			_gim_flag			insert_record			( const char * table_name , _gim_Uint8 n , const char ** fields , const char ** values );
			/*! Sets a property of the database (mode, type, ...).
				\param property	property to set
				\param value	new value of the property
				\return		__GIM_OK on success
			*/
			_gim_flag			set_property			( _gim_flag property , _gim_flag value );
			/*! Creates the environment folder of the database on disk.
				\return		__GIM_OK on success
			*/
			_gim_flag			make_env				( void );
			/*! Checks the consistency of the database.
				\return		__GIM_OK when the database is consistent
			*/
			_gim_flag			check					( void );
			/*! Initializes the database: loads its configuration and tables.
				\return		__GIM_OK on success
			*/
			_gim_flag			init					( void );
			/*! Reads an existing database from its environment folder.
				\param dbname	name of the database to read
				\return		__GIM_OK on success
			*/
			_gim_flag			read					( const char * dbname );
			/*! Tells whether the environment folder of a database exists.
				\param dbname	name of the database
				\return		__GIM_YES when it exists, __GIM_NO otherwise
			*/
			_gim_flag			exist_environment		( const char * dbname );
			/*! Creates an empty table.
				\param table_name	name of the new table
				\return		__GIM_OK on success
			*/
			_gim_flag			create_table			( const char * table_name );
			/*! Tells whether a table exists.
				\param table_name	name of the table
				\return		__GIM_YES when the table exists, __GIM_NO otherwise
			*/
			_gim_flag			table_exist				( char * table_name );
			/*! Returns the handle of a table given its name.
				\param table_name	name of the table
				\return		handle of the table, or NULL when it does not exist
			*/
			_gim_db_table *		seek_table				( char * table_name );
			/*! Adds a field to a table, without a default value.
				\param table_name	name of the table
				\param field_name	name of the new field
				\param is_key		__GIM_YES when the field is the key of the table
				\param field_type	one of the GIM_DB_TYPE_* constants
				\return		__GIM_OK on success
			*/
			_gim_flag			add_field_to_table		( const char * table_name , const char * field_name , _gim_flag is_key , _gim_flag field_type );
			/*! Adds a field to a table, with a default value.
				\param table_name	name of the table
				\param field_name	name of the new field
				\param is_key		__GIM_YES when the field is the key of the table
				\param field_type	one of the GIM_DB_TYPE_* constants
				\param value		default value of the field
				\return		__GIM_OK on success
			*/
			_gim_flag			add_field_to_table		( const char * table_name , const char * field_name , _gim_flag is_key , _gim_flag field_type , _gim_db_value * value );
			/*! Creates the database from a GDBS script file.
				\param gdbs_name	path of the GDBS script
				\return		__GIM_OK when every command of the script succeeds
			*/
			_gim_flag			init_from_gdbs			( const char * gdbs_name );
			/*! Checks the syntax of the current GDBS line.
				\param NoT	number of tokens of the line
				\return		__GIM_OK when the syntax is valid
			*/
			_gim_flag			gdbs_line_syntax_check	( _gim_Uint8 NoT );
			/*! Splits a GDBS line into tokens.
				\param line	line to split (modified in place)
				\return		number of tokens found
			*/
			_gim_int8			gdbs_tokenizer			( char * line );
			/*! Reads the next line of the GDBS script.
				\return		the line, or NULL at the end of the script
			*/
			char * 				gdbs_getline			( void );
			/*! Opens the GDBS script file.
				\return		__GIM_OK on success
			*/
			_gim_flag			gdbs_open				( void );
			/*! Closes the GDBS script file.
			*/
			void				gdbs_close				( void );
			/*! Tells whether the end of the GDBS script has been reached.
				\return		__GIM_YES at the end of the file
			*/
			_gim_flag			gdbs_feof				( void );
			/*! Executes the GDBS commands read from the script.
				\return		status of the first failing command, or __GIM_OK
			*/
			_gim_flag			gdbs_execute			( void );

		private:
			/*! Reads the tables of the database from the configuration files.
				\return		__GIM_OK on success
			*/
			_gim_flag			read_tables				( void );
			/*! Creates the database files for a new database.
				\param dbname	name of the database
				\return		__GIM_OK on success
			*/
			_gim_flag			create_db				( const char * dbname );
			/*! Adds a table to the database in memory.
				\param table_name	name of the table
				\return		__GIM_OK on success
			*/
			_gim_flag			add_table				( const char * table_name );
			/*! Removes a table from the database in memory.
				\param table_name	name of the table
				\return		__GIM_OK on success
			*/
			_gim_flag			del_table				( const char * table_name );
			/*! Adds a field to a table handle, without a default value.
				\param tbl			table handle
				\param field_name	name of the field
				\param is_key		__GIM_YES when the field is the key
				\param field_type	one of the GIM_DB_TYPE_* constants
				\return		__GIM_OK on success
			*/
			_gim_flag			add_field				( _gim_db_table * tbl , const char * field_name , _gim_flag is_key , _gim_flag field_type );
			/*! Adds a field to a table handle, with a default value.
				\param tbl			table handle
				\param field_name	name of the field
				\param is_key		__GIM_YES when the field is the key
				\param field_type	one of the GIM_DB_TYPE_* constants
				\param value		default value of the field
				\return		__GIM_OK on success
			*/
			_gim_flag			add_field				( _gim_db_table * tbl , const char * field_name , _gim_flag is_key , _gim_flag field_type , _gim_db_value * value );
//			_gim_flag			insert_record			( const char * table_name , _gim_db_record * record );
//			_gim_flag			delete_record			( const char * table_name , _gim_db_record * record );
			/*! Returns the handle of a table given its name (internal lookup).
				\param table_name	name of the table
				\return		handle of the table, or NULL
			*/
			_gim_db_table *		get_table_handle		( const char * table_name );
			/*! Parses and runs one command of the GDBS script.
				\param commad	the command line
				\return		__GIM_OK on success
			*/
			_gim_flag			command_parser			( const char * commad );
			/*! Converts a GDBS keyword into the matching flag.
				\param index	index of the keyword in the command
				\param context	context of the keyword (database, table or field)
				\return		the flag, or __GIM_ERROR when the keyword is not valid
			*/
			_gim_flag			string_to_flag			( _gim_Uint8 index , _gim_flag context );
			/*! Checks the version of the database format.
				\param maj	major version
				\param min	minor version
				\return		__GIM_OK when the version is supported
			*/
			_gim_flag			gim_db_version			( int maj , int min );
			/*! GDBS command CREATE TABLE.
				\param table_name	name of the table
				\return		__GIM_OK on success
			*/
			_gim_flag			make_create_table		( char * table_name );
			/*! GDBS command SET DB.
				\param value	value of the database property
				\return		__GIM_OK on success
			*/
			_gim_flag			make_set_db				( _gim_flag value );
			/*! GDBS command SET TABLE (needs a pinned table).
				\param value	value of the table property
				\return		__GIM_OK on success, __GIM_ERROR when no table is pinned
			*/
			_gim_flag			make_set_table			( _gim_flag value );
			/*! GDBS command ADD FIELD (needs a pinned table).
				\param value	position of the field, starting from 1
				\param type		type of the field
				\param name		name of the field
				\return		__GIM_OK on success, __GIM_ERROR when no table is pinned
			*/
			_gim_flag			make_add_field			( _gim_Uint8 value , _gim_flag type , char * name );
			/*! GDBS command SET FIELD (needs a pinned table).
				\param value	position of the field, starting from 1
				\param type		property to set on the field
				\return		__GIM_OK on success, __GIM_ERROR when no table is pinned
			*/
			_gim_flag			make_set_field			( _gim_Uint8 value , _gim_flag type );
			/*! Computes the checksum of a piece of data.
				\param data	data to check
				\return		the checksum as text
			*/
			char *				db_checksum				( char * data );

			/*! Main database structure. */
			_gim_db_main		* db;

			/*! Lines of the GDBS script being read. */
			_gim_list			* gdbs_script;

			/*! Flag: a GDBS script is running. */
			_gim_flag			GDBS_run;

			char				home[1024];				/*!< home folder of the user */
			char				name[128];				/*!< name of the database */
			char				d_name[1024];			/*!< folder of the database */
			char				file_name[512];			/*!< name of the database configuration file */
			char				file_name_long[1024];	/*!< full path of the database configuration file */
			char				gdbs_file_name[512];	/*!< name of the GDBS script file */
			char				comment[1024];			/*!< comment of the database */

			_gim_handler		* gdbs_file;			/*!< handle of the GDBS script file */
			volatile _gim_flag	gdbs_run;				/*!< state of the GDBS execution */

			volatile _gim_flag	initiated;				/*!< __GIM_YES when the database is initialized */
			volatile _gim_flag	changed;				/*!< __GIM_YES when the configuration must be written */

			SyntaxCoerence_obj  * syntax;				/*!< syntax state of the GDBS parser */
			_gim_lex			* lex;					/*!< lexical analyzer */
			gim_checksum_obj	* cs;					/*!< checksum calculator */

			_gim_Uint32			gdbs_line_num;			/*!< current line of the GDBS script */

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

			/*! gim_db_obj destructor: writes the configuration if changed (PERMANENT databases) and frees the tables.
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
