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
}


void	gim_getopt_obj::scanopt( int argc, char * const argv[] ) {
	_gim_option *  temp_opt = NULL;
	char c;

	while ( ( c = getopt ( argc , argv , options )) != -1 ) {
		temp_opt = (_gim_option *)gim_memory->Alloc ( sizeof( _gim_option ) , __GIM_GETOPT_ITEM , __GIM_HIDE  );
		temp_opt->option = c;
		if ( optarg == NULL ) 
			temp_opt->argument = NULL;
		else
			temp_opt->argument = strdup( optarg );
		temp_opt->opterror = optopt;
//		printf( "opt %c - arg %s - optopt %X\n" , c , optarg , optopt );
//		printf( "opt %c - arg %s - optopt %X\n" , temp_opt->option , temp_opt->argument , temp_opt->opterror );
		if ( ( c != '?' ) && ( c != ':' ) ) {
			temp_opt->status = __GIM_OPT_OK;
			optlist->add_item( temp_opt );
		}
		else {
			temp_opt->option = optopt;
			if ( options[0] == ':' ) {
				if ( c == ':' ) 
					temp_opt->status = __GIM_OPT_NO_ARG;
				if ( c == '?' )
					temp_opt->status = __GIM_OPT_UNKNOWN;
			}
			else {
				if ( c == '?' )
					temp_opt->status = __GIM_OPT_NOT_OK;
			}
			errlist->add_item( temp_opt );
		}
	}
}


void	gim_getopt_obj::scanopt( int argc, char * const argv[] , const char * useropt ) {
	setopt( useropt );
	scanopt( argc , argv );
}


gim_option_item *   gim_getopt_obj::getoption( void ) {
	return (gim_option_item *)optlist->get_item();
}


gim_option_item *   gim_getopt_obj::geterror( void ) {
	return (gim_option_item *)errlist->get_item();
}


void	gim_getopt_obj::nextoption( void ) {
	optlist->next_item();
}


void	gim_getopt_obj::nexterror( void ) {
	errlist->next_item();
}


_gim_Uint32	gim_getopt_obj::opt_members( void ) {
	return  optlist->items();
}


_gim_Uint32	gim_getopt_obj::err_members( void ) {
	return  errlist->items();
}


void	gim_getopt_obj::rewind( void ) {
	optlist->rewind();
	errlist->rewind();
}


_gim_flag   gim_getopt_obj::errors_present( void ) {
	if ( err_members() )
		return __GIM_YES;
	return __GIM_NO;
}


_gim_flag   gim_getopt_obj::search( char opt ) {
	optlist->backup();
	optlist->rewind();
	for( ; optlist->get_item() != NULL ; optlist->next_item() ) {
		_gim_option * tmp = (_gim_option *)optlist->get_item();
		if ( tmp->option == opt ) {
			optlist->restore();
			return __GIM_YES;
		}
	}
	return __GIM_NO;
}



