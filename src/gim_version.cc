//
// gim_version.cc
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

#include "../include/gim_version.h"


#ifdef GKMAKE_VERSIONING
		
char *	gim_version	( void ) {
	static char	gim_ver[256];
	if ( Lexical.str_equal( GIM_VERSION_STATE , "rel" ) == __GIM_YES )
		sprintf( gim_ver , "Gim %s - Ver %d.%d-%d compiled on %s" , (const char *)GIM_MILESTONE , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR , GIM_BUILD_LAST );
	if ( Lexical.str_equal( GIM_VERSION_STATE , "dev" ) == __GIM_YES )
		sprintf( gim_ver , "Gim %s - Ver %d.%d-%d-d compiled on %s" , (const char *)GIM_MILESTONE , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR , GIM_BUILD_LAST );
	return gim_ver;
}

#else

char *	gim_version	( void ) {
	static char	gim_ver[256];
	if ( Lexical.str_equal( GIM_VERSION_STATE , "rel" ) == __GIM_YES )
		sprintf( gim_ver , "Gim %s - Ver %d.%d-%d" , (const char *)GIM_MILESTONE , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR );
	if ( Lexical.str_equal( GIM_VERSION_STATE , "dev" ) == __GIM_YES )
		sprintf( gim_ver , "Gim %s - Ver %d.%d-%d-d" , (const char *)GIM_MILESTONE , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR );
	return gim_ver;
}

#endif

char *	gim_version_small	( void ) {
	static char	gim_ver[256];
	if ( Lexical.str_equal( GIM_VERSION_STATE , "rel" ) == __GIM_YES )
		sprintf( gim_ver , "Gim - Ver %d.%d-%d" , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR );
	if ( Lexical.str_equal( GIM_VERSION_STATE , "dev" ) == __GIM_YES )
		sprintf( gim_ver , "Gim - Ver %d.%d-%d-d" , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR );
	return gim_ver;
}


char *	gim_version_mini	( void ) {
	static char	gim_ver[256];
	if ( Lexical.str_equal( GIM_VERSION_STATE , "rel" ) == __GIM_YES )
		sprintf( gim_ver , "Gim-%d.%d-%d" , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR );
	if ( Lexical.str_equal( GIM_VERSION_STATE , "dev" ) == __GIM_YES )
		sprintf( gim_ver , "Gim-%d.%d-%d-d" , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR );
	return gim_ver;
}


char *	gim_version_micro	( void ) {
	static char	gim_ver[256];
	if ( Lexical.str_equal( GIM_VERSION_STATE , "rel" ) == __GIM_YES )
		sprintf( gim_ver , "%d.%d-%d" , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR );
	if ( Lexical.str_equal( GIM_VERSION_STATE , "dev" ) == __GIM_YES )
		sprintf( gim_ver , "%d.%d-%d-d" , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR );
	return gim_ver;
}


int		gim_check_version	( int maj , int min , int sub ) {
	char	gim_ver[16];
	char	req_ver[16];
	sprintf( gim_ver , "%d%d%d" , GIM_MAJOR , GIM_MINOR , GIM_SUBMINOR );
	sprintf( req_ver , "%d%d%d" , maj , min , sub );
	if ( atoi(gim_ver) >= atoi(req_ver) )
		return __GIM_OK;
	else 
		return __GIM_NOT_OK;
}


int		gim_check_version	( int maj , int min ) {
	char	gim_ver[16];
	char	req_ver[16];
	sprintf( gim_ver , "%d%d" , GIM_MAJOR , GIM_MINOR );
	sprintf( req_ver , "%d%d" , maj , min );
	if ( atoi(gim_ver) >= atoi(req_ver) )
		return __GIM_OK;
	else 
		return __GIM_NOT_OK;
}

