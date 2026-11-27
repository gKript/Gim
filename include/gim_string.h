//
//  gim_string.h
//
//  Gim - Generic Information Manager
//  Version 3.0-0  -  Milestone 3.0
//
//  [http://www.gkript.org]
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
// Gim is developed with Anjuta version 1.2.4  - 3.18.2
//

/*!     \file       gim_string.h
        \version    3.0
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
		\note		Text as an object (gim_string_obj). Rewritten 2026-11-27, NOT COMPILED YET: see SYNC/TODO_GIM.md.
*/

#ifndef _GIM_STRING_OBJ_H_
#define _GIM_STRING_OBJ_H_

	#include "gim_base_header.h"
	#include <stdarg.h>
	#include <string.h>

	/*! \class 		gim_string_obj
		\brief		A text used as an object. The text is always a valid C string: an empty text has the length 0.
		\author		asyntote
		\version	3.0-0
		\date		2026
	*/
	class gim_string_obj {
		public:
			/*! Builds an empty text. */
			gim_string_obj( void );
			/*! Builds a text from a C string.
				\param	to_set	the initial text (NULL means empty)
			*/
			gim_string_obj( const char * to_set );
			/*! Builds a deep copy of another text.
				\param	other	the text to copy
			*/
			gim_string_obj( const gim_string_obj & other );
			/*! Releases the memory of the text. */
			~gim_string_obj( void );

			/*! Replaces the text with a deep copy of another one.
				\param	other	the text to copy
				\return	this object
			*/
			gim_string_obj &	operator=( const gim_string_obj & other );
			/*! Replaces the text with a C string.
				\param	to_set	the new text (NULL means empty)
				\return	this object
			*/
			gim_string_obj &	operator=( const char * to_set );

			/*! \return __GIM_YES when the two texts are equal, __GIM_NO otherwise */
			_gim_flag			operator==( const gim_string_obj & other ) const;
			/*! \return __GIM_YES when the two texts are different, __GIM_NO otherwise */
			_gim_flag			operator!=( const gim_string_obj & other ) const;

			/*! Sets the text. The previous text is replaced.
				\param	to_set	the new text (NULL means empty)
			*/
			void				set( const char * to_set );
			/*! Empties the text. The memory is kept for the next use. */
			void				clean( void );
			/*! Returns the text as a C string. It is never NULL: an empty text gives an empty string.
				The pointer is valid until the next change of the text.
				\return	pointer to the text
			*/
			char *				c_str( void );
			/*! \return the length of the text, in characters */
			_gim_Uint32			size( void );
			/*! \return the length of the text, in characters (same as size()) */
			_gim_Uint32			length( void );
			/*! \return the number of characters the text can hold without allocating more memory */
			_gim_Uint32			capacity( void );
			/*! Makes room for at least n characters, without changing the text.
				\param	n	the number of characters
				\return	__GIM_OK, or __GIM_NOT_OK when the memory is not available
			*/
			_gim_flag			reserve( _gim_Uint32 n );
			/*! \return __GIM_YES when the text is empty, __GIM_NO otherwise */
			_gim_flag			empty( void );

			/*! Appends a text. It also works on an empty text.
				\param	to_cat	the text to append (NULL means nothing)
			*/
			void				cat( const char * to_cat );
			/*! Appends a text formatted with printf rules.
				\param	fmt	the format
			*/
			void				cat_format( const char * fmt , ... );
			/*! Replaces the text with a text formatted with printf rules.
				\param	fmt	the format
			*/
			void				set_format( const char * fmt , ... );
			/*! Replaces the text with a decimal integer. \param v the number */
			void				set_int( _gim_Llong v );
			/*! Replaces the text with a number, written with %g. \param v the number */
			void				set_float( double v );
			/*! \return the text read as a decimal integer (0 when the text is not a number) */
			_gim_Llong			to_int( void );
			/*! \return the text read as a number (0 when the text is not a number) */
			double				to_float( void );

			/*! \return the character at the position, counted from 0, or '\0' when out of range
				\param	pos	the position
			*/
			char				at( _gim_Uint32 pos );
			/*! Inserts a text before a position. A position past the end appends the text.
				\param	pos		the position
				\param	text	the text to insert
			*/
			void				insert( _gim_Uint32 pos , const char * text );
			/*! Removes characters from a position. Nothing happens when the position is past the end.
				\param	pos		the first character to remove
				\param	count	how many characters to remove
			*/
			void				erase( _gim_Uint32 pos , _gim_Uint32 count );
			/*! \return a new text with the characters from pos, for count characters (fewer if the text ends)
				\param	pos		the first character
				\param	count	how many characters
			*/
			gim_string_obj		substr( _gim_Uint32 pos , _gim_Uint32 count );
			/*! Removes the spaces, tabs, carriage returns and line feeds at both ends. */
			void				trim( void );
			/*! Removes the spaces at the beginning. */
			void				trim_left( void );
			/*! Removes the spaces at the end. */
			void				trim_right( void );
			/*! Converts the letters to upper case. */
			void				upper( void );
			/*! Converts the letters to lower case. */
			void				lower( void );
			/*! Replaces every occurrence of old_text with new_text. An empty old_text does nothing.
				\param	old_text	the text to replace
				\param	new_text	the replacement
			*/
			void				replace( const char * old_text , const char * new_text );

			/*! \return the number of fields separated by sep: 0 for an empty text, otherwise the separators plus one
				\param	sep	the separator character
			*/
			_gim_Uint32			fields( char sep );
			/*! Gets one field of the text.
				\param	sep		the separator character
				\param	n		the field, counted from 0
				\param	out		receives the field
				\return	__GIM_YES when the field exists, __GIM_NO otherwise
			*/
			_gim_flag			field( char sep , _gim_Uint32 n , gim_string_obj & out );

			/*! \return __GIM_YES when the text starts with the given text */
			_gim_flag			starts_with( const char * prefix );
			/*! \return __GIM_YES when the text ends with the given text */
			_gim_flag			ends_with( const char * suffix );

			/*! \return __GIM_YES when the text contains the given text, __GIM_NO otherwise */
			_gim_flag			find( const char * to_find );
			/*! \return the position of the first occurrence of the text, or -1 when not found. An empty text is found at 0 */
			int					pos_of( const char * to_find );
			/*! \return the position of the last occurrence of the text, or -1 when not found. An empty text is found at the end */
			int					rpos_of( const char * to_find );

			/*! \return a pointer to the first occurrence of the character inside the text, or NULL.
				The pointer is valid until the next change of the text. */
			char *				left_find( char to_find );
			/*! \return a pointer to the first occurrence of the text inside the text, or NULL. See left_find( char ). */
			char *				left_find( const char * to_find );
			/*! \return a pointer to the first occurrence of the text inside the text, or NULL. See left_find( char ). */
			char *				left_find( const gim_string_obj & to_find );
			/*! \return a pointer to the last occurrence of the character inside the text, or NULL. See left_find( char ). */
			char *				right_find( char to_find );
			/*! \return a pointer to the last occurrence of the text inside the text, or NULL. See left_find( char ). */
			char *				right_find( const char * to_find );
			/*! \return a pointer to the last occurrence of the text inside the text, or NULL. See left_find( char ). */
			char *				right_find( const gim_string_obj & to_find );

			/*! Compares the text with a C string, as strcmp does.
				\return <0, 0 or >0
			*/
			int					compare( const char * to_compare );
			/*! Compares the text with another text, as strcmp does.
				\return <0, 0 or >0
			*/
			int					compare( const gim_string_obj & to_compare ) const;
			/*! \return __GIM_YES when the texts are equal, __GIM_NO otherwise */
			_gim_flag			equal( const char * to_compare );
			/*! \return __GIM_YES when the texts are equal, __GIM_NO otherwise */
			_gim_flag			equal( const gim_string_obj & to_compare ) const;

		private:
			/*! \brief	The text, always NUL-terminated (NULL until the first use). */
			char *				str;
			/*! \brief	The length of the text, in characters. */
			_gim_Uint32			len;
			/*! \brief	The size of the buffer, in bytes, NUL included. */
			_gim_Uint32			cap;

			/*! \brief	The text, or the empty string when there is no buffer. */
			const char *		text( void ) const;
			/*! \brief	Makes the buffer able to hold need characters plus the NUL.
				\return	__GIM_OK, or __GIM_NOT_OK when the memory is not available
			*/
			_gim_flag			grow( _gim_Uint32 need );
			/*! \brief	Appends n characters of t. Works also when t points inside this text. */
			_gim_flag			append_n( const char * t , _gim_Uint32 n );
			/*! \brief	Tells whether a pointer points inside this text buffer. */
			bool				own( const char * t ) const;
	};

/*!	\page	gimdoc_string	Strings

	A text used as an object: it grows as needed, it can be copied and compared, and it has the operations that C strings lack.
	Class: \ref gim_string_obj. Short name: \c _gim_string. Header: gim_string.h

	\tableofcontents

	\section	gimdoc_string_what	What it is for

	C strings are fixed arrays with a terminating zero: they do not grow, they do not know their length, and most of the
	functions of the library overwrite them without checking. A \c gim_string_obj keeps the text in a buffer that Gim
	allocates and enlarges when needed, it always knows its length, and it copies safely.

	Use it for the names of files, the lines of a configuration, the fields of a CSV line, the text of a message that is
	built in parts, and everywhere a text changes size while the program runs. The project rule is to use Gim classes for
	texts before the standard library: this class is the one to use.

	\section	gimdoc_string_before	Before you start

	- Create the ::gim_obj first: the text uses the memory manager of Gim, and without it an empty text cannot grow.
	- The text is always a valid C string. An empty text has length 0 and \c c_str() returns an empty string, never NULL.
	- A text is copied when it is assigned or passed by value: the copy is independent of the original.

	\section	gimdoc_string_example	Examples

	Building a text in parts and reading it as a C string:
	\code{.cpp}
	_gim_string name;
	name.set( "report" );
	name.cat( "-" );
	name.cat_format( "%03d.txt" , 7 );
	printf( "%s (%u characters)\n" , name.c_str() , name.size() );   // report-007.txt (14 characters)
	\endcode

	Splitting a CSV line on the semicolon and reading the second field:
	\code{.cpp}
	_gim_string line( "anna;42;rome" );
	_gim_string field;
	if ( line.field( ';' , 1 , field ) == __GIM_YES )
		printf( "age: %lld\n" , field.to_int() );     // age: 42
	\endcode

	Changing a text and searching in it:
	\code{.cpp}
	_gim_string path( "  C:/data/config.conf  " );
	path.trim();                                      // "C:/data/config.conf"
	if ( path.ends_with( ".conf" ) == __GIM_YES )
		path.replace( "/" , "\\" );                  // "C:\data\config.conf"
	int dot = path.rpos_of( "." );                    // 15, the last dot
	_gim_string ext = path.substr( dot , 100 );       // ".conf"
	\endcode

	Comparing texts:
	\code{.cpp}
	_gim_string a( "abc" ) , b( "abd" );
	if ( a == b ) { }  // never
	if ( a.compare( b ) < 0 ) { }  // a comes first
	\endcode

	\section	gimdoc_string_methods	The methods, by purpose

	| Purpose | Methods |
	|---------|---------|
	| Set and read | set(), c_str(), size(), length(), capacity(), empty(), clean(), reserve() |
	| Append and format | cat(), cat_format(), set_format() |
	| Numbers | set_int(), set_float(), to_int(), to_float() |
	| Characters | at(), insert(), erase(), substr() |
	| Spaces and case | trim(), trim_left(), trim_right(), upper(), lower() |
	| Replace | replace() |
	| Fields | fields(), field() |
	| Prefix and suffix | starts_with(), ends_with() |
	| Search | find(), pos_of(), rpos_of(), left_find(), right_find() |
	| Compare | compare(), equal(), operator==, operator!= |
	| Copy | constructor, operator= |

	The details of each one are in \ref gim_string_obj.

	\section	gimdoc_string_returns	Return values

	| Method | Returns |
	|--------|---------|
	| find(), starts_with(), ends_with() | __GIM_YES or __GIM_NO |
	| pos_of(), rpos_of() | the position counted from 0, or -1 when not found |
	| left_find(), right_find() | a pointer inside the text, or NULL when not found |
	| compare() | less than 0, 0 or greater than 0, as strcmp |
	| equal(), operator== , operator!= | __GIM_YES or __GIM_NO (operators return the same flags) |
	| field() | __GIM_YES when the field exists, __GIM_NO otherwise |
	| reserve() | __GIM_OK, or __GIM_NOT_OK when the memory is not available |
	| to_int(), to_float() | the value read; 0 when the text does not start with a number |

	\section	gimdoc_string_pointers	Pointers into the text

	c_str(), left_find() and right_find() return pointers into the buffer of the text. A pointer is valid only until the next
	change of the text: a cat(), set(), insert() or any other change can move the buffer, and the old pointer then points
	to freed memory. Copy the text, or read the pointer again, after a change.

	\section	gimdoc_string_threads	Threads

	Not thread-safe: the object has no lock. Use a text from one thread only, or protect it with a lock of your own.

	\section	gimdoc_string_warnings	Warnings and limits

	- A format passed to cat_format() or set_format() must not use the text itself as an argument: the text may move while the
	  format is printed.
	- When the memory is not available, set(), cat() and insert() leave the text as it was; reserve() reports it.
	- find() and pos_of() search the first occurrence, rpos_of() the last one: an empty search text is found at 0 (pos_of) or at the end (rpos_of).
	- The text has no encoding: each character is a byte. Accents and other characters outside ASCII take more than one position.
	- upper() and lower() change only the letters of the English alphabet.
	- to_int() and to_float() stop at the first character that is not a digit, so "12abc" gives 12 and "abc" gives 0. They do
	  not report an error: check the text before if it matters.
	- A text lives in the heap: many small texts cost more than a few large ones. Reuse a text with clean() instead of creating a new one.
	- The class is not yet tested on a build: see the status of the class in SYNC/TODO_GIM.md before depending on it.
*/


#endif // _GIM_STRING_OBJ_H_
