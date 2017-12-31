/***************************************************************************
 *            gkmake_heder.c
 *
 *  Tue May  9 10:18:12 2006
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
 
#include "bmake_main.h"


void hand_shake( void ) {
  puts  ( "  gkmake -   -   -   gKript make\n" );
  printf( "  Version-   -   -   %s\n" , GKMAKE_VERSION );  
  puts  ( "  Author -   -   -   asyntote [gKript.org]" );
  puts  ( "  Web    -   -   -   www.gkript.org\n" );
}
 

void print_help( void ) {
	puts( "  Project description:" );
	puts( "   -R    -   -   -   Delete the configuration file." ); 
	puts( "   -N (name) -   -   The project name." ); 
	puts( "   -A (name) -   -   The author's name." ); 
	puts( "   -P (prefix)   -   The prefix for the project." );
	puts( "   -I (path) -   -   Includes path." );
	puts( "   -H (YES/NO)   -   Project header file generation." );
	puts( "   -F (name) -   -   Header file name." );
	puts( "   -E (command)  -   The command for the editor you prefere to use." );
	puts( "   -D (path) -   -   Documentation path." );
	puts( "   -X (command)  -   Documentation command." );
	puts( "   -S (YES/NO)   -   Always use SUDO to install the project. Save this decision on the configuration file." );
	puts( "   -C (YES/NO)   -   Always clean before make the project. Save this decision on the configuration file." );
	puts( "   -T ([1-n]/NO) -   Make the project in multi thread with the specify cores number. Save this decision on the configuration file." );
	puts( "   -M (number >= 0)  Total build number." );
	puts( "   -B (number >= 0)  Succesful build number." );
	puts( "" );
	puts( "  Build configurations:" );
	puts( "   -t ([1-n])-   -   Make the project in multi thread with the specify cores number." );
	puts( "   -c    -   -   -   Clean before make." );
	puts( "   -n    -   -   -   Distclean for the project." );
	puts( "   -z    -   -   -   Generate the the project tarball." );
	puts( "   -d    -   -   -   Build the documentation." );
	puts( "   -i    -   -   -   Install the project." );
	puts( "   -s    -   -   -   Use SUDO to install." );
	puts( "   -a    -   -   -   Launch autotools." );
	puts( "" );
	puts( "  Build details:" );
	puts( "   -o    -   -   -   Print the configuration file for this project." );
	puts( "   -e    -   -   -   Edit the configuration file for this project." );
	puts( "   -h    -   -   -   Print this help text." );
	puts( "" );
	puts( "  Example :" );
	puts( "         gkmake -Ngkmake -Aasyntote -PGKM -P4" );
	puts( "" );
}


void	make_header( void ) {
	header->append("/***************************************************************************\n" );
	header->append(" *            %s\n" , gkmake.header_file_name );
	header->append(" *\n" );
	header->append(" *  %s\n" , gkmake.last_build );
	header->append(" *  Copyright  2006  %s\n" , gkmake.author  );
	header->append(" *  gkmake version : %s\n" , GKMAKE_VERSION );
	header->append(" *  asyntote@gkript.org\n" );
	header->append(" ***************************************************************************\n" );
	header->append(" *\n" );
	header->append(" *\n" );
	header->append(" *  This program is free software; you can redistribute it and/or modify\n" );
	header->append(" *  it under the terms of the GNU General Public License as published by\n" );
	header->append(" *  the Free Software Foundation; either version 2 of the License, or\n" );
	header->append(" *  (at your option) any later version.\n" );
	header->append(" *\n" );
	header->append(" *  This program is distributed in the hope that it will be useful,\n" );
	header->append(" *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n" );
	header->append(" *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" );
	header->append(" *  GNU General Public License for more details.\n" );
	header->append(" *\n" );
	header->append(" *  You should have received a copy of the GNU General Public License\n" );
	header->append(" *  along with this program; if not, write to the Free Software\n" );
	header->append(" *  Foundation, Inc., 59 Temple Place\n" ); 
	header->append(" *  Suite 330, Boston, MA 02111-1307, USA.\n" );
	header->append(" */\n" );
	header->append("\n" );
	header->append("    /*!\n" );
	header->append("        \\version   gkmake %s\n" , GKMAKE_VERSION );
	header->append("        \\brief     File generated by gkmake<BR>\n" );
	header->append("                   gkmake is an opensource project release under the terms<BR>\n" );
	header->append("                   of the GNU General Public License v3<BR>\n" );
	header->append("                   Distribuite with GIM static library<BR><BR>\n" );
	header->append("                   http://www.gkript.org<BR>\n" );
	header->append("    */\n" );
	header->append("\n\n" );
	header->append( "#ifndef\t%s_GKMAKE_VERSIONING\n", gkmake.prefix );
	header->append( "#define\t%s_GKMAKE_VERSIONING\n\n", gkmake.prefix );
	header->append( "   #define %s_AUTHOR                       %s\n", gkmake.prefix , gkmake.author );
	header->append( "   #define %s_GKMAKE_TOTAL_BUILD           %d\n", gkmake.prefix , gkmake.tot_build );
	header->append( "   #define %s_GKMAKE_BUILD                 %d\n", gkmake.prefix , gkmake.ok_build );
	header->append( "   #define %s_GKMAKE_LAST_BUILD            \"%s\"\n\n", gkmake.prefix , gkmake.last_build );
	header->append( "#endif\n\n" );
}



char * str_up( char * line , int len ) {
	static char	* tmp = NULL;
	int		a = 0;
	char	c = 0;
	tmp = strdup( line );
	for ( a = 0 ; a < len ; a++ ) {
		c = tmp[a];
		tmp[a] = toupper(c);
	}
	return tmp;
}

char * str_down ( char * line , int len ) {
	static char	* tmp = NULL;
	int		a = 0;
	char	c = 0;
	tmp = strdup( line );
	for ( a = 0 ; a < len ; a++ ) {
		c = tmp[a];
		tmp[a] = tolower(c);
	}
	return tmp;
}


int gkmake_exist ( char * fname ) {
	FILE    * fp;

	if ( ( fp = fopen( fname , "r" ) ) ) {
		fclose( fp );
		return 1; 
	}
	return 0;
}


