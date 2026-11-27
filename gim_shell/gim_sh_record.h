//
// gim_sh_record.h
//
// Gim Shell - "insert" and "select" commands: records of the current database (phase 3).
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

#ifndef _GIM_SH_RECORD_H_
#define _GIM_SH_RECORD_H_

	// insert <table> <field> <value> [<field> <value> ...]
	int cmd_insert( int argc , char ** argv );
	// select <table> [where <field> <op> <value>]     op: = != < <= > >=
	int cmd_select( int argc , char ** argv );
	// update <table> set <field> <value> [...] where <field> <op> <value> [and ...]
	int cmd_update( int argc , char ** argv );
	// delete <table> where <field> <op> <value> [and ...]
	int cmd_delete( int argc , char ** argv );

#endif // _GIM_SH_RECORD_H_
