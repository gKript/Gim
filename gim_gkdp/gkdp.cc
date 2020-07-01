//
// gkdp.cc
//
// Gim  -  Generic Information Manager static library
// Version 2.5-x
// AsYntote
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%201.0?by=version]
//
/*#############################################################################

	Copyright (C) 2002, 2003 
	Danilo Zannoni (AsYntote)

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

#include <gim/gim.h>

#define	GKDP_MAJOR			0
#define GKDP_MINOR			3
#define	GKDP_SUBMINOR		1
#define	GKDP_VERSION		"0.3-1"

#define GKDP_DEBUG			__GIM_YES
#define __GIM_KAOS_TRSHLD	55

#define _LINE				puts("")


void hand_shake( void ) {
	printf( "gkdp creator V%d.%d\n" , GKDP_MAJOR , GKDP_MINOR );
	_LINE;
	puts(	"gKript data package generator" );
	printf( "Powered by: %s\n" , gim_version_small() );
	puts(	"Coded by gKript.org" );
	puts(	"	!asyntote" );
	puts(	"	!skymatrix" );
	_LINE;
}


void usage( void ) {
	puts(	"Usage:" );
	puts( 	"  gkdp  GKP_file_name  path  [password]" );
	_LINE;
}


int main ( int argc , char *argv[] ) {
	_gim_flag	mode = __GIM_NO;
	int stat=0;
	
	if ( gim_check_version(2,8,7) != __GIM_OK ) {
		_LINE;
		printf( "%s\n\n" , gim_version() );
		printf( "Gim is not updated at the required version.\n" );
		printf( "For %s is necessary Gim >= 2.8-7\n" , argv[0] );
		exit( -1 );
	}
		
	hand_shake();
	
	if ( ( argc < 3 ) || ( argc > 4 ) ) {
		usage();
		exit( -1 );
	}

	gim_set_application_name( "Gkdp_creator" );
	gim_obj			* gim  = new gim_obj;
	gim_gkdp_obj	* gkdp = new gim_gkdp_obj;
	
	_gim_string	gkp_file_name( argv[1] );
	_gim_string	source( argv[2] );
	_gim_string	passw;
	_gim_string	gkp_system( "gimstat " );
	
	if ( GKDP_DEBUG == __GIM_YES ) {
	   	gim->conf->ChangeKeyFlag( "debug", "f_debug" , __GIM_YES );
		gim->conf->AddKeyComment( "debug", "f_debug" , PRSR_AFTER , "Forced YES by main program" );
	}
  	gim->conf->ChangeKey( "crypt", "iterations" , 5 );
  	gim->conf->Write();

	if ( argc > 1 ) {
	
		if ( argc == 4 ) {
			passw.set( argv[3] );
			gkdp->set_password( passw.c_str() );
			mode = __GIM_YES;
			puts("Crypt activated");
		}
		if ( argc == 3 ) {
			mode = __GIM_NO;
			puts("Crypt NOT activated");
		}
	
		printf("Generating %s..." , gkp_file_name.c_str() );
		fflush( stdout );
		gkdp->New( gkp_file_name.c_str() , mode );
		gkdp->set_prg( "Gkdp_creator" , 0 , 3 , 1 );
		gkdp->new_path( source.c_str() );
		gkdp->write();
		puts( "Done!\n");
		
		gkp_system.cat( gkp_file_name.c_str() );
		stat = system( gkp_system.c_str() );
//		printf("%d - %d\n" , stat , stat/256);
		if ( (stat/256) < __GIM_KAOS_TRSHLD ) {
			puts("\n  WARNING:");
			puts("      The content of this file does not have a high level of gKaos and could be violated.\n      We recommend to repeat now this command with the encryption option activated simply typing a password after the file name and the path.");
			puts( "\n          gkfp nome.gkp path [password]\n" );
		}
	}
	
	delete gkdp;
	delete gim;
	
	return 0;
}
