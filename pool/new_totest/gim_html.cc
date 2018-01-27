//
// gim_html.cc
//
// Gim  -  Generic Information Manager static library
// Version 2.0-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%201.0?by=version]
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

#include "../include/gim_html.h"

gim_html_obj	* gim_html;


_gim_flag	gim_html_obj::Export( const char * to_export ) {
	if ( page->fh != NULL ) {
		if ( to_export != NULL ) {
			make_indent();
			fprintf( page->fh->fp , "%s\n" , to_export );
		}
		else {
			gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export" , "You cannot pass NULL as parameter" , __GIM_NOT_OK );
			return __GIM_NOT_OK;
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::Export" , "Page file must be opened in write mode before" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	return __GIM_OK;
}


_gim_flag	gim_html_obj::Export_tag_open( const char * tag ) {
	char compose[64];
	if ( page->fh != NULL ) {
		if ( tag != NULL ) {
			make_indent();
			sprintf( compose , "<%s>" , tag );
			fprintf( page->fh->fp , "%s\n" , compose );
			indent_level++;
		}
		else {
			gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_tag_open" , "You cannot pass NULL as parameter" , __GIM_NOT_OK );
			return __GIM_NOT_OK;
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::Export_tag_open" , "Page file must be opened in write mode before" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	return __GIM_OK;
}


_gim_flag	gim_html_obj::Export_tag( const char * tag ) {
	char compose[64];
	if ( page->fh != NULL ) {
		if ( tag != NULL ) {
			make_indent();
			sprintf( compose , "<%s>" , tag );
			fprintf( page->fh->fp , "%s\n" , compose );
		}
		else {
			gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_tag_open" , "You cannot pass NULL as parameter" , __GIM_NOT_OK );
			return __GIM_NOT_OK;
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::Export_tag_open" , "Page file must be opened in write mode before" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	return __GIM_OK;
}


_gim_flag	gim_html_obj::Export_tag_close( const char * tag ) {
	char compose[64];
	if ( page->fh != NULL ) {
		if ( tag != NULL ) {
			indent_level--;
			make_indent();
			sprintf( compose , "</%s>" , tag );
			fprintf( page->fh->fp , "%s\n" , compose );
		}
		else {
			gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_tag_close" , "You cannot pass NULL as parameter" , __GIM_NOT_OK );
			return __GIM_NOT_OK;
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::Export_tag_close" , "Page file must be opened in write mode before" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	return __GIM_OK;
}


_gim_flag	gim_html_obj::Export_with_tag( const char *  tag , const char * to_export ) {
	char compose[8192];
	if ( page->fh != NULL ) {
		if ( ( to_export != NULL ) && ( tag != NULL ) ) {
			make_indent();
			sprintf( compose , "<%s>%s</%s>\n" , tag , to_export , tag );
			fprintf( page->fh->fp , "%s" , compose );
		}
		else {
			gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_with_tag" , "You cannot pass NULL as parameter" , __GIM_NOT_OK );
			return __GIM_NOT_OK;
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::Export_with_tag" , "Page file must be opened in write mode before" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	return __GIM_OK;
}



_gim_flag	gim_html_obj::new_page( const char * title , const char * comment , const char * file_name ) {
	gim_error->set( "gim_html_obj::new_page" , "New html page requested..." );
	strcpy( page->title , title );
	strcpy( page->comment , comment );
	strcpy( page->file_name , file_name );
	gim_error->set( "gim_html_obj::new_page" , "Parameter setted" );
	page->fh = gim_file_manager->open( page->file_name , __GIM_FILE_POINTER , __GIM_WRITE );
	if ( page->fh == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::new_page" , "File NOT CORRECTLY opened for writing" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	else {
		gim_error->set( "gim_html_obj::new_page" , "File correctly opened for writing... Now i define the initial html code" );
		Export_tag_open( "html" );
		Export_tag_open( "head" );
		Export_with_tag( "title" , page->title );
		Export_tag_close( "head" );
		gim_error->set( "gim_html_obj::new_page" , "done!!!" );
	}
	return __GIM_OK;
}


_gim_flag	gim_html_obj::close_page( void ) {
	if ( page == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::close_page" , "Page not initialized" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	if ( page->fh == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::close_page" , "Page initialized but the file handler is NULL " , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	Export_tag_close( "html" );
	gim_file_manager->close( page->fh );
	return __GIM_OK;
}


void	gim_html_obj::make_indent( void ) {
	char ind[256];
	__GIM_CLEAR( ind , 256 , char );
	for( int c = 0 ; c < indent_level ; c++ ) {
		strcat( ind , GIM_HTML_INDENT );
	}
	fprintf( page->fh->fp , "%s" , ind );	
}
