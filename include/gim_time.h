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

/*!	\file		gim_time.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_TIME_H_
#define _GIM_TIME_H_


	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <sys/time.h>
	#include <sys/resource.h>
	#include <string.h>

	#include "gim_define.h"
	#include "gim_custom_type.h"


	/*!	\class		gim_time_obj
		\brief		Class useful to manage a timer with resoluti up to Usec.
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	1.0-0
		\date		2003-2007
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
				\brief	Use this function in order obtain the difference from start\n
				\return The instant time in string format
			*/
			char *	get_usec( void );
				
			/*! \fn		char 	* process( void )
				\brief	Use this function in order obtain the process time\n
				\return The process time
			*/
			char 	* process( void );
			
			char	* date( void );
			
			int		process_sec( void );
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
			};
	};

	extern gim_time_obj		* gim_time;

#endif /* _GIM_TIME_H_ */
