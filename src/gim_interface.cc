//
// gim_interface.h
//
// Gim  -  Generic Information Manager static library
// Version 2.0-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.0?by=version]
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

/*!     \file       gim_interface.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include "../include/gim_interface.h"
#include "../include/gim_error.h"
#include "../include/gim_memory.h"
#include <mutex>

gim_interface_obj	* gim_interface;

#ifdef _WIN32
	#include <windows.h>
#endif

/* Reads the cumulative CPU counters of the whole system: idle time and total time.
   Returns 1 on success, 0 if the counters cannot be read. */
static int gim_read_cpu_counters( double * idle , double * total ) {
#ifdef _WIN32
	FILETIME		idle_ft , kern_ft , user_ft;
	ULARGE_INTEGER	i , k , u;
	if ( !GetSystemTimes( &idle_ft , &kern_ft , &user_ft ) )
		return 0;
	i.LowPart = idle_ft.dwLowDateTime;	i.HighPart = idle_ft.dwHighDateTime;
	k.LowPart = kern_ft.dwLowDateTime;	k.HighPart = kern_ft.dwHighDateTime;
	u.LowPart = user_ft.dwLowDateTime;	u.HighPart = user_ft.dwHighDateTime;
	*idle  = (double) i.QuadPart;
	*total = (double) ( k.QuadPart + u.QuadPart );	/* kernel time already includes idle time */
	return 1;
#else
	FILE * f = fopen( "/proc/stat" , "rb" );
	unsigned long long user = 0 , nice = 0 , system = 0 , idle_v = 0 , iowait = 0 , irq = 0 , softirq = 0 , steal = 0;
	int n;
	if ( f == NULL )
		return 0;
	n = fscanf( f , "cpu %llu %llu %llu %llu %llu %llu %llu %llu" , &user , &nice , &system , &idle_v , &iowait , &irq , &softirq , &steal );
	fclose( f );
	if ( n < 4 )
		return 0;
	*idle  = (double) ( idle_v + iowait );
	*total = (double) ( user + nice + system + idle_v + iowait + irq + softirq + steal );
	return 1;
#endif
}

gim_interface_obj::gim_interface_obj() {
	data	= new gim_interface_data;
	backup	= new gim_interface_data;
	memset( data , 0 , sizeof( gim_interface_data ) );
	memset( backup , 0 , sizeof( gim_interface_data ) );
	if ( data->old_cpu_total == 0 ) {
		
	}
}


gim_interface_obj::~gim_interface_obj() {
	delete backup;
	delete data;
}


_gim_flag	gim_interface_obj::check( void ) {
		if ( memcmp( data , backup , sizeof( gim_interface_data ) ) == 0 )
			return __GIM_NO;
	return __GIM_YES;
}


_gim_flag	gim_interface_obj::check( gim_interface_data * to_check ) {
	if ( to_check != NULL ) {
		if ( memcmp( data , to_check , sizeof( gim_interface_data ) ) == 0 )
			return __GIM_NO;
	}
	return __GIM_YES;
}


gim_interface_data	* gim_interface_obj::get( void ) {
	align();
	return data;
}


void		gim_interface_obj::align( void ) {
	memcpy( backup , data , sizeof( gim_interface_data ) );
}


void		gim_interface_obj::clear( void ) {
	memset( data	, 0 , sizeof( gim_interface_data ) );
	memset( backup	, 0 , sizeof( gim_interface_data ) );
}


void		gim_interface_obj::clear( gim_interface_data * to_clear ) {
	memset( to_clear , 0 , sizeof( gim_interface_data ) );
}


void		gim_interface_obj::update_load_avg	( void ) {
	double lavg[3];
#if ( GET_LOAD_AVG == true ) 
	getloadavg( lavg , 3 );
	data->load_avg_om  = lavg[0];
	data->load_avg_fm  = lavg[1];
	data->load_avg_ftm = lavg[2];
#else
	data->load_avg_om  = 0;
	data->load_avg_fm  = 0;
	data->load_avg_ftm = 0;
#endif	
}


double		gim_interface_obj::get_lavg_om	( void ) {
	return data->load_avg_om;
}


double		gim_interface_obj::get_lavg_fm	( void ) {
	return data->load_avg_fm;
}

double		gim_interface_obj::get_lavg_ftm	( void ) {
	return data->load_avg_ftm;
}

void		gim_interface_obj::init_perc_cpu_load( void ) {
	double idle , total;
	std::lock_guard<std::recursive_mutex> gim_guard_( gim_global_lock() );
	if ( gim_read_cpu_counters( &idle , &total ) ) {
		data->cpu_idle = data->old_cpu_idle = idle;
		data->cpu_total = data->old_cpu_total = total;
	}
}


void		gim_interface_obj::sample( void ) {
	std::lock_guard<std::recursive_mutex> gim_guard_( gim_global_lock() );
	update_load_avg();
	double idle , total;
	if ( gim_read_cpu_counters( &idle , &total ) ) {
		data->old_cpu_idle	= data->cpu_idle;
		data->old_cpu_total	= data->cpu_total;
		data->cpu_idle		= idle;
		data->cpu_total		= total;
		double d_total = data->cpu_total - data->old_cpu_total;
		double d_idle  = data->cpu_idle - data->old_cpu_idle;
		if ( data->old_cpu_total > 0 && d_total > 0 ) {
			double busy = 100.0 * ( 1.0 - d_idle / d_total );
			if ( busy < 0.0 )	busy = 0.0;
			if ( busy > 100.0 )	busy = 100.0;
			data->cpu_busy_perc = (float) busy;
		}
	}
	if ( gim_memory != NULL ) {
		data->mem_allocated			= gim_memory->Allocated();
		data->mem_allocated_peek	= gim_memory->Allocated_peek();
		data->mem_max_allocable		= (_gim_Uint32) gim_memory->Max_allocable();
	}
}

void		gim_interface_obj::snapshot( gim_interface_data * out ) {
	if ( out == NULL )
		return;
	std::lock_guard<std::recursive_mutex> gim_guard_( gim_global_lock() );
	memcpy( out , data , sizeof( gim_interface_data ) );
}
