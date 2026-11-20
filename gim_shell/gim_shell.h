//
// gim_shell.h
//
// Gim Shell  -	It will be an application within the project Gim.
//				Will be useful to set all the files conf of any application,
//				to manage the keys and for controlling any GKP file.
//
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

#ifndef _GIM_SHELL_H_
#define _GIM_SHELL_H_
			
	#include "../include/gim.h"
	#include "../include/gim_list.h"
	#include "gim_sh_environ_obj.h"
	#include "gim_sh_callback.h"
	 
	extern gim_obj * gim;
	
	#define GIM_SH_DEFAULT_PROMPT	"($l@$n)_$p_$$_"

	#define GIM_SH_LINE_MAX		512
	#define GIM_SH_ARGV_MAX		16

	// result codes of a shell command
	#define GIM_SH_CMD_OK		0
	#define GIM_SH_CMD_ERROR	1
	#define GIM_SH_CMD_EXIT		2

	int gim_sh_mainloop( void );

	// tokenizes 'line' in place (whitespace separated, "double quotes" group words)
	// returns argc, argv points inside 'line'
	int gim_sh_tokenize( char * line , char ** argv , int max_args );
	// executes one command line (modified in place), returns a GIM_SH_CMD_* code
	int gim_sh_execute( char * line );
		
#endif // _GIM_SHELL_H_
