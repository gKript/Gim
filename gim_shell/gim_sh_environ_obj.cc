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
		
_gim_kvalue			* gsh_conf;
	
char prompt[256];

void set_gim_sh_environ( void ) {
	gsh_conf = new 	_gim_kvalue;
	char gsh_conf_path[1024];
	sprintf( gsh_conf_path , "%sgim_shell.conf" , gim_environ->GetKeySTR( "GIMSHELL" , "home" ) );
	if ( gsh_conf->Read( gsh_conf_path ) == __GIM_NOT_EXIST ) {
		gsh_conf->Up( gsh_conf_path , "GimShell_configuration_file" );
		gsh_conf->SetLex( __LEX_B );
		gsh_conf->AddSection( "variables" );
		gsh_conf->AddKey( "variables" , "prompt" , "($l@$n)_$p_$$_" );
		gsh_conf->Write();
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
	strcpy( format , gsh_conf->GetKeySTR( "variables" , "prompt" ) );
//	puts( format);
	__GIM_CLEAR( tmp	, 256 , char );
	__GIM_CLEAR( prompt , 256 , char );
	for ( c = 0 , len = strlen( format ) ; c <= len ; c++ ) {
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

