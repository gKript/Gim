//
// gim_sh_record.h
//
// Gim Shell - "insert" and "select" commands: records of the current database (phase 3).
//
/*#############################################################################

	Released under the terms of the GNU General Public License v2, as the rest of Gim.

#############################################################################*/

#ifndef _GIM_SH_RECORD_H_
#define _GIM_SH_RECORD_H_

	// insert <table> <field> <value> [<field> <value> ...]
	int cmd_insert( int argc , char ** argv );
	// select <table> [where <field> <op> <value>]     op: = != < <= > >=
	int cmd_select( int argc , char ** argv );

#endif // _GIM_SH_RECORD_H_
