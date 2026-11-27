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

/*!	\file		gim_db.h
	\version	2.5-0
	\brief		Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

/*!	\class		gim_db_obj
	\brief		A light database of tables, fields and records, kept in memory or saved on disk.
					Guide with examples, limits and warnings: \ref gimdoc_db
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
	\todo GDBS UPDATE and DELETE accept only '=' in the conditions: the shell and the API accept the other operators.
	\todo Select is a linear scan that re-reads every key as text: add an index if tables grow large.
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
			/*! Sets the name of the database. The name is used for the folder and the configuration files. It has 1 to 63 characters; spaces become _.
				\param dbname	name of the database
				\return		__GIM_OK on success, __GIM_ERROR if the name is empty or too long
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
			/*! Adds one record to a table. The values of INT, FLOAT, BOOL and FLAG fields are checked against the type, the others are stored as text. The KEY field cannot repeat.
					A value longer than 63 characters or empty is not stored: the reason is logged as CRITICAL, but insert_record() still returns __GIM_OK.
				For PERMANENT databases the table data is written to disk.
				\param table_name	name of the table
				\param n			number of (field, value) pairs
				\param fields		array of n field names
				\param values		array of n values, as text
				\return		__GIM_OK on success, __GIM_ERROR otherwise (the reason is logged)
			*/
			_gim_flag			insert_record			( const char * table_name , _gim_Uint8 n , const char ** fields , const char ** values );
				/*! Updates the fields of the records that match all the conditions (AND).
					The change is validated before anything is modified: a refused update leaves the table untouched.
					Setting the KEY field is allowed only when exactly one record matches, and the new key must not exist in another record.
					With ncond = 0 every record matches.
					\param table_name	name of the table
					\param ncond		number of conditions
					\param cond			array of ncond conditions
					\param nset			number of (field, value) pairs to set
					\param fields		array of nset field names
					\param values		array of nset values, as text (same rules as insert_record)
					\param updated		receives the number of records changed (0 when none matches, which is not an error)
					\return		__GIM_OK on success, __GIM_ERROR otherwise (the reason is logged)
				*/
				_gim_flag			update_record			( const char * table_name , _gim_Uint8 ncond , const _gim_db_cond * cond , _gim_Uint8 nset , const char ** fields , const char ** values , int * updated );
				/*! Deletes the records that match all the conditions (AND). With ncond = 0 every record is deleted.
					For PERMANENT databases the table is written to disk once, at the end.
					\param table_name	name of the table
					\param ncond		number of conditions
					\param cond			array of ncond conditions
					\param deleted		receives the number of records deleted (0 when none matches, which is not an error)
					\return		__GIM_OK on success, __GIM_ERROR otherwise (the reason is logged)
				*/
				_gim_flag			delete_record			( const char * table_name , _gim_Uint8 ncond , const _gim_db_cond * cond , int * deleted );
				/*! Tells if a stored value satisfies "stored op wanted". Numbers are compared as numbers, the other types as text.
					\param type		data type of the field (GIM_DB_TYPE_*)
					\param stored	the value stored in the record
					\param op		one of  =  !=  <  <=  >  >=
					\param wanted	the value of the condition
					\return	1 when it matches, 0 otherwise
				*/
				static int			value_matches			( _gim_flag type , const char * stored , const char * op , const char * wanted );
				/*! Tells if op is one of  =  !=  <  <=  >  >=
					\return	1 when valid, 0 otherwise
				*/
				static int			op_is_valid				( const char * op );
				/*! Drops a table: removes it from the database, from the configuration and, for PERMANENT databases, deletes its two files.
					The remaining tables are renumbered in the configuration. A pinned table is unpinned first. Records are not kept.
					\param table_name	name of the table
					\return		__GIM_OK on success, __GIM_ERROR when the table does not exist
				*/
				_gim_flag			drop_table				( const char * table_name );
				/*! Exports the records of a table to a CSV file, in the layout of the AI trainer (label,ch0,ch0_ok,...).
					The header is the list of the field names in table order; one line per record; values as text.
					A value with a comma or a double quote is quoted (""). FLOAT values are written without the padding.
					\param table_name	name of the table
					\param path			file to write (replaced if it exists)
					\param sha256_hex	receives the SHA-256 of the written file as 64 hex digits (buffer of 65 bytes)
					\return		__GIM_OK on success, __GIM_ERROR otherwise (the reason is logged)
				*/
				_gim_flag			export_csv				( const char * table_name , const char * path , char * sha256_hex );
				/*! Imports a CSV file into a table. The header must name every field of the table exactly once, in any order.
					The whole file is checked before the first insert: columns, values and, when the table has a KEY,
					that every KEY value is new (not repeated in the file, not already in the table). Records already
					inserted by this call are removed again if an insert still fails. The KEY is optional.
					Limits: a value cannot contain a line break; the values follow the same rules as insert_record.
					\param table_name	name of the table
					\param path			file to read
					\param imported	receives the number of records added (0 on error)
					\param sha256_hex	receives the SHA-256 of the file read as 64 hex digits (buffer of 65 bytes)
					\return		__GIM_OK on success, __GIM_ERROR otherwise (the reason is logged)
				*/
				_gim_flag			import_csv				( const char * table_name , const char * path , int * imported , char * sha256_hex );
			/*! Sets a property of the database (mode, type, ...). The change is not written to the configuration file.
				\param property	property to set
				\param value	new value of the property
				\return		__GIM_OK on success
			*/
			_gim_flag			set_property			( _gim_flag property , _gim_flag value );
			/*! Creates the environment folder of the database on disk.
				\return		__GIM_OK on success
			*/
			_gim_flag			make_env				( void );
			/*! NOT IMPLEMENTED: declared only, a program that calls it does not link. Checks the consistency of the database.
				\return		__GIM_OK when the database is consistent
			*/
			_gim_flag			check					( void );
			/*! Initializes the database: reads its configuration and tables, or prepares a new configuration.
				\return		GIM_DB_READ when an existing database was read, GIM_DB_NEW when a new configuration was made, __GIM_ERROR on failure. It never returns __GIM_OK
			*/
			_gim_flag			init					( void );
			/*! Reads an existing database from its folder. Sets the name of the object.
				\param dbname	name of the database to read
				\return		__GIM_OK when the configuration file exists (also if the database could not be opened afterwards: check tables_count()), __GIM_NOT_EXIST when it does not, __GIM_ERROR for a wrong name
			*/
			_gim_flag			read					( const char * dbname );
			/*! Tells whether the configuration file of a database exists. Sets the name of the object.
				\param dbname	name of the database
				\return		__GIM_EXIST when it exists, __GIM_NOT_EXIST when it does not, __GIM_ERROR for a wrong name
			*/
			_gim_flag			exist_environment		( const char * dbname );
			/*! NOT IMPLEMENTED: declared only, a program that calls it does not link. Use the CREATE TABLE command of a script. Creates an empty table.
				\param table_name	name of the new table
				\return		__GIM_OK on success
			*/
			_gim_flag			create_table			( const char * table_name );
			/*! Tells whether a table exists. The table name is a char *, not a const char *.
				\param table_name	name of the table
				\return		__GIM_YES when the table exists, __GIM_NO when not, __GIM_ERROR when the database has no tables
			*/
			_gim_flag			table_exist				( char * table_name );
			/*! Returns the handle of a table given its name. The table name is a char *, not a const char *.
				\param table_name	name of the table
				\return		handle of the table, or NULL when it does not exist
			*/
			_gim_db_table *		seek_table				( char * table_name );
			/*! NOT IMPLEMENTED: declared only, a program that calls it does not link. Use the ADD FIELD command of a script. Adds a field to a table, without a default value.
				\param table_name	name of the table
				\param field_name	name of the new field
				\param is_key		__GIM_YES when the field is the key of the table
				\param field_type	one of the GIM_DB_TYPE_* constants
				\return		__GIM_OK on success
			*/
			_gim_flag			add_field_to_table		( const char * table_name , const char * field_name , _gim_flag is_key , _gim_flag field_type );
			/*! NOT IMPLEMENTED: declared only, a program that calls it does not link. Use the ADD FIELD command of a script. Adds a field to a table, with a default value.
				\param table_name	name of the table
				\param field_name	name of the new field
				\param is_key		__GIM_YES when the field is the key of the table
				\param field_type	one of the GIM_DB_TYPE_* constants
				\param value		default value of the field
				\return		__GIM_OK on success
			*/
			_gim_flag			add_field_to_table		( const char * table_name , const char * field_name , _gim_flag is_key , _gim_flag field_type , _gim_db_value * value );
			/*! Creates the database from a GDBS script file, and runs it. The file must exist. It works once for each object.
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
					gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gim_db_obj" , "DB : initialization error : use of db is not allowed without the application name" , __GIM_ERROR );
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


/*!	\page	gimdoc_db	Database

	A light database of tables, fields and records, kept in memory or saved on disk. \n
	Class: \ref gim_db_obj. Header: gim_db.h, with the types in gim_db_structures.h

	\tableofcontents

	\section	gimdoc_db_what	What it is for

	- A database has a name, a type (volatile or permanent) and a mode.
	- It holds tables. A table has fields, each with a name and a data type, and records.
	- A table can have one KEY field: its value cannot be repeated in the table.
	- Databases and tables are created by a \ref gimdoc_files_gdbs "GDBS script". Records are added, changed and removed
	  by the code or by the commands of the script. A whole table is removed with DROP TABLE, and a table can be copied to
	  and from a CSV file.
	- The shell \ref gimdoc_tool_gimshell works on the same databases.

	\warning	Fields cannot be removed or renamed. UPDATE changes values; DROP TABLE removes a table with all its records.

	\section	gimdoc_db_before	Before you start

	- Call \c gim_set_application_name() \b before creating a gim_db_obj. Without a name the object is not usable.
	- A database remembers the name of the application that created it. Opening it from another application is a FATAL
	  error: with the default settings of \ref gimdoc_error the program ends.
	- Create the ::gim_obj first: the database uses its memory manager, its log and its home folder.

	\section	gimdoc_db_example	Examples

	Creating a database from a script and adding a record. The script is the one in \ref gimdoc_files_gdbs.
	\code{.cpp}
	gim_set_application_name( "stationdb" );
	gim_obj * gim = new gim_obj;
	gim_db_obj * db = new gim_db_obj;

	if ( db->init_from_gdbs( "anagrafica.gdbs" ) == __GIM_OK ) {
		const char * fields[] = { "codice" , "nome" };
		const char * values[] = { "1" , "Rossi" };
		if ( db->insert_record( "clienti" , 2 , fields , values ) != __GIM_OK )
			puts( "record not added: the reason is in the log" );
	}
	delete db;
	delete gim;
	\endcode

	Opening a permanent database (the script had \c SET \c DB \c PERMANENT):
	\code{.cpp}
	gim_db_obj * db = new gim_db_obj;
	if ( ( db->exist_environment( "anagrafica" ) == __GIM_EXIST ) && ( db->read( "anagrafica" ) == __GIM_OK ) )
		printf( "%d table(s)\n" , db->tables_count() );
	delete db;
	\endcode

	Reading the records. Every record is a section of the data file of the table, and the keys are the fields:
	\code{.cpp}
	_gim_db_table * t = db->seek_table( (char *) "clienti" );
	if ( ( t != NULL ) && ( t->Tdata != NULL ) && ( t->Tdata->ParserIsUp() == __GIM_YES ) ) {
		for ( int r = 0 ; r < t->Tdata->GetHowManySection() ; r++ ) {
			char * record = t->Tdata->GetSectionName( (_gim_int16) r );
			char * name = t->Tdata->GetKeySTR( record , "nome" );
			printf( "%s\n" , name ? name : "-" );
		}
	}
	\endcode

	Changing and removing records. An empty condition list would match every record: here one condition is given.
	\code{.cpp}
	_gim_db_cond where[ 1 ];
	where[ 0 ].field = "codice";
	where[ 0 ].op    = "=";
	where[ 0 ].value = "1";

	const char * fields[] = { "nome" };
	const char * values[] = { "Bianchi" };
	int changed = 0 , removed = 0;

	db->update_record( "clienti" , 1 , where , 1 , fields , values , &changed );
	db->delete_record( "clienti" , 1 , where , &removed );
	\endcode

	Copying a table to a CSV file, and loading the file into a table with the same fields:
	\code{.cpp}
	char sha[ 65 ];
	int imported = 0;
	db->export_csv( "clienti" , "clienti.csv" , sha );
	db->import_csv( "clienti_copia" , "clienti.csv" , &imported , sha );
	\endcode

	Removing a table with all its records:
	\code{.cpp}
	db->drop_table( "clienti" );
	\endcode

	\section	gimdoc_db_methods	The methods, by purpose

	| Purpose | Methods |
	|---------|---------|
	| Create from a script | init_from_gdbs() |
	| Open an existing database | read(), exist_environment() |
	| Name and properties | set_name(), get_name(), set_property() |
	| Tables | tables_count(), table_at(), seek_table(), table_exist(), drop_table() |
	| Records | insert_record(), update_record(), delete_record() |
	| CSV files | export_csv(), import_csv() |
	| Conditions | value_matches(), op_is_valid() |

	The methods gdbs_open(), gdbs_close(), gdbs_getline(), gdbs_tokenizer(), gdbs_feof(), gdbs_line_syntax_check() and
	gdbs_execute() are the steps of init_from_gdbs(): do not call them from a program. \n
	init() is in the table of the return values below. make_env() creates the folder of a permanent database: the
	commands of the script call it. check() is not implemented.

	\section	gimdoc_db_changes	Changing and removing records

	- A \ref _gim_db_cond has a field, an operator and a value, all as text. The operators are  =  !=  <  <=  >  >= .
	  Numbers are compared as numbers, the other types as text. The conditions are joined by AND.
	- With no conditions, update_record() changes every record and delete_record() removes every record. The script and the
	  shell refuse it: a DELETE needs a WHERE.
	- update_record() checks the whole change before it changes anything: a refused update leaves the table untouched.
	  Changing the KEY is allowed only when exactly one record matches, and the new key must not be in another record.
	- No record matching is not an error: the counter says 0.
	- With a PERMANENT database, delete_record() writes the table file once, at the end.

	\section	gimdoc_db_csv	CSV files

	The layout is the one of the dataset of the AI trainer: the first line has the names of the fields in table order,
	then one line per record. A value with a comma or a double quote is quoted, with the quote doubled. FLOAT values are
	written without padding. export_csv() returns the SHA-256 of the file written, as 64 hex digits.

	import_csv() reads the whole file and checks it before the first insert: the header must name every field of the
	table exactly once, in any order; every value must follow the rules of insert_record(); when the table has a KEY,
	every key must be new, not repeated in the file and not already in the table. The KEY is optional. If an insert still
	fails, the records added by that call are removed again. It returns the SHA-256 of the file read.

	\warning	A value cannot contain a line break. With a PERMANENT database every row of import_csv() rewrites the table
				file: slow for big files. The CSV files have no GDBS command yet.

	\section	gimdoc_db_operators	Operators in scripts and in the shell

	| Where | Conditions | Operators |
	|-------|------------|-----------|
	| API: update_record(), delete_record() | any number | =  !=  <  <=  >  >= |
	| Shell: update, delete | any number | =  !=  <  <=  >  >= |
	| GDBS: UPDATE, DELETE | equality only | = (in the script, = is a separator of the language) |

	In a GDBS script the values cannot contain spaces (there are no quotes), and a line has at most 20 tokens.

	\section	gimdoc_db_types	Types and properties

	\b Database \n
	| Item | Values |
	|------|--------|
	| Type | \c GIM_DB_VOLATILE (memory only), \c GIM_DB_PERMANENT (saved on disk). The default is volatile |
	| Mode | \c GIM_DB_SAVE_MEMORY, \c GIM_DB_PERFORMANCE (default), \c GIM_DB_BALANCED. It is saved, but nothing in the code read for this page uses it |

	\b Table \n
	\c GIM_DB_TB_VOLATILE (default), \c GIM_DB_TB_PERMANENT, \c GIM_DB_TB_VIRTUAL (not stored, always volatile). A table of a volatile
	database cannot be permanent, and the other way round.

	\b Fields \n
	| Type | What insert_record() checks | How it is stored |
	|------|-----------------------------|------------------|
	| \c GIM_DB_TYPE_INT | a whole number | decimal text |
	| \c GIM_DB_TYPE_FLOAT | a number | 5 decimals |
	| \c GIM_DB_TYPE_BOOL, \c GIM_DB_TYPE_FLAG | YES, NO, 1 or 0 | YES or NO |
	| \c GIM_DB_TYPE_STRING, \c _DATE, \c _TIME, \c _TIMESTAMP, \c _PERCENTAGE | nothing | text |

	Properties of a field (SET FIELD): \c KEY, \c NOT_KEY, \c IS_INDEX, \c IS_NOT_INDEX, \c AUTOINCREMENTAL, \c IS_UNIQUE. \n
	Only \c KEY has an effect: insert_record() refuses a second record with the same key. The others, for now, are only recorded in the structure file.

	\section	gimdoc_db_disk	Files on disk

	Only for permanent databases. The folder is <tt>&lt;gim home&gt;/&lt;name of the database&gt;/</tt>, where the gim home comes from \ref gimdoc_files_conf :

	| File | Content |
	|------|---------|
	| \c name.conf | Lex B. Section \c VERSION (major, minor) and section \c DB (application_name, name, type, mode, tables, and \c tabN_name for each table) |
	| \c table.conf | Lex B. Section \c TABLE (name, file, items, type), \c STRUCT (fields_number, key_field_number, ...) and \c field (\c field_N_name, \c field_N_type) |
	| \c table_data.conf | Lex A. Each record is a section named with an MD5 identifier; its keys are the fields |

	\ref gimdoc_files_lex describes the syntaxes. The files are written by the library: do not edit them by hand.
	drop_table() of a permanent table deletes its two files. The destructor writes the configuration file again if it changed.

	\section	gimdoc_db_threads	Threads

	Not thread-safe: insert_record(), update_record(), delete_record() and the script reader use static buffers, and the
	key/value class is not thread-safe either (see \ref gimdoc_kv_threads). Use a database from one thread only.

	\section	gimdoc_db_return	Return values

	| Method | Returns |
	|--------|---------|
	| init() | \c GIM_DB_READ (existing database read), \c GIM_DB_NEW (new configuration), or __GIM_ERROR. Never __GIM_OK |
	| read() | __GIM_OK when the configuration file exists, __GIM_NOT_EXIST when it does not, __GIM_ERROR for a wrong name |
	| exist_environment() | __GIM_EXIST, __GIM_NOT_EXIST or __GIM_ERROR (wrong name) |
	| table_exist() | __GIM_YES, __GIM_NO, or __GIM_ERROR when the database has no tables |
	| insert_record() | __GIM_OK or __GIM_ERROR |
	| update_record() | __GIM_OK (also when no record matches: \a updated is 0) or __GIM_ERROR |
	| delete_record() | __GIM_OK (also when no record matches: \a deleted is 0) or __GIM_ERROR |
	| drop_table() | __GIM_OK, or __GIM_ERROR when the table does not exist |
	| export_csv(), import_csv() | __GIM_OK, or __GIM_ERROR (the reason is logged) |
	| init_from_gdbs() | __GIM_OK, __GIM_ERROR, or a negative code of the script (-5 missing ; , -6 too many tokens) |

	\warning	read() says __GIM_OK even if the database could not be opened afterwards (a wrong version, for example).
				Check the result with tables_count() and the log.

	\section	gimdoc_db_notimpl	Declared but not implemented

	These methods are in the header but have no code: a program that calls them does not link. Use a script instead.

	| Method | Use instead |
	|--------|-------------|
	| create_table() | the CREATE TABLE command |
	| add_field_to_table() | the ADD FIELD command |
	| check() | nothing yet |

	\section	gimdoc_db_limits	Warnings and limits

	- The names of the database, of the tables and of the fields are at most 63 characters. Spaces in the name of a database or a table become \c _.
	- A value of a field is at most 63 characters and cannot be empty (the key/value rules). insert_record() logs the reason as
	  CRITICAL, but it still returns __GIM_OK: the record is added without that field. Check the length before.
	- At most 255 tables and 255 fields in a table.
	- The INSERT command of a script takes the values by position, in the order of the fields: see \ref gimdoc_gdbs_insert.
	- \c SET \c DB \c VOLATILE on a name that already has a folder deletes the folder with all its files.
	- A script file that does not exist stops the program. Check it before with \c gim_file_manager->exist().
	- init_from_gdbs() works once for each object: for a second script create another gim_db_obj.
	- init_from_gdbs() can leave a half-created database if the script fails in the middle.
	- The pointers to the tables (\c _gim_db_table) and the strings returned by the key/value class are valid until the database is deleted.
	- seek_table() and table_exist() take a \c char * and not a \c const char *.
	- The fields of a table are numbered from 1 in the script. A script with the first field numbered 0 (as the old files in \c db/) loses that field when the database is read again.
	- The shell, when it saves a record, passes by insert_record(): the INSERT of a script does not do the same checks.
*/


#endif //_GIM_DB_OBJ_H_
