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
#include "../include/gim_kvalue.h"
#include <exception>
#include <string>

/* Classes that use or plan a thread. Add a name here when a new class gets a thread. */
static const char * const gim_thread_known_classes[] = { "gim_interface" , "gim_serial" , "gim_stat" };
#define GIM_THREAD_KNOWN_CLASSES_NUMBER	( sizeof( gim_thread_known_classes ) / sizeof( gim_thread_known_classes[ 0 ] ) )

_gim_flag	gim_thread_allowed( const char * class_name ) {
	_gim_flag	allowed;

	if ( gim_conf == nullptr ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_thread_allowed" , "configuration not loaded: thread permission defaults to YES" , __GIM_OK );
		return __GIM_YES;
	}
	/* read only: a missing key means the default YES; gim_thread_add_missing_config writes it at startup */
	if ( ( gim_conf->ExistSection( "thread" ) == __GIM_NOT_EXIST ) || ( gim_conf->ExistKey( "thread" , class_name ) == __GIM_NOT_EXIST ) )
		return __GIM_YES;
	allowed = gim_conf->GetKeyFLAG( "thread" , class_name );
	if ( allowed == __GIM_YES ) {
		gim_error->set( GIM_ERROR_MESSAGE , "gim_thread_allowed" , ( std::string( class_name ) + ": thread permitted by configuration" ).c_str() , __GIM_OK );
		return __GIM_YES;
	}
	gim_error->set( GIM_ERROR_MESSAGE , "gim_thread_allowed" , ( std::string( class_name ) + ": thread disabled by configuration ([thread] key = NO)" ).c_str() , __GIM_OK );
	return __GIM_NO;
}

void		gim_thread_report_config( void ) {
	for ( size_t i = 0 ; i < GIM_THREAD_KNOWN_CLASSES_NUMBER ; i++ )
		gim_thread_allowed( gim_thread_known_classes[ i ] );
}

_gim_flag	gim_thread_obj::start_if_allowed( const char * class_name , std::function< void( gim_thread_obj & ) > body ) {
	if ( gim_thread_allowed( class_name ) != __GIM_YES ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_thread_obj::start_if_allowed" , ( std::string( class_name ) + ": start ignored, thread disabled by configuration" ).c_str() , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	return start( body );
}

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

_gim_flag	gim_thread_add_missing_config( void ) {
	_gim_flag	added = __GIM_NO;

	if ( gim_conf == nullptr )
		return __GIM_NO;
	if ( gim_conf->ExistSection( "thread" ) == __GIM_NOT_EXIST ) {
		gim_conf->AddSection( "thread" );
		added = __GIM_YES;
	}
	for ( size_t i = 0 ; i < GIM_THREAD_KNOWN_CLASSES_NUMBER ; i++ ) {
		if ( gim_conf->ExistKey( "thread" , gim_thread_known_classes[ i ] ) == __GIM_NOT_EXIST ) {
			gim_conf->AddKeyFlag( "thread" , gim_thread_known_classes[ i ] , __GIM_YES );
			gim_error->set( GIM_ERROR_MESSAGE , "gim_thread_add_missing_config" , ( std::string( gim_thread_known_classes[ i ] ) + ": key added to [thread] with default YES" ).c_str() , __GIM_OK );
			added = __GIM_YES;
		}
	}
	return added;
}