//
// gim_gkp_file_list.h
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

/*!	\file		gim_gkp_file_list.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_GKP_FILE_LIST_H
#define _GIM_GKP_FILE_LIST_H

	#include "gim_checksum.h"

	#define __GIM_ROOT		0
	#define	__GIM_MASTER	0
	#define	__GIM_SLAVE		1

	/*!
		\enum	file_type
		\brief	The list of file type you can find on a filesystem. \n
	*/	
	enum file_type {
		UNKNOWN = -1,	/*!<	This is an unknown file */
		REGFILE = 1,	/*!<	This is a regular file */
		EMPTY,			/*!<	This is an empty regular file */
		REGNODE,		/*!<	This is a directory */
		LINK,			/*!<	This is a link */
		SOCK,			/*!<	This is a socket */
		CHR,			/*!<	This is a character */
		FIFO,			/*!<	This is a fifo */
		BLK,			/*!<	This is a block */
		ROOT			/*!<	This is the root of our internal tree */
	};
	
	enum get_mode {
		__GIM_POINTER = 1,
		__GIM_BUFFER,
	};

	/*!	\struct gim_file_list_struct
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief	This is a sigle member of the file list.
	*/
	struct gim_file_list_struct {
		char		name[256];				/*!<	The file name */
		char		path[1024];				/*!<	The file path */
		_gim_Uint32	size;					/*!<	The chunk size */
		char		hash[GIM_SHA1_SIZE];	/*!<	The original chunk hash 	*/
		_gim_Uint8	type;					/*!<	The type of the file	*/
		int			id;						/*!<	The file id. An incremental number  */

		struct stat	stat;					/*!<	The stat of the file	*/
		
		_gim_buffer	buffer;					/*!<	The buffer contains the file data before flush in write. The buffer contains file data when you ::take a file in read	*/
		_gim_map	handle;					/*!<	The ::_gim_map will be the index of a file in file mapping  */


		gim_file_list_struct * link;		/*!<	To find the next member in memory or \a NULL if is reached the end of the file list  */
	};
	

	
#endif /* _GIM_GKP_FILE_LIST_H */
