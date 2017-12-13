//
// gim_html.h
//
// Gim  -  Generic Information Manager static library
// Version 2.0-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.0?by=version]
//
/*#############################################################################

	Copyright (C) 2002, 2007 
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

/*!	\file		gim_html.h
	\version	2.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://gkript.org/gk_gpl_v2.php ]
*/

#ifndef _GIM_HTML_OBJ_H_
#define _GIM_HTML_OBJ_H_

	#include "gim_base_header.h"
	#include "gim_string.h"
	#include "gim_file.h"

	/*!	\struct		gim_html_page
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is used by Gim to make an html page.
	*/
	struct gim_html_page {
		char			title[512];
		char		 	comment[512];
		char			file_name[2048];
		_gim_handler	* fh;
	};


	/*!	\struct		gim_html_properities
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is used by Gim to hold trace of the html properities.
	*/
	struct gim_html_properities {
		_gim_string	name;
		char		* c_value;
		int			i_value;
		float		f_value;
		
		gim_html_properities	* next ;
	};


	/*! \class 		gim_html_obj
		\brief		The Gim's html obj is useful to easily make by c++ code web page. \n
					In this example we create a simple html page. \n
					\code 
	
	#include <gim/gim.h>

	int main ( int argc , char *argv[] ) {
		gim_obj * gim = new gim_obj;
		_gim_html * test = new _gim_html;
		test->new_page( "The first test page " , "no comment" , "./test_page.html" );
		test->Export_tag_open( "center" );
			test->Export_with_tag( "h1" , "This is the first page generated with Gim 2.0[1.9-6 dev]<br><br>" );
			test->Export( gim_version() );
		test->Export_tag_close( "center" );
		delete test;
		delete gim;
		return 0;
	}
		
					\endcode
					This is the result. \n
					\code 

	<html>
	<head>
	<title>The first test page </title>
	</head>
	<center>
	<h1>This is the first page generated with Gim 2.0[1.9-6 dev]<br><br></h1>
	Gim - Ver 1.9-568
	</center>
	</html>

					\endcode
					

		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008
	*/
		class gim_html_obj {
		public:
			/*! \fn		_gim_flag new_page ( char * title , char * comment , char * file_name )
				\brief	 This method is the first step after the declaration of the html class. \n
				\param	title		The title of the new html page
				\param	comment		A comment to the new html page
				\param	file_name	The file name of the new html page
				\return	\a __GIM_OK if success or \a __GIM_ERROR if fail
			*/
			_gim_flag	new_page				( const char * title , const char * comment , const char * file_name );

			/*! \fn			_gim_flag page_css ( char * css_name )
				\brief		This method is useful ti include a css page in the new html page.
				\warning	<b>This method has not yet been developed.</b>
				\todo		<b>Development of this method has not yet been scheduled.</b>
			*/
			_gim_flag	page_css				( const char * css_name );

			/*! \fn			_gim_flag page_css_from_gkp ( char * css_name )
				\brief		This method is useful ti include a css page taken from a GKDP in the new html page.
				\warning	<b>This method has not yet been developed.</b>
				\todo		<b>Development of this method has not yet been scheduled.</b>
			*/
			_gim_flag	page_css_from_gkp		( const char * css_name );

			/*! \fn			_gim_flag add_properities ( _gim_flag prop_name , char  * value )
				\brief		This method is useful to add a properities to an html obj.
				\warning	<b>This method has not yet been developed.</b>
				\todo		<b>Development of this method has not yet been scheduled.</b>
			*/
			_gim_flag	add_properities			( _gim_flag prop_name , const char  * value );

			/*! \fn			_gim_flag add_properities ( _gim_flag prop_name , int   value )
				\brief		This method is useful to add a properities to an html obj.
				\warning	<b>This method has not yet been developed.</b>
				\todo		<b>Development of this method has not yet been scheduled.</b>
			*/
			_gim_flag	add_properities			( _gim_flag prop_name , int   value );

			/*! \fn			_gim_flag add_properities ( _gim_flag prop_name , float value )
				\brief		This method is useful to add a properities to an html obj.
				\warning	<b>This method has not yet been developed.</b>
				\todo		<b>Development of this method has not yet been scheduled.</b>
			*/
			_gim_flag	add_properities			( _gim_flag prop_name , float value );
				
			/*! \fn			void render_properities ( void )
				\brief		This method is useful to add a properities to an html obj.
				\warning	<b>This method has not yet been developed.</b>
				\todo		<b>Development of this method has not yet been scheduled.</b>
			*/
			void		render_properities		( void );
				
			/*! \fn			_gim_flag new_paragraph ( void )
				\brief		This method is useful to open a new paragraph to an html page.
				\warning	<b>This method has not yet been developed.</b>
				\todo		<b>Development of this method has not yet been scheduled.</b>
			*/
			_gim_flag 	new_paragraph			( void );
				
			/*! \fn			_gim_flag add_data_to_paragraph ( _gim_buffer data )
				\brief		This method is useful to add a data to a paragraph.
				\warning	<b>This method has not yet been developed.</b>
				\todo		<b>Development of this method has not yet been scheduled.</b>
			*/
			_gim_flag	add_data_to_paragraph	( _gim_buffer data );
				
			/*! \fn			_gim_flag close_paragraph ( void )
				\brief		This method is useful to close a paragraph.
				\warning	<b>This method has not yet been developed.</b>
				\todo		<b>Development of this method has not yet been scheduled.</b>
			*/
			_gim_flag	close_paragraph			( void );
				
			/*! \fn			_gim_flag close_page ( void )
				\brief		This method is useful to close the html page.
				\return		\a __GIM_OK on success , \a __GIM_NOT_OK on fail
			*/
			_gim_flag	close_page				( void );
				
			/*! \fn		_gim_flag Export ( char * to_export )
				\brief	This method is useful to print a string in the html page.
				\param	to_export		The string to export
				\return	\a __GIM_OK if success or \a __GIM_NOT_OK if fail
			*/
			_gim_flag	Export					( const char * to_export );
				
			/*! \fn		_gim_flag Export_tag ( char * tag )
				\brief	This method is useful to print a \a TAG in the html page.
				\param	tag		The \a TAG to export without symbols <>
				\return	\a __GIM_OK if success or \a __GIM_NOT_OK if fail
			*/
			_gim_flag	Export_tag				( const char * tag );
				
			/*! \fn		_gim_flag Export_tag_open ( char * tag )
				\brief	This method is useful to open a \a TAG in the html page.
				\param	tag		The \a TAG to open without symbols <>
				\return	\a __GIM_OK if success or \a __GIM_NOT_OK if fail
			*/
			_gim_flag	Export_tag_open			( const char * tag );
				
			/*! \fn		_gim_flag Export_tag_close ( char * tag )
				\brief	This method is useful to close a \a TAG in the html page.
				\param	tag		The \a TAG to close without symbols <>
				\return	\a __GIM_OK if success or \a __GIM_NOT_OK if fail
			*/
			_gim_flag	Export_tag_close		( const char * tag );
				
			/*! \fn		_gim_flag Export_with_tag ( char *  tag , char * to_export )
				\brief	This method is useful to export a string enclosed  in a \a TAG ( open and close ) in the html page.
				\param	to_export	The string to export
				\param	tag			The \a TAG to export without symbols <>
				\return	\a __GIM_OK if success or \a __GIM_NOT_OK if fail
			*/
			_gim_flag	Export_with_tag			( const char *  tag , const char * to_export );
			
		private:
			void		destroy_prop_list		( void );
			
			gim_html_page			* page;
			gim_html_properities	* start_prop;
			gim_html_properities	* cur_prop;
				
		public:
			
			inline gim_html_obj() {
				gim_error->set( "gim_html_obj::gim_html_obj" , "page initialization start" );
				page = (gim_html_page *)gim_memory->Alloc( sizeof( gim_html_page )  , __GIM_HTML_PAGE , __GIM_HIDE );
				if ( page == NULL ) {
					gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::gim_html_obj" , "    Memory NOT CORRECTLY allocated succesfully" , __GIM_ERROR );
				}
				else {
					gim_error->set( "gim_html_obj::gim_html_obj" , "    Memory allocated succesfully" );
					page->fh	= NULL;
					start_prop	= NULL;
					cur_prop	= NULL;
					gim_error->set( "gim_html_obj::gim_html_obj" , "    Costructor correctly finish his job" );
				}
			};
		
			inline ~gim_html_obj() {
				gim_error->set( "gim_html_obj::~gim_html_obj" , "Destructor start" );
				if ( page->fh != NULL ) {
					gim_error->set( "gim_html_obj::~gim_html_obj" , "    Closing page on destruct" );
					close_page();
				}
				gim_memory->Free( page );
				if ( start_prop != NULL ) {
					gim_error->set( "gim_html_obj::~gim_html_obj" , "    Destroy the list of properities on destruct" );
					//destroy_prop_list();
				}
			};
	};

#endif // _GIM_HTML_OBJ_H_
