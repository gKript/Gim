//
// gim_time.h
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

/*!     \file       gim_time.h
		\version    1.0-0
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#ifndef _GIM_TIME_H_
#define _GIM_TIME_H_


	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include "gim_platform.h"
	#include <string.h>

	#include "gim_define.h"
	#include "gim_custom_type.h"


	/*!	\class		gim_time_obj
		\brief		Class useful to manage a timer with resolution up to microseconds.
		\author		asyntote
		\version	1.0-0
		\date		2003-2026
	*/
	class gim_time_obj {
		public:
			/*! \fn		void	start( void )
				\brief	Use this function in order to mark the moment of start
			*/
			void	start( void );

			/*! \fn		void	stop( void )
				\brief	Use this function in order to mark the moment of stop
			*/
			void	stop( void );

			/*! \fn		unsigned int	get( void )
				\brief	Use this function in order obtain the difference, absolute or relative,\n
						between the \a start and the \a get (relative) or\n
						between the \a start and the \a stop ( absolute).\n
						Calling \a get before \a stop a partial result is obtained and the count is not interrupted
				\return	the relative time in expressed in seconds
			*/
			unsigned int	get( void );

			/*! \fn		void	start_usec( void )
				\brief	Use this function in order to mark the moment of start
			*/
			void	start_usec( void );

			/*! \fn		char *	get_usec( void )
				\brief	NOT thread-safe: the result is a static buffer shared by all the objects. Use get_usec_buf().
				\return The time since start_usec(), as "%2ld.%06ld"
			*/
			char *	get_usec( void );

			/*! \fn		size_t	get_usec_buf( char * buf , size_t size )
				\brief	Thread-safe version of get_usec(): writes the time since start_usec() in buf.
				\param	buf		the output buffer (at least 32 characters are enough)
				\param	size	the size of buf
				\return	the number of characters written, 0 when buf is NULL or size is 0
			*/
			size_t	get_usec_buf( char * buf , size_t size );

			/*! \fn		char 	* process( void )
				\brief	NOT thread-safe: the result is a static buffer shared by all the objects. Use process_buf().
				\return The CPU time of the process, as "%3ld.%03ld"
			*/
			char 	* process( void );

			/*! \fn		size_t	process_buf( char * buf , size_t size )
				\brief	Thread-safe version of process(): writes the CPU time of the process in buf.
				\param	buf		the output buffer (at least 32 characters are enough)
				\param	size	the size of buf
				\return	the number of characters written, 0 when buf is NULL or size is 0
			*/
			size_t	process_buf( char * buf , size_t size );

			/*! \fn		char	* date( void )
				\brief	Returns the current local date, as "%A %d-%m-%Y %H:%M:%S".
				\return	a string allocated with strdup(): the caller must free() it. NULL is not checked.
			*/
			char	* date( void );

			/*! \fn		size_t	date_buf( char * buf , size_t size )
				\brief	Thread-safe version of date(): writes the current local date in buf.
				\param	buf		the output buffer (at least 64 characters)
				\param	size	the size of buf
				\return	the number of characters written, 0 when the buffer is too small
			*/
			size_t	date_buf( char * buf , size_t size );

			/*! \fn		int		process_sec( void )
				\brief	Seconds of the current wall-clock time (not the CPU time).
			*/
			int		process_sec( void );

			/*! \fn		int 	process_msec( void )
				\brief	Milliseconds inside the current second of the wall clock.
			*/
			int 	process_msec( void );

		private:
			void	tv_removal( void );

			time_t	t_start;
			time_t	t_stop;
			time_t	t_diff;
			struct timeval	tvstart;
			struct timeval	tvstop;
			struct timeval	tvdiff;
			struct timeval	now;
			struct rusage r_usage;

		public:
			/*!	the constructor
			*/
			gim_time_obj() {
				t_start = 0;
				t_stop  = 0;
				t_diff  = 0;
				tvstart.tv_sec = 0;
				tvstart.tv_usec = 0;
				tvstop.tv_sec = 0;
				tvstop.tv_usec = 0;
				tvdiff.tv_sec = 0;
				tvdiff.tv_usec = 0;
				now.tv_sec = 0;
				now.tv_usec = 0;
				memset( &r_usage , 0 , sizeof( r_usage ) );
			};
	};

	extern gim_time_obj		* gim_time;


	/*!	\page	gimdoc_time	Time
		\brief	Timers and dates: elapsed time in seconds, in microseconds, CPU time of the process, current date.

		Class: \ref gim_time_obj. Header: gim_time.h. Status: REVISED 2026-11-27, NOT COMPILED: see SYNC/TODO_GIM.md.

		\section	gimdoc_time_what	What it is for

		Three kinds of timing are needed by Gim: the seconds between two events (start() and stop(), or get()), the
		microseconds between two points of the same code (start_usec() and get_usec_buf()), and the CPU time used by the
		process (process_buf()). The class also gives the current local date (date_buf()) for logs and headers.

		\section	gimdoc_time_example	Example

		Measuring a piece of code in microseconds, from one thread:
		\code{.cpp}
		gim_time_obj t;
		char elapsed[32];
		t.start_usec();
		// ... the work ...
		t.get_usec_buf( elapsed , sizeof( elapsed ) );     // "  0.000123"
		printf( "done in %s s\n" , elapsed );
		\endcode

		Date of a log line:
		\code{.cpp}
		char when[64];
		gim_time_obj t;
		t.date_buf( when , sizeof( when ) );              // "Friday 27-11-2026 10:15:00"
		\endcode

		\section	gimdoc_time_methods	The methods, by purpose

		| Purpose | Methods |
		|---------|---------|
		| Seconds between two events | start(), stop(), get() |
		| Microseconds | start_usec(), get_usec_buf() (thread-safe), get_usec() (static buffer) |
		| CPU time of the process | process_buf() (thread-safe), process() (static buffer) |
		| Current date | date_buf() (thread-safe), date() (allocated with strdup) |
		| Wall clock | process_sec(), process_msec() |

		\section	gimdoc_time_threads	Threads

		One object is not shared between threads. Two objects in two threads are safe with the *_buf() methods. get_usec()
		and process() return a static buffer: do not use them from more than one thread. date() is safe, but it returns a
		heap string the caller must free.

		\section	gimdoc_time_warnings	Warnings and limits

		- The start() / stop() / get() methods count whole seconds (time()): they are not for microseconds.
		- date() returns a string allocated with strdup(): free() it. Some callers in src do not free it yet: see the TODO.
		- process_msec() and process_sec() read the wall clock, although the name suggests the CPU time. Check before use.
		- The methods on struct timeval and rusage depend on gim_platform.h on Windows: NOT verified on that platform yet.
		- The class has no lock: each object is used by one thread at a time.
	*/

#endif /* _GIM_TIME_H_ */
