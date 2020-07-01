// main.cc
//
// Copyright (C) 2020 - Danilo
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#include <stdio.h>
#include <unistd.h>             
#include <gim/gim.h>
#include <gcg_main_header.h>


void hand_shake( void ) {
  puts  ( "  GimClassGen    -   gKript Gim Class Generator\n" );
  printf( "  Version-   -   -   %s\n" , GCG_VERSION );  
  puts  ( "  Author -   -   -   asyntote  [gKript.org]" );
  puts  ( "  Author -   -   -   Skymatrix [gKript.org]" );
}


int main( int argc , char **argv ) {
	
	if ( gim_check_version(2,8,7) != __GIM_OK ) {
		printf( "%s\n\n" , gim_version() );
		printf( "Gim is not updated at the required version.\n" );
		printf( "For %s is necessary Gim >= 2.8-7\n" , argv[0] );
		exit( -1 );
	}

	gim_set_application_name( "GimClassGen" );
    gim_obj *				gim = new gim_obj;
    gim_ascii_file_obj *	src_templ  = new gim_ascii_file_obj;
    src_templ->set_dimension( 1 *  __GIM_KB );
    gim_ascii_file_obj *	head_templ = new gim_ascii_file_obj;
    head_templ->set_dimension( 1 *  __GIM_KB );
   	gim_getopt_obj * 		opt = new gim_getopt_obj;
   	
   	gim->conf->ChangeKeyFlag( "debug", "f_debug" , __GIM_YES );
	gim->conf->AddKeyComment( "debug", "f_debug" , PRSR_AFTER , "Forced YES by main program" );
	gim->conf->Write();
	

	hand_shake();
	_LINE;
	printf( "  I am %s on %s\n" , gim->identity->login() , gim->identity->node() );
	_LINE;


	delete opt;
	delete src_templ;
	delete head_templ;
    delete gim;
    
    exit( 0 );
}

