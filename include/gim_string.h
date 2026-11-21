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
	\brief		Text objects of Gim (gim_string_obj).
				Copyright (C) 2002, 2003<BR>
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
		char		str[32768];		/*!< the text */
		char		str_bkup[32768];	/*!< backup of the text */
		_gim_Uint16 str_len;		/*!< length of the text */
	};

	/*! \class 		gim_string_obj
		\brief		This class allows you to use the texts as objects.
		\todo		left_find(), right_find() and equal() are not implemented yet: they always return
					NULL or __GIM_NOT_OK.
		\todo		clean() clears str_bkup twice and does not clear the text itself.
		\todo		cat() appends only when the text is not empty; size() and length() return str_len,
					which is not updated by set() and cat().
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008
	*/
	class gim_string_obj {
		public:
			/*! Sets the text.
				\param	to_set	the new text
			*/
			void		set			( const char * to_set );
			/*! Sets the text.
				\param	to_set	the new text
			*/
			void		set			( char * to_set );
			/*! Empties the text (see the todo of the class). */
			void		clean		( void );
			/*! Returns the text as a C string.
				\return	pointer to the text
			*/
			char *		c_str		( void );
			/*! Returns the length of the text.
				\return	the length
			*/
			_gim_Uint16 size		( void );
			/*! Returns the length of the text (same as size()).
				\return	the length
			*/
			_gim_Uint16 length		( void );
			/*! Returns the maximum length of the text.
				\return	32768
			*/
			_gim_Uint16 capacity	( void );
			/*! Tells whether the text is empty.
				\return	__GIM_YES if empty, __GIM_NO otherwise
			*/
			_gim_flag	empty		( void );
			/*! Appends a text (only when the text is not empty).
				\param	to_cat	the text to append
			*/
			void		cat			( const char * to_cat );
			/*! Appends a text (only when the text is not empty).
				\param	to_cat	the text to append
			*/
			void		cat			( char * to_cat );
			/*! Tells whether a text is inside this one.
				\param	to_find	the text to look for
				\return	__GIM_YES when found, __GIM_NO otherwise
			*/
			_gim_flag	find		( char * to_find );
			/*! Tells whether a text is inside this one.
				\param	to_find	the text to look for
				\return	__GIM_YES when found, __GIM_NO otherwise
			*/
			_gim_flag	find		( const char * to_find );
			/*! Searches a character from the left (not implemented: returns NULL).
				\param	to_find	the character
				\return	NULL
			*/
			char *		left_find	( char to_find );
			/*! Searches a text from the left (not implemented: returns NULL).
				\param	to_find	the text
				\return	NULL
			*/
			char *		left_find	( char * to_find );
			/*! Searches a text from the left (not implemented: returns NULL).
				\param	to_find	the text
				\return	NULL
			*/
			char *		left_find	( gim_string_obj to_find );
			/*! Searches a character from the right (not implemented: returns NULL).
				\param	to_find	the character
				\return	NULL
			*/
			char *		right_find	( char to_find );
			/*! Searches a text from the right (not implemented: returns NULL).
				\param	to_find	the text
				\return	NULL
			*/
			char *		right_find	( char * to_find );
			/*! Searches a text from the right (not implemented: returns NULL).
				\param	to_find	the text
				\return	NULL
			*/
			char *		right_find	( gim_string_obj to_find );
			/*! Compares the text with another one (not implemented: returns __GIM_NOT_OK).
				\param	to_compare	the text to compare
				\return	__GIM_NOT_OK
			*/
			_gim_flag	equal		( char * to_compare );
			/*! Compares the text with another object (not implemented: returns __GIM_NOT_OK).
				\param	to_compare	the object to compare
				\return	__GIM_NOT_OK
			*/
			_gim_flag	equal		( gim_string_obj to_compare );


		private:
			/*! \brief	The text data. */
			gim_string_data	* data;

		public:
			/*! Builds an empty text. */
			inline gim_string_obj() {
				gim_error->set( "gim_string_obj::gim_string_obj" , "String requested" );
				data = new gim_string_data;
				__GIM_CLEAR( data->str , 32768 , char );
				__GIM_CLEAR( data->str_bkup , 32768 , char );
				data->str_len = 0;
				gim_error->set( "gim_string_obj::gim_string_obj" , "Done" );
			};

			/*! Builds a text from a C string.
				\param	to_set	the initial text
			*/
			inline gim_string_obj( char * to_set ) {
				gim_error->set( "gim_string_obj::gim_string_obj" , "String requested with parameter" );
				data = new gim_string_data;
				__GIM_CLEAR( data->str , 32768 , char );
				__GIM_CLEAR( data->str_bkup , 32768 , char );
				strcpy( data->str , to_set );
				data->str_len = strlen( data->str );
				gim_error->set( "gim_string_obj::gim_string_obj" , "Done" );
			};

			/*! Releases the text. */
			inline ~gim_string_obj() {
				delete data;
			};
	};



#endif // _GIM_STRING_OBJ_H_

