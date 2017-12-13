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

int main ( int argc , char *argv[] ) {
	_gim_flag	mode = __GIM_NO;
	
/*	if ( ( argc < 3 ) || ( argc > 4 ) ) {
		puts( "gkdp creator V0.3 usage :" );
		puts( "gkdp nome.gkp path [password]\n" );
		puts( "gkdp coded by AsYntote" );
		puts( "asyntote@gkript.org" );
		exit( -1 );
	}
*/	
	if ( gim_check_version(2,3,0) != __GIM_OK ) {
		printf( "%s\n\n" , gim_version() );
		printf( "Gim is not updated at the required version.\n" );
		printf( "For %s is necessary Gim >= 1.6-0\n" , argv[0] );
		exit( -1 );
	}
		
	gim_set_application_name( "Gkdp_creator" );
	gim_obj			* gim  = new gim_obj;
	gim_gkdp_obj	* gkdp = new gim_gkdp_obj;
	_gim_mempage	* mem  = new _gim_mempage;

	_gim_directory	* dir;

	puts("Gkdp creator 0.3-1");
	printf( "  Powered by %s\n" , gim_version() );
	
	if ( argc > 1 ) {
/*	
		if ( argc == 4 ) {
			gkdp->set_password( argv[3] );
			mode = __GIM_YES;
			puts("Crypt activated");
		}
		if ( argc == 3 ) {
			mode = __GIM_NO;
			puts("Crypt NOT activated");
		}
	
		printf("Generating %s..." , argv[1] );
		fflush( stdout );
		gkdp->New( argv[1] , mode );
		gkdp->set_prg( "Gkdp_creator" , 0 , 3 , 0 );
		gkdp->new_path( argv[2] );
		gkdp->write();
		puts( "Done!");

*/	
		_gim_dir_item	* test = NULL;
	
		dir = new _gim_directory;
		dir->recursion( __GIM_NO );
		dir->also_hide( __GIM_YES );
		if ( argc > 1 ) 
			dir->make( argv[1] );
		else {
			dir->make( "./" );
		}
		while( test = dir->get_item( GIM_DIR_INCREMENT ) ) {
			if ( test ) {
				char message[256];
				char type[16];
				switch( test->type ) {
					case REGFILE : {
						strcpy( type , "         " );
						sprintf( message , "%-10d%s%-40s" , test->size , type , test->name );
						break;
					}
					case EMPTY : {
						strcpy( type , "         " );
						sprintf( message , "%-10d%s%-40s" , test->size , type , test->name );
						break;
					}
					case REGNODE : {
						strcpy( type , " - Dir - " );
						sprintf( message , "          %s%-40s" , type , test->name );
						break;
					}
					default : {
						strcpy( type , "         " );
						sprintf( message , "          %s%-40s" , type , test->name );
						break;
					}
				}
				puts( message );
			}
			dir->free_item( test );
		}

	}
		
	delete dir;
	
	delete mem;
	delete gkdp;
	delete gim;
	
	return 0;
}
