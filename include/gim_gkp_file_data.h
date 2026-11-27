//
// gim_gkp_file_data.h
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

/*!	\file		gim_gkp_file_data.h
	\version	1.0-0
	\brief		Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/ 		

#ifndef _GIM_GKP_FILE_DATA_H
#define _GIM_GKP_FILE_DATA_H

	/*!	\struct gim_file_chunk_struct
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief	This is one small structure, in order to verify, that it precedes each file within a Gkp
	*/
	struct gim_file_chunk_struct {
		_gim_Uint8	compression;		/*!<	Tell if chunk is compressed and eventualy by wich kind of compression */
		_gim_Uint32	chunck_size;
		char		id[GIM_MD5_SIZE];
		char		chash[GIM_SHA1_SIZE];
		char		xhash[GIM_SHA1_SIZE];
	};



#endif /* _GIM_GKP_FILE_DATA_H */
