//
// gkfp.cc
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

int main ( int argc , char *argv[] ) {
	if ( argc < 2 ) {
		puts( "gkfp creator V0.3 usage :" );
		puts( "gkfp nome.gkp path [password]\n" );
		puts( "gkfp coded by AsYntote" );
		puts( "asyntote@gkript.org" );
		exit( -1 );
	}

	gim_set_application_name( "Gkfp" );

	gim_obj		* gim = new gim_obj;
	_gim_gkfp	* gkp = new _gim_gkfp;

	int stat=0;
	
	_gim_string	gkp_file_name( argv[1] );
	_gim_string	gkp_system( "gimstat " );
	
	if ( gkp_file_name.find( ".gkp" ) == __GIM_NO ) {
		if ( ( argc < 3 ) || ( argc > 4 ) ) {
			puts( "gkfp creator V0.3 usage :" );
			puts( "gkfp nome.gkp path [password]\n" );
			puts( "gkfp coded by AsYntote" );
			puts( "asyntote@gkript.org" );
			exit( -1 );
		}
		puts( "Generating mode" );
		gkp_file_name.cat( ".gkp" );
		if ( argc == 4 ) {
			gkp->New( gkp_file_name.c_str() , __GIM_YES , __GIM_FOG , __GIM_YES , __GIM_YES );
			gkp->set_password( argv[3] );
			puts("Crypt ACTIVATED");
		}
		else {
			gkp->New( gkp_file_name.c_str() , __GIM_YES , __GIM_SUN , __GIM_YES , __GIM_YES );
			puts("Crypt NOT acivated");
		}
		printf("Generating %s..." , gkp_file_name.c_str() );
		fflush( stdout );
		gkp->Set_prg( "Gkfp_creator" , 0 , 1 , 0 );
		gkp->New_path( argv[2] );
		gkp->Write();
		puts( "Done!");
		gkp_system.cat( gkp_file_name.c_str() );
		stat = system( gkp_system.c_str() );
//		printf("%d - %d\n" , stat , stat/256);
		if ( (stat/256) < 60 ) {
			puts("WARNING:");
			puts("The content of this file does not have a high level of gKaos and could be violated.\nWe recommend to repeat now this command with the encryption option activated simply typing a password after the file name and the path.");
			puts( "\ngkfp nome.gkp path [password]\n" );
		}
	}
	else {
		if ( ( argc < 2 ) || ( argc > 3 ) ) {
			puts( "gkfp creator V0.3 usage :" );
			puts( "gkfp nome.gkp path [password]\n" );
			puts( "gkfp coded by AsYntote" );
			puts( "asyntote@gkript.org" );
			exit( -1 );
		}
		puts( "Extracting mode\n" );
		if ( argc == 3 ) {
			gkp->set_password( argv[2] );
			puts("Crypt ACTIVATED");
		}
		gkp->Extract( gkp_file_name.c_str() );
		puts("\ndone!!!");
	}

	delete gkp;
	delete gim;
	
	return 0;
}
