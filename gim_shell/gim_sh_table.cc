//
// gim_sh_table.cc
//
// Gim Shell - "table" command: list and show the tables of the current database (phase 2).
//   table list              name, type, number of fields and records of every table
//   table show <name>       fields (with type and KEY) and records of one table
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

#include "gim_shell.h"
#include <stdio.h>
#include <string.h>

static const char * field_type_name( _gim_flag t ) {
	switch ( t ) {
		case GIM_DB_TYPE_INT:		return "INT";
		case GIM_DB_TYPE_FLOAT:		return "FLOAT";
		case GIM_DB_TYPE_PERCENTAGE:	return "PERCENTAGE";
		case GIM_DB_TYPE_BOOL:		return "BOOL";
		case GIM_DB_TYPE_FLAG:		return "FLAG";
		case GIM_DB_TYPE_STRING:	return "STRING";
		case GIM_DB_TYPE_DATE:		return "DATE";
		case GIM_DB_TYPE_TIME:		return "TIME";
		case GIM_DB_TYPE_TIMESTAMP:	return "TIMESTAMP";
	}
	return "?";
}

static const char * table_type_name( _gim_flag t ) {
	switch ( t ) {
		case GIM_DB_TB_VOLATILE:	return "VOLATILE";
		case GIM_DB_TB_PERMANENT:	return "PERMANENT";
		case GIM_DB_TB_VIRTUAL:		return "VIRTUAL";
	}
	return "?";
}

// number of records of a table: one section per record in its data parser
static int table_records( _gim_db_table * t ) {
	if ( t->Tdata == NULL || t->Tdata->ParserIsUp() != __GIM_YES )
		return 0;
	int n = t->Tdata->GetHowManySection();
	return ( n > 0 ) ? n : 0;
}

static int table_drop( int argc , char ** argv );
static int table_export( int argc , char ** argv );
static int table_import( int argc , char ** argv );

static int table_usage( void ) {
	puts( "usage: table list" );
	puts( "       table show <name>" );
	puts( "       table drop <name>      removes the table and its data (PERMANENT: deletes its files)" );
	puts( "       table export <name> <file.csv>   writes the records as CSV (label,ch0,ch0_ok,... layout)" );
	puts( "       table import <name> <file.csv>   adds the records of a CSV file (checked first; a KEY must stay unique)" );
	return GIM_SH_CMD_ERROR;
}

static int table_list( void ) {
	gim_db_obj * db = gim_sh_current_db();
	if ( db == NULL ) {
		puts( "no database selected (use 'db use <name>' or 'db create')" );
		return GIM_SH_CMD_ERROR;
	}

	_gim_Uint8 n = db->tables_count();
	if ( n == 0 ) {
		puts( "no tables in the current database" );
		return GIM_SH_CMD_OK;
	}

	printf( "%-20s %-10s %8s %8s\n" , "TABLE" , "TYPE" , "FIELDS" , "RECORDS" );
	for ( _gim_Uint8 i = 1 ; i <= n ; i++ ) {
		_gim_db_table * t = db->table_at( i );
		if ( t == NULL )
			continue;
		printf( "%-20s %-10s %8d %8d\n" , t->name , table_type_name( t->type ) , ( int ) t->fields_number , table_records( t ) );
	}
	return GIM_SH_CMD_OK;
}

static int table_show( int argc , char ** argv ) {
	if ( argc < 3 )
		return table_usage();

	gim_db_obj * db = gim_sh_current_db();
	if ( db == NULL ) {
		puts( "no database selected (use 'db use <name>' or 'db create')" );
		return GIM_SH_CMD_ERROR;
	}

	_gim_db_table * t = db->seek_table( argv[ 2 ] );
	if ( t == NULL ) {
		printf( "table '%s' not found\n" , argv[ 2 ] );
		return GIM_SH_CMD_ERROR;
	}

	int key = ( t->Tstruct != NULL ) ? t->Tstruct->GetKeyINT( "STRUCT" , "key_field_number" ) : -1;
	printf( "table %s   type %s   fields %d   records %d\n" , t->name , table_type_name( t->type ) ,
			( int ) t->fields_number , table_records( t ) );

	puts( "fields:" );
	if ( t->fields != NULL ) {
		int i = 0;
		t->fields->rewind();
		for ( ; i < t->fields->items() ; i++ ) {
			_gim_db_field * f = (_gim_db_field *) t->fields->get_item();
			if ( f == NULL )
				break;
			printf( "  %2d  %-20s %-12s%s\n" , i + 1 , f->name , field_type_name( f->type ) , ( ( i + 1 ) == key ) ? "  KEY" : "" );
			t->fields->next_item();
		}
	}

	int n = table_records( t );
	if ( n > 0 ) {
		puts( "records:" );
		for ( int s = 0 ; s < n ; s++ ) {
			char * sec = t->Tdata->GetSectionName( ( _gim_int16 ) s );
			if ( sec == NULL )
				continue;
			printf( "  [%d] " , s + 1 );
			int keys = t->Tdata->GetHowManyKey( sec );
			for ( int k = 0 ; k < keys ; k++ ) {
				char * kn = t->Tdata->GetKeyName( sec , ( _gim_int16 ) k );
				char * kv = ( kn != NULL ) ? t->Tdata->GetKeySTR( sec , kn ) : NULL;
				printf( "%s%s=%s" , k ? ", " : "" , kn ? kn : "?" , kv ? kv : "?" );
			}
			puts( "" );
		}
	}
	else
		puts( "records: none" );
	return GIM_SH_CMD_OK;
}

int cmd_table( int argc , char ** argv ) {
	if ( argc < 2 )
		return table_usage();
	if ( strcmp( argv[ 1 ] , "list" ) == 0 )
		return table_list();
	if ( strcmp( argv[ 1 ] , "show" ) == 0 )
		return table_show( argc , argv );
	if ( strcmp( argv[ 1 ] , "drop" ) == 0 )
		return table_drop( argc , argv );
	if ( strcmp( argv[ 1 ] , "export" ) == 0 )
		return table_export( argc , argv );
	if ( strcmp( argv[ 1 ] , "import" ) == 0 )
		return table_import( argc , argv );
	return table_usage();
}

// table drop <name>: removes the table; the records go with it
static int table_drop( int argc , char ** argv ) {
	if ( argc < 3 )
		return table_usage();
	gim_db_obj * db = gim_sh_current_db();
	if ( db == NULL ) {
		puts( "no database selected (use 'db use <name>' or 'db create')" );
		return GIM_SH_CMD_ERROR;
	}
	if ( db->drop_table( argv[ 2 ] ) != __GIM_OK ) {
		printf( "table drop: '%s' not dropped (see the messages above)\n" , argv[ 2 ] );
		return GIM_SH_CMD_ERROR;
	}
	printf( "table '%s' dropped\n" , argv[ 2 ] );
	return GIM_SH_CMD_OK;
}

// table export <name> <file.csv>: writes the records and prints the SHA-256 of the file
static int table_export( int argc , char ** argv ) {
	if ( argc < 4 ) {
		puts( "usage: table export <name> <file.csv>" );
		return GIM_SH_CMD_ERROR;
	}
	gim_db_obj * db = gim_sh_current_db();
	if ( db == NULL ) {
		puts( "no database selected (use 'db use <name>' or 'db create')" );
		return GIM_SH_CMD_ERROR;
	}
	char sha[ 65 ];
	if ( db->export_csv( argv[ 2 ] , argv[ 3 ] , sha ) != __GIM_OK ) {
		printf( "table export: '%s' not exported (see the messages above)\n" , argv[ 2 ] );
		return GIM_SH_CMD_ERROR;
	}
	printf( "table '%s' exported to %s\nsha256 %s\n" , argv[ 2 ] , argv[ 3 ] , sha );
	return GIM_SH_CMD_OK;
}

// table import <name> <file.csv>: adds the records of the file and prints the SHA-256 of the file read
static int table_import( int argc , char ** argv ) {
	if ( argc < 4 ) {
		puts( "usage: table import <name> <file.csv>" );
		return GIM_SH_CMD_ERROR;
	}
	gim_db_obj * db = gim_sh_current_db();
	if ( db == NULL ) {
		puts( "no database selected (use 'db use <name>' or 'db create')" );
		return GIM_SH_CMD_ERROR;
	}
	int n = 0;
	char sha[ 65 ];
	if ( db->import_csv( argv[ 2 ] , argv[ 3 ] , &n , sha ) != __GIM_OK ) {
		printf( "table import: nothing imported into '%s' (see the messages above)\n" , argv[ 2 ] );
		return GIM_SH_CMD_ERROR;
	}
	printf( "%d record(s) imported into '%s' from %s\nsha256 %s\n" , n , argv[ 2 ] , argv[ 3 ] , sha );
	return GIM_SH_CMD_OK;
}
