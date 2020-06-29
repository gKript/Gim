//
// gim_string.h
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
	and
	Adriano Zannoni (Tatti)

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

/*!	\file		gim_string.h
	\version	2.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://gkript.org/gk_gpl_v2.php ]
*/ 		

#ifndef _GIM_STRING_OBJ_H_
#define _GIM_STRING_OBJ_H_

	#include "gim_base_header.h"

	/*!	\struct		gim_string_data
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is used by Gim to manage a string.
	*/
	struct gim_string_data {
		char		str[32768];
		char		str_bkup[32768];
		_gim_Uint16 str_len;
	};

	/*! \class 		gim_string_obj
		\brief		This class allows you to use the texts as objects.
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008
	*/
	class gim_string_obj {
		public:

			void		set			( const char * to_set );
			void		set			( char * to_set );
			void		clean		( void );
			char *		c_str		( void );
			_gim_Uint16 size		( void );
			_gim_Uint16 length		( void );
			_gim_Uint16 capacity	( void );
			_gim_flag	empty		( void );
			void		cat			( const char * to_cat );
			void		cat			( char * to_cat );
			_gim_flag	find		( char * to_find );
			char *		left_find	( char to_find );
			char *		left_find	( char * to_find );
			char *		left_find	( gim_string_obj to_find );
			char *		right_find	( char to_find );
			char *		right_find	( char * to_find );
			char *		right_find	( gim_string_obj to_find );
			_gim_flag	equal		( char * to_compare );
			_gim_flag	equal		( gim_string_obj to_compare );
			
				
		private:

			gim_string_data	* data;
			
		public:
			inline gim_string_obj() {
				gim_error->set( "gim_string_obj::gim_string_obj" , "String requested" );
				data = new gim_string_data;
				__GIM_CLEAR( data->str , 32768 , char );
				__GIM_CLEAR( data->str_bkup , 32768 , char );
				data->str_len = 0;
				gim_error->set( "gim_string_obj::gim_string_obj" , "Done" );
			};
		
			inline gim_string_obj( char * to_set ) {
				gim_error->set( "gim_string_obj::gim_string_obj" , "String requested with parameter" );
				data = new gim_string_data;
				__GIM_CLEAR( data->str , 32768 , char );
				__GIM_CLEAR( data->str_bkup , 32768 , char );
				strcpy( data->str , to_set );
				data->str_len = strlen( data->str );
				gim_error->set( "gim_string_obj::gim_string_obj" , "Done" );
			};

			inline ~gim_string_obj() {
				delete data;
			};
	};

	

#endif // _GIM_STRING_OBJ_H_

