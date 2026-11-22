//
//  gim_thread.cc
//
//  Gim  -  Generic Information Manager
//  See gim_thread.h.
//
/*#############################################################################

	This file is part of Gim library.

	Gim is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Gim is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

#############################################################################*/

#include "../include/gim_thread.h"
#include "../include/gim_error.h"
#include <exception>

gim_thread_obj::gim_thread_obj() : th( nullptr ) , stop_flag( false ) , running( false ) {
}

gim_thread_obj::~gim_thread_obj() {
	request_stop();
	join();
}

_gim_flag	gim_thread_obj::start( std::function< void( gim_thread_obj & ) > body ) {
	if ( running.load() )
		return __GIM_NOT_OK;
	join();
	stop_flag.store( false );
	running.store( true );
	th = new std::thread( [ this , body ]() {
		try {
			body( *this );
		}
		catch ( const std::exception & e ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_thread_obj::start" , e.what() , __GIM_NOT_OK );
		}
		catch ( ... ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_thread_obj::start" , "unknown exception in the thread" , __GIM_NOT_OK );
		}
		running.store( false );
	} );
	return __GIM_OK;
}

void		gim_thread_obj::request_stop( void ) {
	stop_flag.store( true );
}

bool		gim_thread_obj::stop_requested( void ) const {
	return stop_flag.load();
}

_gim_flag	gim_thread_obj::is_running( void ) const {
	return running.load() ? __GIM_YES : __GIM_NO;
}

void		gim_thread_obj::join( void ) {
	if ( th != nullptr ) {
		if ( th->joinable() )
			th->join();
		delete th;
		th = nullptr;
	}
}
