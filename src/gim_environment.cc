// gim_environment.cc
//
// Gim  -  Generic Information Manager static library
// Version 1.0-0
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
//
// Gim is developed with Anjuta version 1.2.4
//

#include "../include/gim.h"

char			gim_application_name[256];

gim_conf_data	* env_data;
gim_home_obj	* gim_home;

void	gim_home_obj::up( void ) {
	char fdebug_name[256];
	struct stat stat_home;
	if ( initialized == __GIM_YES ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Environment aleady up" , __GIM_ERROR );
	}
	if ( initialized == __GIM_NO  ) {
		_gim_flag env_changed = __GIM_NO;
		gim_error->set( "gim_home_obj::up" , "Start to making gim environment" );
		gim_identity->get();

		sprintf( env_data->environ				, "%s/%s/%s"		, gim_identity->Home	, GIM_ENVIRON_NAME , GIM_ENVIRON_FILE_NAME );
		
		if ( gim_environ->Read( env_data->environ ) != __GIM_OK ) {
			gim_environ->Up( env_data->environ , GIM_ENVIRONMENT_TITLE );
			gim_environ->SetLex( __LEX_B );
		}
		if ( strlen( gim_application_name ) == 0 ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_home_obj::up" , "The application have not setted the name for the environment " , __GIM_ERROR );
			gim_error->set( GIM_ERROR_CRITICAL , "gim_home_obj::up" , "    - This is very dangerous. Other apllications that uses Gim will be damaged by this choice" , __GIM_ERROR );
			gim_error->set( GIM_ERROR_CRITICAL , "gim_home_obj::up" , "    - We suggest to set always an application name in order to separate each applications conf" , __GIM_ERROR );
			sprintf( env_data->home 			, "%s/%s/"		, gim_identity->Home	, GIM_ENVIRON_NAME );
			sprintf( env_data->general_home		, "%s/%s/"		, gim_identity->Home	, GIM_ENVIRON_NAME );
			sprintf( env_data->Programs			, "%s%s/"		, env_data->home		, GIM_PROGRAMS_DIR_NAME );
			sprintf( env_data->conf				, "%s%s"		, env_data->home		, GIM_CONFIGURATION_FILE_NAME );
			sprintf( fdebug_name				, "%s%s"		, env_data->home		, GIM_DEBUG_FILE_NAME );
			if ( stat( env_data->home 			, &stat_home ) ) {
				mkdir( env_data->home 			, 0777 );
				gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Main env not found... created" , __GIM_ERROR );
			}
		}
		else {
			sprintf( env_data->general_home		, "%s/%s/"		, gim_identity->Home	, GIM_ENVIRON_NAME );
			sprintf( env_data->home 			, "%s/%s/%s/"	, gim_identity->Home	, GIM_ENVIRON_NAME , gim_application_name );
			sprintf( env_data->Programs			, "%s/%s/%s/"	, gim_identity->Home	, GIM_ENVIRON_NAME , GIM_PROGRAMS_DIR_NAME );
			sprintf( env_data->conf				, "%s%s"		, env_data->home		, GIM_CONFIGURATION_FILE_NAME );
			sprintf( fdebug_name				, "%s%s"		, env_data->home		, GIM_DEBUG_FILE_NAME );
			if ( stat( env_data->general_home	, &stat_home ) ) {
				mkdir( env_data->general_home	, 0777 );
				gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Main env not found... created" , __GIM_ERROR );
			}
			if ( stat( env_data->home			, &stat_home ) ) {
				mkdir( env_data->home			, 0777 );
				gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Main env not found... created" , __GIM_ERROR );
			}
		}
		sprintf( env_data->PrivateKeyPath	, "%s%s/"	, env_data->general_home , GIM_PRIVATE_KEY_DIR_NAME  );
		sprintf( env_data->OtherKeysPath	, "%s%s/"	, env_data->general_home , GIM_OTHER_KEYS_DIR_NAME );
		IF_EXIST_SECTION( gim_environ , "gim" ) {
			IF_NOT_EXIST_KEY( gim_environ , "gim" , "home" ) {
				gim_environ->AddKey( "gim" , "home" , env_data->general_home );
				env_changed = __GIM_YES;
			}
			IF_NOT_EXIST_KEY( gim_environ , "gim" , "programs" ) {
				gim_environ->AddKey( "gim" , "programs" , env_data->Programs );
				env_changed = __GIM_YES;
			}
			IF_NOT_EXIST_KEY( gim_environ , "gim" , "PrivateKeyPath" ) {
				gim_environ->AddKey( "gim" , "PrivateKeyPath" , env_data->PrivateKeyPath );
				env_changed = __GIM_YES;
			}
			IF_NOT_EXIST_KEY( gim_environ , "gim" , "OtherKeysPath" ) {
				gim_environ->AddKey( "gim" , "OtherKeysPath" , env_data->OtherKeysPath );
				env_changed = __GIM_YES;
			}
		}
		else {
			gim_environ->AddSection( "gim" );
			gim_environ->AddKey( "gim" , "home"		, env_data->general_home );
			gim_environ->AddKey( "gim" , "programs" , env_data->Programs );
			gim_environ->AddKey( "gim" , "PrivateKeyPath" , env_data->PrivateKeyPath );
			gim_environ->AddKey( "gim" , "OtherKeysPath" , env_data->OtherKeysPath );
			env_changed = __GIM_YES;
		}
		if ( strlen( gim_application_name ) != 0 ) {
			IF_EXIST_SECTION( gim_environ , gim_application_name ) {
				IF_NOT_EXIST_KEY( gim_environ , gim_application_name , "home" ) {
					gim_environ->AddKey( gim_application_name , "home" , env_data->home );
					env_changed = __GIM_YES;
				}
				IF_NOT_EXIST_KEY( gim_environ , gim_application_name , "configuration" ) {
					gim_environ->AddKey( gim_application_name , "configuration" , env_data->conf );
					env_changed = __GIM_YES;
				}
				IF_NOT_EXIST_KEY( gim_environ , gim_application_name , "debug_fname" ) {
					gim_environ->AddKey( gim_application_name , "debug_fname" , fdebug_name );
					env_changed = __GIM_YES;
				}
			}
			else {
				gim_environ->AddSection( gim_application_name );
				gim_environ->AddKey( gim_application_name , "home" , env_data->home );
				gim_environ->AddKey( gim_application_name , "configuration" , env_data->conf );
				gim_environ->AddKey( gim_application_name , "debug_fname" , fdebug_name );
				env_changed = __GIM_YES;
			}
		}
		if ( stat( env_data->PrivateKeyPath , &stat_home ) ) {
			mkdir( env_data->PrivateKeyPath	, 0777 );
			gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Private key env not found... created" , __GIM_ERROR );
		}
		if ( stat( env_data->OtherKeysPath , &stat_home ) ) {
			mkdir( env_data->OtherKeysPath	, 0777 );
			gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Other key env not found... created" , __GIM_ERROR );
		}
		if ( stat( env_data->Programs , &stat_home ) ) {
			mkdir( env_data->Programs	, 0777 );
			gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Other programs env not found... created" , __GIM_ERROR );
		}
		if ( env_changed == __GIM_YES ) {
			gim_environ->SetLex( __LEX_B );
			gim_environ->Write();
		}
		if ( gim_conf->Read( env_data->conf ) != __GIM_OK ) {
			gim_memory->hide( __GIM_NO );
			gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Creation a default configuration file" , __GIM_ERROR );
			gim_conf->Up( env_data->conf , GIM_CONFIGURATION_TITLE );
			gim_conf->SetLex( __LEX_B );
			gim_conf->AddSection( "version" );
			gim_conf->AddSection( "system" );
			gim_conf->AddSection( "debug" );
			gim_conf->AddSection( "error" );
			gim_conf->AddSection( "memory" );
			gim_conf->AddSection( "crypt" );
			gim_conf->AddSection( "alias" );
			gim_conf->AddSection( "compatibility" );
			gim_conf->AddSection( "key" );
			gim_conf->AddSection( "Programs" );
			gim_conf->AddKey(		"version" 			, "name"				, GIM_NAME  );
			gim_conf->AddKey(		"version" 			, "major"				, GIM_MAJOR );
			gim_conf->AddKey(		"version" 			, "minor"				, GIM_MINOR );
			gim_conf->AddKey(		"version" 			, "subminor"			, GIM_SUBMINOR );
#ifdef GKMAKE_VERSIONING
			gim_conf->AddKey(		"version" 			, "built"				, GIM_GKMAKE_BUILD );
#endif
			gim_conf->AddKey(		"version" 			, "version"				, gim_version_small() );
			gim_conf->AddKeyFlag(	"system" 			, "sig_trap"			, __GIM_ON );
			gim_conf->AddKey(		"crypt" 			, "iterations"			, 2 );
			gim_conf->AddKeyFlag(	"error"				, "verbose_on_critical"	, __GIM_YES );
			gim_conf->AddKey( 		"debug"				, "debug_format"		, DEBUG_FORMAT_DEFAULT );
			gim_conf->AddKeyFlag(	"debug"				, "debug"				, __GIM_NO );
			gim_conf->AddKeyFlag(	"debug"				, "f_debug"				, __GIM_NO );
			gim_conf->AddKey( 		"debug"				, "f_debug_file"		, fdebug_name );
			gim_conf->AddKeyFlag(	"memory"			, "hide_in_log"			, __GIM_YES );
			gim_conf->AddKeyFlag(	"memory"			, "virtual_memory"		, __GIM_OFF );
			gim_conf->AddKeyFlag(	"memory"			, "alloc_limit"			, __GIM_YES );
			gim_conf->AddKey(		"memory"			, "limit"				, "75%" );
			gim_conf->AddKeyFlag(	"memory"			, "default_lock"		, __GIM_MEM_LOCK );
			gim_conf->AddKeyFlag(	"alias"				 , "enable"				, __GIM_YES );
			gim_conf->AddKeyFlag(	"alias"				, "lex"					, __LEX_A );
			gim_conf->AddKeyFlag(	"compatibility"		, "enable"				, __GIM_NO );
			gim_conf->AddKeyFlag(	"compatibility"		, "lex"					, __LEX_A );
			gim_conf->AddKey(		"key"				, "priv_path"			, env_data->PrivateKeyPath );
			gim_conf->AddKey(		"key"				, "other_path"			, env_data->OtherKeysPath );
			gim_conf->AddKey(		"Programs"			, "generic_path"		, env_data->Programs );
			gim_conf->AddKey(		"Programs"			, "program_path"		, env_data->home );
			
			add_default_comment();
			gim_conf->Write();
			gim_error->set( "gim_home_obj::up" , "Done!" );
		}
		if ( gim_conf->prsr_obj != NULL ) {
			_gim_flag changed = __GIM_NO;
			IF_EXIST_KEY_IN_CONF( "system" , "sig_trap" ) {
				if ( gim_conf->GetKeyFLAG( "system" , "sig_trap" ) == __GIM_ON ) 
					gim_signal_handler();
				else if ( gim_conf->GetKeyFLAG( "system" , "sig_trap" ) == __GIM_OFF ) 
					gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Gim signal handler DISABLED" , __GIM_ERROR );
				else {
					gim_error->set( GIM_ERROR_CRITICAL , "gim_home_obj::up" , "UNKNOWN value in SYSTEM::sig_trap" , __GIM_ERROR );
					gim_error->set( GIM_ERROR_CRITICAL , "gim_home_obj::up" , "Gim signal handler ACTIVATED per default" , __GIM_ERROR );
					gim_signal_handler();
				}
			}
			IF_EXIST_KEY_IN_CONF( "error" , "verbose_on_critical" ) {
				gim_error->set( GIM_VERBOSE_ON_CRITICAL , gim_conf->GetKeyFLAG( "error" , "verbose_on_critical" ) );
				if ( gim_error->get( GIM_VERBOSE_ON_CRITICAL ) == __GIM_YES )
					gim_error->set( "gim_home_obj::up" , "verbose_on_critical activated" );
				else
					gim_error->set( "gim_home_obj::up" , "verbose_on_critical de-activated" );
			}
			IF_EXIST_KEY_IN_CONF( "debug" , "debug" ) {
				internal_gim->debug = gim_conf->GetKeyFLAG( "debug" , "debug" );
				if ( internal_gim->debug == __GIM_YES ) {
					gim_error->set( GIM_ON_FATAL_VERBOSE , __GIM_YES );
					gim_error->set( GIM_ON_FATAL_EXIT , __GIM_YES );
					gim_error->set( GIM_ALWAYSE_VERBOSE , __GIM_YES );
					gim_error->set( GIM_AUTOGET , __GIM_YES );
					gim_error->set( "gim_home_obj::up" , "Visual debug activated" );
				}
			}
			IF_EXIST_KEY_IN_CONF( "debug" , "f_debug" ) {
				IF_EXIST_KEY_IN_CONF( "debug" , "f_debug_file" ) {	
					if ( gim_conf->GetKeyFLAG( "debug" , "f_debug" ) == __GIM_YES ) {
						gim_error->set( GIM_ON_FATAL_VERBOSE , __GIM_YES );
						gim_error->set( GIM_ON_FATAL_EXIT , __GIM_YES );
						gim_error->set( GIM_AUTOGET , __GIM_YES );
						gim_error->log_enable = __GIM_YES;
						gim_log->start();
						gim_error->set( "gim_home_obj::up" , "File debug activated" );
					}
					else {
						gim_error->log_enable = __GIM_NO;
					}
				}
			}
			IF_EXIST_KEY_IN_CONF( "memory" , "hide_in_log" ) {
				gim_memory->hide( gim_conf->GetKeyFLAG( "memory" , "hide_in_log" ) );
			}
			IF_EXIST_KEY_IN_CONF( "memory" , "virtual_memory" ) {
				gim_memory->Vmem( gim_conf->GetKeyFLAG( "memory" , "virtual_memory" ) );
			}
			else {
				gim_conf->AddKeyFlag( "memory"			, "virtual_memory"	, __GIM_ON );
				changed = __GIM_YES;
			}
			IF_EXIST_KEY_IN_CONF( "memory" , "default_lock" ) {
				gim_memory->set_default_lock( gim_conf->GetKeyFLAG( "memory" , "default_lock" ) );
				if ( gim_conf->GetKeyFLAG( "memory" , "default_lock" ) == __GIM_MEM_LOCK )
						gim_error->set( "gim_home_obj::up" , "The allocated memory was LOCKED by default" );
				else
						gim_error->set( "gim_home_obj::up" , "The allocated memory was UNLOCKED by default" );
			}
			IF_EXIST_KEY_IN_CONF( "memory" , "alloc_limit" ) {
				IF_EXIST_KEY_IN_CONF ( "memory" , "limit" ) {
					if ( gim_conf->GetKeyFLAG( "memory" , "virtual_memory" ) == __GIM_OFF ) {
						gim_memory->mem_alloc_limit( gim_conf->GetKeyFLAG( "memory" , "alloc_limit" ) , (_gim_Uint8)gim_conf->GetKeyINT( "memory" , "limit" ) );
					}
					else {
						gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Allocation memory disabled due virtual memory is ON" , __GIM_ERROR );
					}
				}
			}
			IF_EXIST_KEY_IN_CONF( "memory" , "log_in_file" ) {
				if ( gim_conf->GetKeyFLAG( "memory" , "log_in_file" ) == __GIM_ON ) {
					//attivare il memory log
				}
			}
			else {
				gim_conf->AddKeyFlag( "memory" , "log_in_file" , __GIM_NO );
				changed = __GIM_YES;
			}
			IF_EXIST_KEY_IN_CONF( "memory" , "log_file_name" ) {
			}
			else {
				gim_conf->AddKey( "memory" , "log_file_name" , "test_log_manager_file_name" );
				changed = __GIM_YES;
			}
			IF_EXIST_KEY_IN_CONF( "version" , "milestone" ) {
				if ( Lexical.str_equal( gim_conf->GetKeySTR( "version" , "milestone" ) , GIM_MILESTONE ) == __GIM_NO ) {
					gim_conf->ChangeKey( "version" , "milestone" , GIM_MILESTONE );
					changed = __GIM_YES;
					gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Gim major version updated" , __GIM_ERROR  );
				}
			}
			else {
				gim_conf->AddKey( "version" , "milestone" , GIM_MILESTONE );
			}
			IF_EXIST_KEY_IN_CONF( "version" , "major" ) {
				if ( gim_conf->GetKeyINT( "version" , "major" ) != GIM_MAJOR ) {
					gim_conf->ChangeKey( "version" , "major" , GIM_MAJOR );
					changed = __GIM_YES;
					gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Gim major version updated" , __GIM_ERROR );
				}
			}
			IF_EXIST_KEY_IN_CONF( "version" , "minor" ) {
				if ( gim_conf->GetKeyINT( "version" , "minor" ) != GIM_MINOR ) {
					gim_conf->ChangeKey( "version" , "minor" , GIM_MINOR );
					changed = __GIM_YES;
					gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Gim minor version updated" , __GIM_ERROR );
				}
			}
			IF_EXIST_KEY_IN_CONF( "version" , "subminor" ) {
				if ( gim_conf->GetKeyINT( "version" , "subminor" ) != GIM_SUBMINOR ) {
					gim_conf->ChangeKey( "version" , "subminor" , GIM_SUBMINOR );
					changed = __GIM_YES;
					gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Gim subminor version updated" , __GIM_ERROR );
				}
			}
			#ifdef GKMAKE_VERSIONING
				IF_EXIST_KEY_IN_CONF( "version" , "built" ) {
					if ( gim_conf->GetKeyINT( "version" , "built" ) != GIM_GKMAKE_BUILD ) {
						gim_conf->ChangeKey( "version" , "built" , GIM_GKMAKE_BUILD );
						changed = __GIM_YES;
						gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Gim built version updated" , __GIM_ERROR );
					}
				}
			#endif
			IF_EXIST_KEY_IN_CONF( "version" , "version" ) {
				if ( Lexical.str_equal( gim_conf->GetKeySTR( "version" , "version" ) , GIM_VERSION ) == __GIM_NO ) {
					gim_conf->ChangeKey( "version" , "version" , GIM_VERSION );
					changed = __GIM_YES;
					gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Gim version updated" , __GIM_ERROR );
				}
			}
			if ( changed == __GIM_YES ) {
				gim_conf->ChangeKey( "version" , "version" , GIM_VERSION );
				add_default_comment();
				gim_conf->Write();
				gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Something changed in configuration. File updated" , __GIM_ERROR );
			}
		}
		if ( gim_conf->GetKeyFLAG( "alias" , "enable" ) == __GIM_YES ) {
			char alias_name[1024];
			sprintf( alias_name , "%s%s" , env_data->home , GIM_ALIAS_FILE_NAME );
			if ( gim_alias->Read( alias_name ) != __GIM_OK ) {
				gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Creation of a default alias file" , __GIM_ERROR );
				gim_alias->Up( alias_name , GIM_ALIAS_TITLE );
				gim_alias->SetLex( gim_conf->GetKeyFLAG( "alias" , "lex" ) );
				gim_alias->AddSection( "me" );
				gim_alias->AddKey( "me" , "nick" , gim_identity->Login );
				gim_alias->AddSection( gim_identity->Login );
				if ( strlen( gim_identity->Login ) )
					gim_alias->AddKey( gim_identity->Login , "Login"		, gim_identity->Login );
				if ( strlen( gim_identity->Real_name ) ) {
					gim_alias->AddKey( gim_identity->Login , "Real_name"	, gim_identity->Real_name );
					//gim_alias->AddKeyComment( gim_identity->Login , "Real_name"	, PRSR_INLINE , "This is the real user name" );
				}
				if ( strlen( gim_identity->System_name ) )
					gim_alias->AddKey( gim_identity->Login , "System_name"	, gim_identity->System_name );
				if ( strlen( gim_identity->Node_name ) )
					gim_alias->AddKey( gim_identity->Login , "Node_name"	, gim_identity->Node_name );
				if ( strlen( gim_identity->Release ) )
					gim_alias->AddKey( gim_identity->Login , "Release"		, gim_identity->Release );
				if ( strlen( gim_identity->Arch ) )
					gim_alias->AddKey( gim_identity->Login , "Architetture"	, gim_identity->Arch );
				if ( strlen( gim_identity->Home ) )
					gim_alias->AddKey( gim_identity->Login , "Home"			, gim_identity->Home );
				gim_alias->AddKey( gim_identity->Login , "Host_id"			, (int)gim_identity->host_id );
				gim_alias->Write();
			}
			else {
				gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Checking for ME section configuration..." , __GIM_ERROR  );
				if ( gim_alias->ExistSection( gim_identity->Login ) == __GIM_EXIST ) {
					IF_EXIST_KEY_IN_CONF( "me" , "nick" ) {
						if ( strcmp( gim_alias->GetKeySTR( "me" , "nick" ) , gim_identity->Login ) ) {
							gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "ME::nick is not equal to the Login" , __GIM_ERROR  );
							gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "But the Login section exist...updating" , __GIM_ERROR  );
							gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , gim_alias->GetKeySTR( "me" , "nick" ) , __GIM_ERROR  );
							gim_alias->RenameSection( gim_identity->Login , gim_alias->GetKeySTR( "me" , "nick" ) );
							gim_alias->Write();
							gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "Updated" , __GIM_ERROR );
						}
					}
					else 
						gim_error->set( "gim_home_obj::up" , "ME section seems ok" );
				}
				if ( gim_alias->ExistSection( gim_alias->GetKeySTR( "me" , "nick" ) ) != __GIM_EXIST ) {
					gim_error->set( GIM_ERROR_WARNING , "gim_home_obj::up" , "The Nick  section was not found" , __GIM_ERROR );
					_gim_int16 n;
					n = gim_alias->GetHowManySection();
					if ( n > 0 ) {
						if ( gim_alias->ExistKey( gim_alias->GetSectionName( 1 ) , "nick" ) ) {
							gim_alias->RenameSection( gim_alias->GetSectionName( 1 ) , gim_alias->GetKeySTR( "me" , "nick" ) );
							gim_alias->Write();
							gim_error->set( "gim_home_obj::up" , "alias.conf updated." );
						}
						else {
							gim_error->set( GIM_ALWAYSE_VERBOSE , __GIM_YES );
							gim_error->set( GIM_ERROR_CRITICAL , "gim_home_obj::up" , "Some wrong data in alias.conf" , __GIM_ERROR );
							gim_error->set( GIM_ALWAYSE_VERBOSE , __GIM_NO );
						}
					}
				}
			}
		}
		gim_error->set( "gim_home_obj::up" , "Succesfully loaded" );
		initialized = __GIM_YES;
	}
}


void	gim_home_obj::down( void ) {
	gim_error->set( "gim_home_obj::env_down" , "Env goes down" );
	gim_environ->Down();
	if ( gim_conf->GetKeyFLAG( "alias" , "enable" ) == __GIM_YES ) 
		gim_alias->Down();
	gim_conf->Down();
	initialized = __GIM_NO;
	gim_error->set( "gim_home_obj::env_down" , "Env now is down" );
}


char *	gim_home_obj::get_generic_programs_env( void ) {
	return env_data->Programs;
}

char *	gim_home_obj::get_programs_env( void ) {
	return env_data->home;
}


char *	gim_home_obj::get_other_keys_path( void ) {
	return env_data->PrivateKeyPath;
}


void	gim_set_application_name( const char * app_name ) {
	__GIM_CLEAR( gim_application_name , 256 , char );
	strcpy( gim_application_name , app_name );
}


void	gim_home_obj::add_default_comment( void ) {
	gim_conf->AddKeyComment(	"debug"			, "debug_format"	, PRSR_AFTER , DEBUG_FORMAT_COMMENT );
	gim_conf->AddComment(		"Put your comments beside a key and in any other location.\n# E.g.\n#  __LEX_A\n#    login = asyntote   # this is a comment\n#  __LEX_B\n#    version::name = Gim   # this is a comment" );
}


