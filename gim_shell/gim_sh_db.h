//
// gim_sh_db.h
//
// Gim Shell - "db" command: create, list and select Gim databases (phase 1).
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

#ifndef _GIM_SH_DB_H_
#define _GIM_SH_DB_H_

	// db create <name> [script.gdbs] | db list | db use <name> | db
	int cmd_db( int argc , char ** argv );

	// dblist: same as 'db list'
	int cmd_dblist( int argc , char ** argv );

	// closes the current database (called once, at shell exit)
	void gim_sh_db_shutdown( void );

	// the database selected with 'db use' or 'db create' (NULL = none)
	gim_db_obj * gim_sh_current_db( void );

#endif // _GIM_SH_DB_H_
