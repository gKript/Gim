//
// main.cc
//
// Gim Shell  -  a GIM application to work 
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

gim_obj			* gim;
_gim_mempage	* memory;
_gim_list		* list;

int main( int argc , char **argv ) {
	if ( gim_check_version(2,3,0) != __GIM_OK ) {
		printf( "%s\n\n" , gim_version() );
		printf( "Gim is not updated at the required version.\n" );
		printf( "For GimShell is necessary Gim >= 2.3-0\n" );
		exit( -1 );
    }
	gim_set_application_name( "GimShell" );
	gim_set_application_version( "0.1.2" );
	gim = new gim_obj;
//	memory = new _gim_mempage;				// test puorpose only
	
/*	gim_db_obj * ndb = new gim_db_obj( "test db" , GIM_DB_VOLATILE , GIM_DB_PERFORMANCE );		// test puorpose only
	ndb->create_table( "first" );
	ndb->create_table( "second" );
	ndb->create_table( "third" );
*/
//	gim_error->app( GIM_ERROR_FATAL , "::main" , "test di error con : %s - %s" , gim_version_small() , gim_version_micro() );

	puts( "Gim shell 0.1.2" );
	puts( gim_version_small() );
	puts("");
	set_gim_sh_environ();
	set_prompt();
	gim_sh_mainloop();
	gim_sh_db_shutdown();
	shut_gim_sh_environ();

//	delete ndb;			// test puorpose only
//	delete memory;		// test puorpose only

	delete gim;
	return 0;
}
