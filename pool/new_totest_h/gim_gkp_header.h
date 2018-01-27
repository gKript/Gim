//
// gim_gkp_header.h
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

/*!	\file		gim_gkp_header.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_GKP_HEADE_H
#define _GIM_GKP_HEADE_H

	#include "gim_base_header.h"

	#define	__GKP_TITLE_DATA	"GKDP"
	#define	__GKP_TITLE_FILE	"GKFP"

	/*! \struct	gim_file_header_struct
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief	This is the header struct of a Gkp file ( gkdp and gkfp ).
	*/
	struct gim_file_header_struct {
		char		title[5];		/*!<	These are the first 5 chars of a Gkp file and they define the type of gkp */
		_gim_Uint8	gim_major;		/*!<	The major version number of the Gim used by the program generator	*/
		_gim_Uint8	gim_minor;		/*!<	The minor version number of the Gim used by the program generator	*/
		_gim_Uint16	gim_subminor;	/*!<	The subminor version number of the Gim used by the program generator	*/
		char		program[32];	/*!<	The string that identifies the program that has generated the gkp	*/
		_gim_Uint8	major;			/*!<	The major version number of the program generator	*/
		_gim_Uint8	minor;			/*!<	The minor version number of the program generator	*/
		_gim_Uint16	built;			/*!<	The built version number of the program generator	*/
		char		date[64];		/*!<	The creation timestamp	*/
		char 		user[256];		/*!<	The user name that create the Gkp	*/
		char		hostname[256];	/*!<	The hostname of the creator	*/
		char 		to[256];		/*!<	\todo	in Gim 1.5 	*/
		char 		to_node[256];	/*!<	\todo	in Gim 1.5	*/
		_gim_Uint32	members;		/*!<	The number of files stored into the Gkp file	*/
		_gim_flag	compression;	/*!<	A flag to indicate if the Gkp is compressed or not	*/
		_gim_flag	recursive;		/*!<	A flag to indicate if the file list have a recoursive file system	*/
		_gim_flag	crypt;			/*!<	A flag to indicate if the Gkp is crypted or not	*/
		_gim_flag	hashed_file;	/*!<	A flag to indicate if the Gkp is with file hashed or not 	*/
		_gim_flag	xhk;			/*!<	\todo	in Gim 1.5	*/
	};
	
	/*!	\struct	gim_file_header_verify
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief	This struct is useful to verify if the header is intact or damaged. Gim test the header each time is open.
	*/
	struct gim_file_header_verify {
		char		hash[GIM_MD5_SIZE];		/*!<	The digest of the header	*/
	};


	
#endif /* _GIM_GKP_HEADE_H */
