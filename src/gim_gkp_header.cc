//
// gim_gkp_header.cc
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

/*!     \file       gim_gkp_header.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include "../include/gim.h"

char *	gim_gkdp_obj::hread( _gim_map map ) {
	memcpy ( &header , map , sizeof( header ) );
	map += sizeof( header );
	memcpy ( &verify , map , sizeof( verify ) );
	map += sizeof( verify );
	return ( map );
}

_gim_flag	gim_gkdp_obj::hwrite( _gim_handler * out ) {
	_gim_time	time;
	char message[256];
	strcpy( header.date , time.date() ); 
	strcpy( verify.hash , gim_checksum->md5( (char *)&header , sizeof( header ) ) );
	sprintf( message , "Calculated header hash : %s" , verify.hash );
	gim_error->set( "gim_gkdp_obj::hwrite" , message );
	gim_file_manager->flush( out , (char *)&header , sizeof( header ) );
	gim_error->set( "gim_gkdp_obj::hwrite" , "Header flushed" );
	gim_file_manager->flush( out , (char *)&verify , sizeof( verify ) );
	gim_error->set( "gim_gkdp_obj::hwrite" , "Header hash flushed" );
	return __GIM_OK;
}
