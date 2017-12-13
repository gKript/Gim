/***************************************************************************
 *            gim_gkfp_header.cc
 *
 *  Sun Jun 24 17:06:57 2007
 *  Copyright  2007  AsYntote
 *  asyntote@gkript.org
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
