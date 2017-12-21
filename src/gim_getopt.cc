//
//  gim_getopt.cc
//
//  Gim - Generic Information Manager
//  Version 2.4-1  -  Milestone 2.5
//  Creator - asyntote
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
// Gim is developed with Anjuta version 1.2.4  - 2.32.1.1
//


#include    "../include/gim_base_header.h"
#include    "../include/gim_getopt.h"

void	gim_getopt_obj::setopt( const char * useropt ) {
	strcpy( options , useropt );
	gim_error->set( GIM_ERROR_CRITICAL , "gim_getopt_obj::setopt()" , "Item NOT FOUND" , __GIM_ERROR );
}


void	gim_getopt_obj::scanopt( int argc, char * const argv[] ) {
	gim_option_st *  temp_opt = NULL;
	char c;

	while ( ( c = getopt ( argc , argv , options )) != -1 ) {
		temp_opt = (gim_option_st *)gim_memory->Alloc ( sizeof( gim_option_st ) );
		temp_opt->option = c;
		temp_opt->argument = optarg;
		temp_opt->opterror = optopt;

		if ( c == '?' ) {
			if ( ( optopt == c ) && ( optarg == NULL ) ) 
				temp_opt->status = __GIM_OPT_NO_ARG;
			else if ( optopt != c )
				temp_opt->status = __GIM_OPT_UNKNOWN;
		}
		else 
			temp_opt->status = __GIM_OPT_OK;
		//optlist.add_item( (void *)temp_opt );
	}


}




