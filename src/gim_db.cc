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
	if ( dbname ) {
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
	gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::set_name" , "The db name cannot be NULL" , __GIM_ERROR );
	return __GIM_ERROR;
}

		

_gim_flag	gim_db_obj::set_properities( _gim_flag properities , _gim_flag value ) {
	switch ( properities ) {
		case GIM_DB_SAVE_MEMORY : {
			if ( value == __GIM_YES ) {
				db->mode = GIM_DB_SAVE_MEMORY;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "properities" , "mode" , GIM_DB_SAVE_MEMORY );
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : mode setted to GIM_DB_SAVE_MEMORY" );
			}
			else {
				db->mode = GIM_DB_BALANCED;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "properities" , "mode" , GIM_DB_BALANCED );
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : mode setted per default to GIM_DB_BALANCED" );
			}
			break;
		}
		case GIM_DB_PERFORMANCE : {
			if ( value == __GIM_YES ) {
				db->mode = GIM_DB_PERFORMANCE;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "properities" , "mode" , GIM_DB_PERFORMANCE );
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : mode setted to GIM_DB_PERFORMANCE" );
			}
			else {
				db->mode = GIM_DB_BALANCED;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "properities" , "mode" , GIM_DB_BALANCED );
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : mode setted per default to GIM_DB_BALANCED" );
			}
			break;
		}
		case GIM_DB_BALANCED : {
			if ( value == __GIM_YES ) {
				db->mode = GIM_DB_BALANCED;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "properities" , "mode" , GIM_DB_BALANCED );
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : mode setted to GIM_DB_BALANCED" );
			}
			else {
				db->mode = GIM_DB_BALANCED;
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : mode setted per default to GIM_DB_BALANCED" );
				gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::set_db_properities" , "DB :     You can't set GIM_DB_BALANCED to __GIM_MO" , __GIM_ERROR );
				changed		= __GIM_NO;
				return __GIM_ERROR;
			}
			break;
		}
		case GIM_DB_VOLATILE : {
			if ( value == __GIM_YES ) {
				db->type = GIM_DB_VOLATILE;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "properities" , "type" , GIM_DB_VOLATILE );
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : type setted to GIM_DB_VOLATILE" );
			}
			else {
				db->mode = GIM_DB_PERMANENT;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "properities" , "type" , GIM_DB_PERMANENT );
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : mode setted per default to GIM_DB_PERMANENT" );
			}
			break;
		}
		case GIM_DB_PERMANENT : {
			if ( value == __GIM_YES ) {
				db->type = GIM_DB_PERMANENT;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "properities" , "type" , GIM_DB_PERMANENT );
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : type setted to GIM_DB_PERMANENT" );
			}
			else {
				db->mode = GIM_DB_VOLATILE;
				if ( initiated == __GIM_YES )
					db->conf->ChangeKey( "properities" , "type" , GIM_DB_VOLATILE );
				gim_error->set( "gim_db_obj::set_db_properities" , "DB : mode setted per default to GIM_DB_VOLATILE" );
			}
			break;
		}
		default : {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::set_db_properities" , "DB :     Unknown properities." , __GIM_ERROR );
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
			db->conf->ChangeKey( "properities" , "name" , name );
			db->conf->Write();
		}
		if ( ( db->conf->GetKeyINT( "Properities" , "type" ) != db->type ) || ( db->conf->GetKeyINT( "Properities" , "mode" ) != db->mode ) ) {
			initiated	= __GIM_YES;
			set_properities( db->mode	, __GIM_YES );
			set_properities( db->type	, __GIM_YES );
			db->conf->Write();
			gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::init" , "DB : inconsistency between the required values and the configuration . Rewrote" , __GIM_ERROR );
		}
		else {
			set_properities( db->conf->GetKeyINT( "Properities" , "mode" )	, __GIM_YES );
			set_properities( db->conf->GetKeyINT( "Properities" , "type" )	, __GIM_YES );
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


char * 	gim_db_obj::gdbs_getline( FILE * fp ) {
	static char line[PRSR_MAX_LINE];
	__GIM_VCLEAR( line , PRSR_MAX_LINE , char ,'\0' );
	fgets( line , PRSR_MAX_LINE , fp );
	if ( feof(fp) ) 
		syntax->Feof = __GIM_ON;
	return line;
}



_gim_int8 gim_db_obj::gdbs_tokenizer( char * line , const char * separator ) {
	static unsigned int	i;
	unsigned int		a , b = 0;
	_gim_flag			for_exit = __GIM_NO;

	i = 0;
	__GIM_CLEAR( Tok , 1 , Tok );
	for( a = 0 ; ( ( a < ( strlen( line ) - 1 ) ) || ( for_exit == __GIM_NO ) ) ; a++ ) {
		if ( i > 19 ) {
			gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::db_tokenizer" , "too much token in line" , __GIM_ERROR  );
			return __SYNTAX_ERROR;
		}
		if ( lex->is_in_string( line[a] , separator ) ) {
			if ( ( line[a] == '#' ) || ( line[a] == '-' ) || ( line[a] == '\n' ) ) {
				for_exit = __GIM_YES;
			}
			else {
				Tok[i][b] = '\0' ;
				i++;
				b=0;
			}
		}
		else {
			Tok[i][b] = line[a];
			b++;
		}
	}
	return i;
}
		
