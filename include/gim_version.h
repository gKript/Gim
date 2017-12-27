//
// gim_version.h
//
// Gim  -  Generic Information Manager static library
// Version 2.0-0
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

/*!	\file		gim_version.h
	\version	2.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_VERSION_H
#define _GIM_VERSION_H
	
	#include <stdio.h>
	#include <stdlib.h>

	#include "gim_custom_type.h"
	#include "gim_lexical.h"
	
	//~ #ifdef	DEBUG_LEVEL_0
	//~	#define		GKMAKE_VERSIONING
	//~ #endif
	
	//#include	"gim_bmake.h"
	
	#define GIM_VERSION_STATE   "dev"  // "rel" for release or "dev" for devel

	#define GIM_VERSION			"2.7-0"	/*!<	The string version of Gim	*/
	#define	GIM_MILESTONE		"3.0"

	#define	GIM_MAJOR		2			/*!<	The major version of Gim	*/
	#define	GIM_MINOR		6			/*!<	The minor version of Gim	*/

	#ifdef		GKMAKE_VERSIONING		/*!<	When defined Gim include the bmake file generetad by bmake that is a built counter */
		#define GIM_SUBMINOR		GIM_GKMAKE_BUILD
		#define	GIM_BUILD_LAST		GIM_GKMAKE_LAST_BUILD
	#else
		#define GIM_SUBMINOR		0	
	#endif
	


	/*!	\fn		char * gim_version( void )
		\brief	Use this function to print on \a stdout the Gim version with extended infos
	*/
	char *		gim_version			( void );
				 
	/*!	\fn		char * gim_version_small( void )
		\brief	Use this function to print on \a stdout the Gim version with basic infos
	*/
	char *		gim_version_small	( void );
	
	/*!	\fn		char * gim_version_micro( void )
		\brief	Use this function to print on \a stdout the Gim version with no infos
	*/
	char *		gim_version_micro	( void );

 	/*!	\fn		int gim_check_version( int maj , int min , int sub )
		\brief	Use this function to check if Gim is at the required version
		\param	maj	The major version number of th required gim
		\param	min	The minor version number of th required gim
		\param	sub	The subminor version number of th required gim
		\return \a __GIM_OK if Gim is at the required version, 
				\a __GIM_NOT_OK if not.
	*/
	int			gim_check_version	( int maj , int min , int sub );

#endif /* _GIM_VERSION_H */
