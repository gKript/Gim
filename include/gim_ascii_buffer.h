//
//  gim_ascii_buffer.h
//
//  Gim - Generic Information Manager
//  Version 2.4-1  -  Milestone 2.5
//  Creator - asyntote
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
// Gim is developed with Anjuta version 1.2.4  - 2.32.1.1
//

/*!     \file        gim_ascii_buffer
        \version     2.5
	\brief       Copyright (C) 2002, 2003<BR>
		     Danilo Zannoni (AsYntote)<BR>
		     Released under the terms of the GNU General Public License v2<BR>
		     [ http://www.gkript.org/gkript-gpl.html ]
*/

/*!     \class       gim_list_obj
        \brief       "Put your brief here"
        \author      asyntote
        \version     2.5
        \date        2003-2018
*/

#ifndef _GIM_ASCII_BUFFER_OBJ_H_
#define _GIM_ASCII_BUFFER_OBJ_H_

	#define	 __GIM_FIRST		0
	#define	 __GIM_LAST			1

	/*!	\struct		gim_file_manager_list
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is used by Gim to hold trace of your opearation on files.
					Please read the documentation on gim_file_manager_obj.
	*/

	  /*!  \struct    gim_in_buffer_result
		\brief    This struct is used by Gim to...
	  */
	  struct gim_in_buffer_result {
		_gim_Uint32		class_id;
		char *			pattern;
		_gim_list *		occurrence;
	  };

	typedef	struct gim_in_buffer_result		_gim_in_buffer;	

	/*! \class 		gim_ascii_file_obj
		\brief		The Gim ascii file class is useful to ...\n
					\code 
	
		#include <stdio.h>
		#include <unistd.h>				
		#include <gim/gim.h>
		int main( int argc , char **argv ) {
			gim_obj * gim = new gim_obj;
			_gim_ascii_buffer * test = new _gim_ascii_buffer;
			
			
			delete test;
			delete gim;
			return 0;
		}
			
					\endcode	

		\author		Corrado Tumiati	-	skymatrix@alice.it
		\author		Danilo Zannoni	-	asyntote@gmail.com
		\author		gKript.org
		\version	3.0
		\date		2003-2020
	*/
	    class gim_ascii_file_obj {
		public:
					 _gim_flag				set_dimension( _gim_Uint32 size );
					 _gim_flag				append( char * to_append );
//					 _gim_flag				append( const char * to_append );
					 _gim_flag				append( const char * format , ... );
//					 _gim_flag				append( _gim_string * str_to_append );
					 _gim_flag				substitute( char * variable , char * text );
					 _gim_flag				insert_variable( char * variable );
					 _gim_int32				lenght( void );
					 _gim_int32				lines( void );
                     _gim_in_buffer *      	is_in_buffer( const char * to_find );
                     _gim_in_buffer *      	is_in_buffer( char * to_find );
                     _gim_flag				in_buffer_subst( _gim_in_buffer * resulting , char * new_part );
				     char *					get_buffer( void );					 
					 _gim_flag				seekf( _gim_flag position );
					 _gim_flag				seek( _gim_int32 position );
					 char *					load( char * filename , char * new_buffer , _gim_flag overwrite );
					 
		private:
					 _gim_int32				index;
					 _gim_int32				dimension;
					 char *					chrbuf;

					 prsr_lexical_class *   lex;

		public:


			/*!gim_list_obj constructor
		    */
		    inline gim_ascii_file_obj() {
				index = 0;
				dimension = 0;
				lex = new prsr_lexical_class;
				gim_error->set( "gim_list_obj::gim_ascii_file_obj" , "A new ascii buffer allocated and inizialized" );
		    };

		    /*!gim_list_obj distructor
		    */
		    inline ~gim_ascii_file_obj() {
		    	delete lex;
/*		    	if ( chrbuf != NULL ) {
		    		gim_memory->Unlock_and_free( chrbuf );
		    	}
*/				gim_error->set( "gim_list_obj::gim_ascii_file_obj" , "An ascii buffer deleted" );
		    };
    };

#endif // _GIM_ASCII_BUFFER_OBJ_H_

