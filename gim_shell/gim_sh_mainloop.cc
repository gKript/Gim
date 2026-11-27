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

#include "gim_shell.h"

int gim_sh_mainloop( ) {
	char in_line[GIM_SH_LINE_MAX];
	int running = 1;

	while ( running ) {
		printf( "%s" , prompt );
		fflush( stdout );

		// EOF (Ctrl+D / Ctrl+Z) or read error: leave the shell
		if ( fgets( in_line , GIM_SH_LINE_MAX , stdin ) == NULL ) {
			puts( "" );
			break;
		}

		size_t len = strlen( in_line );
		if ( len && in_line[len-1] == '\n' ) {
			in_line[--len] = '\0';
			if ( len && in_line[len-1] == '\r' )
				in_line[--len] = '\0';
		} else if ( len == GIM_SH_LINE_MAX - 1 ) {
			// line too long: discard the rest so it is not run as a new command
			int ch;
			while ( ( ch = getchar() ) != '\n' && ch != EOF )
				;
			puts( "gimshell: line too long, ignored" );
			continue;
		}

		if ( gim_sh_execute( in_line ) == GIM_SH_CMD_EXIT )
			running = 0;
	}

	return 0;
}


