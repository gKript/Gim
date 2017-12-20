//
// gim_define
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

/*!	\file		gim_define.h
	\version	2.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_DEFINE_H
#define _GIM_DEFINE_H

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

	#define GIM_NAME			"Gim"
	#define GIM_EXTENDED_NAME	"Gim <Generic Information Manager> c++ static library made by gKript.org"
	#define GIM_LICENSE			"Released under the terms of the GNU General Public License v3"
	#define GIM_SINCE			"07/12/2003"

	#define __GIM_CLEAR( point , len , type)		memset( point , 0 , sizeof( type ) * len )
	
	#define	MTRAND_TEST_CYCLE	50000

#endif /* _GIM_DEFINE_H */
