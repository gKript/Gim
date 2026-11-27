//
// gim_db.cc
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

/*!	\file		gim_db.cc
	\version	2.5-0
	\brief		Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include "../include/gim_base_header.h"
#include "../include/gim_db.h"
#include "../include/gim_environment.h"
#include "../include/gim_checksum.h"
#include "../include/gim_sha256.h"
#include "../include/gim_file.h"


_gim_flag	gim_db_obj::set_name( const char * dbname ) {
	if ( ( strlen( dbname ) > 64 ) || ( strlen( dbname ) < 1 ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::set_name" , "The db name must be lenght between 1 and 64 std characters" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	char	tmp_name[64];

	strcpy( tmp_name , dbname );
	strcpy( name	, lex->char_subst( tmp_name , ' ' , '_' ) );
	sprintf( home	, "%s%s" , env_data->home , name );
	sprintf( file_name	, "%s.conf" , name );
	sprintf( file_name_long	, "%s/%s" , home , file_name );
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::set_name" , "DB : db name       [%s]" , name );
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::set_name" , "DB : db Fname      [%s]" , file_name );
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::set_name" , "DB : db home       [%s]" , home );
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::set_name" , "DB : db home/Fname [%s]" , file_name_long );
	return __GIM_OK;
}


_gim_flag	gim_db_obj::set_property( _gim_flag property , _gim_flag value ) {
	switch ( property ) {
		case GIM_DB_SAVE_MEMORY : {
			if ( value == __GIM_YES ) {
				db->mode = GIM_DB_SAVE_MEMORY;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "property" , "mode" , GIM_DB_SAVE_MEMORY );
				gim_error->set( "gim_db_obj::set_db_property" , "DB : mode setted to GIM_DB_SAVE_MEMORY" );
			}
			else {
				db->mode = GIM_DB_BALANCED;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "property" , "mode" , GIM_DB_BALANCED );
				gim_error->set( "gim_db_obj::set_db_property" , "DB : mode setted per default to GIM_DB_BALANCED" );
			}
			break;
		}
		case GIM_DB_PERFORMANCE : {
			if ( value == __GIM_YES ) {
				db->mode = GIM_DB_PERFORMANCE;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "property" , "mode" , GIM_DB_PERFORMANCE );
				gim_error->set( "gim_db_obj::set_db_property" , "DB : mode setted to GIM_DB_PERFORMANCE" );
			}
			else {
				db->mode = GIM_DB_BALANCED;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "property" , "mode" , GIM_DB_BALANCED );
				gim_error->set( "gim_db_obj::set_db_property" , "DB : mode setted per default to GIM_DB_BALANCED" );
			}
			break;
		}
		case GIM_DB_BALANCED : {
			if ( value == __GIM_YES ) {
				db->mode = GIM_DB_BALANCED;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "property" , "mode" , GIM_DB_BALANCED );
				gim_error->set( "gim_db_obj::set_db_property" , "DB : mode setted to GIM_DB_BALANCED" );
			}
			else {
				db->mode = GIM_DB_BALANCED;
				gim_error->set( "gim_db_obj::set_db_property" , "DB : mode setted per default to GIM_DB_BALANCED" );
				gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::set_db_property" , "DB :     You can't set GIM_DB_BALANCED to __GIM_MO" , __GIM_ERROR );
				changed		= __GIM_NO;
				return __GIM_ERROR;
			}
			break;
		}
		case GIM_DB_VOLATILE : {
			if ( value == __GIM_YES ) {
				db->type = GIM_DB_VOLATILE;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "property" , "type" , GIM_DB_VOLATILE );
				gim_error->set( "gim_db_obj::set_db_property" , "DB : type setted to GIM_DB_VOLATILE" );
			}
			else {
				db->mode = GIM_DB_PERMANENT;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "property" , "type" , GIM_DB_PERMANENT );
				gim_error->set( "gim_db_obj::set_db_property" , "DB : mode setted per default to GIM_DB_PERMANENT" );
			}
			break;
		}
		case GIM_DB_PERMANENT : {
			if ( value == __GIM_YES ) {
				db->type = GIM_DB_PERMANENT;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "property" , "type" , GIM_DB_PERMANENT );
				gim_error->set( "gim_db_obj::set_db_property" , "DB : type setted to GIM_DB_PERMANENT" );
			}
			else {
				db->mode = GIM_DB_VOLATILE;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "property" , "type" , GIM_DB_VOLATILE );
				gim_error->set( "gim_db_obj::set_db_property" , "DB : mode setted per default to GIM_DB_VOLATILE" );
			}
			break;
		}
		default : {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::set_db_property" , "DB :     Unknown property." , __GIM_ERROR );
			changed		= __GIM_NO;
			return __GIM_ERROR;
		}
	}
	if ( initiated == __GIM_YES )
		changed		= __GIM_YES;
	return __GIM_OK;
}




_gim_flag	gim_db_obj::make_env( void ) {
	struct stat stat_home;
	if ( ( strlen( name ) > 64 ) || ( strlen( name ) < 1 ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::make_env" , "You try to create the DB env but before you must to set the DB name" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( db->type == GIM_DB_PERMANENT ) {
		if ( stat( home	, &stat_home ) ) {
			mkdir( home	, 0777 );
			gim_error->set( "gim_db_obj::make_env" , "DB : env created" );
		}
		else 
			gim_error->set( "gim_db_obj::make_env" , "DB : env already exist" );
	}
	else if ( db->type == GIM_DB_VOLATILE ) {
		if ( stat( home	, &stat_home ) )
			gim_error->set( "gim_db_obj::make_env" , "DB : env not created : Volatile db" );
		else {
			gim_error->set( "gim_db_obj::make_env" , "DB : DB is set as VOLATILE but env exists. I remove it..." );
			char command[256];
			sprintf( command , "rm -rf %s" , home );
			system( command );
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::make_env" , "The DB state is unknown" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	return __GIM_OK;
}


_gim_flag	gim_db_obj::init( void ) {
	_gim_flag	res;
	initiated = __GIM_NO;
	switch( db->conf->Read( file_name_long ) ) {
		case __GIM_OK : {
			gim_error->set( "gim_db_obj::init" , "DB : Properities read succesfully" );
			db->tables_number = db->conf->GetKeyINT( "DB" , "tables" );
			db->type = db->conf->GetKeyINT( "DB" , "type" );
			db->mode = db->conf->GetKeyINT( "DB" , "mode" );
			db->tables_number = db->conf->GetKeyINT( "DB" , "tables" );
			res = GIM_DB_READ;
			break;
		}
		case __GIM_NOT_EXIST : {
			db->conf->Up( file_name_long , name );
			db->conf->SetLex( __LEX_B );
			db->conf->AddSection( "VERSION" );
			db->conf->AddKey( "VERSION" , "major"				, GIM_MAJOR );
			db->conf->AddKey( "VERSION"	, "minor"				, GIM_MINOR );
			db->conf->AddSection( "DB" );
			db->conf->AddKey( "DB"		, "application_name"	, gim_application_name );
			db->conf->AddKey( "DB"		, "name"				, name );
			db->conf->AddKey( "DB"		, "type"				, db->type );
			db->conf->AddKey( "DB"		, "mode"				, db->mode );
			db->conf->AddKey( "DB"		, "tables"				, db->tables_number );
			if ( db->type == GIM_DB_PERMANENT ) {
				make_env();
				db->conf->Write();
				gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::init" , "DB : Conf file wrote" , __GIM_ERROR );
			}
			if ( db->type == GIM_DB_VOLATILE ) {
				gim_error->set( "gim_db_obj::init" , "DB : type VOLATILE : conf only in memory" );
			res = GIM_DB_NEW;
			}
			break;
		}
		case __LEX_UNKNOWN : {
			gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::init" , "DB : Conf file Lex unknown. Rewriting" , __GIM_ERROR );
			db->conf->Up( file_name , name );
			db->conf->SetLex( __LEX_B );
			db->conf->AddSection( "VERSION" );
			db->conf->AddKey( "VERSION" , "major"				, GIM_MAJOR );
			db->conf->AddKey( "VERSION"	, "minor"				, GIM_MINOR );
			db->conf->AddSection( "DB" );
			db->conf->AddKey( "DB"		, "application_name"	, gim_application_name );
			db->conf->AddKey( "DB"		, "name"				, name );
			db->conf->AddKey( "DB"		, "file"				, file_name );
			db->conf->AddKey( "DB"		, "type"				, db->type );
			db->conf->AddKey( "DB"		, "mode"				, db->mode );
			db->conf->AddKey( "DB"		, "tables"				, db->tables_number );
			db->conf->Write();
			res = GIM_DB_NEW;
			break;
		}
		case __SYNTAX_ERROR : {
			db->conf->Up( file_name , name );
			db->conf->SetLex( __LEX_B );
			db->conf->AddSection( "VERSION" );
			db->conf->AddKey( "VERSION" , "major"				, GIM_MAJOR );
			db->conf->AddKey( "VERSION"	, "minor"				, GIM_MINOR );
			db->conf->AddSection( "DB" );
			db->conf->AddKey( "DB"		, "application_name"	, gim_application_name );
			db->conf->AddKey( "DB"		, "name"				, name );
			db->conf->AddKey( "DB"		, "file"				, file_name );
			db->conf->AddKey( "DB"		, "type"				, db->type );
			db->conf->AddKey( "DB"		, "mode"				, db->mode );
			db->conf->AddKey( "DB"		, "tables"				, db->tables_number );
			db->conf->Write();
			gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::init" , "DB : Some syntax error in the conf file. Rewriting" , __GIM_ERROR );
			res = GIM_DB_NEW;
			break;
		}
	}
	if ( res == GIM_DB_READ ) {
		initiated	= __GIM_YES;

		if ( Lexical.str_equal( db->conf->GetKeySTR( "DB" , "application_name" ) , gim_application_name  ) != __GIM_YES ) {
			gim_error->Set( GIM_ERROR_FATAL , "gim_db_obj::init" , "DB : WRONG APPLICATION NAME [DB %s - APP %s]" , db->conf->GetKeySTR( "DB" , "application_name" ) , gim_application_name );
			initiated	= __GIM_NO;
			return __GIM_ERROR; 
		}
		if ( gim_db_version( db->conf->GetKeyINT( "VERSION" , "major" ) , db->conf->GetKeyINT( "VERSION" , "minor" ) ) == __GIM_OK ) 
			gim_error->set( "gim_db_obj::init" , "DB : This version of Gim is compatible" );
		else {
			gim_error->set( GIM_ERROR_FATAL , "gim_db_obj::init" , "DB : This version of Gim is not compatible with this DB" , __GIM_ERROR );
			initiated	= __GIM_NO;
			gdbs_run = __GIM_NOT_READY;
			return __GIM_ERROR;
		}
		if ( db->tables_number ) {
			read_tables();
		}
	}
	return res;
}

_gim_flag   gim_db_obj::read_tables( void ) {
	char	DB_Tname[32];

	for ( _gim_Uint8 c = 1 ; c <= db->tables_number ; c++ ) {
		sprintf( DB_Tname , "tab%d_name" , c );
		_gim_db_table * Ttmp = (_gim_db_table *)gim_memory->Alloc( sizeof( _gim_db_table ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
		Ttmp->Tstruct = new _gim_prsr;
		Ttmp->Tdata = new _gim_prsr;
		strcpy( Ttmp->name		, db->conf->GetKeySTR( "DB" , DB_Tname ) );
		strcpy( Ttmp->comment   , "" );
		sprintf( Ttmp->file_name_struct			,   "%s.conf" , Ttmp->name );
		sprintf( Ttmp->file_name_data			,	"%s_data.conf" , Ttmp->name );
		sprintf( Ttmp->file_name_struct_long	,   "%s/%s" , home , Ttmp->file_name_struct );
		sprintf( Ttmp->file_name_data_long		,	"%s/%s" , home , Ttmp->file_name_data );
		Ttmp->Tstruct->Read( Ttmp->file_name_struct_long );
			Ttmp->Tdata->Read( Ttmp->file_name_data_long );		// the records (missing file = no records yet)

		Ttmp->type  = Ttmp->Tstruct->GetKeyINT( "TABLE"   , "type" );
		Ttmp->items = Ttmp->Tstruct->GetKeyINT( "TABLE"   , "items" );
		Ttmp->fields_number = Ttmp->Tstruct->GetKeyINT( "STRUCT"  , "fields_number" );
		Ttmp->key_field_number = Ttmp->Tstruct->GetKeyINT( "STRUCT"  , "key_field_number" );
		Ttmp->sizeof_per_record = 0;
		Ttmp->fields  = new _gim_list;
		Ttmp->records = new _gim_list;

		if ( Ttmp->fields_number ) {
			for ( _gim_Uint8 f = 1 ; f <= Ttmp->fields_number ; f++ ) {	// field ids are 1..N (as written by ADD FIELD)
				char Fname[128];
				char Ftype[128];
				sprintf( Fname , "field_%d_name" , f );
				sprintf( Ftype , "field_%d_type" , f );
				_gim_db_field * Tfield = (_gim_db_field *)gim_memory->Alloc( sizeof( _gim_db_field ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
				if ( Ttmp->Tstruct->GetKeySTR( "field" , Fname ) == NULL ) {
						gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::read_tables" , "field key %s missing in table %s" , Fname , Ttmp->name );
						continue;
					}
					strcpy( Tfield->name , Ttmp->Tstruct->GetKeySTR( "field" , Fname ) ); 
				Tfield->type = Ttmp->Tstruct->GetKeyINT( "field"   , Ftype );
				Ttmp->fields->add_item( Tfield );
				gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::make_add_field" , "[id field %d] [type %d] [name %s]" , f , Tfield->type , Tfield->name );
			}
		}
		
		db->tables->add_item( Ttmp );
	}
	return __GIM_OK;
}


_gim_flag   gim_db_obj::gdbs_feof( void ) {
	if ( syntax->Feof == __GIM_OFF )
		return __GIM_NO;
	return __GIM_YES;
}


_gim_flag   gim_db_obj::read( const char * dbname ) {
	if ( ( strlen( dbname ) > 64 ) || ( strlen( dbname ) < 1 ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::read" , "The db name must be lenght between 1 and 64 std characters" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	char		tmp_name[64];
	_gim_flag   result;

	strcpy( tmp_name	, dbname );
	strcpy( name	, lex->char_subst( tmp_name , ' ' , '_' ) );
	sprintf( home	, "%s%s" , env_data->home , name );
	sprintf( file_name	, "%s.conf" , name );
	sprintf( file_name_long	, "%s/%s" , home , file_name );
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::read" , " Reading DB : %s" , lex->string_trunc( file_name_long , 60 ) );
	result = db->conf->Exist( file_name_long );
	if ( result == __GIM_NOT_EXIST ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::read" , "I cannot found the DB file." , __GIM_ERROR );
		return __GIM_NOT_EXIST;
	}
	init();
	return __GIM_OK;
}


_gim_flag	gim_db_obj::exist_environment( const char * dbname ) {
	if ( ( strlen( dbname ) > 64 ) || ( strlen( dbname ) < 1 ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::exist_environment" , "The db name must be lenght between 1 and 64 std characters" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	char		tmp_name[64];
	_gim_flag   result;
	strcpy( tmp_name	, dbname );
	strcpy( name	, lex->char_subst( tmp_name , ' ' , '_' ) );
	sprintf( home	, "%s%s" , env_data->home , name );
	sprintf( file_name	, "%s.conf" , name );
	sprintf( file_name_long	, "%s/%s" , home , file_name );
	result = db->conf->Exist( file_name_long );
	if ( result != __GIM_EXIST ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::exist_environment" , "I cannot found the DB environment." , __GIM_ERROR );
		return __GIM_NOT_EXIST;
	}
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::exist_environment" , "DB environment succesfully found." );
	return __GIM_EXIST;
}


char * 	gim_db_obj::gdbs_getline( void ) {
	if ( ! gdbs_file ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::gdbs_getline" , "No open file" , __GIM_ERROR  );
		return NULL;
	}

	static char line[PRSR_MAX_LINE];

	__GIM_VCLEAR( line , PRSR_MAX_LINE , char ,'\0' );
	fgets( line , PRSR_MAX_LINE , gdbs_file->fp );
	if ( feof( gdbs_file->fp ) ) 
		syntax->Feof = __GIM_ON;
	strcat( line, "\0" );
	return line;
}


_gim_flag	gim_db_obj::gdbs_open( void ) {
	if ( !strlen( gdbs_file_name ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::gdbs_open" , "No file name set" , __GIM_ERROR  );
		return __GIM_ERROR;
	}

	gdbs_file = gim_file_manager->open( gdbs_file_name , __GIM_FILE_POINTER , __GIM_READ );

	if ( ! gdbs_file ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::gdbs_open" , "File not found" , __GIM_ERROR  );
		return __GIM_ERROR;
	}
	return __GIM_OK;
}


void	gim_db_obj::gdbs_close( void ) {
	if ( !strlen( gdbs_file_name ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::gdbs_close" , "No file name set" , __GIM_ERROR  );
	}
	else if ( ! gdbs_file ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::gdbs_close" , "File is not open" , __GIM_ERROR  );
	}
	else {
		gim_file_manager->close( gdbs_file );
		gim_error->set( "prsr_lexical_class::gdbs_close" , "File closed" );
	}
}


/* ---- GDBS helpers for UPDATE and DELETE (they read the global token array Tok) ---- */

// keyword check, case insensitive: kw is the keyword in capitals
static int gdbs_kw( const char * word , const char * kw ) {
	for ( ; *word && *kw ; word++ , kw++ )
		if ( toupper( (unsigned char) *word ) != *kw )
			return 0;
	return *word == '\0' && *kw == '\0';
}

// position of the WHERE token, searching from the 4th token; -1 when missing
static int gdbs_find_where( int NoT ) {
	for ( int t = 3 ; t < NoT ; t++ )
		if ( gdbs_kw( Tok[ t ] , "WHERE" ) )
			return t;
	return -1;
}

// counts the <field> <value> conditions from token t to NoT, joined by AND; -1 when the syntax is wrong
static int gdbs_count_conds( int t , int NoT ) {
	int n = 0;
	while ( t < NoT ) {
		if ( t + 1 >= NoT )
			return -1;				// a field without its value
		n++;
		t += 2;
		if ( t < NoT ) {
			if ( ! gdbs_kw( Tok[ t ] , "AND" ) )
				return -1;
			t++;
			if ( t >= NoT )
				return -1;			// AND with nothing after it
		}
	}
	return n;
}

_gim_flag	gim_db_obj::gdbs_line_syntax_check( _gim_Uint8 NoT ) {
	static _gim_Uint8   id;

	id = 0;
	if      ( ( lex->str_equal( Tok[0] , "CREATE" ) == __GIM_YES ) && ( NoT == 3 ) )
		id += 10;
	else if ( ( lex->str_equal( Tok[0] , "ADD" ) == __GIM_YES ) && ( ( NoT == 5 ) || ( NoT == 6 ) ) )
		id += 20;
	else if ( ( lex->str_equal( Tok[0] , "SET" ) == __GIM_YES ) && ( ( NoT == 4 ) || ( NoT == 3 ) ) )
		id += 30;
	else if ( ( lex->str_equal( Tok[0] , "PIN" ) == __GIM_YES ) && ( NoT == 3 ) )
		id += 40;
	else if ( ( lex->str_equal( Tok[0] , "UNPIN" ) == __GIM_YES ) && ( NoT == 2 ) )
		id += 50;
	else if ( ( lex->str_equal( Tok[0] , "INSERT" ) == __GIM_YES ) && ( NoT >= 4 ) )
		id += 60;
	else if ( ( lex->str_equal( Tok[0] , "UPDATE" ) == __GIM_YES ) && ( NoT >= 8 ) )
		id += 70;
	else if ( ( lex->str_equal( Tok[0] , "DELETE" ) == __GIM_YES ) && ( NoT >= 4 ) )
		id += 80;
	else if ( ( lex->str_equal( Tok[0] , "DROP" ) == __GIM_YES ) && ( NoT == 3 ) )
		id += 90;
	else
		id = 0;

	if      ( ( lex->str_equal( Tok[1] , "DB" ) == __GIM_YES ) && ( id == 10 ) )
		id += 1;
	else if ( ( lex->str_equal( Tok[1] , "TABLE" ) == __GIM_YES ) && ( id == 10 ) )
		id += 2;
	else if ( ( lex->str_equal( Tok[1] , "FIELD" ) == __GIM_YES ) && ( id == 20 ) )
		id += 1;
	else if ( ( lex->str_equal( Tok[1] , "DB" ) == __GIM_YES ) && ( id == 30 ) )
		id += 1;
	else if ( ( lex->str_equal( Tok[1] , "TABLE" ) == __GIM_YES ) && ( id == 30 ) )
		id += 2;
	else if ( ( lex->str_equal( Tok[1] , "FIELD" ) == __GIM_YES ) && ( id == 30 ) )
		id += 3;
	else if ( ( lex->str_equal( Tok[1] , "TABLE" ) == __GIM_YES ) && ( id == 40 ) )
		id += 1;
	else if ( ( lex->str_equal( Tok[1] , "TABLE" ) == __GIM_YES ) && ( id == 50 ) )
		id += 1;
	else if ( id == 60 )
		id += 1;
	else if ( ( id == 70 ) && ( lex->str_equal( Tok[2] , "SET" ) == __GIM_YES ) )
		id += 1;
	else if ( ( id == 80 ) && ( lex->str_equal( Tok[2] , "WHERE" ) == __GIM_YES ) )
		id += 1;
	else if ( ( id == 90 ) && ( lex->str_equal( Tok[1] , "TABLE" ) == __GIM_YES ) )
		id += 1;
	else
		id = 0;

	if ( id == 0 ) {
		gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN TOKEN : ID = %2d - NoT = %d - [%s] [%s]", id , NoT , Tok[0] , Tok[1] );
		return __SYNTAX_ERROR;
	}
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_line_syntax_check" , "ID = %2d - NoT = %d - [%s] [%s]", id , NoT , Tok[0] , Tok[1] );
	switch ( id ) {
		case 11 : {
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_CREATE_DB;
			strcpy( line->fparameter , Tok[2] );
			gdbs_script->add_item( (void *)line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! CREATE DB command added to list." );
			return __GDBS_CREATE_DB;
		}
		case 12 : {
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_CREATE_TABLE;
			strcpy( line->fparameter , Tok[2] );
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! CREATE TABLE command added to list." );
			return __GDBS_CREATE_DB;
		}
		case 21 : {
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_ADD_FIELD;
			line->fvalue = atoi( Tok[2] );
			line->svalue = string_to_flag( 3 , __GIM_FIELD );
			if ( line->svalue == __GIM_ERROR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN PROPERTIES [%s]." , Tok[3] );
				return __SYNTAX_ERROR;
			}
			strcpy( line->tparameter , Tok[4] );
			if ( NoT == 6 ) 
				line->tvalue = atoi( Tok[5] );
			gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_line_syntax_check" , "[id field %d] [type %d] [name %s]" , line->fvalue , line->svalue , line->tparameter );
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! ADD FIELD command added to list." );
			return __GDBS_ADD_FIELD;
		}
		case 31 : {
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_SET_DB;
			line->fvalue = string_to_flag( 2 , __GIM_DB );
			if ( line->fvalue == __GIM_ERROR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN PROPERTIES [%s]." , Tok[2] );
				return __SYNTAX_ERROR;
			}
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! SET DB command added to list." );
			return __GDBS_SET_DB;
		}
		case 32 : {
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_SET_TABLE;
			line->fvalue = string_to_flag( 2 , __GIM_TABLE );
			if ( line->fvalue == __GIM_ERROR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN PROPERTIES [%s]." , Tok[2] );
				return __SYNTAX_ERROR;
			}
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! SET TABLE command added to list." );
			return __GDBS_SET_TABLE;
		}
		case 33 : {
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_SET_FIELD;
			line->fvalue = atoi( Tok[2] );
			line->svalue = string_to_flag( 3 , __GIM_FIELD );
			if ( line->svalue == __GIM_ERROR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN PROPERTIES [%s]." , Tok[3] );
				return __SYNTAX_ERROR;
			}
//			printf( "%s = %d  -  %s = %d\n" ,  Tok[2] , line->fvalue , Tok[3] , line->svalue );
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! SET TABLE command added to list." );
			return __GDBS_SET_FIELD;
		}
		case 41 : {
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_PIN_TABLE;
			strcpy( line->fparameter , Tok[2] );
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! SET TABLE command added to list." );
			return __GDBS_PIN_TABLE;
		}
		case 51 : {
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_UNPIN_TABLE;
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! SET TABLE command added to list." );
			return __GDBS_UNPIN_TABLE;
		}
		case 61 : {
//			_gim_Uint8 iter = 0;
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_INSERT;
			strcpy( line->fparameter , Tok[1] );
			line->ins = new _gim_list;
//			iter = ( ( NoT - 2 ) / 2 ) ;
			for( int t = 2 ; t < ( NoT ) ; t++ ) {
				_gim_gdbs_line_insert * Tlins = (_gim_gdbs_line_insert *)gim_memory->Alloc( sizeof( _gim_gdbs_line_insert ) , __GIM_GDBS_LINE , __GIM_HIDE );
				if ( Tlins == NULL ) {
					gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
					return __GIM_ERROR;
				}
				strcpy( Tlins->label , Tok[t] );
				strcpy( Tlins->value.value.Char , Tok[++t] );
				gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_line_syntax_check" , "GDBS Insert: Data [%16s = %-16s]" , Tlins->label , Tlins->value.value.Char );
				line->ins->add_item( Tlins );
			}
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! SET TABLE command added to list." );
			return __GDBS_INSERT;
		}
		case 71 :		// UPDATE <table> SET <field> <value> [...] WHERE <field> <value> [AND ...]
		case 81 : {		// DELETE <table> WHERE <field> <value> [AND ...]
			// validate everything before allocating the line
			int w = ( id == 71 ) ? gdbs_find_where( NoT ) : 2;		// DELETE: WHERE is Tok[2], checked by the second level
			int first_cond = ( id == 71 ) ? w + 1 : 3;
			int nset = ( id == 71 ) ? ( w - 3 ) : 0;
			if ( ( id == 71 ) && ( ( w < 0 ) || ( nset < 2 ) || ( nset % 2 != 0 ) ) ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UPDATE: expected SET <field> <value> pairs and then WHERE" , __GIM_ERROR );
				return __SYNTAX_ERROR;
			}
			if ( ( id == 81 ) && ( w != 2 ) ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "DELETE: expected WHERE after the table name" , __GIM_ERROR );
				return __SYNTAX_ERROR;
			}
			int ncond = gdbs_count_conds( first_cond , NoT );
			if ( ncond <= 0 ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "WHERE needs at least one <field> <value> condition (AND between conditions)" , __GIM_ERROR );
				return __SYNTAX_ERROR;
			}

			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = ( id == 71 ) ? __GDBS_UPDATE : __GDBS_DELETE;
			strcpy( line->fparameter , Tok[1] );
			line->ins = new _gim_list;
			line->cnd = new _gim_list;
			for ( int t = 3 ; t < ( id == 71 ? w : 3 ) ; t += 2 ) {
				_gim_gdbs_line_insert * Tlins = (_gim_gdbs_line_insert *)gim_memory->Alloc( sizeof( _gim_gdbs_line_insert ) , __GIM_GDBS_LINE , __GIM_HIDE );
				strcpy( Tlins->label , Tok[t] );
				strcpy( Tlins->value.value.Char , Tok[t + 1] );
				line->ins->add_item( Tlins );
			}
			for ( int t = first_cond ; t < NoT ; ) {
				_gim_gdbs_line_insert * Tc = (_gim_gdbs_line_insert *)gim_memory->Alloc( sizeof( _gim_gdbs_line_insert ) , __GIM_GDBS_LINE , __GIM_HIDE );
				strcpy( Tc->label , Tok[t] );
				strcpy( Tc->value.value.Char , Tok[t + 1] );
				line->cnd->add_item( Tc );
				t += 2;
				if ( ( t < NoT ) && gdbs_kw( Tok[t] , "AND" ) )
					t++;
			}
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , ( id == 71 ) ? "Syntax ok! UPDATE command added to list." : "Syntax ok! DELETE command added to list." );
			return ( id == 71 ) ? __GDBS_UPDATE : __GDBS_DELETE;
		}
		case 91 : {		// DROP TABLE <table>
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->lline = gdbs_line_num;
			line->command = __GDBS_DROP_TABLE;
			strcpy( line->fparameter , Tok[2] );
			line->ins = new _gim_list;
			line->cnd = new _gim_list;
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! DROP TABLE command added to list." );
			return __GDBS_DROP_TABLE;
		}
	}
	return 0;
}


_gim_flag	gim_db_obj::init_from_gdbs( const char * gdbs_name ) {
	_gim_int8 t = 0;
	_gim_int8 s = 0;
	_gim_int8 f = 0;
	_gim_int8 l = 1;

	gdbs_line_num = 1;

	if ( gdbs_run != __GIM_NOT_RUNNING ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::init_from_gdbs" , "GDBS engin is not in the correct state" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	gdbs_run = __GIM_RUNNING;
	gim_error->Set( GIM_ERROR_OK , "gim_db_obj::init_from_gdbs" , "Start reading GDBS file [%s]" , gdbs_name );
	strcpy( gdbs_file_name , gdbs_name );
	gdbs_open();
	while ( ( ! gdbs_feof() ) && ( t >= 0 ) ) {
		gim_error->Set( GIM_ERROR_DEBUG , "prsr_lexical_class::init_from_gdbs" , "Parsing line %d..." , l );
		t = gdbs_tokenizer( gdbs_getline() );
		if ( t < 0 ) {
			if ( t ==  __TOO_MUCH_TOKEN ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "Parse: too much token in line %d" , l );
				return __TOO_MUCH_TOKEN;
			}
			else if ( t == __MISSING_STOP_CHAR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "Parse: Missing line terminator in line %d" , l );
				return __MISSING_STOP_CHAR;
			}
			else if ( t == __SYNTAX_ERROR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "Parse: Syntax error in line %d" , l );
				return __MISSING_STOP_CHAR;
			}
			else {
				gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "Parse: Something wrong in line %d" , l );
				return __MISSING_STOP_CHAR;
			}
		}
		else if ( t > 0 ) {
			s = gdbs_line_syntax_check( t );
			if ( s < 0 ) {
				if ( s == __SYNTAX_ERROR ) {
					gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "Syntax: Syntax error in line %d" , l );
					return __GIM_ERROR;
				}
				else {
					gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "Syntax: Something wrong in syntax check in line %d" , l );
					return __GIM_ERROR;
				}
			}
		}
		else
			gim_error->Set( GIM_ERROR_DEBUG , "prsr_lexical_class::init_from_gdbs" , "SKIPPED" );
		l++;
		gdbs_line_num++;
	}
	gdbs_close();
	f = gdbs_execute();
	if ( f != __GIM_OK ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::init_from_gdbs" , "GDBS script failed: see the previous messages" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	gim_error->Set( GIM_ERROR_MESSAGE , "prsr_lexical_class::init_from_gdbs" , "GDBS: all done!" );
	return __GIM_OK;
}


_gim_int8 gim_db_obj::gdbs_tokenizer( char * line ) {
	{	// blank lines (spaces, tabs, CR, LF only) are skipped, not syntax errors
		const char * p = line;
		while ( *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' )
			p++;
		if ( *p == '\0' )
			return __SKIPPED;
	}
	if ( !strlen( line ) )
		return __SKIPPED;

	static unsigned int	i;
	unsigned int		a , b = 0;
	volatile _gim_flag	for_exit = __GIM_NO , termination = __GIM_NO , comment = __GIM_NO;

	i = 0;
	__GIM_VCLEAR( Tok , 1 , Tok , '\0' );
	strcat( line , " " );
	if ( lex->is_in_string( GDBS_TERMINATOR_CHAR , line ) == __GIM_YES )
		termination = __GIM_YES;
	for( a = 0 ; ( ( a < ( strlen( line ) - 1 ) ) && ( for_exit == __GIM_NO ) ) ; a++ ) {
		if ( ( i > 19 ) || ( line[a] == GDBS_STOP_CHAR ) ) {
			comment = __GIM_YES;
			break;
		}
		if ( lex->is_in_string( line[a] , GDBS_SKIP_STR ) == __GIM_NO ) {
			if ( lex->is_in_string( line[a] , GDBS_SEPARATOR_STR ) ) {
				if ( b ) {
					Tok[i++][b] = '\0' ;
					b=0;
				}
				else
					continue;
			}
			else
				Tok[i][b++] = line[a];
		}
	}

	gim_error->Set( GIM_ERROR_DEBUG , "prsr_lexical_class::gdbs_tokenizer" , "Summary: [line: %3d] [Len: %3d] [NoT: %2d] [termination: %d] [comment: %d]" , gdbs_line_num , strlen( line ) , i , termination , comment );	
	if ( i > 19 ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::gdbs_tokenizer" , "too much token in line" , __GIM_ERROR  );
		return __TOO_MUCH_TOKEN;
	}
	else if ( ( ( i <= 1 ) && ( comment == __GIM_NO ) && ( strlen( line ) > 2 ) ) ) {
		return __SYNTAX_ERROR;
	}
	else if ( ( i ) && ( strlen( Tok[0] ) ) && ( termination == __GIM_YES ) ) {
		gim_error->Set( GIM_ERROR_DEBUG , "prsr_lexical_class::gdbs_tokenizer" , "Correct GDBS line analyzed [extracted %d token]" , i );
		return i;
	}
	else if ( ( i ) && ( !strlen( Tok[0] ) ) && ( termination == __GIM_YES ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::gdbs_tokenizer" , "Syntax error in GDBS file" , __GIM_ERROR  );
		return __SYNTAX_ERROR;
	}
	else if ( ( i ) && ( strlen( Tok[0] ) ) && ( termination == __GIM_NO ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::gdbs_tokenizer" , "Missing line terminator in GDBS file" , __GIM_ERROR  );
		return __MISSING_STOP_CHAR;
	}
	return __SKIPPED;
}


_gim_flag   gim_db_obj::string_to_flag( _gim_Uint8 index , _gim_flag context ) {
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::string_to_flag" , "Tok = %-15s , index = %2d , Context = %2d" , Tok[ index ] , index , context );
	switch ( context ) {
		case __GIM_DB : {
			if		( lex->str_equal( Tok[ index ] , "VOLATILE" ) )
				return GIM_DB_VOLATILE;
			else if ( lex->str_equal( Tok[ index ] , "PERMANENT" ) )
				return GIM_DB_PERMANENT;
			else if ( lex->str_equal( Tok[ index ] , "SAVE_MEMORY" ) )
				return GIM_DB_SAVE_MEMORY;
			else if ( lex->str_equal( Tok[ index ] , "PERFORMANCE" ) )
				return GIM_DB_PERFORMANCE;
			else if ( lex->str_equal( Tok[ index ] , "BALANCED" ) )
				return GIM_DB_BALANCED;
			break;
		}
		case __GIM_FIELD : {
			if      ( lex->str_equal( Tok[ index ] , "INT" ) )
				return GIM_DB_TYPE_INT;
			else if ( lex->str_equal( Tok[ index ] , "FLOAT" ) )
				return GIM_DB_TYPE_FLOAT;
			else if ( lex->str_equal( Tok[ index ] , "PERCENTAGE" ) )
				return GIM_DB_TYPE_PERCENTAGE;
			else if ( lex->str_equal( Tok[ index ] , "BOOL" ) )
				return GIM_DB_TYPE_BOOL;
			else if ( lex->str_equal( Tok[ index ] , "FLAG" ) )
				return GIM_DB_TYPE_FLAG;
			else if ( lex->str_equal( Tok[ index ] , "STRING" ) )
				return GIM_DB_TYPE_STRING;
			else if ( lex->str_equal( Tok[ index ] , "DATE" ) )
				return GIM_DB_TYPE_DATE;
			else if ( lex->str_equal( Tok[ index ] , "TIME" ) )
				return GIM_DB_TYPE_TIME;
			else if ( lex->str_equal( Tok[ index ] , "TIMESTAMP" ) )
				return GIM_DB_TYPE_TIMESTAMP;
			else if ( lex->str_equal( Tok[ index ] , "IS_INDEX" ) )
				return GIM_DB_IS_INDEX;
			else if ( lex->str_equal( Tok[ index ] , "IS_NOT_INDEX" ) )
				return GIM_DB_IS_NOT_INDEX;
			else if ( lex->str_equal( Tok[ index ] , "KEY" ) )
				return GIM_DB_IS_KEY;
			else if ( lex->str_equal( Tok[ index ] , "NOT_KEY" ) )
				return GIM_DB_IS_NOT_KEY;
			else if ( lex->str_equal( Tok[ index ] , "AUTOINCREMENTAL" ) )
				return GIM_DB_AUTOINCREMENTAL;
			else if ( lex->str_equal( Tok[ index ] , "IS_UNIQUE" ) )
				return GIM_DB_IS_UNIQUE;
			break;
		}
		case __GIM_TABLE : {
			if      ( lex->str_equal( Tok[ index ] , "VOLATILE" ) )
				return GIM_DB_TB_VOLATILE;
			else if ( lex->str_equal( Tok[ index ] , "PERMANENT" ) )
				return GIM_DB_TB_PERMANENT;
			else if ( lex->str_equal( Tok[ index ] , "VIRTUAL" ) )
				return GIM_DB_TB_VIRTUAL;
			break;
		}
	}
	gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::string_to_flag" , "PROPERTIES not found : [%s]" , Tok[ index ] );
	return __GIM_ERROR;
}



_gim_flag	gim_db_obj::gdbs_execute( void ) {
	if ( ! gdbs_script->items() ) {
		gim_error->Set( GIM_ERROR_WARNING , "gim_db_obj::gdbs_execute" , "Script list is empty. Nothing to do." );
		return __GIM_ERROR;
	}
	gdbs_script->rewind();

	_gim_gdbs_line   * line = NULL;
		_gim_flag		result = __GIM_OK;	// first failing command makes the whole script fail

	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING GDBS ITEMS [%3d]..." , gdbs_script->items() );
	do {
		line = (_gim_gdbs_line *)gdbs_script->get_item();
		if ( line != NULL ) {
			switch ( line->command ) {
				case __GDBS_CREATE_DB : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : CREATE DB..." , line->lline  );
					set_name( line->fparameter );
					init();
					break;
				}
				case __GDBS_ADD_FIELD : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : ADD FIELD..." , line->lline  );
					if ( make_add_field( line->fvalue , line->svalue , line->tparameter ) != __GIM_OK )
							result = __GIM_ERROR;
					break;
				}
				case __GDBS_CREATE_TABLE : {
					make_create_table( line->fparameter );
					break;
				}
				case __GDBS_PIN_TABLE : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : PIN TABLE..." , line->lline  );
					if ( db->pin == __GIM_YES ) {
						gim_error->Set( GIM_ERROR_WARNING , "gim_db_obj::gdbs_execute" , "A table is already pinned [%s]" , db->pin_table );
						break;
					}
					if ( table_exist( line->fparameter ) == __GIM_YES ) {
						strcpy( db->pin_table , line->fparameter );
						db->pin = __GIM_YES;
						db->Ttab = seek_table( db->pin_table ); 
						gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::gdbs_execute" , "table succesfully pinned [%s]" , db->pin_table );
					}
					else
						gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_execute" , "table not found [%s]" , line->fparameter );
					break;
				}
				case __GDBS_SET_DB : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : SET DB..." , line->lline  );
					make_set_db( line->fvalue );
					break;
				}
				case __GDBS_SET_FIELD : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : SET FIELD..." , line->lline  );
					if ( make_set_field( line->fvalue , line->svalue ) != __GIM_OK )
							result = __GIM_ERROR;
					break;
				}
				case __GDBS_SET_TABLE : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : SET TABLE..." , line->lline  );
					if ( make_set_table( line->fvalue ) != __GIM_OK )
							result = __GIM_ERROR;
					break;
				}
				case __GDBS_UNPIN_TABLE : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : UNPIN TABLE..." , line->lline  );
					if ( db->pin == __GIM_NO ) {
						gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_execute" , "No table is pinned" , __GIM_ERROR );
						break;
					}
					if ( db->Ttab == NULL ) {
						gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_execute" , "NO table pointer is pinned" , __GIM_ERROR );
						break;
					}
					strcpy( db->pin_table , "" );
					db->pin = __GIM_NO;
					db->Ttab = NULL;
					gim_error->set( GIM_ERROR_MESSAGE , "gim_db_obj::gdbs_execute" , "table succesfully unpinned" , __GIM_OK );
					break;
				}	
				case __GDBS_INSERT : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : INSERT..." , line->lline  );
					if ( db->pin == __GIM_YES ) {
						gim_error->Set( GIM_ERROR_WARNING , "gim_db_obj::gdbs_execute" , "A table is already pinned [%s]" , db->pin_table );
						break;
					}
					if ( table_exist( line->fparameter ) == __GIM_YES ) {
						char H[128];
						strcpy( db->pin_table , line->fparameter );
						db->pin = __GIM_YES;
						db->Ttab = seek_table( db->pin_table ); 
						gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::gdbs_execute" , "table succesfully pinned [%s]" , db->pin_table );
						db->Ttab->items++;
						_gim_db_record * Trec = (_gim_db_record *)gim_memory->Alloc( sizeof( _gim_db_record ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
						strcpy( Trec->hash , "" );
						Trec->index = db->Ttab->items;
						Trec->values = new _gim_list;
						sprintf( H , "Record %d - Address id %p" , db->Ttab->items , Trec );
						strcpy( Trec->hash , cs->md5( H , strlen( H ) ) );
						gim_error->set( GIM_ERROR_MESSAGE , "gim_db_obj::gdbs_execute" , H , __GIM_OK );
						gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "Generated Hash [%s]" , Trec->hash );
						db->Ttab->Tdata->AddSection( Trec->hash );
						db->Ttab->fields->rewind();
						line->ins->rewind();
						_gim_Uint8 iter = db->Ttab->fields->items();
						if ( iter ) {
							for( _gim_Uint8 i = 1 ; i <= iter ; i++ ) {						
								_gim_db_field * Tfield = (_gim_db_field *)db->Ttab->fields->get_item( i );
								_gim_gdbs_line_insert * Tlins = (_gim_gdbs_line_insert *)line->ins->get_item( i );
								_gim_db_value * Tv = (_gim_db_value *)gim_memory->Alloc( sizeof( _gim_db_value ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
								gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "Record [%d] - Label [%16s] - Value[%12s] - Type [%2d]" , db->Ttab->items , Tlins->label , Tlins->value.value.Char , Tfield->type );
								Tv->type = Tfield->type;
								switch ( Tv->type ) {
									case GIM_DB_TYPE_INT : {
										Tv->value.Int = atoi( Tlins->value.value.Char );
										db->Ttab->Tdata->AddKey( Trec->hash , Tfield->name , Tv->value.Int );
										break;
									}
									case GIM_DB_TYPE_FLOAT : {

										break;
									}
									case GIM_DB_TYPE_PERCENTAGE : {
										Tv->value.Int = atoi( Tlins->value.value.Char );
										db->Ttab->Tdata->AddKey( Trec->hash , Tfield->name , Tv->value.Int );
										break;
									}
									case GIM_DB_TYPE_BOOL : {
										if      ( lex->str_equal( lex->str_down( Tlins->value.value.Char , strlen( Tlins->value.value.Char ) ) , "true" ) == __GIM_YES )
											Tv->value.Bool = __GIM_TRUE;
										else if ( lex->str_equal( lex->str_down( Tlins->value.value.Char , strlen( Tlins->value.value.Char ) ) , "false" ) == __GIM_YES )
											Tv->value.Bool = __GIM_FALSE;
										db->Ttab->Tdata->AddKeyFlag( Trec->hash , Tfield->name , Tv->value.Bool );
										break;
									}
									case GIM_DB_TYPE_FLAG : {
										if      ( lex->str_equal( lex->str_down( Tlins->value.value.Char , strlen( Tlins->value.value.Char ) ) , "on" ) == __GIM_YES )
											Tv->value.Flag = __GIM_ON;
										else if ( lex->str_equal( lex->str_down( Tlins->value.value.Char , strlen( Tlins->value.value.Char ) ) , "off" ) == __GIM_YES )
											Tv->value.Flag = __GIM_OFF;
										else if ( lex->str_equal( lex->str_down( Tlins->value.value.Char , strlen( Tlins->value.value.Char ) ) , "yes" ) == __GIM_YES )
											Tv->value.Flag = __GIM_YES;
										else if ( lex->str_equal( lex->str_down( Tlins->value.value.Char , strlen( Tlins->value.value.Char ) ) , "no" ) == __GIM_YES )
											Tv->value.Flag = __GIM_NO;
										db->Ttab->Tdata->AddKeyFlag( Trec->hash , Tfield->name , Tv->value.Flag );
										break;
									}
									case GIM_DB_TYPE_STRING : {
										strcpy( Tv->value.Char , Tlins->value.value.Char );
										db->Ttab->Tdata->AddKey( Trec->hash , Tfield->name , Tv->value.Char );
										break;
									}
									case GIM_DB_TYPE_DATE : {
										strcpy( Tv->value.Char , Tlins->value.value.Char );
										db->Ttab->Tdata->AddKey( Trec->hash , Tfield->name , Tv->value.Char );
										break;
									}
									case GIM_DB_TYPE_TIME : {
										strcpy( Tv->value.Char , Tlins->value.value.Char );
										db->Ttab->Tdata->AddKey( Trec->hash , Tfield->name , Tv->value.Char );
										break;
									}
									case GIM_DB_TYPE_TIMESTAMP : {
										Tv->value.Int = atoi( Tlins->value.value.Char );
										db->Ttab->Tdata->AddKey( Trec->hash , Tfield->name , Tv->value.Int );
										break;
									}
								}
								Trec->values->add_item( Tv );
							}
							if ( db->type == GIM_DB_PERMANENT ) 
								db->Ttab->Tdata->Write();
							line->ins->destroy_list();					
						}
						else {
							
						}
						strcpy( db->pin_table , "" );
						db->pin = __GIM_NO;
						db->Ttab = NULL;
						gim_error->set( GIM_ERROR_MESSAGE , "gim_db_obj::gdbs_execute" , "table succesfully unpinned" , __GIM_OK );
					}
					else
						gim_error->Set( GIM_ERROR_WARNING , "gim_db_obj::gdbs_execute" , "table not found [%s]" , line->fparameter );
					break;
				}
				case __GDBS_UPDATE : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : UPDATE..." , line->lline );
					int nset  = line->ins->items();
					int ncond = line->cnd->items();
					const char * fields[ 255 ];
					const char * values[ 255 ];
					_gim_db_cond conds[ 255 ];
					for ( int i = 1 ; i <= nset ; i++ ) {
						_gim_gdbs_line_insert * Tl = (_gim_gdbs_line_insert *)line->ins->get_item( i );
						fields[ i - 1 ] = Tl->label;
						values[ i - 1 ] = Tl->value.value.Char;
					}
					for ( int i = 1 ; i <= ncond ; i++ ) {
						_gim_gdbs_line_insert * Tc = (_gim_gdbs_line_insert *)line->cnd->get_item( i );
						conds[ i - 1 ].field = Tc->label;
						conds[ i - 1 ].op    = "=";				// GDBS conditions are equalities: '=' is a separator of the language
						conds[ i - 1 ].value = Tc->value.value.Char;
					}
					int updated = 0;
					if ( update_record( line->fparameter , (_gim_Uint8) ncond , conds , (_gim_Uint8) nset , fields , values , &updated ) != __GIM_OK )
						result = __GIM_ERROR;
					line->ins->destroy_list();
					line->cnd->destroy_list();
					break;
				}
				case __GDBS_DELETE : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : DELETE..." , line->lline );
					int ncond = line->cnd->items();
					_gim_db_cond conds[ 255 ];
					for ( int i = 1 ; i <= ncond ; i++ ) {
						_gim_gdbs_line_insert * Tc = (_gim_gdbs_line_insert *)line->cnd->get_item( i );
						conds[ i - 1 ].field = Tc->label;
						conds[ i - 1 ].op    = "=";
						conds[ i - 1 ].value = Tc->value.value.Char;
					}
					int deleted = 0;
					if ( delete_record( line->fparameter , (_gim_Uint8) ncond , conds , &deleted ) != __GIM_OK )
						result = __GIM_ERROR;
					line->ins->destroy_list();
					line->cnd->destroy_list();
					break;
				}
				case __GDBS_DROP_TABLE : {
					gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::gdbs_execute" , "EXECUTING LINE [%3d] : DROP TABLE..." , line->lline );
					if ( drop_table( line->fparameter ) != __GIM_OK )
						result = __GIM_ERROR;
					line->ins->destroy_list();
					line->cnd->destroy_list();
					break;
				}
			}
			gdbs_script->next_item();
		}
	} while ( line != NULL );
	return result;
}


_gim_flag	gim_db_obj::table_exist( char * table_name ) {
	if ( db->tables_number < 1 ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::table_exist" , "Tables number is 0." , __GIM_ERROR );
		return __GIM_ERROR;
	}
	for ( _gim_int32 c = 1 ; c <= db->tables_number ; c++ ) {
		char Ttable[128];
		sprintf( Ttable , "tab%d_name" , c );
		if ( lex->str_equal( table_name , db->conf->GetKeySTR( "DB" , Ttable ) ) == __GIM_YES ) 
			return __GIM_YES;
	}
	gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::table_exist" , "Table not found [%s]" , table_name );
	return __GIM_NO;
}


_gim_db_table *  gim_db_obj::seek_table( char * table_name ) {
	db->tables->rewind();
	for( ; db->tables->get_item() != NULL ; db->tables->next_item() ) {
		_gim_db_table * tmp = (_gim_db_table *)db->tables->get_item();
		if ( lex->str_equal( tmp->name , table_name ) == __GIM_YES ) {
			return tmp;
		}
	}
	return NULL;
}

_gim_flag gim_db_obj::make_create_table( char * table_name ) {
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::make_create_table" , "Executing CREATE TABLE [%s]..." , table_name );
	char	DB_Tname[32];
	_gim_db_table * Ttmp = (_gim_db_table *)gim_memory->Alloc( sizeof( _gim_db_table ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
	strcpy( Ttmp->name		, table_name );
	strcpy( Ttmp->comment   , "" );
	strcpy( Ttmp->name	, lex->char_subst( table_name , ' ' , '_' ) );
	sprintf( Ttmp->name_data				,   "%s_data" , Ttmp->name );
	sprintf( Ttmp->file_name_struct			,   "%s.conf" , Ttmp->name );
	sprintf( Ttmp->file_name_data			,	"%s.conf" , Ttmp->name_data );
	sprintf( Ttmp->file_name_struct_long	,   "%s/%s" , home , Ttmp->file_name_struct );
	sprintf( Ttmp->file_name_data_long		,	"%s/%s" , home , Ttmp->file_name_data );
	Ttmp->type  = GIM_DB_TB_VOLATILE;
	Ttmp->items = 0;
	Ttmp->fields_number = 0;
	Ttmp->autoincrementl_field_number = -1;
	Ttmp->unique_field_number = -1;
	Ttmp->key_field_number = -1;
	Ttmp->sizeof_per_record = 0;
	Ttmp->fields  = new _gim_list;
	Ttmp->records = new _gim_list;

	Ttmp->Tstruct = new _gim_prsr;
	Ttmp->Tstruct->Up( Ttmp->file_name_struct_long , Ttmp->name );
	Ttmp->Tstruct->SetLex( __LEX_B );
	Ttmp->Tstruct->AddSection   ( "TABLE" );
	Ttmp->Tstruct->AddKey		( "TABLE"   , "name"							, Ttmp->name );
	Ttmp->Tstruct->AddKey		( "TABLE"   , "file"							, Ttmp->file_name_struct );
	Ttmp->Tstruct->AddKey		( "TABLE"   , "items"							, Ttmp->items );
	Ttmp->Tstruct->AddKey		( "TABLE"   , "type"							, Ttmp->type );
	Ttmp->Tstruct->AddSection   ( "STRUCT" );
	Ttmp->Tstruct->AddKey		( "STRUCT"  , "fields_number"					, Ttmp->fields_number );
	Ttmp->Tstruct->AddKey		( "STRUCT"  , "key_field_number"				, Ttmp->key_field_number );
	Ttmp->Tstruct->AddKey		( "STRUCT"  , "autoincremental_field_number"	, Ttmp->autoincrementl_field_number );
	Ttmp->Tstruct->AddKey		( "STRUCT"  , "unique_field_number"				, Ttmp->unique_field_number );
//	Ttmp->Tstruct->AddSection   ( "FIELDS" );
	
	Ttmp->Tdata   = new _gim_prsr;
	Ttmp->Tdata->Up( Ttmp->file_name_data_long , Ttmp->name_data );
	Ttmp->Tdata->SetLex( __LEX_A );
	
	db->tables->add_item( Ttmp );
	db->tables_number++;
	sprintf( DB_Tname , "tab%d_name" , db->tables_number );
	db->conf->ChangeKey ( "DB"		, "tables"				, db->tables_number ); 
	db->conf->AddKey	( "DB"		, DB_Tname				, Ttmp->name );
	if ( db->type == GIM_DB_PERMANENT ) 
		db->conf->Write();
	return __GIM_OK;
}


_gim_flag gim_db_obj::make_set_db( _gim_flag value ) {
	gim_error->set(  "gim_db_obj::make_set_db" , "Executing SET DB..."  );
	switch ( value ) {
		case GIM_DB_PERMANENT : {
			db->type = GIM_DB_PERMANENT;
			db->conf->ChangeKey( "DB" , "type" , db->type );
			make_env();
			db->conf->Write();
			gim_error->set(  "gim_db_obj::make_set_db" , "SET type to PERMANENT"  );
			break;
	    }
		case GIM_DB_VOLATILE : {
			db->type = GIM_DB_VOLATILE;
			db->conf->ChangeKey( "DB" , "type" , db->type );
			make_env();
			gim_error->set(  "gim_db_obj::make_set_db" , "SET type to VOLATILE"  );
			break;
	    }
		case GIM_DB_SAVE_MEMORY : {
			db->mode = GIM_DB_SAVE_MEMORY;
			db->conf->ChangeKey( "DB" , "mode" , db->mode );
			if ( db->type == GIM_DB_PERMANENT ) 
				db->conf->Write();
			gim_error->set(  "gim_db_obj::make_set_db" , "SET mode to SAVE MEMORY"  );
			break;
	    }
		case GIM_DB_BALANCED : {
			db->mode = GIM_DB_BALANCED;
			db->conf->ChangeKey( "DB" , "mode" , db->mode );
			if ( db->type == GIM_DB_PERMANENT ) 
				db->conf->Write();
			gim_error->set(  "gim_db_obj::make_set_db" , "SET mode to BALANCED"  );
			break;
	    }
		case GIM_DB_PERFORMANCE : {
			db->mode = GIM_DB_PERFORMANCE;
			db->conf->ChangeKey( "DB" , "mode" , db->mode );
			if ( db->type == GIM_DB_PERMANENT ) 
				db->conf->Write();
			gim_error->set(  "gim_db_obj::make_set_db" , "SET mode to PERFORMANCE"  );
			break;
	    }
	}
	return __GIM_OK;
}


_gim_flag gim_db_obj::make_set_table( _gim_flag value ) {
	if ( db->Ttab == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::make_set_table" , "SET TABLE without a pinned table: use PIN TABLE first" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::create_set_table" , "Executing SET TABLE [%d]..." , value );
	if ( db->type == GIM_DB_PERMANENT ) {
		switch ( value ) {
			case GIM_DB_TB_PERMANENT : {
				db->Ttab->type = GIM_DB_TB_PERMANENT;
				db->Ttab->Tstruct->ChangeKey( "TABLE" , "type" , db->Ttab->type );
				db->Ttab->Tstruct->Write();
				db->Ttab->Tdata->Write();
				gim_error->set(  "gim_db_obj::create_set_table" , "SET TABLE to PERMANENT" );
				break;
			}
			case GIM_DB_TB_VIRTUAL : {
				db->Ttab->type = GIM_DB_TB_VIRTUAL;
				db->Ttab->Tstruct->ChangeKey( "TABLE" , "type" , db->Ttab->type );
				db->Ttab->Tstruct->Write();
				gim_error->set(  "gim_db_obj::create_set_table" , "SET TABLE to VIRTUAL" );
				break;
			}
			case GIM_DB_TB_VOLATILE : {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::create_set_table" , "Impossible to SET this TABLE to VOLATILE. The DB is PERMANENT" , __GIM_ERROR );
				break;
			}
		}
	}
	if ( db->type == GIM_DB_VOLATILE ) {
		switch ( value ) {
			case GIM_DB_TB_PERMANENT : {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::create_set_table" , "Impossible to SET this TABLE to PERMANENT. The DB is PERMANENT" , __GIM_ERROR );
				break;
			}
			case GIM_DB_TB_VIRTUAL : {
				db->Ttab->type = GIM_DB_TB_VIRTUAL;
				db->Ttab->Tstruct->ChangeKey( "TABLE" , "type" , db->Ttab->type );
				gim_error->set(  "gim_db_obj::create_set_table" , "SET TABLE to VIRTUAL" );
				break;
			}
			case GIM_DB_TB_VOLATILE : {
				db->Ttab->type = GIM_DB_TB_VOLATILE;
				db->Ttab->Tstruct->ChangeKey( "TABLE" , "type" , db->Ttab->type );
				gim_error->set(  "gim_db_obj::create_set_table" , "SET TABLE to VOLATILE" );
				break;
			}
		}
	}
	return __GIM_OK;
}


_gim_flag gim_db_obj::make_add_field( _gim_Uint8 value , _gim_flag type , char * name ) {
	if ( db->Ttab == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::make_add_field" , "ADD FIELD without a pinned table: use PIN TABLE first" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	char Fname[128];
	char Ftype[128];

	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::make_add_field" , "[id field %d] [type %d] [name %s]" , value , type , name );
	sprintf( Fname , "field_%d_name" , value );
	sprintf( Ftype , "field_%d_type" , value );

	_gim_db_field * Tfield = (_gim_db_field *)gim_memory->Alloc( sizeof( _gim_db_field ) , __GIM_MEM_DB_MAIN , __GIM_HIDE );
	strcpy( Tfield->name , name ); 
	Tfield->type = type;
	db->Ttab->fields->add_item( Tfield );

	if ( db->Ttab->Tstruct->ExistSection( "field" ) != __GIM_EXIST ) 
		db->Ttab->Tstruct->AddSection( "field" );
	db->Ttab->Tstruct->AddKey( "field" , Fname , name );
	db->Ttab->Tstruct->AddKey( "field" , Ftype , type );
	db->Ttab->fields_number++;
	db->Ttab->Tstruct->ChangeKey( "struct" , "fields_number" , db->Ttab->fields_number );
	if ( db->type == GIM_DB_PERMANENT ) 
		db->Ttab->Tstruct->Write();
	return __GIM_OK;
}


_gim_flag gim_db_obj::make_set_field( _gim_Uint8 value , _gim_flag type ) {
	if ( db->Ttab == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::make_set_field" , "SET FIELD without a pinned table: use PIN TABLE first" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	gim_error->Set( GIM_ERROR_DEBUG , "gim_db_obj::make_set_field" , "[id field %d] [type %d]" , value , type );
	switch( type ) {
/*		case GIM_DB_IS_INDEX : {
			break;
		}
*/		case GIM_DB_IS_KEY : {
			db->Ttab->Tstruct->ChangeKey    ( "struct" , "key_field_number" , value );
			break;
		}
		case GIM_DB_AUTOINCREMENTAL : {
			db->Ttab->Tstruct->ChangeKey    ( "struct" , "autoincremental_field_number" , value );
			break;
		}
		case GIM_DB_IS_UNIQUE : {
			db->Ttab->Tstruct->ChangeKey    ( "struct" , "unique_field_number" , value );
			break;
		}
	}
	if ( db->type == GIM_DB_PERMANENT ) 
		db->Ttab->Tstruct->Write();
	return __GIM_OK;
}	


_gim_flag	gim_db_obj::gim_db_version	( int maj , int min ) {
	char	db_ver[8];

	sprintf( db_ver , "%d%d" , maj , min );
	if ( atoi(db_ver) >= 23 )
		return __GIM_OK;
	else 
		return __GIM_NOT_OK;
}





_gim_Uint8 gim_db_obj::tables_count( void ) {
	return db->tables->items();
}


// index is 1-based, as the tabN_name keys of the DB configuration
_gim_db_table * gim_db_obj::table_at( _gim_Uint8 index ) {
	if ( ( index < 1 ) || ( index > db->tables->items() ) )
		return NULL;
	db->tables->rewind();
	for ( _gim_Uint8 c = 1 ; c < index ; c++ )
		db->tables->next_item();
	return (_gim_db_table *)db->tables->get_item();
}


// Adds one record to a table. 'fields' and 'values' are n pairs; every field of the table must be given once.
// Values are checked against the field types and the KEY field must stay unique.
/* ---- helpers shared by insert_record, update_record and delete_record ---- */

// checks a value against the type of its field and writes the normalised text into out (512 bytes).
// op_name is the command name used in the messages ("INSERT" or "UPDATE").
static _gim_flag db_normalise( const char * op_name , _gim_db_field * f , const char * given , char * out ) {
	char * end = NULL;
	if ( f->type == GIM_DB_TYPE_INT ) {
		long v = strtol( given , &end , 10 );
		if ( ( end == given ) || ( *end != '\0' ) ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::db_normalise" , "%s: field %s wants an INT, got '%s'" , op_name , f->name , given );
			return __GIM_ERROR;
		}
		sprintf( out , "%ld" , v );
	}
	else if ( f->type == GIM_DB_TYPE_FLOAT ) {
		strtod( given , &end );
		if ( ( end == given ) || ( *end != '\0' ) ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::db_normalise" , "%s: field %s wants a FLOAT, got '%s'" , op_name , f->name , given );
			return __GIM_ERROR;
		}
		strncpy( out , given , 511 );
		out[ 511 ] = '\0';
	}
	else if ( ( f->type == GIM_DB_TYPE_BOOL ) || ( f->type == GIM_DB_TYPE_FLAG ) ) {
		if ( strcmp( given , "YES" ) == 0 || strcmp( given , "1" ) == 0 )
			strcpy( out , "YES" );
		else if ( strcmp( given , "NO" ) == 0 || strcmp( given , "0" ) == 0 )
			strcpy( out , "NO" );
		else {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::db_normalise" , "%s: field %s wants YES/NO, got '%s'" , op_name , f->name , given );
			return __GIM_ERROR;
		}
	}
	else {
		strncpy( out , given , 511 );
		out[ 511 ] = '\0';
	}
	return __GIM_OK;
}

// the field with this name, or NULL. Leaves the cursor of the field list where it was found.
static _gim_db_field * db_find_field( _gim_db_table * t , const char * name ) {
	t->fields->rewind();
	for ( int i = 0 ; i < t->fields->items() ; i++ ) {
		_gim_db_field * f = (_gim_db_field *) t->fields->get_item();
		if ( strcmp( f->name , name ) == 0 )
			return f;
		t->fields->next_item();
	}
	return NULL;
}

// name of the KEY field, or NULL when the table has none
static _gim_db_field * db_key_field( _gim_db_table * t ) {
	int key = t->Tstruct->GetKeyINT( "STRUCT" , "key_field_number" );
	if ( key < 1 )
		return NULL;
	t->fields->rewind();
	for ( int i = 1 ; i <= t->fields->items() ; i++ ) {
		_gim_db_field * f = (_gim_db_field *) t->fields->get_item();
		if ( i == key )
			return f;
		t->fields->next_item();
	}
	return NULL;
}

// checks that every condition names a field of the table and uses a valid operator; fills ctype with the field types
static int db_check_conditions( _gim_db_table * t , int ncond , const _gim_db_cond * cond , _gim_flag * ctype , const char * op_name ) {
	for ( int c = 0 ; c < ncond ; c++ ) {
		_gim_db_field * f = db_find_field( t , cond[ c ].field );
		if ( f == NULL ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::db_check_conditions" , "%s: table '%s' has no field '%s'" , op_name , t->name , cond[ c ].field );
			return 0;
		}
		if ( ! gim_db_obj::op_is_valid( cond[ c ].op ) ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::db_check_conditions" , "%s: operator '%s' is not valid" , op_name , cond[ c ].op );
			return 0;
		}
		ctype[ c ] = f->type;
	}
	return 1;
}

// collects the sections (records) that match all the conditions, in table order.
// The names are copied into a buffer of 64 bytes per record, allocated with gim_memory: the caller frees it.
// GetSectionName() and GetKeySTR() return shared buffers, so each name is copied before it is used again.
static int db_collect( _gim_db_table * t , int ncond , const _gim_db_cond * cond , const _gim_flag * ctype , char ** names_out ) {
	int total = t->Tdata->GetHowManySection();
	if ( total < 0 )
		total = 0;
	char * names = (char *) gim_memory->Alloc( ( total > 0 ? total : 1 ) * 64 , __GIM_MEM_DB_RECORD , __GIM_HIDE );
	int count = 0;
	for ( int s = 0 ; s < total ; s++ ) {
		char * sec = t->Tdata->GetSectionName( (_gim_int16) s );
		if ( sec == NULL )
			continue;
		char secname[ 64 ];
		strncpy( secname , sec , 63 );
		secname[ 63 ] = '\0';
		int ok = 1;
		for ( int c = 0 ; c < ncond && ok ; c++ ) {
			char * stored = t->Tdata->GetKeySTR( secname , cond[ c ].field );
			if ( stored == NULL || ! gim_db_obj::value_matches( ctype[ c ] , stored , cond[ c ].op , cond[ c ].value ) )
				ok = 0;
		}
		if ( ok ) {
			strncpy( names + count * 64 , secname , 63 );
			names[ count * 64 + 63 ] = '\0';
			count++;
		}
	}
	*names_out = names;
	return count;
}

_gim_flag gim_db_obj::insert_record( const char * table_name , _gim_Uint8 n , const char ** fields , const char ** values ) {
	_gim_db_table * t = seek_table( (char *) table_name );
	if ( t == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::insert_record" , "INSERT: table not found" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( ( t->fields == NULL ) || ( t->Tdata == NULL ) || ( t->Tstruct == NULL ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::insert_record" , "INSERT: table is not complete" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	_gim_Uint8 nf = (_gim_Uint8) t->fields->items();
	if ( n != nf ) {
		gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::insert_record" , "INSERT: the table has %d fields, %d given" , nf , n );
		return __GIM_ERROR;
	}

	// values in table order, normalised (INT as decimal, BOOL as YES/NO, FLOAT as is)
	static char norm[ 255 ][ 512 ];
	const char * vals[ 255 ];
	const char * keyname = NULL;
	const char * keyval = NULL;
	int key = t->Tstruct->GetKeyINT( "STRUCT" , "key_field_number" );

	t->fields->rewind();
	for ( _gim_Uint8 i = 0 ; i < nf ; i++ ) {
		_gim_db_field * f = (_gim_db_field *) t->fields->get_item();
		t->fields->next_item();

		const char * given = NULL;
		for ( _gim_Uint8 j = 0 ; j < n ; j++ ) {
			if ( strcmp( fields[ j ] , f->name ) == 0 ) {
				if ( given != NULL ) {
					gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::insert_record" , "INSERT: field %s given twice" , f->name );
					return __GIM_ERROR;
				}
				given = values[ j ];
			}
		}
		if ( given == NULL ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::insert_record" , "INSERT: field %s is missing" , f->name );
			return __GIM_ERROR;
		}

		if ( db_normalise( "INSERT" , f , given , norm[ i ] ) != __GIM_OK )
			return __GIM_ERROR;
		vals[ i ] = norm[ i ];

		if ( ( key >= 1 ) && ( i + 1 == key ) ) {
			keyname = f->name;
			keyval = norm[ i ];
		}
	}

	// KEY must be unique in the table
	if ( keyname != NULL && t->Tdata->ParserIsUp() == __GIM_YES ) {
		int have = t->Tdata->GetHowManySection();
		for ( int s = 0 ; s < have ; s++ ) {
			char * sec = t->Tdata->GetSectionName( (_gim_int16) s );
			char * kv = ( sec != NULL ) ? t->Tdata->GetKeySTR( sec , keyname ) : NULL;
			if ( ( kv != NULL ) && ( strcmp( kv , keyval ) == 0 ) ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::insert_record" , "INSERT: KEY %s = %s already exists" , keyname , keyval );
				return __GIM_ERROR;
			}
		}
	}

	// a unique section name for the record (the hash is not reused between sessions)
	// items can go down after a delete, so the same time and count can come back: try again until the id is free
	char H[ 256 ];
	char hash[ 64 ];
	int attempt = 0;
	do {
		sprintf( H , "%s-%d-%ld-%p-%d" , t->name , t->items , (long) time( NULL ) , (void *) t , attempt++ );
		strncpy( hash , cs->md5( H , strlen( H ) ) , sizeof( hash ) - 1 );
		hash[ sizeof( hash ) - 1 ] = '\0';
	} while ( ( t->Tdata->ParserIsUp() == __GIM_YES ) && ( t->Tdata->ExistSection( hash ) == __GIM_EXIST ) );

	// a table saved with no records has no data parser yet: bring it up as make_create_table does
	if ( t->Tdata->ParserIsUp() != __GIM_YES ) {
		char title[ 80 ];
		sprintf( title , "%s_data" , t->name );
		t->Tdata->Up( t->file_name_data_long , title );
		t->Tdata->SetLex( __LEX_A );
	}
	t->Tdata->AddSection( hash );
	t->fields->rewind();
	for ( _gim_Uint8 i = 0 ; i < nf ; i++ ) {
		_gim_db_field * f = (_gim_db_field *) t->fields->get_item();
		t->fields->next_item();
		if ( f->type == GIM_DB_TYPE_INT )
			t->Tdata->AddKey( hash , f->name , atoi( vals[ i ] ) );
		else if ( f->type == GIM_DB_TYPE_FLOAT )
			t->Tdata->AddKey( hash , f->name , (float) atof( vals[ i ] ) );
		else if ( ( f->type == GIM_DB_TYPE_BOOL ) || ( f->type == GIM_DB_TYPE_FLAG ) )
			t->Tdata->AddKeyFlag( hash , f->name , ( strcmp( vals[ i ] , "YES" ) == 0 ) ? __GIM_YES : __GIM_NO );
		else
			t->Tdata->AddKey( hash , f->name , vals[ i ] );
	}

	t->items++;
	t->Tstruct->ChangeKey( "TABLE" , "items" , t->items );
	if ( db->type == GIM_DB_PERMANENT ) {
		t->Tdata->Write();
		t->Tstruct->Write();
	}
	gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::insert_record" , "INSERT: record added to [%s] (items %d)" , t->name , t->items );
	return __GIM_OK;
}


int gim_db_obj::op_is_valid( const char * op ) {
	return strcmp( op , "=" ) == 0 || strcmp( op , "!=" ) == 0 || strcmp( op , "<" ) == 0 ||
		   strcmp( op , "<=" ) == 0 || strcmp( op , ">" ) == 0 || strcmp( op , ">=" ) == 0;
}

int gim_db_obj::value_matches( _gim_flag type , const char * stored , const char * op , const char * wanted ) {
	int cmp;
	if ( ( type == GIM_DB_TYPE_INT ) || ( type == GIM_DB_TYPE_FLOAT ) ) {
		double a = atof( stored );
		double b = atof( wanted );
		cmp = ( a < b ) ? -1 : ( a > b ) ? 1 : 0;
	}
	else
		cmp = strcmp( stored , wanted );

	if ( strcmp( op , "=" ) == 0 )  return cmp == 0;
	if ( strcmp( op , "!=" ) == 0 ) return cmp != 0;
	if ( strcmp( op , "<" ) == 0 )  return cmp < 0;
	if ( strcmp( op , "<=" ) == 0 ) return cmp <= 0;
	if ( strcmp( op , ">" ) == 0 )  return cmp > 0;
	if ( strcmp( op , ">=" ) == 0 ) return cmp >= 0;
	return 0;
}

_gim_flag gim_db_obj::update_record( const char * table_name , _gim_Uint8 ncond , const _gim_db_cond * cond , _gim_Uint8 nset , const char ** fields , const char ** values , int * updated ) {
	*updated = 0;
	_gim_db_table * t = seek_table( (char *) table_name );
	if ( t == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::update_record" , "UPDATE: table not found" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( ( t->fields == NULL ) || ( t->Tdata == NULL ) || ( t->Tstruct == NULL ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::update_record" , "UPDATE: table is not complete" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	_gim_flag ctype[ 255 ];
	if ( ! db_check_conditions( t , ncond , cond , ctype , "UPDATE" ) )
		return __GIM_ERROR;
	if ( nset == 0 )
		return __GIM_OK;
	if ( t->Tdata->ParserIsUp() != __GIM_YES )
		return __GIM_OK;			// no record has ever been saved: nothing to change

	// every new value is checked and normalised before anything is changed
	_gim_db_field * sf[ 255 ];
	char * nv = (char *) gim_memory->Alloc( nset * 512 , __GIM_MEM_DB_RECORD , __GIM_HIDE );
	for ( int k = 0 ; k < nset ; k++ ) {
		sf[ k ] = db_find_field( t , fields[ k ] );
		if ( sf[ k ] == NULL ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::update_record" , "UPDATE: table '%s' has no field '%s'" , t->name , fields[ k ] );
			gim_memory->Free( nv );
			return __GIM_ERROR;
		}
		for ( int j = 0 ; j < k ; j++ ) {
			if ( strcmp( fields[ j ] , fields[ k ] ) == 0 ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::update_record" , "UPDATE: field %s given twice" , fields[ k ] );
				gim_memory->Free( nv );
				return __GIM_ERROR;
			}
		}
		if ( db_normalise( "UPDATE" , sf[ k ] , values[ k ] , nv + k * 512 ) != __GIM_OK ) {
			gim_memory->Free( nv );
			return __GIM_ERROR;
		}
	}

	char * names = NULL;
	int n = db_collect( t , ncond , cond , ctype , &names );
	if ( n == 0 ) {
		gim_memory->Free( names );
		gim_memory->Free( nv );
		gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::update_record" , "UPDATE: no record of [%s] matches the conditions" , t->name );
		return __GIM_OK;
	}

	// KEY: only one record may change, and the new key must not be used by another record
	_gim_db_field * keyf = db_key_field( t );
	int key_set = -1;
	if ( keyf != NULL )
		for ( int k = 0 ; k < nset ; k++ )
			if ( sf[ k ] == keyf )
				key_set = k;
	if ( key_set >= 0 ) {
		if ( n > 1 ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::update_record" , "UPDATE: the KEY %s can be set only when one record matches (%d match)" , keyf->name , n );
			gim_memory->Free( names );
			gim_memory->Free( nv );
			return __GIM_ERROR;
		}
		int total = t->Tdata->GetHowManySection();
		for ( int s = 0 ; s < total ; s++ ) {
			char * sec = t->Tdata->GetSectionName( (_gim_int16) s );
			if ( sec == NULL )
				continue;
			char secname[ 64 ];
			strncpy( secname , sec , 63 );
			secname[ 63 ] = '\0';
			if ( strcmp( secname , names ) == 0 )
				continue;
			char * kv = t->Tdata->GetKeySTR( secname , keyf->name );
			if ( ( kv != NULL ) && ( strcmp( kv , nv + key_set * 512 ) == 0 ) ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::update_record" , "UPDATE: KEY %s = %s already exists" , keyf->name , nv + key_set * 512 );
				gim_memory->Free( names );
				gim_memory->Free( nv );
				return __GIM_ERROR;
			}
		}
	}

	// the change itself: the values are already valid, so a failure here is reported but not expected
	for ( int i = 0 ; i < n ; i++ ) {
		char * sec = names + i * 64;
		for ( int k = 0 ; k < nset ; k++ ) {
			_gim_db_field * f = sf[ k ];
			char * v = nv + k * 512;
			_gim_flag rc;
			if ( f->type == GIM_DB_TYPE_INT )
				rc = t->Tdata->ChangeKey( sec , f->name , atoi( v ) );
			else if ( f->type == GIM_DB_TYPE_FLOAT )
				rc = t->Tdata->ChangeKey( sec , f->name , (float) atof( v ) );
			else if ( ( f->type == GIM_DB_TYPE_BOOL ) || ( f->type == GIM_DB_TYPE_FLAG ) )
				rc = t->Tdata->ChangeKeyFlag( sec , f->name , ( strcmp( v , "YES" ) == 0 ) ? __GIM_YES : __GIM_NO );
			else
				rc = t->Tdata->ChangeKey( sec , f->name , (const char *) v );
			if ( rc != __GIM_OK ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::update_record" , "UPDATE: cannot change field %s of [%s]" , f->name , t->name );
				gim_memory->Free( names );
				gim_memory->Free( nv );
				return __GIM_ERROR;
			}
		}
	}

	if ( db->type == GIM_DB_PERMANENT )
		t->Tdata->Write();
	*updated = n;
	gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::update_record" , "UPDATE: %d record(s) changed in [%s]" , n , t->name );
	gim_memory->Free( names );
	gim_memory->Free( nv );
	return __GIM_OK;
}

_gim_flag gim_db_obj::delete_record( const char * table_name , _gim_Uint8 ncond , const _gim_db_cond * cond , int * deleted ) {
	*deleted = 0;
	_gim_db_table * t = seek_table( (char *) table_name );
	if ( t == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::delete_record" , "DELETE: table not found" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( ( t->fields == NULL ) || ( t->Tdata == NULL ) || ( t->Tstruct == NULL ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::delete_record" , "DELETE: table is not complete" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	_gim_flag ctype[ 255 ];
	if ( ! db_check_conditions( t , ncond , cond , ctype , "DELETE" ) )
		return __GIM_ERROR;
	if ( t->Tdata->ParserIsUp() != __GIM_YES )
		return __GIM_OK;			// no record has ever been saved

	char * names = NULL;
	int n = db_collect( t , ncond , cond , ctype , &names );
	for ( int i = 0 ; i < n ; i++ ) {
		if ( t->Tdata->DelSection( names + i * 64 ) != __GIM_OK ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::delete_record" , "DELETE: cannot remove a record of [%s]" , t->name );
			gim_memory->Free( names );
			return __GIM_ERROR;
		}
	}
	if ( n > 0 ) {
		t->items -= n;
		t->Tstruct->ChangeKey( "TABLE" , "items" , t->items );
		if ( db->type == GIM_DB_PERMANENT ) {
			t->Tdata->Write();
			t->Tstruct->Write();
		}
	}
	*deleted = n;
	gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::delete_record" , "DELETE: %d record(s) removed from [%s] (items %d)" , n , t->name , t->items );
	gim_memory->Free( names );
	return __GIM_OK;
}

_gim_flag gim_db_obj::drop_table( const char * table_name ) {
	_gim_db_table * t = seek_table( (char *) table_name );
	if ( t == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::drop_table" , "DROP: table not found" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	// position in the list = number tabN in the configuration (both start from 1, same order)
	int pos = 0;
	_gim_Uint32 id = 0;
	db->tables->rewind();
	for ( int i = 1 ; i <= db->tables->items() ; i++ ) {
		if ( (_gim_db_table *) db->tables->get_item() == t ) {
			pos = i;
			id = db->tables->get_id();
			break;
		}
		db->tables->next_item();
	}
	if ( pos == 0 ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::drop_table" , "DROP: table is not in the database list" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	// a pinned table must not stay pinned: db->Ttab would dangle
	if ( ( db->pin == __GIM_YES ) && ( db->Ttab == t ) ) {
		db->pin = __GIM_NO;
		db->Ttab = NULL;
		strcpy( db->pin_table , "" );
	}

	db->tables->del_item( id );

	// renumber tab1_name .. tabN_name without the dropped one
	int n = db->tables_number;
	char name_now[ 64 ];
	char key_now[ 32 ];
	char key_next[ 32 ];
	for ( int c = pos ; c < n ; c++ ) {
		sprintf( key_now , "tab%d_name" , c );
		sprintf( key_next , "tab%d_name" , c + 1 );
		strncpy( name_now , db->conf->GetKeySTR( "DB" , key_next ) , sizeof( name_now ) - 1 );
		name_now[ sizeof( name_now ) - 1 ] = '\0';
		db->conf->ChangeKey( "DB" , key_now , name_now );
	}
	sprintf( key_now , "tab%d_name" , n );
	db->conf->DelKey( "DB" , key_now );
	db->tables_number--;
	db->conf->ChangeKey( "DB" , "tables" , db->tables_number );

	// the parsers are released before their files are deleted
	if ( t->Tstruct != NULL ) {
		t->Tstruct->Down();
		delete t->Tstruct;
	}
	if ( t->Tdata != NULL ) {
		t->Tdata->Down();
		delete t->Tdata;
	}
	if ( t->fields != NULL ) {
		t->fields->rewind();
		for ( int i = 1 ; i <= t->fields->items() ; i++ ) {
			gim_memory->Free( t->fields->get_item() );
			t->fields->next_item();
		}
		delete t->fields;
	}
	delete t->records;

	if ( db->type == GIM_DB_PERMANENT ) {
		gim_file_manager->DELETE( t->file_name_struct_long );
		gim_file_manager->DELETE( t->file_name_data_long );
		db->conf->Write();
	}
	gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::drop_table" , "DROP: table [%s] dropped (tables left %d)" , t->name , db->tables_number );
	gim_memory->Free( t );
	return __GIM_OK;
}

/* ---- CSV import and export: the layout of the AI trainer (label,ch0,ch0_ok,...) ---- */

// 64 hex digits of a SHA-256 digest (32 bytes) plus the terminator
static void db_csv_hex( const unsigned char * d , char * out ) {
	for ( int i = 0 ; i < 32 ; i++ )
		sprintf( out + 2 * i , "%02x" , d[ i ] );
	out[ 64 ] = '\0';
}

// writes bytes to the file and feeds them to the running digest
static void db_csv_put( gim_sha256_obj * sha , sha2_context * ctx , _gim_handler * out , const char * data , int len ) {
	sha->SHA256_update( ctx , (unsigned char *) data , len );
	gim_file_manager->flush( out , (_gim_buffer) data , len );
}

// splits one line (no line break inside) into fields, unquoting them in work (at least strlen(line)+1 bytes).
// fields[] points into work. Returns the number of fields, or -1 when the quoting is wrong or there are too many.
static int db_csv_parse( const char * line , char * work , char ** fields , int max ) {
	int n = 0;
	const char * p = line;
	char * w = work;
	for ( ; ; ) {
		if ( n >= max )
			return -1;
		fields[ n ] = w;
		if ( *p == '"' ) {
			int closed = 0;
			p++;
			while ( *p ) {
				if ( *p == '"' ) {
					if ( p[ 1 ] == '"' ) {		// "" inside a quoted field is one quote
						*w++ = '"';
						p += 2;
					}
					else {
						p++;
						closed = 1;
						break;
					}
				}
				else
					*w++ = *p++;
			}
			if ( !closed || ( *p != '\0' && *p != ',' ) )
				return -1;
		}
		else {
			while ( *p && *p != ',' )
				*w++ = *p++;
		}
		*w++ = '\0';
		n++;
		if ( *p == ',' ) {
			p++;
			continue;
		}
		break;
	}
	return n;
}

// the field of the table with this name, or NULL
static _gim_db_field * db_field_named( _gim_db_table * t , const char * name ) {
	return db_find_field( t , name );
}

_gim_flag gim_db_obj::export_csv( const char * table_name , const char * path , char * sha256_hex ) {
	_gim_db_table * t = seek_table( (char *) table_name );
	if ( t == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::export_csv" , "EXPORT: table not found" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( ( t->fields == NULL ) || ( t->Tdata == NULL ) || ( t->Tstruct == NULL ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::export_csv" , "EXPORT: table is not complete" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	const int nf = t->fields->items();
	if ( nf < 1 || nf > 255 ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::export_csv" , "EXPORT: the table must have from 1 to 255 fields" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	_gim_handler * out = gim_file_manager->open( path , __GIM_FILE_POINTER , __GIM_WRITE );
	if ( out == NULL ) {
		gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::export_csv" , "EXPORT: cannot open [%s] for writing" , path );
		return __GIM_ERROR;
	}

	gim_sha256_obj sha;
	sha2_context ctx;
	sha.SHA256_init( &ctx );

	// header: the field names in table order
	t->fields->rewind();
	for ( int i = 0 ; i < nf ; i++ ) {
		_gim_db_field * f = (_gim_db_field *) t->fields->get_item();
		t->fields->next_item();
		if ( i > 0 )
			db_csv_put( &sha , &ctx , out , "," , 1 );
		db_csv_put( &sha , &ctx , out , f->name , (int) strlen( f->name ) );
	}
	db_csv_put( &sha , &ctx , out , "\n" , 1 );

	// one line per record (no records when the table data was never saved)
	char * names = NULL;
	int nrec = 0;
	if ( t->Tdata->ParserIsUp() == __GIM_YES )
		nrec = db_collect( t , 0 , NULL , NULL , &names );
	char line[ 1100 ];
	for ( int r = 0 ; r < nrec ; r++ ) {
		const char * sec = names + r * 64;
		t->fields->rewind();
		for ( int i = 0 ; i < nf ; i++ ) {
			_gim_db_field * f = (_gim_db_field *) t->fields->get_item();
			t->fields->next_item();
			char * v = t->Tdata->GetKeySTR( sec , f->name );
			if ( v == NULL )
				v = (char *) "";
			char tmp[ 512 ];
			strncpy( tmp , v , sizeof( tmp ) - 1 );
			tmp[ sizeof( tmp ) - 1 ] = '\0';
			if ( f->type == GIM_DB_TYPE_FLOAT ) {			// the stored float is padded on the left
				char * q = tmp;
				while ( *q == ' ' )
					q++;
				memmove( tmp , q , strlen( q ) + 1 );
			}
			if ( strchr( tmp , '\n' ) || strchr( tmp , '\r' ) ) {
				gim_memory->Free( names );
				gim_file_manager->close( out );
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::export_csv" , "EXPORT: value of field %s has a line break" , f->name );
				return __GIM_ERROR;
			}
			int k = 0;
			int needs = ( strchr( tmp , ',' ) != NULL ) || ( strchr( tmp , '"' ) != NULL );
			if ( needs )
				line[ k++ ] = '"';
			for ( const char * p = tmp ; *p ; p++ ) {
				if ( *p == '"' )
					line[ k++ ] = '"';
				line[ k++ ] = *p;
			}
			if ( needs )
				line[ k++ ] = '"';
			line[ k ] = '\0';
			if ( i > 0 )
				db_csv_put( &sha , &ctx , out , "," , 1 );
			db_csv_put( &sha , &ctx , out , line , k );
		}
		db_csv_put( &sha , &ctx , out , "\n" , 1 );
	}

	unsigned char digest[ 32 ];
	sha.SHA256_final( &ctx , digest );
	db_csv_hex( digest , sha256_hex );
	gim_memory->Free( names );
	gim_file_manager->close( out );
	gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::export_csv" , "EXPORT: %d record(s) of [%s] written to %s" , nrec , t->name , path );
	return __GIM_OK;
}

// copies the next line of buf (without '\n' and a final '\r') into lbuf, NUL-terminated.
// *pos is moved past the line. Returns the length of the line, or -1 at the end of the buffer.
static int db_csv_next( char ** pos , char * end , char * lbuf , int maxl ) {
	if ( *pos > end )
		return -1;
	char * p = *pos;
	int n = 0;
	while ( p < end && *p != '\n' ) {
		if ( n < maxl )
			lbuf[ n ] = *p;
		n++;
		p++;
	}
	*pos = ( p < end ) ? p + 1 : end + 1;
	if ( n > maxl )
		n = maxl;
	if ( n > 0 && lbuf[ n - 1 ] == '\r' )
		n--;
	lbuf[ n ] = '\0';
	return n;
}

// the longest line of buf (for the work areas)
static int db_csv_longest( const char * buf , int size ) {
	int best = 0 , cur = 0;
	for ( int i = 0 ; i < size ; i++ ) {
		if ( buf[ i ] == '\n' ) {
			if ( cur > best )
				best = cur;
			cur = 0;
		}
		else
			cur++;
	}
	return ( cur > best ) ? cur : best;
}

// true when the line has only spaces and tabs
static int db_csv_blank( const char * line , int len ) {
	for ( int i = 0 ; i < len ; i++ )
		if ( line[ i ] != ' ' && line[ i ] != '\t' )
			return 0;
	return 1;
}

_gim_flag gim_db_obj::import_csv( const char * table_name , const char * path , int * imported , char * sha256_hex ) {
	*imported = 0;
	sha256_hex[ 0 ] = '\0';
	_gim_db_table * t = seek_table( (char *) table_name );
	if ( t == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: table not found" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( ( t->fields == NULL ) || ( t->Tdata == NULL ) || ( t->Tstruct == NULL ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: table is not complete" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	const int nf = t->fields->items();
	if ( nf < 1 || nf > 255 ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: the table must have from 1 to 255 fields" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	// the KEY is optional (trainer datasets have repeated labels): when there is one, its values must be new
	_gim_db_field * keyf = db_key_field( t );

	_gim_handler * in = gim_file_manager->open( path , __GIM_FILE_POINTER , __GIM_READ );
	if ( in == NULL ) {
		gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: cannot open [%s]" , path );
		return __GIM_ERROR;
	}
	char * buf = (char *) gim_file_manager->load( in );
	const int size = (int) in->size;
	if ( buf == NULL ) {
		gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: cannot read [%s]" , path );
		gim_file_manager->close( in );
		return __GIM_ERROR;
	}

	// the digest is of the file as read
	gim_sha256_obj sha;
	sha2_context ctx;
	unsigned char digest[ 32 ];
	sha.SHA256_init( &ctx );
	sha.SHA256_update( &ctx , (unsigned char *) buf , size );
	sha.SHA256_final( &ctx , digest );
	db_csv_hex( digest , sha256_hex );

	// the buffer is never changed: each line is copied into lbuf, unquoted fields go to work
	const int maxl = db_csv_longest( buf , size ) + 2;
	char * lbuf = (char *) gim_memory->Alloc( maxl , __GIM_MEM_DB_RECORD , __GIM_HIDE );
	char * work = (char *) gim_memory->Alloc( maxl , __GIM_MEM_DB_RECORD , __GIM_HIDE );
	char * fields[ 255 ];
	const char * hdr_names[ 255 ];
	_gim_db_field * colf[ 255 ];
	int ncol = 0;
	int status = __GIM_OK;
	int bad_line = 0;			// line of the first refused record
	int inserted = 0;
	char * end = buf + size;

	// KEY values seen in the file so far (copies allocated with gim_memory: the list frees them when destroyed)
	gim_list_obj * seen = new gim_list_obj;

	// pass 1 checks every line; pass 2 inserts. On an insert failure the lines before it are removed by KEY.
	for ( int pass = 1 ; pass <= 2 && status == __GIM_OK ; pass++ ) {
		char * pos = buf;
		int lineno = 0;
		int header_done = 0;
		for ( ; ; ) {
			int len = db_csv_next( &pos , end , lbuf , maxl - 1 );
			if ( len < 0 )
				break;
			lineno++;
			const int blank = db_csv_blank( lbuf , len );
			if ( !header_done ) {
				if ( blank ) {
					gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: the file has no header" , __GIM_ERROR );
					status = __GIM_ERROR;
					break;
				}
				int n = db_csv_parse( lbuf , work , fields , 255 );
				if ( n < 1 ) {
					gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: bad quoting in the header" , __GIM_ERROR );
					status = __GIM_ERROR;
					break;
				}
				ncol = n;
				for ( int c = 0 ; c < n && status == __GIM_OK ; c++ ) {
					colf[ c ] = db_field_named( t , fields[ c ] );
					if ( colf[ c ] == NULL ) {
						gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: column [%s] is not a field of [%s]" , fields[ c ] , t->name );
						status = __GIM_ERROR;
						break;
					}
					hdr_names[ c ] = colf[ c ]->name;
					for ( int j = 0 ; j < c ; j++ )
						if ( colf[ j ] == colf[ c ] ) {
							gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: column [%s] given twice" , fields[ c ] );
							status = __GIM_ERROR;
						}
				}
				if ( status == __GIM_OK && ncol != nf ) {
					gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: the header has %d columns, the table has %d fields" , ncol , nf );
					status = __GIM_ERROR;
				}
				header_done = 1;
				continue;
			}
			if ( blank )
				continue;
			int n = db_csv_parse( lbuf , work , fields , 255 );
			if ( n != ncol ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: line %d has %d fields, the header has %d" , lineno , n , ncol );
				status = __GIM_ERROR;
				bad_line = lineno;
				break;
			}
			if ( pass == 1 ) {
				for ( int c = 0 ; c < ncol && status == __GIM_OK ; c++ ) {
					char tmp[ 512 ];
					if ( fields[ c ][ 0 ] == '\0' ) {
						gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: line %d, field %s is empty" , lineno , colf[ c ]->name );
						status = __GIM_ERROR;
					}
					else if ( strlen( fields[ c ] ) > 63 ) {
						gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: line %d, field %s is longer than 63 characters" , lineno , colf[ c ]->name );
						status = __GIM_ERROR;
					}
					else if ( db_normalise( "IMPORT" , colf[ c ] , fields[ c ] , tmp ) != __GIM_OK )
						status = __GIM_ERROR;
				}
				if ( status == __GIM_OK && keyf != NULL ) {
					// the KEY must be new: not twice in the file, not already in the table (insert_record would refuse it later)
					int kc = -1;
					for ( int c = 0 ; c < ncol ; c++ )
						if ( colf[ c ] == keyf )
							kc = c;
					const char * kv = fields[ kc ];
					for ( _gim_Uint32 s = 1 ; s <= seen->items() && status == __GIM_OK ; s++ )
						if ( strcmp( (const char *) seen->get_item( s ) , kv ) == 0 ) {
							gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: line %d, KEY %s = %s appears twice in the file" , lineno , keyf->name , kv );
							status = __GIM_ERROR;
						}
					if ( status == __GIM_OK && t->Tdata->ParserIsUp() == __GIM_YES ) {
						_gim_db_cond kcond;
						kcond.field = keyf->name;
						kcond.op = "=";
						kcond.value = kv;
						_gim_flag kt[ 1 ] = { keyf->type };
						char * found = NULL;
						if ( db_collect( t , 1 , &kcond , kt , &found ) > 0 ) {
							gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: line %d, KEY %s = %s already exists in [%s]" , lineno , keyf->name , kv , t->name );
							status = __GIM_ERROR;
						}
						gim_memory->Free( found );
					}
					if ( status == __GIM_OK ) {
						char * copy = (char *) gim_memory->Alloc( strlen( kv ) + 1 , __GIM_MEM_DB_RECORD , __GIM_HIDE );
						strcpy( copy , kv );
						seen->add_item( copy );
					}
				}
				if ( status != __GIM_OK )
					bad_line = lineno;
				continue;
			}
			if ( insert_record( table_name , (_gim_Uint8) ncol , hdr_names , (const char **) fields ) != __GIM_OK ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::import_csv" , "IMPORT: line %d refused: the records added before it are removed again" , lineno );
				status = __GIM_ERROR;
				bad_line = lineno;
				break;
			}
			inserted++;
		}
		if ( pass == 2 && status != __GIM_OK && bad_line > 0 && keyf != NULL ) {
			// roll back: remove by KEY the records of the data lines before bad_line
			char * rpos = buf;
			int rno = 0;
			int rheader = 0;
			for ( ; ; ) {
				int len = db_csv_next( &rpos , end , lbuf , maxl - 1 );
				if ( len < 0 )
					break;
				rno++;
				if ( rno >= bad_line )
					break;
				if ( !rheader ) {
					rheader = 1;
					continue;
				}
				if ( db_csv_blank( lbuf , len ) )
					continue;
				int n = db_csv_parse( lbuf , work , fields , 255 );
				for ( int c = 0 ; c < n && c < ncol ; c++ ) {
					if ( colf[ c ] != keyf )
						continue;
					_gim_db_cond kc;
					kc.field = keyf->name;
					kc.op = "=";
					kc.value = fields[ c ];
					int removed = 0;
					delete_record( table_name , 1 , &kc , &removed );
				}
			}
		}
	}

	delete seen;					// frees the copies of the KEY values
	gim_memory->Free( work );
	gim_memory->Free( lbuf );
	gim_file_manager->close( in );
	if ( status != __GIM_OK ) {
		sha256_hex[ 0 ] = '\0';
		return __GIM_ERROR;
	}
	*imported = inserted;
	gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::import_csv" , "IMPORT: %d record(s) added to [%s] from %s" , inserted , t->name , path );
	return __GIM_OK;
}

char * gim_db_obj::get_name( void ) {
	return name;
}


char * gim_db_obj::db_checksum( char * data ) {
	static char sum[16];
	static _gim_Uint32  gKsum = 0;
	_gim_Uint32 tmp = 0;
	_gim_Uchar len = 0 , c;

	gKsum = 0;
	while ( data[len] != '\0' ) {
		gKsum += data[len] + ( 0b10100100010000100000100000010000 >> len ) ^ ( 0b11101110111011101110111011101111 << len );
		len++;
	}
	for ( c = 1 ; c <= 32 ; c++ ) {
		tmp += ( gKsum << 1 ) ^ c;
		gKsum ^= tmp;
	}
	sprintf( sum , "%08X" , gKsum );
    return sum;
}



