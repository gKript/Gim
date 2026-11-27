//
// gim_sh_db.cc
//
// Gim Shell - "db" command: create, list and select Gim databases (phase 1).
//   db create <name> [script.gdbs]   create from a GDBS script, or an empty volatile DB
//   db list                          list the permanent databases in the Gim home
//   db use <name>                    load a permanent database and make it current
//   db                               show the current database
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
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// the database currently selected in the shell (NULL = none)
static gim_db_obj * sh_db = NULL;

// case insensitive, like the command names
static int sh_same( const char * a , const char * b ) {
	for ( ; *a && *b ; a++ , b++ ) {
		if ( tolower( (unsigned char) *a ) != tolower( (unsigned char) *b ) )
			return 0;
	}
	return *a == *b;
}

static void db_close_current( void ) {
	if ( sh_db ) {
		delete sh_db;
		sh_db = NULL;
	}
}

static int db_usage( void ) {
	puts( "usage: db create <name> [script.gdbs]" );
	puts( "       db list" );
	puts( "       db use <name>" );
	puts( "       db            (show the current database)" );
	return GIM_SH_CMD_ERROR;
}

static int db_create( int argc , char ** argv ) {
	if ( argc < 3 )
		return db_usage();

	const char * name = argv[ 2 ];
	const char * script = ( argc > 3 ) ? argv[ 3 ] : NULL;
	gim_db_obj * d = new gim_db_obj;

	if ( script ) {
		_gim_flag r = d->init_from_gdbs( script );
		if ( r != __GIM_OK ) {
			printf( "db create: script '%s' is not valid (code %d), no database created\n" , script , ( int ) r );
			delete d;
			return GIM_SH_CMD_ERROR;
		}
		if ( strcmp( d->get_name() , name ) != 0 )
			printf( "db create: note, the script created the database '%s' (you asked for '%s')\n" , d->get_name() , name );
	}
	else {
		d->set_name( name );
		d->init();
	}

	db_close_current();
	sh_db = d;
	printf( "database '%s' ready (current)\n" , d->get_name() );
	return GIM_SH_CMD_OK;
}

static int db_list( void ) {
	gim_directory_obj dir( env_data->home , __GIM_NO , __GIM_YES );
	int found = 0;

	// get_item() returns NULL when the list is over
	for ( ;; ) {
		_gim_dir_item * item = dir.get_item( GIM_DIR_INCREMENT );
		if ( item == NULL )
			break;

		// a permanent database is a folder <home>/<name>/ holding <name>.conf
		const char * base = item->name;
		for ( const char * p = item->name ; *p ; p++ ) {
			if ( *p == '/' || *p == '\\' )
				base = p + 1;
		}
		char conf[ 2100 ];
		snprintf( conf , sizeof( conf ) , "%s/%s.conf" , item->name , base );
		FILE * f = fopen( conf , "r" );
		if ( f != NULL ) {
			fclose( f );
			int is_current = ( sh_db != NULL ) && ( strcmp( sh_db->get_name() , base ) == 0 );
			printf( "  %s%s\n" , base , is_current ? "   (current)" : "" );
			found++;
		}
		dir.free_item( item );
	}

	if ( found == 0 )
		puts( "no permanent database found (volatile databases are not saved)" );
	return GIM_SH_CMD_OK;
}

static int db_use( int argc , char ** argv ) {
	if ( argc < 3 )
		return db_usage();

	gim_db_obj * d = new gim_db_obj;
	if ( d->read( argv[ 2 ] ) != __GIM_OK ) {
		printf( "db use: database '%s' not found\n" , argv[ 2 ] );
		delete d;
		return GIM_SH_CMD_ERROR;
	}

	db_close_current();
	sh_db = d;
	printf( "database '%s' selected (current)\n" , d->get_name() );
	return GIM_SH_CMD_OK;
}

int cmd_db( int argc , char ** argv ) {
	if ( argc < 2 ) {
		if ( sh_db )
			printf( "current database: '%s'\n" , sh_db->get_name() );
		else
			puts( "no database selected" );
		return GIM_SH_CMD_OK;
	}
	if ( sh_same( argv[ 1 ] , "create" ) )
		return db_create( argc , argv );
	if ( sh_same( argv[ 1 ] , "list" ) )
		return db_list();
	if ( sh_same( argv[ 1 ] , "use" ) )
		return db_use( argc , argv );
	return db_usage();
}

// 'dblist' is a shortcut for 'db list'
int cmd_dblist( int argc , char ** argv ) {
	return db_list();
}

void gim_sh_db_shutdown( void ) {
	db_close_current();
}

gim_db_obj * gim_sh_current_db( void ) {
	return sh_db;
}
