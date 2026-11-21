//
// gim_sh_db.h
//
// Gim Shell - "db" command: create, list and select Gim databases (phase 1).
//
/*#############################################################################

	Released under the terms of the GNU General Public License v2, as the rest of Gim.

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
