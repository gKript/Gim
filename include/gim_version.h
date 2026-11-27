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

/*!	\file		gim_version.h
	\version	2.0-0
	\brief		Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/ 		

#ifndef _GIM_VERSION_H
#define _GIM_VERSION_H
	
	#include <stdio.h>
	#include <stdlib.h>

	#include "gim_custom_type.h"
	#include "gim_lexical.h"
	
	#define	__GIM_RELEASE	1
	#define	__GIM_DEVELOP	2
	
	//~ #ifdef	DEBUG_LEVEL_0
//	#define		GKMAKE_VERSIONING
	//~ #endif

//	#include	"gim_gkmake.h"

	/*!	\def	GIM_NAME
				Is the \a short name of this library
		\def	GIM_EXTENDED_NAME
				Is the \a long name of this library
		\def	GIM_SINCE
				The official first release on http://gkript.org/
		\def	__GIM_CLEAR( point , len , type )
				With this macro you can \a clear a buffer filling it with '0'
			\param	point	Is the pointer to a buffer you want to clean
			\param	len		Is the \a lenght of the buffer
			\param	type	Is the \a type of the buffer (e.g. char)
		
		\def	MTRAND_TEST_CYCLE
				Describe how many iterations will be necessry for the MersenneTwister test
	*/

	#define GIM_NAME				"Gim"
	#define GIM_EXTENDED_NAME		"Gim <Generic Information Manager> c++ static library made by gKript.org"
	#define GIM_LICENSE				"Released under the terms of the GNU General Public License v3"
	#define CNC_SINCE				"07/12/2003"
	#define GIM_SINCE				"10/03/2007"
	
	#define GIM_VERSION_STATE	   __GIM_DEVELOP

	#define GIM_VERSION				"2.9-2"		/*!<	The string version of Gim	*/
	#define	GIM_MILESTONE			"3.0"

	#define	GIM_MAJOR				2			/*!<	The major version of Gim	*/
	#define	GIM_MINOR				9			/*!<	The minor version of Gim	*/

	/*	Build counters (gim_gkmake.h) change at every gkmake run. They are read only by
		src/gim_version.cc (which defines GIM_VERSION_INTERNAL); every other file gets them
		through the functions below, so a new build recompiles one file instead of the whole library. */
	#if ( GIM_VERSION_STATE == __GIM_DEVELOP )				/*!<	When defined Gim include the bmake file generetad by bmake that is a built counter */
		#define	GKMAKE_VERSIONING
	#endif

	#ifdef GIM_VERSION_INTERNAL
		#if ( GIM_VERSION_STATE == __GIM_DEVELOP )
			#include	"gim_gkmake.h"
			#define GIM_SUBMINOR		GIM_GKMAKE_BUILD
			#define	GIM_BUILD_LAST		GIM_GKMAKE_LAST_BUILD
		#elif ( GIM_VERSION_STATE == __GIM_RELEASE )
			#define GIM_SUBMINOR		1
		#endif
	#else
		#define GIM_SUBMINOR		gim_version_subminor()		/*!<	Same value as before, now read at run time */
		#define	GIM_BUILD_LAST		gim_version_build_last()
	#endif

	/*!	\fn		int gim_version_subminor( void )
		\brief	The subminor version of Gim (the successful build number in develop state)
	*/
	int			gim_version_subminor	( void );

	/*!	\fn		const char * gim_version_build_last( void )
		\brief	Date of the last build written by gkmake (empty string in release state)
	*/
	const char *	gim_version_build_last	( void );



	/*!	\fn		char * gim_version( void )
		\brief	Use this function to print on \a stdout the Gim version with extended infos
	*/
	char *		gim_version			( void );
				 
	/*!	\fn		char * gim_version_small( void )
		\brief	Use this function to print on \a stdout the Gim version with basic infos
	*/
	char *		gim_version_small	( void );
	
	/*!	\fn		char * gim_version_small( void )
		\brief	Use this function to print on \a stdout the Gim version with basic infos - Same as small but without space in the string
	*/
	char *		gim_version_mini	( void );
	
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

 	/*!	\fn		int gim_check_version( int maj , int min , int sub )
		\brief	Use this function to check if Gim is at the required version
		\param	maj	The major version number of th required gim
		\param	min	The minor version number of th required gim
		\return \a __GIM_OK if Gim is at the required version, 
				\a __GIM_NOT_OK if not.
	*/
	int			gim_check_version	( int maj , int min );

#endif /* _GIM_VERSION_H */
