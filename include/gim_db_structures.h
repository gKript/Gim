//
// gim_db_structures.h
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

/*!	\file		gim_db_structures.h
	\version	2.5-0
	\brief		Data structures and constants of the Gim database (gim_db_obj).
				Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#ifndef _GIM_DB_STRUCTURES_H_
#define _GIM_DB_STRUCTURES_H_

	#include "gim_checksum.h"

 	extern char		gim_application_name[256];	/*!< name of the application that uses the database */

	#define GIM_DB_MODE				"mode"		/*!< name of the mode property in the configuration */
	#define GIM_DB_TYPE				"type"		/*!< name of the type property in the configuration */

	#define __MISSING_STOP_CHAR		-5			/*!< GDBS: the command has no terminating ';' */
	#define __TOO_MUCH_TOKEN		-6			/*!< GDBS: the command has too many tokens */
	#define __NO_NEW_LINE			-7			/*!< GDBS: the line has no line terminator */
	#define __SKIPPED				0			/*!< GDBS: the line is skipped (empty or comment) */

	#define	GDBS_SEPARATOR_STR		" =;"		/*!< GDBS: characters that separate tokens */
	#define	GDBS_TERMINATOR_CHAR	';'			/*!< GDBS: character that ends a command */
	#define GDBS_SKIP_STR			"\t"		/*!< GDBS: characters skipped at the start of a line */
	#define	GDBS_STOP_CHAR			'#'			/*!< GDBS: character that starts a comment */


	/*! Commands of the GDBS script language. */
	enum	GDBS_commands {
		__GDBS_CREATE_DB =1,	/*!< CREATE DB */
		__GDBS_CREATE_TABLE,	/*!< CREATE TABLE */
		__GDBS_ADD_FIELD,		/*!< ADD FIELD */
		__GDBS_SET_DB,			/*!< SET DB */
		__GDBS_SET_TABLE,		/*!< SET TABLE */
		__GDBS_SET_FIELD,		/*!< SET FIELD */
		__GDBS_PIN_TABLE,		/*!< PIN TABLE */
		__GDBS_UNPIN_TABLE,		/*!< UNPIN TABLE */
		__GDBS_INSERT,			/*!< INSERT */
		__GDBS_UPDATE,			/*!< UPDATE <table> SET <field> <value> [...] WHERE <field> <value> [AND ...] */
		__GDBS_DELETE,			/*!< DELETE <table> WHERE <field> <value> [AND ...] */
		__GDBS_DROP_TABLE,		/*!< DROP TABLE <table> */
	};


	/*! Storage type of the database. */
	enum	gim_db_type {
		GIM_DB_VOLATILE = 10,		/*!< the database lives in memory only */
		GIM_DB_PERMANENT,			/*!< the database is saved on disk */
	};

	/*! Working mode of the database (trade-off between memory and speed). */
	enum	gim_db_mode {
		GIM_DB_SAVE_MEMORY = 15,	/*!< use as little memory as possible */
		GIM_DB_PERFORMANCE,			/*!< use memory to be fast */
		GIM_DB_BALANCED,			/*!< compromise between the two */
	};

	/*! Data types of the fields. */
	enum	gim_db_field_data_type {
		GIM_DB_TYPE_INT = 20,		/*!< integer number */
		GIM_DB_TYPE_FLOAT,			/*!< floating point number */
		GIM_DB_TYPE_PERCENTAGE,		/*!< percentage */
		GIM_DB_TYPE_BOOL,			/*!< boolean */
		GIM_DB_TYPE_FLAG,			/*!< flag */
		GIM_DB_TYPE_STRING,			/*!< text */
		GIM_DB_TYPE_DATE,			/*!< date */
		GIM_DB_TYPE_TIME,			/*!< time */
		GIM_DB_TYPE_TIMESTAMP,		/*!< date and time (ISO 8601 text) */
		GIM_DB_IS_INDEX = 30,		/*!< field property: indexed */
		GIM_DB_IS_NOT_INDEX,		/*!< field property: not indexed */
		GIM_DB_IS_KEY,				/*!< field property: key of the table */
		GIM_DB_IS_NOT_KEY,			/*!< field property: not the key */
		GIM_DB_AUTOINCREMENTAL,		/*!< field property: value increases automatically */
		GIM_DB_IS_UNIQUE,			/*!< field property: values must be unique */
	};

	/*! Kind of a table. */
	enum	gim_db_table_properties {
		GIM_DB_TB_VOLATILE = 40,	/*!< the table lives in memory only */
		GIM_DB_TB_PERMANENT,		/*!< the table is saved on disk */
		GIM_DB_TB_VIRTUAL,			/*!< the table is not stored: it is always volatile */
	};

	/*! Context of a GDBS keyword: database, table or field. */
	enum	gim_db_context {
	    __GIM_DB = 45,				/*!< database */
		__GIM_TABLE,				/*!< table */
		__GIM_FIELD,				/*!< field */
	};

	/*! Status of a database on disk. */
	enum	gim_db_status {
		GIM_DB_NOT_EXIST = 50,		/*!< the database does not exist */
		GIM_DB_CONF_NOT_EXIST,		/*!< the configuration file does not exist */
		GIM_DB_GDB_NOT_EXIST,		/*!< the GDBS script does not exist */
		GIM_DB_OK,					/*!< the database is valid */
		GIM_DB_READ,				/*!< the database has been read */
		GIM_DB_NEW,					/*!< the database has been created */
	};

	/*! A value of a field, one member for each data type. */
	typedef union	gim_db_value_u	{
		int			Int;			/*!< GIM_DB_TYPE_INT */
		float		Float;			/*!< GIM_DB_TYPE_FLOAT */
		_gim_flag	Bool;			/*!< GIM_DB_TYPE_BOOL */
		_gim_flag	Flag;			/*!< GIM_DB_TYPE_FLAG */
		char		Char[512];		/*!< text types */
	} _gim_db_value_u;


	/*! A typed value. */
	struct _gim_db_value {
		_gim_flag			type;	/*!< one of the GIM_DB_TYPE_* constants */
		_gim_db_value_u		value;	/*!< the value */
	};


	/*! A field of a table. */
	struct _gim_db_field {
		char				name[64];	/*!< name of the field */
		_gim_flag			type;		/*!< data type of the field */
	};

	/*! A record of a table. */
	struct _gim_db_record {
		char				hash[GIM_MD5_SIZE];	/*!< unique identifier of the record (MD5 text) */
		_gim_Uint32			index;				/*!< position of the record in the table */
		_gim_list			* values;			/*!< the values of the record */
	};


	/*! A virtual table: not stored, always volatile. */
	struct _gim_db_virtual_table {
		char			name[64];		/*!< name of the table */
		char			comment[512];	/*!< comment of the table */
//		_gim_flag		type;							//	Virtual table is always GIM_DB_TB_VOLATILE
		_gim_Uint8		n_fields;		/*!< number of fields */
		_gim_Uint8		n_records;		/*!< number of records */
		_gim_Uint32		sizeof_per_record;	/*!< size of one record, in bytes */
	};


	/*! A table of the database. */
	struct _gim_db_table {
		char			name[64];		/*!< name of the table */
		char			comment[512];	/*!< comment of the table */
		char			name_data[64];	/*!< name of the data part */
		char			file_name_struct[512];		/*!< name of the structure file */
		char			file_name_data[512];		/*!< name of the data file */
		char			file_name_struct_long[512];	/*!< full path of the structure file */
		char			file_name_data_long[512];	/*!< full path of the data file */
		_gim_flag		type;							/*!< kind of the table (gim_db_table_properties) */
		_gim_int32		items;							/*!< number of records */
		_gim_Uint8		fields_number;					/*!< number of fields per record */
		_gim_int16		key_field_number;				/*!< id of the KEY field (fields are numbered from 1), -1 when there is none */
		_gim_int16		autoincrementl_field_number;	/*!< id of the AUTOINCREMENTAL field, -1 when there is none */
		_gim_int16		unique_field_number;		/*!< id of the UNIQUE field, -1 when there is none */
		_gim_Uint32		sizeof_per_record;				/*!< not used yet */

		_gim_list *		fields;			/*!< list of the _gim_db_field of the table */
		_gim_list *		records;		/*!< list of the _gim_db_record of the table */

		_gim_prsr		* Tstruct;		/*!< parser of the structure file */
		_gim_prsr		* Tdata;		/*!< parser of the data file */

	};


	/*! The main structure of a database. */
	struct _gim_db_main {
		_gim_flag			mode;			/*!< working mode (gim_db_mode) */
		_gim_flag			type;			/*!< storage type (gim_db_type) */
		_gim_Uint8			tables_number;	/*!< number of tables */

		_gim_prsr			* conf;			/*!< parser of the database configuration file */

		char				pin_table[64];	/*!< name of the pinned table (used by the GDBS SET and ADD commands) */
		_gim_flag			pin;			/*!< __GIM_YES when a table is pinned */
		_gim_db_table *		Ttab;			/*!< handle of the pinned table */
		_gim_list			* tables;		/*!< list of the _gim_db_table of the database */
	};


	/*! A label and a value of an INSERT command of a GDBS script. */
	struct _gim_gdbs_line_insert {
		char			label[64];		/*!< name of the field */
		_gim_db_value   value;			/*!< value of the field */
	};


	/*! A parsed line of a GDBS script. */
	struct _gim_gdbs_line {
		_gim_Uint32				lline;			/*!< number of the line in the script */
		_gim_flag				command;		/*!< command of the line (GDBS_commands) */
		char					fparameter[256];	/*!< first parameter */
		char					sparameter[256];	/*!< second parameter */
		char					tparameter[256];	/*!< third parameter */
		_gim_flag				fvalue;			/*!< type of the first parameter */
		_gim_flag				svalue;			/*!< type of the second parameter */
		_gim_flag				tvalue;			/*!< type of the third parameter */
		_gim_list *				ins;			/*!< pairs of label and value of an INSERT or UPDATE command (the fields to set) */
		_gim_list *				cnd;			/*!< conditions of an UPDATE or DELETE command (field and value, equality only) */
	};


	/*! One condition of a record search: field, operator and value.
		\a op is one of  =  !=  <  <=  >  >=  . Numbers (INT, FLOAT) are compared as numbers, the other types as text. */
	struct _gim_db_cond {
		const char *	field;		/*!< name of the field */
		const char *	op;			/*!< operator */
		const char *	value;		/*!< value to compare with, as text */
	};

#endif // _GIM_DB_STRUCTURES_H_
