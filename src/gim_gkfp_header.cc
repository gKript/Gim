/***************************************************************************
 *            gim_gkfp_header.cc
 *
 *  Sun Jun 24 17:06:57 2007
 *  Copyright (C) 2007-2026 AsYntote
 *  asyntote@gkript.org
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
 
#include "../include/gim.h"

char *	gim_gkfp_obj::hread( _gim_map map ) {
	memcpy ( &header , map , sizeof( header ) );
	map += sizeof( header );
	memcpy ( &verify , map , sizeof( verify ) );
	map += sizeof( verify );
	return ( map );
}

_gim_flag	gim_gkfp_obj::hwrite( _gim_handler * out ) {
	_gim_time	time;
	strcpy( header.date , time.date() ); 
	strcpy( verify.hash , gim_checksum->md5( (char *)&header , sizeof( header ) ) );
	gim_file_manager->flush( out , (char *)&header , sizeof( header ) );
	gim_file_manager->flush( out , (char *)&verify , sizeof( verify ) );
	return __GIM_OK;
}
