//
// gim_sh_table.cc
//
// Gim Shell - "table" command: list and show the tables of the current database (phase 2).
//   table list              name, type, number of fields and records of every table
//   table show <name>       fields (with type and KEY) and records of one table
//
/*#############################################################################

	Released under the terms of the GNU General Public License v2, as the rest of Gim.

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

static int table_usage( void ) {
	puts( "usage: table list" );
	puts( "       table show <name>" );
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
	return table_usage();
}
