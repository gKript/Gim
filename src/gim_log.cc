//
// gim_log.cc
//
// Gim  -  Generic Information Manager static library
// Version 1.0-0
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
//
// Gim is developed with Anjuta version 1.2.4
//

/*!     \file       gim_log.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include	"../include/gim_log.h"
#include <mutex>
#include <set>

/* Pool of the thread numbers. A thread takes the smallest free number on its first log line and gives it back when it ends,
   so the next thread reuses it: T# is a slot (how many threads run at the same time), the thread id of the system is the unique key. */
struct gim_thread_slots_obj {
	std::mutex		lock;
	std::set< int >	free_numbers;
	int				next = 1;
	int take( void ) {
		std::lock_guard< std::mutex > guard( lock );
		if ( !free_numbers.empty() ) {
			int number = *free_numbers.begin();
			free_numbers.erase( free_numbers.begin() );
			return number;
		}
		return next++;
	}
	void give( int number ) {
		std::lock_guard< std::mutex > guard( lock );
		free_numbers.insert( number );
	}
};

static gim_thread_slots_obj & gim_thread_slots( void ) {
	static gim_thread_slots_obj slots;
	return slots;
}

/* one per thread: the destructor runs when the thread ends and returns its number to the pool */
struct gim_thread_number_obj {
	int number = 0;
	~gim_thread_number_obj() {
		if ( number > 0 )
			gim_thread_slots().give( number );
	}
};

/* thread column: blank on the main thread, T1 T2 ... on the others */
static const char * gim_thread_tag( void ) {
	static thread_local gim_thread_number_obj slot;
	static thread_local char tag[ 8 ];
	if ( gim_is_main_thread() )
		return "   ";
	if ( slot.number == 0 )
		slot.number = gim_thread_slots().take();
	sprintf( tag , "T%-2d" , slot.number );
	return tag;
}

gim_time_obj	debug_timer;

gim_log_obj		* gim_log;
/* header of the log file: what it is, which program and versions, when, and one column title for each
   field of the debug format, with the same width as the field, so the titles follow debug::debug_format */
static void gim_write_log_header( FILE * fd , const char * file_name , const char * fmt ) {
	char cols[1024] = "";
	char piece[128];
	size_t i;
	fprintf( fd , "#########################################################################################\n" );
	fprintf( fd , "# Gim log\n" );
	fprintf( fd , "# program  : %s   version: %s\n" , gim_application_name , gim_application_version );
	fprintf( fd , "# Gim      : %s\n" , gim_version_mini() );
	fprintf( fd , "# started  : %s   pid: %ld\n" , debug_timer.date() , (long) gim_getpid() );
	fprintf( fd , "# file     : %s\n" , file_name );
	fprintf( fd , "# format   : %s\n" , fmt );
	fprintf( fd , "#########################################################################################\n" );
	for ( i = 0 ; fmt[ i ] ; i++ ) {
		if ( fmt[ i ] == '$' ) {
			i++;
			switch ( fmt[ i ] ) {
				case 't' : sprintf( piece , "%11s" , "time(s)" ); break;
				case 'p' : sprintf( piece , "%5s" , "tid" ); break;
				case 'T' : sprintf( piece , "%-3s" , "thr" ); break;
				case 'k' : sprintf( piece , "%-4s" , "kind" ); break;
				case 'f' : sprintf( piece , "%-48s" , "function" ); break;
				case 'm' : sprintf( piece , "%-45s" , "message" ); break;
				case 'v' : sprintf( piece , "%3s" , "val" ); break;
				case '$' : sprintf( piece , "$" ); break;
				default  : piece[ 0 ] = '\0'; break;
			}
		}
		else if ( fmt[ i ] == '_' )
			sprintf( piece , " " );
		else
			sprintf( piece , "%c" , fmt[ i ] );
		strncat( cols , piece , sizeof( cols ) - strlen( cols ) - 1 );
	}
	fprintf( fd , "%s\n" , cols );
	fflush( fd );
}

void gim_log_obj::start	( void ) {
	char message[256];
	if ( ! initialized ) {
		gim_error->Set( GIM_ERROR_MESSAGE , "gim_log_obj::start" , "Start: %s" , debug_timer.date() );
		if ( __GIM_LOG_FORCE == __GIM_YES ) {
			log_fd = fopen ( __GIM_LOG_FILENAME , "wb" );
			if ( log_fd == NULL ) {
				gim_error->set( "gim_log_obj::start" , "I cannot open in write mode the debug file on disk" );
			}
			else {
				initialized = __GIM_YES;
				enabled = __GIM_YES;
				debug_timer.start_usec();
				strcpy( format , "|$t|$p|$k_$f_|_$m" );
				gim_write_log_header( log_fd , __GIM_LOG_FILENAME , format );
				gim_error->set( "gim_log_obj::start" , "Start before configuration file due __GIM_LOG_FORCE" );
				sprintf( message , "File log start on %s" , debug_timer.date() );
				gim_error->set( "gim_log_obj::start" , message );
				gim_error->set( "gim_log_obj::start" , "This log use a standard format [ |$t|$p|$k_$f_|_$m ] due __GIM_LOG_FORCE" );
			}
		}
		else if ( __GIM_LOG_FORCE == __GIM_NO ) {
			if ( ! enabled ) {
				if ( gim_conf->prsr_obj != NULL ) {
					IF_EXIST_KEY_IN_CONF( "debug" , "f_debug" ) {
						enabled	= gim_conf->GetKeyFLAG( "debug" , "f_debug" );
					}
					IF_EXIST_KEY_IN_CONF( "DEBUG" , "debug_format" ) {
						strcpy( format , gim_conf->GetKeySTR( "DEBUG" , "debug_format" ) );
					}
					else {
						gim_error->set( GIM_ERROR_CRITICAL , "gim_log_obj::addline_format" , "Format NOT present in configuration file" , __GIM_ERROR );
					}
				}
				else {
					gim_error->Set( GIM_ERROR_WARNING , "gim_log_obj::start" , "I cannot start the File debug in gim " );
					gim_error->Set( GIM_ERROR_WARNING , "gim_log_obj::start" , "    because the gim.conf isn't loaded yet" );
				}
			}
			if ( enabled ) {
				log_fd = fopen ( gim_conf->GetKeySTR( "debug" , "f_debug_file" ) , "wb" );
				if ( log_fd == NULL ) {
					gim_error->set( "gim_log_obj::start" , "I cannot open the file dabug in write mode" );
				}
				else {
					initialized = __GIM_YES;
					debug_timer.start_usec();
					gim_write_log_header( log_fd , gim_conf->GetKeySTR( "debug" , "f_debug_file" ) , format );
					gim_error->Set( GIM_ERROR_MESSAGE , "gim_log_obj::start" , "Start to use gim File Debug" );
					gim_error->Set( GIM_ERROR_MESSAGE , "gim_log_obj::start" , "File log start on %s" , debug_timer.date() );
					gim_error->Set( GIM_ERROR_MESSAGE , "gim_log_obj::start" , "%s" , gim_version() );
				}
			}
		}
	}
	else {
		gim_error->set( GIM_ERROR_WARNING , "gim_log_obj::start" , "The timer is already initialized" , __GIM_OK );
	}
}


void gim_log_obj::stop( void ) {
	if ( enabled ) {
		if ( initialized == __GIM_YES ) {
			if ( log_fd != NULL ) {
				if ( gim_error ) gim_error->set( "gim_log_obj::stop" , 4 );
				fflush ( log_fd );
				fclose ( log_fd );
				log_fd = NULL;
				initialized = __GIM_NO;
			}
			else {
				if ( gim_error ) gim_error->set( "gim_log_obj::stop" , 5 );
			}
		}
	}
}


void	gim_log_obj::addline( const char *t , const char *func , const char *mess , int val ) {
	if ( enabled ) {
		if ( initialized == __GIM_YES ) {
			if ( log_fd != NULL ) {
				time_t	tp;
				char	timestamp[256] , function[256];
				time( &tp );
				strftime( timestamp , 256 , "%A %x %H:%M:%S" , localtime ( &tp ) );
				sprintf( function , "%s()" , func );
				if ( Lexical.str_equal( strdup( t ) , strdup( GIM_ERROR_FATAL ) ) == __GIM_YES )
//					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , gim_gettid() , gim_thread_tag() , __GIM_LOG_FATAL , Lexical.string_trunc( function , 48 ) , mess );
					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , gim_gettid() , gim_thread_tag() , __GIM_LOG_FATAL , Lexical.string_trunc( function , 48 ) , mess );
				else 
					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , gim_gettid() , gim_thread_tag() , t ,  Lexical.string_trunc( function , 48 ) , mess );
				fflush( log_fd );
			}
			else {
				gim_error->set( "gim_log_obj::addline" , 7 );
			}
		}
	}
}


void	gim_log_obj::addline( pid_t cur_pid , const char *t , const char *func , const char *mess , int val ) {
	if ( enabled ) {
		if ( initialized == __GIM_YES ) {
			if ( log_fd != NULL ) {
				time_t	tp;
				char	timestamp[256] , function[256];
				time( &tp );
				strftime( timestamp , 256 , "%A %x %H:%M:%S" , localtime ( &tp ) );
				sprintf( function , "%s()" , func );
				if ( Lexical.str_equal( strdup( t ) , strdup( GIM_ERROR_FATAL ) ) == __GIM_YES )
					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , cur_pid , (cur_pid!=gim_getpid())?"TH":"  " , __GIM_LOG_FATAL , Lexical.string_trunc( function , 48 ) , mess );
				else 
					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , cur_pid , (cur_pid!=gim_getpid())?"TH":"  " , t , Lexical.string_trunc( function , 48 ) , mess );
				fflush( log_fd );
			}
			else {
				gim_error->set( "gim_log_obj::addline" , 7 );
			}
		}
	}
}


void	gim_log_obj::addline_format( const char *t , const char *func , const char *mess , int val ) {
	if ( enabled ) {
		if ( initialized == __GIM_YES ) {
			if ( log_fd == NULL )
				return;
			char tmp[1024];
			int c, len;
			strcpy( tmp , "" );
			for ( c = 0 , len = strlen( format ) ; c <= len ; c++ ) {
				switch ( format[c] ) {
					case '$' : {
						switch ( format[++c] ) {
							case 't' : {
								sprintf( tmp , "%s%11s" , tmp , debug_timer.get_usec() );
								break;
							}
							case 'p' : {
								sprintf( tmp , "%s%5d" , tmp , gim_gettid() );
								break;
							}
							case 'k' : {
								if ( Lexical.str_equal( strdup( t ) , strdup( GIM_ERROR_FATAL ) ) == __GIM_YES )
									sprintf( tmp , "%s%-4s" , tmp , __GIM_LOG_FATAL );
								else
									sprintf( tmp , "%s%-4s" , tmp , t );
								break;
							}
							case 'f' : {
								sprintf( tmp , "%s%s" , tmp , Lexical.string_trunc( (char *)func , 48 ) );
								break;
							}
							case 'm' : {
								sprintf( tmp , "%s%-45s" , tmp , mess );
								break;
							}
							case 'v' : {
								sprintf( tmp , "%s%3d" , tmp , val );
								break;
							}
							case 'T' : {
								sprintf( tmp , "%s%s" , tmp , gim_thread_tag() );
								break;
							}
							case '$' : {
								sprintf( tmp , "%s$" , tmp );
								break;
							}
							default : {
								gim_error->set( GIM_ERROR_WARNING , "gim_log_obj::addline_format" , "Unknow variable in DEBUG::debug_format value" , __GIM_NOT_OK );
								break;
							}
						}
					}
					case '_' : {
						sprintf( tmp , "%s " , tmp );
						break;
					}
					default : {
						sprintf( tmp , "%s%c" , tmp , format[c] );
						break;
					}
				}
			}
			fprintf( log_fd , "%s\n" , tmp );
			fflush( log_fd );
		}
	}
}


void	gim_log_obj::addline_format( pid_t cur_pid , const char *t , const char *func , const char *mess , int val ) {
	if ( enabled ) {
		if ( initialized == __GIM_YES ) {
			if ( log_fd == NULL )
				return;
			char tmp[1024];
			int c, len;
			strcpy( tmp , "" );
			for ( c = 0 , len = strlen( format ) ; c <= len ; c++ ) {
				switch ( format[c] ) {
					case '$' : {
						switch ( format[++c] ) {
							case 't' : {
								sprintf( tmp , "%s%11s" , tmp , debug_timer.get_usec() );
								break;
							}
							case 'p' : {
								sprintf( tmp , "%s%5d" , tmp , gim_gettid() );
								break;
							}
							case 'k' : {
								if ( Lexical.str_equal( strdup( t ) , strdup( GIM_ERROR_FATAL ) ) == __GIM_YES )
									sprintf( tmp , "%s%-4s" , tmp , __GIM_LOG_FATAL );
								else
									sprintf( tmp , "%s%-4s" , tmp , t );
								break;
							}
							case 'f' : {
								sprintf( tmp , "%s%s" , tmp , Lexical.string_trunc( (char *)func , 48 ) );
								break;
							}
							case 'm' : {
								sprintf( tmp , "%s%-45s" , tmp , mess );
								break;
							}
							case 'v' : {
								sprintf( tmp , "%s%3d" , tmp , val );
								break;
							}
							case 'T' : {
								sprintf( tmp , "%s%s" , tmp , (cur_pid!=gim_getpid())?"TH":"  "  );
								break;
							}
							case '$' : {
								sprintf( tmp , "%s$" , tmp );
								break;
							}
							default : {
								gim_error->set( GIM_ERROR_WARNING , "gim_log_obj::addline_format" , "Unknow variable in DEBUG::debug_format value" , __GIM_NOT_OK );
								break;
							}
						}
					}
					case '_' : {
						sprintf( tmp , "%s " , tmp );
						break;
					}
					default : {
						sprintf( tmp , "%s%c" , tmp , format[c] );
						break;
					}
				}
			}
			fprintf( log_fd , "%s\n" , tmp );
			fflush( log_fd );
		}
	}
}


_gim_log	* gim_log_obj::add_log_system	( const char * program_name , const char * log_file_name ) {
	return NULL;
}

_gim_log	* gim_log_obj::add_log_system	( const char * program_name , const char * log_file_name , const char * log_path_name ) {
	return NULL;
}
			
_gim_flag	gim_log_obj::is_open	( void ) {
	if ( log_fd == NULL )
		return __GIM_NO;
	else 
		return __GIM_YES;
}
