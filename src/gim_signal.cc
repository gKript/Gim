//
// gim_signal.cc
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

#include "../include/gim_signal.h"

volatile sig_atomic_t fatal_error_in_progress = __GIM_NO;

void	gim_signal_received( int sig) {
	if ( fatal_error_in_progress == __GIM_YES )
		raise( sig );
	fatal_error_in_progress = __GIM_YES;
	gim_error->set( GIM_ERROR_CRITICAL , "::gim_signal_received" , "Signal received" , sig );
	char fault[512] , buf[512];
	gim_error->set( GIM_ON_FATAL_EXIT , __GIM_YES );
	strcpy( fault , strsignal( sig ) );
	snprintf( buf , sizeof( buf ), "Gim : [ %s ]", fault );
	signal(SIGINT , SIG_DFL );
	signal(SIGQUIT, SIG_DFL );
	signal(SIGKILL, SIG_DFL );
	signal(SIGABRT, SIG_DFL );
	signal(SIGTERM, SIG_DFL );
	signal(SIGSEGV, SIG_DFL );
	gim_error->set( GIM_ERROR_FATAL , "::gim_signal_received" , buf , sig );
}

void	gim_signal_handler( void ) {
	signal(SIGINT , gim_signal_received );
	signal(SIGQUIT, gim_signal_received );
	signal(SIGKILL, gim_signal_received );
	signal(SIGABRT, gim_signal_received );
	signal(SIGTERM, gim_signal_received );
	signal(SIGSEGV, gim_signal_received );
	gim_error->set( "::gim_signal_handler" , "Signal are now managed by Gim" );
}

void	gim_forever( void ) {
	gim_error->set( GIM_ALWAYSE_VERBOSE , __GIM_YES );
	gim_error->set( "::gim_forever" , "Type control-c to exit" );
	gim_error->set( GIM_ALWAYSE_VERBOSE , __GIM_NO );
	while( 1 );
}

void	gim_fatal_test( void ) {
	gim_error->set( GIM_ERROR_FATAL , "::gim_fatal_test" , "This is a test for a FATAL occurrence" , __GIM_ERROR );
}
