/***************************************************************************
 *            bmake_heder.h
 *
 *  Tue May  9 10:20:23 2006
 *  Copyright  2006  AsYntote
 *  asyntote@gkript.org
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _BMAKE_HEDER_H
#define _BMAKE_HEDER_H


	#include <gim/gim.h>
	
	#define	GKMAKE_VERSION			"0.8-5"
	#define MAKEFILE				"Makefile"
	#define	GKMAKE_FILE_COUNTER		".gkmake_conf"
	#define GKMAKE_INCLUDE			"_bmake.h"
	#define GKMAKE_COMMAND			"make"
	#define GKMAKE_MESSAGE			"GKM"
	#define	BUFF_DIM				2048
	
//	#define gch(a,b,c)				printf(a),scanf(b,c)
	

	struct old_gkmake_st {
		char	prefix[64];
		int		make;
		int		make_ok;
	} ;


	struct gkmake_st {
		
		_gim_flag	coder_exist;
	
		char 		login[64];
		char 		node[64];
		char 		arch[64];
		char		gim_rel[64];
		char		gkmake_rel[64];
		
		
			
		char		prefix[64];
		char		include_path[256];
		char		prj_name[128];
		char		author[128];
		char		documentation[256];
		char		doc_command[256];
		char		last_build[256];
		_gim_flag   sudo;
		_gim_flag	parallel;
		_gim_flag   clean;
		_gim_flag   header_file;
		char		header_file_name[64];
		char		editor_command[64];
		char		premake_script[256];
		char		postmake_script[256];
		int			tot_build;
		int			ok_build;
		
		int			prj_sum_total_build;
		int			prj_sum_ok_build;
		
		int			num_core;
	};

	void	make_header				( void );
	char *	get_current_dir_name	( void );
	int 	gkmake_exist			( char * );
	void	gkmake_status			( void );
	void	gkmake_header			( FILE * , char * );
	void 	gkmake_install			( void );
	void 	new_prj					( char * );
	void	convert_from_old		( char * );
	char *	str_up					( char * line , int len );
	char *	str_down				( char * line , int len );

	void	hand_shake				( void );
	void	print_help				( void );
		
	extern 	gkmake_st				gkmake;
	extern	old_gkmake_st			gkmake_old;
	extern	char					timestamp[256];
	extern  gim_ascii_file_obj *	header;




#endif /* _BMAKE_HEDER_H */
