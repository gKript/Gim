//
// gim_sh_environ_obj.cc
//
// Gim Shell  -	It will be an application within the project Gim.
//				Will be useful to set all the files conf of any application,
//				to manage the keys and for controlling any GKP file.
//
// Version 0.1-0
// AsYntote - SkyMatrix
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

#include "gim_shell.h"
		
_gim_kvalue			* gsh_conf;
	
char prompt[256];
static int gsh_conf_ready = 0;		// 1 if gsh_conf is a usable parser object

void set_gim_sh_environ( void ) {
	gsh_conf = new 	_gim_kvalue;
	char gsh_conf_path[1024];
	sprintf( gsh_conf_path , "%sgim_shell.conf" , gim_environ->GetKeySTR( "GIMSHELL" , "home" ) );
	_gim_flag read_result = gsh_conf->Read( gsh_conf_path );
	if ( read_result == __GIM_NOT_EXIST ) {
		gsh_conf->Up( gsh_conf_path , "GimShell_configuration_file" );
		gsh_conf->SetLex( __LEX_B );
		gsh_conf->AddSection( "variables" );
		gsh_conf->AddKey( "variables" , "prompt" , GIM_SH_DEFAULT_PROMPT );
		gsh_conf->Write();
		gsh_conf_ready = 1;
	}
	else if ( read_result == __GIM_OK )
		gsh_conf_ready = 1;
	else {
		// unreadable conf: the shell keeps working with the default prompt
		gim_error->set( GIM_ERROR_WARNING , "set_gim_sh_environ" , "Cannot read gim_shell.conf, using defaults" , __GIM_NOT_OK );
		gsh_conf_ready = 0;
	}
}

void shut_gim_sh_environ( void ) {
	gsh_conf->Down();
	delete gsh_conf;
}


void set_prompt( void ) {
	char format[256];
	char tmp[256];
	int c, len;
	// GetKeySTR returns NULL if the key is missing (e.g. the conf file was read badly)
	const char * conf_prompt = gsh_conf_ready ? gsh_conf->GetKeySTR( "variables" , "prompt" ) : NULL;
	if ( ! conf_prompt || ! *conf_prompt ) {
		gim_error->set( GIM_ERROR_WARNING , "set_prompt" , "Key variables::prompt not found, using default prompt" , __GIM_NOT_OK );
		conf_prompt = GIM_SH_DEFAULT_PROMPT;
	}
	strncpy( format , conf_prompt , sizeof( format ) - 1 );
	format[ sizeof( format ) - 1 ] = '\0';
//	puts( format);
	__GIM_CLEAR( tmp	, 256 , char );
	__GIM_CLEAR( prompt , 256 , char );
	for ( c = 0 , len = strlen( format ) ; c < len ; c++ ) {
		switch ( format[c] ) {
			case '$' : {
				switch ( format[++c] ) {
					case 'l' : {
//						puts( gim->identity->login() );
						strcat( tmp , gim->identity->login() );
						break;
					}
					case 'n' : {
//						puts( gim->identity->node() );
						strcat( tmp , gim->identity->node() );
						break;
					}
					case 'p' : {
//						puts( get_current_dir_name() );
						strcat( tmp , get_current_dir_name() );
						break;
					}
					case '$' : {
//						putchar( '$' );
						strcat( tmp , "$" );
						break;
					}
					case '_' : {
//						putchar( '_' );
						strcat( tmp , "_" );
						break;
					}
					default : {
						gim_error->set( GIM_ERROR_WARNING , "gim_log_obj::addline_format" , "Unknow variable in DEBUG::debug_format value" , __GIM_NOT_OK );
						break;
					}
				}
				break;
			}
			case '_' : {
//				putchar( ' ' );
				strcat( tmp , " " );
				break;
			}
			default : {
//				putchar( format[c] );
				sprintf( tmp , "%s%c" , tmp , format[c] );
				break;
			}
		}
	}
	strcpy( prompt , tmp );
}

