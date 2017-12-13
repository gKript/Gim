//
// gim_gkp_utils.h
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

/*!	\file		gim_gkp_utils.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GKP_UTILS_H
#define _GKP_UTILS_H

	/*! \class 		gim_utils_obj
		\warning	\n <b>It is not possible to use directly this class.</b>
		\internal
		\brief		It is a clas with some useful methods for Gim
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	1.0-0
		\date		2003-2007
	*/
	class gim_utils_obj {
		public:
			char *		pwd( void );
			char *		del_path( char * str );
			char *		keep_path( char * str );
			char *		str_subtraction( char * main_str , char * to_subtract );
			const char	* dir_name_check( const char * dname );
			_gim_flag 	isdir( const char * );
			_gim_flag 	isregular( const char * );
			_gim_flag 	islink( const char * );
			_gim_flag 	isdevice( const char * );
	};

#endif /* _GKP_UTILS_H */
