//
//  gim_picgim.cc
//
//  Gim - Generic Information Manager
//  Version 2.4-1  -  Milestone 2.5
//  Creator - asyntote
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
// Gim is developed with Anjuta version 1.2.4  - 2.32.1.1
//


#include    "../include/gim_base_header.h"
#include    "../include/gim_picgim.h"


_gim_Uint8	gim_picgim_obj::crc8_ssp( const _gim_Uint8 * data , _gim_Uint32 length ) {
	_gim_Uint8 crc = 0x00;
	for ( _gim_Uint32 i = 0 ; i < length ; i++ ) {
		crc ^= data[ i ];
		for ( int bit = 0 ; bit < 8 ; bit++ ) {
			if ( crc & 0x01 )
				crc = ( _gim_Uint8 )( ( crc >> 1 ) ^ 0x8C );
			else
				crc = ( _gim_Uint8 )( crc >> 1 );
		}
	}
	return crc;
}

_gim_Uint32	gim_picgim_obj::crc32_gcp( const _gim_Uint8 * data , _gim_Uint32 length ) {
	_gim_Uint32 crc = 0xFFFFFFFF;
	for ( _gim_Uint32 i = 0 ; i < length ; i++ ) {
		crc ^= data[ i ];
		for ( int bit = 0 ; bit < 8 ; bit++ ) {
			if ( crc & 0x01 )
				crc = ( crc >> 1 ) ^ 0xEDB88320;
			else
				crc >>= 1;
		}
	}
	return crc ^ 0xFFFFFFFF;
}

