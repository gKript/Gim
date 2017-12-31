//
// gim_sh_mainloop.c
//
// Gim Shell  -  a GIM application to work 
// Version 0.1-0
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

#include "gim_shell.h"

int gim_sh_mainloop( ) {
    int exit = 1;
    char in_line[512];
    
    __GIM_CLEAR( in_line , 512 , char );
    while ( exit ) {
        printf( "%s" , prompt );
        fgets( in_line , 512 , stdin ); 
        if ( strlen( gim_sh_buff2str( in_line , 512 ) ) ) {
			if ( exit )
				puts( gim_sh_buff2str( in_line , 512 ) );
			if ( Lexical.str_equal( gim_sh_buff2str( in_line , 512 ) , "exit" ) == __GIM_YES )
	            exit = 0;
        }
    }
    
}

char * gim_sh_buff2str( char * in , int charlen ) {
	static char * out;
	_gim_flag found = 0;
	out = (char *)gim->memory->Alloc( sizeof(char) * charlen );
	__GIM_CLEAR( out , sizeof( out ) , char );
	for( int a = 0 ; a <= charlen ; a++ ) {
		if ( in[a] != '\n' )
			out[a] = in[a];
		else {
			out[a] = '\0';
			found = 1;
			a = charlen;
		}
	}
	if ( ! found )
		out[0] = '\0';
	return out;
}


