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

#include	"../include/gim_log.h"

gim_log_obj		* gim_log;
gim_time_obj	debug_timer;

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
//					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , syscall(SYS_gettid) , (syscall(SYS_gettid) != getpid())?"TH":"  " , __GIM_LOG_FATAL , Lexical.string_trunc( function , 48 ) , mess );
					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , syscall(SYS_gettid) , (syscall(SYS_gettid) != getpid())?"TH":"  " , __GIM_LOG_FATAL , Lexical.string_trunc( function , 48 ) , mess );
				else 
					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , syscall(SYS_gettid) , (syscall(SYS_gettid) != getpid())?"TH":"  " , t ,  Lexical.string_trunc( function , 48 ) , mess );
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
					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , cur_pid , (cur_pid!=getpid())?"TH":"  " , __GIM_LOG_FATAL , Lexical.string_trunc( function , 48 ) , mess );
				else 
					fprintf( log_fd , "%s %5d %s %s %-45s : %s\n" , debug_timer.get_usec() , cur_pid , (cur_pid!=getpid())?"TH":"  " , t , Lexical.string_trunc( function , 48 ) , mess );
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
			if ( log_fd != NULL ) {
			}
			else {
				gim_error->set( "gim_log_obj::addline_format" , 7 );
			}
			char tmp[1024];
			int c, len;
			strcpy( tmp , "" );
			for ( c = 0 , len = strlen( format ) ; c <= len ; c++ ) {
				switch ( format[c] ) {
					case '$' : {
						switch ( format[++c] ) {
							case 't' : {
								sprintf( tmp , "%s%s" , tmp , debug_timer.get_usec() );
								break;
							}
							case 'p' : {
								sprintf( tmp , "%s%5d" , tmp , syscall(SYS_gettid) );
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
								sprintf( tmp , "%s%s" , tmp , (syscall(SYS_gettid) != getpid()) ? "TH" : "  " );
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
			if ( log_fd != NULL ) {
			}
			else {
				gim_error->set( "gim_log_obj::addline_format" , 7 );
			}
			char tmp[1024];
			int c, len;
			strcpy( tmp , "" );
			for ( c = 0 , len = strlen( format ) ; c <= len ; c++ ) {
				switch ( format[c] ) {
					case '$' : {
						switch ( format[++c] ) {
							case 't' : {
								sprintf( tmp , "%s%s" , tmp , debug_timer.get_usec() );
								break;
							}
							case 'p' : {
								sprintf( tmp , "%s%5d" , tmp , syscall(SYS_gettid) );
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
								sprintf( tmp , "%s%s" , tmp , (cur_pid!=getpid())?"TH":"  "  );
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
