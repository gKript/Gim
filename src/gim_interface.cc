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

	Copyright (C) 2002, 2007 
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

#include "../include/gim_interface.h"

gim_interface_obj	* gim_interface;

gim_interface_obj::gim_interface_obj() {
	data	= new gim_interface_data;
	backup	= new gim_interface_data;
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
	FILE *stat;
	stat = fopen( "/proc/stat" , "rb" );
	if ( stat ) {
		
	}
	
}

