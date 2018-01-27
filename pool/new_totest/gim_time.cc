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

#include	"../include/gim_time.h"

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

char *	gim_time_obj::get_usec( void ) {
	static char	res[32];
	gettimeofday( &tvstop , NULL );
	tv_removal();
	sprintf( res , "%2d.%06d" , (unsigned int)tvdiff.tv_sec , (unsigned int)tvdiff.tv_usec );
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

char *	gim_time_obj::process( void ) {
	static char res[32];
	getrusage( RUSAGE_SELF , &r_usage );
	sprintf( res , "%3d.%03d" , (unsigned int)r_usage.ru_utime.tv_sec , (unsigned int)( r_usage.ru_utime.tv_usec / 1000 ) );
	return res;
}

int		gim_time_obj::process_sec( void ) {
	gettimeofday( &now , NULL );
	return tvstop.tv_sec;
}

int		gim_time_obj::process_msec( void ) {
	gettimeofday( &now , NULL );
	return tvstop.tv_usec;
}

char	* gim_time_obj::date( void ) {
	time_t	tp;
	char		timestamp[64];
	time( &tp );
	strftime( timestamp , 64 , "%A %d-%m-%Y %H:%M:%S" , localtime ( &tp ) );
	return strdup( timestamp );
}
