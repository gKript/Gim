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

/*!     \file       gim_html.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include "../include/gim_html.h"

gim_html_obj	* gim_html;


/* copies src into a fixed field, always null terminated, NULL is an empty string */
static void copy_field( char * dst , size_t dst_size , const char * src ) {
	if ( src == NULL ) {
		dst[0] = '\0';
		return;
	}
	strncpy( dst , src , dst_size - 1 );
	dst[ dst_size - 1 ] = '\0';
}

/* a tag is valid if it has a name (no leading blank), and no '<' or '>' ( attributes are allowed ) */
static _gim_flag tag_is_valid( const char * tag ) {
	if ( tag == NULL || *tag == '\0' || *tag == ' ' || *tag == '\t' )
		return __GIM_NOT_OK;
	for ( const char * p = tag ; *p ; p++ ) {
		if ( *p == '<' || *p == '>' )
			return __GIM_NOT_OK;
	}
	return __GIM_OK;
}

/* length of the tag name: everything up to the first blank */
static int tag_name_length( const char * tag ) {
	int n = 0;
	while ( tag[n] && tag[n] != ' ' && tag[n] != '\t' && tag[n] != '\n' && tag[n] != '\r' )
		n++;
	return n;
}


_gim_flag	gim_html_obj::check_open( const char * caller ) {
	if ( page == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , caller , "Page object not allocated" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	if ( ( page->fh == NULL ) || ( page->fh->fp == NULL ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , caller , "Page file must be opened in write mode before" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	return __GIM_OK;
}

_gim_flag	gim_html_obj::check_tag( const char * tag ) {
	return tag_is_valid( tag );
}

void	gim_html_obj::make_indent( void ) {
	for ( int i = 0 ; i < indent_level * GIM_HTML_INDENT_WIDTH ; i++ )
		fputc( ' ' , page->fh->fp );
}

/* writes text escaping the characters that have a meaning in HTML */
void	gim_html_obj::write_escaped( const char * text ) {
	FILE * fp = page->fh->fp;
	for ( const char * p = text ; *p ; p++ ) {
		switch ( *p ) {
			case '<'  : fputs( "&lt;"   , fp ); break;
			case '>'  : fputs( "&gt;"   , fp ); break;
			case '&'  : fputs( "&amp;"  , fp ); break;
			case '"'  : fputs( "&quot;" , fp ); break;
			case '\'' : fputs( "&#39;"  , fp ); break;
			default   : fputc( *p , fp );       break;
		}
	}
}

/* writes an HTML comment; "--" is not allowed inside a comment, so it is split */
void	gim_html_obj::write_comment( const char * comment ) {
	FILE * fp = page->fh->fp;
	char prev = '\0';
	make_indent();
	fputs( "<!-- " , fp );
	for ( const char * p = comment ; *p ; p++ ) {
		if ( *p == '-' && prev == '-' )
			fputc( ' ' , fp );			/* "--" is not allowed inside a comment */
		fputc( *p , fp );
		prev = *p;
	}
	fputs( " -->\n" , fp );
}

void	gim_html_obj::write_open_tag( const char * tag ) {
	make_indent();
	fprintf( page->fh->fp , "<%s>\n" , tag );
	indent_level++;
}

void	gim_html_obj::write_close_tag( const char * tag ) {
	if ( indent_level > 0 )
		indent_level--;
	make_indent();
	fprintf( page->fh->fp , "</%.*s>\n" , tag_name_length( tag ) , tag );
}


_gim_flag	gim_html_obj::is_open( void ) {
	if ( ( page != NULL ) && ( page->fh != NULL ) && ( page->fh->fp != NULL ) )
		return __GIM_OK;
	return __GIM_NOT_OK;
}


_gim_flag	gim_html_obj::Export( const char * to_export ) {
	if ( check_open( "gim_html_obj::Export" ) != __GIM_OK )
		return __GIM_NOT_OK;
	if ( to_export == NULL ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export" , "You cannot pass NULL as parameter" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	make_indent();
	fprintf( page->fh->fp , "%s\n" , to_export );
	return __GIM_OK;
}

_gim_flag	gim_html_obj::Export_text( const char * text ) {
	if ( check_open( "gim_html_obj::Export_text" ) != __GIM_OK )
		return __GIM_NOT_OK;
	if ( text == NULL ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_text" , "You cannot pass NULL as parameter" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	make_indent();
	write_escaped( text );
	fputc( '\n' , page->fh->fp );
	return __GIM_OK;
}

_gim_flag	gim_html_obj::Export_tag( const char * tag ) {
	if ( check_open( "gim_html_obj::Export_tag" ) != __GIM_OK )
		return __GIM_NOT_OK;
	if ( check_tag( tag ) != __GIM_OK ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_tag" , "Tag is NULL, empty or contains < >" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	make_indent();
	fprintf( page->fh->fp , "<%s>\n" , tag );
	return __GIM_OK;
}

_gim_flag	gim_html_obj::Export_tag_open( const char * tag ) {
	if ( check_open( "gim_html_obj::Export_tag_open" ) != __GIM_OK )
		return __GIM_NOT_OK;
	if ( check_tag( tag ) != __GIM_OK ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_tag_open" , "Tag is NULL, empty or contains < >" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	write_open_tag( tag );
	return __GIM_OK;
}

_gim_flag	gim_html_obj::Export_tag_close( const char * tag ) {
	if ( check_open( "gim_html_obj::Export_tag_close" ) != __GIM_OK )
		return __GIM_NOT_OK;
	if ( check_tag( tag ) != __GIM_OK ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_tag_close" , "Tag is NULL, empty or contains < >" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	write_close_tag( tag );
	return __GIM_OK;
}

_gim_flag	gim_html_obj::Export_with_tag( const char * tag , const char * to_export ) {
	if ( check_open( "gim_html_obj::Export_with_tag" ) != __GIM_OK )
		return __GIM_NOT_OK;
	if ( ( check_tag( tag ) != __GIM_OK ) || ( to_export == NULL ) ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_with_tag" , "Bad tag or NULL text" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	make_indent();
	fprintf( page->fh->fp , "<%s>%s</%.*s>\n" , tag , to_export , tag_name_length( tag ) , tag );
	return __GIM_OK;
}

_gim_flag	gim_html_obj::Export_with_tag_text( const char * tag , const char * text ) {
	if ( check_open( "gim_html_obj::Export_with_tag_text" ) != __GIM_OK )
		return __GIM_NOT_OK;
	if ( ( check_tag( tag ) != __GIM_OK ) || ( text == NULL ) ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::Export_with_tag_text" , "Bad tag or NULL text" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	make_indent();
	fprintf( page->fh->fp , "<%s>" , tag );
	write_escaped( text );
	fprintf( page->fh->fp , "</%.*s>\n" , tag_name_length( tag ) , tag );
	return __GIM_OK;
}


_gim_flag	gim_html_obj::new_page( const char * title , const char * comment , const char * file_name ) {
	gim_error->set( "gim_html_obj::new_page" , "New html page requested..." );
	if ( page == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::new_page" , "Page object not allocated" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( ( file_name == NULL ) || ( *file_name == '\0' ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::new_page" , "File name is NULL or empty" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( page->fh != NULL )						// a page is already open: close it before starting another
		close_page();

	copy_field( page->title     , sizeof( page->title )     , title );
	copy_field( page->comment   , sizeof( page->comment )   , comment );
	copy_field( page->file_name , sizeof( page->file_name ) , file_name );
	indent_level = 0;

	page->fh = gim_file_manager->open( page->file_name , __GIM_FILE_POINTER , __GIM_WRITE );
	if ( ( page->fh == NULL ) || ( page->fh->fp == NULL ) ) {
		if ( page->fh != NULL )
			gim_file_manager->close( page->fh );
		page->fh = NULL;
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::new_page" , "File NOT CORRECTLY opened for writing" , __GIM_ERROR );
		return __GIM_ERROR;
	}

	gim_error->set( "gim_html_obj::new_page" , "File correctly opened for writing... Now i define the initial html code" );
	fputs( "<!DOCTYPE html>\n" , page->fh->fp );
	write_open_tag( "html" );
	write_open_tag( "head" );
	Export_tag( "meta charset=\"utf-8\"" );
	Export_with_tag_text( "title" , page->title );
	if ( page->comment[0] != '\0' )
		write_comment( page->comment );
	write_close_tag( "head" );
	write_open_tag( "body" );
	gim_error->set( "gim_html_obj::new_page" , "done!!!" );
	return __GIM_OK;
}


_gim_flag	gim_html_obj::close_page( void ) {
	if ( page == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_html_obj::close_page" , "Page not initialized" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	if ( page->fh == NULL ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_html_obj::close_page" , "No page is open" , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	if ( page->fh->fp != NULL ) {
		write_close_tag( "body" );
		write_close_tag( "html" );
	}
	gim_file_manager->close( page->fh );
	page->fh = NULL;					/* the handler is released by close(): never use it again */
	indent_level = 0;
	return __GIM_OK;
}
