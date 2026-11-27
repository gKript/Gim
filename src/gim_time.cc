//
// gim_time.cc
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

/*!     \file       gim_time.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include	"../include/gim_time.h"

/* REVISED 2026-11-27, NOT COMPILED: the new *_buf() functions and the fixes of process_sec() / process_msec() / date()
   have to be built and tested on another PC (see test/gim_time_test.cc and SYNC/TODO_GIM.md). */

gim_time_obj	* gim_time;

void	gim_time_obj::start( void ) {
	t_start = time( NULL );
	t_stop	= 0;
}

void	gim_time_obj::stop( void ) {
	if ( t_start ) {
		t_stop = time( NULL );
		get();
	}
	else {
	}
}

unsigned int	gim_time_obj::get( void ) {
	_gim_flag stop;
	if ( t_stop == 0 ) {
		this->stop();
		stop= __GIM_NO;
	}
	else
		stop = __GIM_YES;
	t_diff = t_stop - t_start;
	if ( stop == __GIM_NO )
		t_stop = 0;
	return ( (unsigned int) t_diff );
}

void	gim_time_obj::start_usec( void ) {
	gettimeofday( &tvstart , NULL );
}

/* Writes the elapsed time since start_usec() in buf, format "%2ld.%06ld" (seconds.microseconds).
   Thread-safe: it works only on the members of this object, never on a shared buffer.
   Returns the number of characters written (as snprintf), 0 when size is 0. */
size_t	gim_time_obj::get_usec_buf( char * buf , size_t size ) {
	if ( buf == NULL || size == 0 )
		return 0;
	gettimeofday( &tvstop , NULL );
	tv_removal();
	return (size_t) snprintf( buf , size , "%2ld.%06ld" , (long) tvdiff.tv_sec , (long) tvdiff.tv_usec );
}

/* NOT thread-safe: the result is a static buffer shared by all the objects and all the threads. Use get_usec_buf(). */
char *	gim_time_obj::get_usec( void ) {
	static char	res[32];
	get_usec_buf( res , sizeof( res ) );
	return res;
}

void	gim_time_obj::tv_removal( void ) {
	tvdiff.tv_sec = tvstop.tv_sec - tvstart.tv_sec;
	tvdiff.tv_usec = tvstop.tv_usec - tvstart.tv_usec;
	if (tvdiff.tv_sec < 0 && tvdiff.tv_usec > 0)
		tvdiff.tv_sec++, tvdiff.tv_usec -= 1000000;
	if (tvdiff.tv_sec > 0 && tvdiff.tv_usec < 0)
		tvdiff.tv_sec--, tvdiff.tv_usec += 1000000;
}

/* Writes the CPU time of the process in buf, format "%3ld.%03ld" (seconds.milliseconds).
   Thread-safe: it uses only the member r_usage of this object. */
size_t	gim_time_obj::process_buf( char * buf , size_t size ) {
	if ( buf == NULL || size == 0 )
		return 0;
	getrusage( RUSAGE_SELF , &r_usage );
	return (size_t) snprintf( buf , size , "%3ld.%03ld" , (long) r_usage.ru_utime.tv_sec , (long) ( r_usage.ru_utime.tv_usec / 1000 ) );
}

/* NOT thread-safe: static buffer shared by all the objects and all the threads. Use process_buf(). */
char *	gim_time_obj::process( void ) {
	static char res[32];
	process_buf( res , sizeof( res ) );
	return res;
}

/* Seconds of the current time (the wall clock, not the CPU time). Fixed: it used to return the stale value of tvstop. */
int		gim_time_obj::process_sec( void ) {
	gettimeofday( &now , NULL );
	return (int) now.tv_sec;
}

/* Milliseconds inside the current second of the wall clock. Fixed: it used to return microseconds of the stale tvstop.
   NOTE: the name suggests the CPU time, but the value is the wall clock: check the callers before using it. */
int		gim_time_obj::process_msec( void ) {
	gettimeofday( &now , NULL );
	return (int) ( now.tv_usec / 1000 );
}

/* Writes the current local date in buf, format "%A %d-%m-%Y %H:%M:%S". Thread-safe (localtime_r / localtime_s).
   Returns the number of characters written, 0 when the buffer is too small. */
size_t	gim_time_obj::date_buf( char * buf , size_t size ) {
	time_t		tp;
	struct tm	tm_now;
	if ( buf == NULL || size == 0 )
		return 0;
	time( &tp );
#ifdef _WIN32
	localtime_s( &tm_now , &tp );
#else
	localtime_r( &tp , &tm_now );
#endif
	return strftime( buf , size , "%A %d-%m-%Y %H:%M:%S" , &tm_now );
}

/* Returns the current local date in a string allocated with strdup(): the CALLER must free() it.
   NOTE: callers in src (gim_log, gim_gkp_header, gim_gkfp_header) do not free it yet: see SYNC/TODO_GIM.md. */
char	* gim_time_obj::date( void ) {
	char	timestamp[64];
	if ( date_buf( timestamp , sizeof( timestamp ) ) == 0 )
		timestamp[0] = '\0';
	return strdup( timestamp );
}
