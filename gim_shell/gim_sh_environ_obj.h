//
// gim_sh_environ_obj.h
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

#ifndef _GIM_SH_ENVIRON_OBJ_H_
#define _GIM_SH_ENVIRON_OBJ_H_
		
	extern char prompt[256];		
	extern _gim_kvalue	* gsh_conf;

	void set_gim_sh_environ		( void );
	void shut_gim_sh_environ	( void );
	void set_prompt				( void );
	
#endif // _GIM_SH_ENVIRON_OBJ_H_
