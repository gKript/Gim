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
			gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::set_name" , "The db name must be lenght between 1 and 63 std characters" , __GIM_ERROR );
			return __GIM_ERROR;
		}
		char	tmp_name[64];
		char	message[256];
		strcpy( tmp_name	, dbname );
		strcpy( db->name	, Lexical.char_subst( tmp_name , ' ' , '_' ) );
		sprintf( home	, "%s%s" , env_data->home , db->name );
		sprintf( c_name	, "%s%s/%s.conf" , env_data->home , db->name , db->name );
		sprintf( d_name	, "%s%s/%s.gdb" , env_data->home , db->name , db->name );
		sprintf( message , "DB : db name setted [%s]" , db->name );
		gim_error->set( "gim_db_obj::set_name" , message );
		return __GIM_OK;
	}
	gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::set_name" , "The db name cannot be NULL" , __GIM_ERROR );
	return __GIM_ERROR;
}


char *	gim_db_obj::get_name( void ) {
	return db->name;
}


_gim_flag	gim_db_obj::set_db_properities( _gim_flag properities , _gim_flag value ) {
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
	if ( ( strlen( db->name ) > 64 ) || ( strlen( db->name ) < 1 ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::make_env" , "You try to create the DB env but before you must to set the DB name" , __GIM_ERROR );
		return __GIM_ERROR;
	}
//qq	puts( home );
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


_gim_flag	gim_db_obj::check( void ) {
	struct stat stat_home;
//	puts( home );
	if ( stat( home	, &stat_home ) ) {
		gim_error->set( "gim_db_obj::check" , "DB : db not exist" );
		return GIM_DB_NOT_EXIST;
	}
	if ( gim_file_manager->exist( c_name ) == __GIM_NOT_EXIST ) {
		gim_error->set( "gim_db_obj::check" , "DB : conf file not exist : not configured" );
		return GIM_DB_CONF_NOT_EXIST;
	}
	if ( gim_file_manager->exist( c_name ) == __GIM_NOT_EXIST ) {
		gim_error->set( "gim_db_obj::check" , "DB : gdb file not exist : not configured" );
		return GIM_DB_GDB_NOT_EXIST;
	}
	return GIM_DB_OK;
}

_gim_flag	gim_db_obj::init( void ) {
	_gim_flag	res;
//	puts( c_name );
	switch( db->conf->Read( c_name ) ) {
		case __GIM_OK : {
			gim_error->set( "gim_db_obj::init" , "DB : Properities read succesfully" );
			db->n_tables = db->conf->GetKeyINT( "Properities" , "tables" );
			res = GIM_DB_READ;
			break;
		}
		case __GIM_NOT_EXIST : {
			db->conf->Up( c_name , db->name );
			db->conf->SetLex( __LEX_B );
			db->conf->AddSection( "Properities" );
			db->conf->AddKey( "Properities"	, "name"			, db->name );
			db->conf->AddKey( "Properities"	, "mode"			, db->mode );
			db->conf->AddKey( "Properities"	, "type"			, db->type );
			db->conf->AddKey( "Properities"	, "tables"			, db->n_tables );
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
			db->conf->Up( c_name , db->name );
			db->conf->SetLex( __LEX_B );
			db->conf->AddSection( "Properities" );
			db->conf->AddKey( "Properities"	, "name"			, db->name );
			db->conf->AddKey( "Properities"	, "mode"			, db->mode );
			db->conf->AddKey( "Properities"	, "type"			, db->type );
			db->conf->AddKey( "Properities"	, "tables"			, db->n_tables );
			db->conf->AddKey( "Properities"	, "Gim"				, gim_version_micro() );
			db->conf->AddKey( "Properities"	, "Gim_maj"			, GIM_MAJOR );
			db->conf->AddKey( "Properities"	, "Gim_min"			, GIM_MINOR );
			db->conf->Write();
			res = GIM_DB_NEW;
			break;
		}			
		case __SYNTAX_ERROR : {
			db->conf->Up( c_name , db->name );
			db->conf->SetLex( __LEX_B );
			db->conf->AddSection( "Properities" );
			db->conf->AddKey( "Properities"	, "name"			, db->name );
			db->conf->AddKey( "Properities"	, "mode"			, db->mode );
			db->conf->AddKey( "Properities"	, "type"			, db->type );
			db->conf->AddKey( "Properities"	, "tables"			, db->n_tables );
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
		if ( Lexical.str_equal( db->conf->GetKeySTR( "Properities" , "name" ) , db->name  ) != __GIM_YES ) {
			gim_error->set( "gim_db_obj::init" , "DB : inconsistency between the given name and the configuration . Rewriting" );
			db->conf->ChangeKey( "properities" , "name" , db->name );
			db->conf->Write();
		}
		if ( ( db->conf->GetKeyINT( "Properities" , "type" ) != db->type ) || ( db->conf->GetKeyINT( "Properities" , "mode" ) != db->mode ) ) {
			initiated	= __GIM_YES;
			set_db_properities( db->mode	, __GIM_YES );
			set_db_properities( db->type	, __GIM_YES );
			db->conf->Write();
			gim_error->set( GIM_ERROR_WARNING , "gim_db_obj::init" , "DB : inconsistency between the required values and the configuration . Rewrote" , __GIM_ERROR );
		}
		else {
			set_db_properities( db->conf->GetKeyINT( "Properities" , "mode" )	, __GIM_YES );
			set_db_properities( db->conf->GetKeyINT( "Properities" , "type" )	, __GIM_YES );
		}
	}
	initiated	= __GIM_YES;
	return res;
}


_gim_flag	gim_db_obj::create_table( const char * table_name ) {
	char message[256];
	add_table( table_name );
	sprintf( message , "DB : new table created [%s]" , table_name );
	gim_error->set( "gim_db_obj::create_table" , message );
	return __GIM_OK;
}


_gim_flag	gim_db_obj::add_table( const char * table_name ) {
	_gim_db_table * tmp_tbl = db->first_table;
	_gim_db_table * new_tbl = NULL;
	if ( ( ! strlen( table_name ) ) || ( strlen( table_name ) >= 64 ) )
		return __GIM_ERROR;
	if ( db->first_table != NULL ) {
		for( ; tmp_tbl->next != NULL ; tmp_tbl = tmp_tbl->next ) ;
		new_tbl = ( _gim_db_table * )gim_memory->Alloc( sizeof( _gim_db_table ) , __GIM_MEM_DB_TABLE , __GIM_HIDE );
		strcpy( new_tbl->name , table_name );
		new_tbl->next = NULL;
		new_tbl->first_field = NULL;
		new_tbl->first_record = NULL;
		tmp_tbl->next = new_tbl;
		db->n_tables + 1;
		db->conf->ChangeKey( "properities" , "tables" , db->n_tables );
		changed = __GIM_YES;
	}
	if ( db->first_table == NULL ) {
		new_tbl = ( _gim_db_table * )gim_memory->Alloc( sizeof( _gim_db_table ) , __GIM_MEM_DB_TABLE , __GIM_HIDE );
		strcpy( new_tbl->name , table_name  );
		new_tbl->next = NULL;
		new_tbl->first_field = NULL;
		new_tbl->first_record = NULL;
		db->first_table = new_tbl;
		db->n_tables + 1;
		db->conf->ChangeKey( "properities" , "tables" , db->n_tables );
		changed = __GIM_YES;
	}
	return __GIM_OK;
}


_gim_flag	gim_db_obj::add_field_to_table( const char * table_name , const char * field_name , _gim_flag is_key , _gim_flag field_type , _gim_db_value * value ) {
	_gim_db_table	* tbl;
	char message[256];
	tbl = get_table_handle( table_name );
	if ( ! tbl ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::add_field_to_table" , "DB : Table not found" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	add_field( tbl , field_name , is_key , field_type , value );
	return __GIM_OK;
}


_gim_flag	gim_db_obj::add_field( _gim_db_table * tbl , const char * field_name , _gim_flag is_key , _gim_flag field_type , _gim_db_value * value ) {
	_gim_db_field   * tmp_fld = tbl->first_field;
	_gim_db_field   * new_fld = NULL;
	if ( tbl->first_field != NULL ) {
	}
	if ( tbl->first_field == NULL ) {
	}
	return __GIM_OK;
}

_gim_db_table *		gim_db_obj::get_table_handle( const char * table_name ) {
	_gim_db_table	* t_tbl = db->first_table;
	_gim_db_table	* n_tbl = NULL;
	for( ; t_tbl != NULL ; t_tbl = n_tbl ) {
		n_tbl = t_tbl->next;
		if ( Lexical.str_equal( table_name , t_tbl->name ) == __GIM_YES )
			return t_tbl;
	}
	gim_error->set( GIM_ERROR_CRITICAL , "gim_db_obj::get_table_handle" , "DB : Table not found" , __GIM_ERROR );
	return NULL;
}



