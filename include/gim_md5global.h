//
// gim_md5global.h
//
// Gim  -  Generic Information Manager static library
// Version 1.0-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//	[http://phorus.nibbles.it/trac/gkript/milestone/Gim%201.0?by=version]
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

/*!	\file		gim_md5global.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

/*
 * For license terms, see the file COPYRIGHT in the parent directory.
 *
 * md5global.h    Global declarations for MD5 module used by fetchmail
 *
 */

#ifndef _GIM_MD5GLOBAL_H_
#define _GIM_MD5GLOBAL_H_
	/* GLOBAL.H - RSAREF types and constants
	 */
	
	/* force prototypes on, we need ANSI C anyway */
	#ifndef PROTOTYPES
		#define PROTOTYPES 1
	#endif
	
	/* POINTER defines a generic pointer type */
	typedef unsigned char *POINTER;
	
	/* UINT2 defines a two byte word */
	typedef unsigned short int UINT2;
	
	/* UINT4 defines a four byte word */
	//#if SIZEOF_INT == 4
	typedef unsigned int UINT4;
	//#endif
	
	/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
	If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
	  returns an empty list.
	 */
	#if PROTOTYPES
		#define PROTO_LIST(list) list
	#else
		#define PROTO_LIST(list) ()
	#endif

#endif  // _GIM_MD5GLOBAL_H_
