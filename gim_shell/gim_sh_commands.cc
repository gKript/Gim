//
// gim_sh_commands.cc
//
// Gim Shell  -  a GIM application to work
// Version 0.1-0
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

#include "gim_shell.h"

typedef int ( * gim_sh_cmd_fn )( int argc , char ** argv );

struct gim_sh_command {
	const char *	name;
	gim_sh_cmd_fn	fn;
	const char *	help;
};

static int cmd_help		( int argc , char ** argv );
static int cmd_exit		( int argc , char ** argv );
static int cmd_version	( int argc , char ** argv );

// command table: to add a command, write its function and add a row here
static const gim_sh_command gim_sh_commands[] = {
	{ "help"	, cmd_help		, "list the available commands" },
	{ "version"	, cmd_version	, "show the Gim version" },
	{ "exit"	, cmd_exit		, "leave the shell" },
	{ "quit"	, cmd_exit		, "same as exit" },
	{ "db"		, cmd_db		, "manage databases: db create | list | use" },
	{ "dblist"	, cmd_dblist	, "list the databases (same as 'db list')" },
	{ "table"	, cmd_table		, "show tables: table list | show <name>" },
	{ "insert"	, cmd_insert	, "add a record: insert <table> <field> <value> ..." },
	{ "select"	, cmd_select	, "list records: select <table> [where ... [and ...]] [order by <f> [desc]] [limit n | last n]" },
	{ NULL		, NULL			, NULL }
};

static int gim_sh_strcasecmp( const char * a , const char * b ) {
	for ( ; *a && *b ; a++ , b++ ) {
		char ca = ( *a >= 'A' && *a <= 'Z' ) ? *a + 32 : *a;
		char cb = ( *b >= 'A' && *b <= 'Z' ) ? *b + 32 : *b;
		if ( ca != cb )
			return ca - cb;
	}
	return (unsigned char)*a - (unsigned char)*b;
}

int gim_sh_tokenize( char * line , char ** argv , int max_args ) {
	int argc = 0;
	char * p = line;

	while ( *p && argc < max_args ) {
		while ( *p == ' ' || *p == '\t' )
			p++;
		if ( ! *p )
			break;
		if ( *p == '"' ) {
			argv[argc++] = ++p;
			while ( *p && *p != '"' )
				p++;
		} else {
			argv[argc++] = p;
			while ( *p && *p != ' ' && *p != '\t' )
				p++;
		}
		if ( *p )
			*p++ = '\0';
	}
	return argc;
}

int gim_sh_execute( char * line ) {
	char * argv[GIM_SH_ARGV_MAX];
	int argc = gim_sh_tokenize( line , argv , GIM_SH_ARGV_MAX );

	if ( argc == 0 )
		return GIM_SH_CMD_OK;			// empty line

	for ( const gim_sh_command * c = gim_sh_commands ; c->name ; c++ ) {
		if ( gim_sh_strcasecmp( argv[0] , c->name ) == 0 )
			return c->fn( argc , argv );
	}
	printf( "gimshell: unknown command '%s' (try 'help')\n" , argv[0] );
	return GIM_SH_CMD_ERROR;
}

static int cmd_help( int argc , char ** argv ) {
	puts( "Available commands:" );
	for ( const gim_sh_command * c = gim_sh_commands ; c->name ; c++ )
		printf( "  %-10s %s\n" , c->name , c->help );
	return GIM_SH_CMD_OK;
}

static int cmd_version( int argc , char ** argv ) {
	puts( gim_version() );
	return GIM_SH_CMD_OK;
}

static int cmd_exit( int argc , char ** argv ) {
	return GIM_SH_CMD_EXIT;
}
