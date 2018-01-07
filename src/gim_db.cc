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

/*!	\file		gim_db.cc
	\version	2.5-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/

#include "../include/gim_base_header.h"
#include "../include/gim_db.h"
#include "../include/gim_environment.h"
#include "../include/gim_file.h"


_gim_flag	gim_db_obj::set_name( const char * dbname ) {
	if ( ( strlen( dbname ) > 64 ) || ( strlen( dbname ) < 1 ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::set_name" , "The db name must be lenght between 1 and 64 std characters" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	char	tmp_name[64];
	char	message[256];

	strcpy( tmp_name	, dbname );
	strcpy( name	, lex->char_subst( tmp_name , ' ' , '_' ) );
	sprintf( home	, "%s%s" , env_data->home , name );
	sprintf( file_name	, "%s%s/%s.conf" , env_data->home , name , name );
	sprintf( d_name	, "%s%s/%s.gdb" , env_data->home , name , name );
	sprintf( message , "DB : db name setted [%s]" , name );
	gim_error->set( "gim_db_obj::set_name" , message );
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
	puts( home );
	if ( db->type == GIM_DB_PERMANENT ) {
		if ( stat( home	, &stat_home ) ) {
			mkdir( home	, 0777 );
			gim_error->set( "gim_db_obj::make_env" , "DB : env created" );
		}
		else 
			gim_error->set( "gim_db_obj::make_env" , "DB : env already exist" );
	}
	else 
		gim_error->set( "gim_db_obj::make_env" , "DB : env not created : Volatile db" );
	return __GIM_OK;
}


_gim_flag	gim_db_obj::init( void ) {
	_gim_flag	res;
//	puts( c_name );
	switch( db->conf->Read( file_name ) ) {
		case __GIM_OK : {
			gim_error->set( "gim_db_obj::init" , "DB : Properities read succesfully" );
			db->tables_number = db->conf->GetKeyINT( "Properities" , "tables" );
			res = GIM_DB_READ;
			break;
		}
		case __GIM_NOT_EXIST : {
			make_env();
			db->conf->Up( file_name , name );
			db->conf->SetLex( __LEX_B );
			db->conf->AddSection( "Properities" );
			db->conf->AddKey( "Properities"	, "name"			, name );
			db->conf->AddKey( "Properities"	, "mode"			, db->mode );
			db->conf->AddKey( "Properities"	, "type"			, db->type );
			db->conf->AddKey( "Properities"	, "tables"			, db->tables_number );
			db->conf->AddKey( "Properities"	, "Gim"				, gim_version_micro() );
			db->conf->AddKey( "Properities"	, "Gim_maj"			, GIM_MAJOR );
			db->conf->AddKey( "Properities"	, "Gim_min"			, GIM_MINOR );
			if ( db->type == GIM_DB_PERMANENT ) {
				db->conf->Write();
				gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::init" , "DB : Conf file not found. Rewriting" , __GIM_ERROR );
			}
			else
				gim_error->set( "gim_db_obj::init" , "DB : type Permanent : conf only in memory" );
			res = GIM_DB_NEW;
			break;
		}
		case __LEX_UNKNOW : {
			gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::init" , "DB : Conf file Lex unknown. Rewriting" , __GIM_ERROR );
			db->conf->Up( file_name , name );
			db->conf->SetLex( __LEX_B );
			db->conf->AddSection( "Properities" );
			db->conf->AddKey( "Properities"	, "name"			, name );
			db->conf->AddKey( "Properities"	, "mode"			, db->mode );
			db->conf->AddKey( "Properities"	, "type"			, db->type );
			db->conf->AddKey( "Properities"	, "tables"			, db->tables_number );
			db->conf->AddKey( "Properities"	, "Gim"				, gim_version_micro() );
			db->conf->AddKey( "Properities"	, "Gim_maj"			, GIM_MAJOR );
			db->conf->AddKey( "Properities"	, "Gim_min"			, GIM_MINOR );
			db->conf->Write();
			res = GIM_DB_NEW;
			break;
		}			
		case __SYNTAX_ERROR : {
			db->conf->Up( file_name , name );
			db->conf->SetLex( __LEX_B );
			db->conf->AddSection( "Properities" );
			db->conf->AddKey( "Properities"	, "name"			, name );
			db->conf->AddKey( "Properities"	, "mode"			, db->mode );
			db->conf->AddKey( "Properities"	, "type"			, db->type );
			db->conf->AddKey( "Properities"	, "tables"			, db->tables_number );
			db->conf->AddKey( "Properities"	, "Gim"				, gim_version_micro() );
			db->conf->AddKey( "Properities"	, "Gim_maj"			, GIM_MAJOR );
			db->conf->AddKey( "Properities"	, "Gim_min"			, GIM_MINOR );
			db->conf->Write();
			gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::init" , "DB : Some syntax error in the conf file. Rewriting" , __GIM_ERROR );
			res = GIM_DB_NEW;
			break;
		}
	}
	if ( res == GIM_DB_READ ) {
		if ( ( db->conf->GetKeyINT( "Properities" , "Gim_maj" ) >= 2 ) && ( db->conf->GetKeyINT( "Properities" , "Gim_min" ) >= 3 ) )
			gim_error->set( "gim_db_obj::init" , "DB : This version of Gim is compatible" );
		else {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::init" , "DB : This version of Gim is not compatible with this DB" , __GIM_ERROR );
			initiated	= __GIM_NO;
			return __GIM_ERROR;
		}
		if ( Lexical.str_equal( db->conf->GetKeySTR( "Properities" , "name" ) , name  ) != __GIM_YES ) {
			gim_error->set( "gim_db_obj::init" , "DB : inconsistency between the given name and the configuration . Rewriting" );
			db->conf->ChangeKey( "property" , "name" , name );
			db->conf->Write();
		}
		if ( ( db->conf->GetKeyINT( "Properities" , "type" ) != db->type ) || ( db->conf->GetKeyINT( "Properities" , "mode" ) != db->mode ) ) {
			initiated	= __GIM_YES;
			set_property( db->mode	, __GIM_YES );
			set_property( db->type	, __GIM_YES );
			db->conf->Write();
			gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::init" , "DB : inconsistency between the required values and the configuration . Rewrote" , __GIM_ERROR );
		}
		else {
			set_property( db->conf->GetKeyINT( "Properities" , "mode" )	, __GIM_YES );
			set_property( db->conf->GetKeyINT( "Properities" , "type" )	, __GIM_YES );
		}
	}
	initiated	= __GIM_YES;
	return res;
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
	sprintf( file_name	, "%s%s/%s.conf" , env_data->home , name , name );
	gim_error->set( "gim_db_obj::read" , "I trying to read the DB" );
	result = db->conf->Read( file_name );
	if ( result == __GIM_NOT_EXIST ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::read" , "I cannot found the DB file." , __GIM_ERROR );
		return __GIM_NOT_EXIST;
	}
	if ( result == __GIM_NOT_OK ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::read" , "Something wrong with the DB file. Sorry." , __GIM_ERROR );
		return __GIM_NOT_EXIST;
	}
	gim_error->set( "gim_db_obj::read" , "DB file succesfully red" );
	return __GIM_OK;
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
		gim_error->set( GIM_ERROR_WARNING , "prsr_lexical_class::gdbs_open" , "No file name set" , __GIM_ERROR  );
	}
	else if ( ! gdbs_file ) {
		gim_error->set( GIM_ERROR_WARNING , "prsr_lexical_class::gdbs_open" , "File is not open" , __GIM_ERROR  );
	}
	else {
		gim_file_manager->close( gdbs_file );
		gim_error->set( "prsr_lexical_class::gdbs_open" , "File closed" );
	}
}


_gim_flag	gim_db_obj::gdbs_line_syntax_check( _gim_Uint8 NoT ) {
	static _gim_Uint8   id;

	id = 0;
	if      ( ( lex->str_equal( Tok[0] , "CREATE" ) == __GIM_YES ) && ( ( NoT == 4 ) || ( NoT == 3 ) ) )
		id += 10;
	else if ( ( lex->str_equal( Tok[0] , "ADD" ) == __GIM_YES ) && ( NoT == 5 ) )
		id += 20;
	else if ( ( lex->str_equal( Tok[0] , "SET" ) == __GIM_YES ) && ( ( NoT == 4 ) || ( NoT == 3 ) ) )
		id += 30;
	else if ( ( lex->str_equal( Tok[0] , "PIN" ) == __GIM_YES ) && ( NoT == 3 ) )
		id += 40;
	else if ( ( lex->str_equal( Tok[0] , "UNPIN" ) == __GIM_YES ) && ( NoT == 2 ) )
		id += 50;
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
	else
		id = 0;

	if ( id == 0 ) {
		gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN TOKEN : ID = %2d - NoT = %d - [%s] [%s]", id , NoT , Tok[0] , Tok[1] );
		return __GIM_ERROR;
	}
	gim_error->Set( "gim_db_obj::gdbs_line_syntax_check" , "ID = %2d - NoT = %d - [%s] [%s]", id , NoT , Tok[0] , Tok[1] );
	switch ( id ) {
		case 11 : {
			_gim_gdbs_line * line = (_gim_gdbs_line *)gim_memory->Alloc( sizeof( _gim_gdbs_line ) , __GIM_GDBS_LINE , __GIM_HIDE );
			if ( line == NULL ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "GDBS line allocation failed." , __GIM_ERROR );
				return __GIM_ERROR;
			}
			line->command = __GDBS_CREATE_DB;
			strcpy( line->fparameter , Tok[2] );
			strcpy( line->sparameter , Tok[3] );
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
			line->command = __GDBS_ADD_FIELD;
			line->fvalue , atoi( Tok[2] );
			line->svalue = string_to_flag( 3 , __GIM_FIELD );
			if ( line->svalue == __GIM_ERROR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN PROPERTIES [%s]." , Tok[3] );
				return __GIM_ERROR;
			}
			strcpy( line->tparameter , Tok[4] );
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
			line->command = __GDBS_SET_DB;
			line->fvalue = string_to_flag( 2 , __GIM_DB );
			if ( line->fvalue == __GIM_ERROR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN PROPERTIES [%s]." , Tok[2] );
				return __GIM_ERROR;
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
			line->command = __GDBS_SET_TABLE;
			line->fvalue = string_to_flag( 2 , __GIM_TABLE );
			if ( line->fvalue == __GIM_ERROR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN PROPERTIES [%s]." , Tok[2] );
				return __GIM_ERROR;
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
			line->command = __GDBS_SET_FIELD;
			line->fvalue , atoi( Tok[2] );
			line->svalue = string_to_flag( 3 , __GIM_FIELD );
			if ( line->svalue == __GIM_ERROR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_db_obj::gdbs_line_syntax_check" , "UNKNOWN PROPERTIES [%s]." , Tok[3] );
				return __GIM_ERROR;
			}
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
			line->command = __GDBS_UNPIN_TABLE;
			gdbs_script->add_item( line );
			gim_error->set( "gim_db_obj::gdbs_line_syntax_check" , "Syntax ok! SET TABLE command added to list." );
			return __GDBS_UNPIN_TABLE;
		}
	}

	return 0;
}


_gim_flag	gim_db_obj::init_from_gdbs( const char * gdbs_name ) {
	_gim_int8 t = 0;
	_gim_int8 s = 0;
	_gim_int8 f = 0;
	_gim_int8 l = 1;

	gim_error->Set( GIM_ERROR_OK , "gim_db_obj::init_from_gdbs" , "Start reading GDBS file [%s]" , gdbs_name );
	strcpy( gdbs_file_name , gdbs_name );
	gdbs_open();
	while ( ( ! gdbs_feof() ) && ( t >= 0 ) ) {
		gim_error->Set( GIM_ERROR_MESSAGE , "prsr_lexical_class::init_from_gdbs" , "Parsing line %d..." , l );
		t = gdbs_tokenizer( gdbs_getline() );
		if ( t < 0 ) {
			if ( t ==  __TOO_MUCH_TOKEN ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "too much token in line %d" , l );
				return __TOO_MUCH_TOKEN;
			}
			else if ( t == __MISSING_STOP_CHAR ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "Missing line terminator in line %d" , l );
				return __MISSING_STOP_CHAR;
			}
			else {
				gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "Something wrong in line %d" , l );
				return __MISSING_STOP_CHAR;
			}
		}
		else if ( t > 0 ) {
			s = gdbs_line_syntax_check( t );
			if ( s <= 0 ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "prsr_lexical_class::init_from_gdbs" , "Something wrong in syntax check in line %d" , l );
				return __GIM_ERROR;
			}
		}
		else
			gim_error->Set( "prsr_lexical_class::init_from_gdbs" , "SKIPPED" );
		l++;
	}
	f = gdbs_execute();
	gim_error->Set( "prsr_lexical_class::init_from_gdbs" , "GDBS: all done!" );

	gdbs_close();
	return __GIM_OK;
}


_gim_int8 gim_db_obj::gdbs_tokenizer( char * line ) {
	if ( !strlen( line ) )
		return __SKIPPED;

	static unsigned int	i;
	unsigned int		a , b = 0;
	volatile _gim_flag	for_exit = __GIM_NO , termination = __GIM_NO;

	i = 0;
	__GIM_VCLEAR( Tok , 1 , Tok , '\0' );
	if ( lex->is_in_string( GDBS_TERMINATOR_CHAR , line ) == __GIM_YES )
		termination = __GIM_YES;
	for( a = 0 ; ( ( a < ( strlen( line ) - 1 ) ) && ( for_exit == __GIM_NO ) ) ; a++ ) {
		if ( ( i > 19 ) || ( line[a] == GDBS_STOP_CHAR ) )
			break;
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

	if ( i > 19 ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::gdbs_tokenizer" , "too much token in line" , __GIM_ERROR  );
		return __TOO_MUCH_TOKEN;
	}
	if ( ( i ) && ( strlen( Tok[0] ) ) && ( termination == __GIM_YES ) ) {
		gim_error->Set( GIM_ERROR_MESSAGE , "prsr_lexical_class::gdbs_tokenizer" , "Correct GDBS line analyzed [extracted %d token]" , i );
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
	gim_error->Set( GIM_ERROR_MESSAGE , "gim_db_obj::string_to_flag" , "Tok = %-15s , index = %2d , Context = %2d" , Tok[ index ] , index , context );
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
		gim_error->Set( GIM_ERROR_WARNING , "gim_db_obj::string_to_flag" , "Script list is empty. Nothing to do." );
		return __GIM_ERROR;
	}
	gdbs_script->rewind();

	_gim_gdbs_line   * line = NULL;

	do {
		line = (_gim_gdbs_line *)gdbs_script->get_item();
		if ( line != NULL ) {
			gim_error->Set( GIM_ERROR_WARNING , "gim_db_obj::string_to_flag" , "Got line %d" , gdbs_script->get_id() );
			//  -----
			gdbs_script->next_item();
		}
	} while ( line != NULL );
	return __GIM_OK;
}




