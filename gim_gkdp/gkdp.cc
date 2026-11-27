//
// gkdp.cc
//
// Gim  -  Generic Information Manager static library
// Version 2.5-x
// AsYntote
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%201.0?by=version]
//
/*#############################################################################

	Copyright (C) 2002-2026 
	Danilo Zannoni (AsYntote)

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

#include <gim/gim.h>

#define	GKDP_MAJOR			0
#define GKDP_MINOR			3
#define	GKDP_SUBMINOR		1
#define	GKDP_VERSION		"0.3-1"

#define GKDP_DEBUG			__GIM_YES
#define __GIM_KAOS_TRSHLD	55

#define _LINE				puts("")

/* quiet: no banner and no status; set by the -q flag or by gkdp::quiet in the Gim configuration */
static int g_quiet = 0;
#ifdef _WIN32
#define GKDP_NULL " >NUL 2>&1"
#else
#define GKDP_NULL " >/dev/null 2>&1"
#endif


#ifndef _WIN32
#include <sys/wait.h>
#endif

/* gimstat returns its GKaos score (0..100) as exit code. -1 means not evaluable (file too small): on Windows system() gives
   the exit code as it is, on POSIX it is packed in bits 8..15. Anything outside 0..100 is returned as -1. */
static int gkaos_score( int rc ) {
#ifdef _WIN32
	int s = rc;
#else
	int s = WIFEXITED( rc ) ? WEXITSTATUS( rc ) : -1;
#endif
	return ( s < 0 || s > 100 ) ? -1 : s;
}

void hand_shake( void ) {
	if ( !g_quiet ) printf( "gkdp creator V%d.%d\n" , GKDP_MAJOR , GKDP_MINOR );
	if ( !g_quiet ) _LINE;
	if ( !g_quiet ) puts(	"gKript data package generator" );
	if ( !g_quiet ) printf( "Powered by: %s\n" , gim_version_small() );
	if ( !g_quiet ) puts(	"Coded by gKript.org" );
	if ( !g_quiet ) puts(	"	!asyntote" );
	if ( !g_quiet ) puts(	"	!skymatrix" );
	_LINE;
}


void usage( void ) {
	puts(	"Usage:" );
	puts( 	"  gkdp  GKP_file_name  path  [password]" );
	_LINE;
}


int main ( int argc , char *argv[] ) {
	_gim_flag	mode = __GIM_NO;
	int stat=0 , score=-1;
	
	if ( gim_check_version(2,8,7) != __GIM_OK ) {
		_LINE;
		printf( "%s\n\n" , gim_version() );
		printf( "Gim is not updated at the required version.\n" );
		printf( "For %s is necessary Gim >= 2.8-7\n" , argv[0] );
		exit( -1 );
	}
		
	
	gim_set_application_name( "Gkdp_creator" );
	gim_set_application_version( GKDP_VERSION );
	gim_obj			* gim  = new gim_obj;
	/* options through the Gim scanner (gim_getopt_obj). -q makes this call quiet. The scanner puts the options
	   first: the positional arguments then start at optind */
	gim_getopt_obj * gkopt = new gim_getopt_obj;
	gkopt->scanopt( argc , argv , ":q" );
	if ( gkopt->search( 'q' ) == __GIM_YES )
		g_quiet = 1;
	char * pos_argv[ 8 ];
	int pos_argc = 1 , k;
	pos_argv[ 0 ] = argv[ 0 ];
	for ( k = optind ; k < argc && pos_argc < 7 ; k++ )
		pos_argv[ pos_argc++ ] = argv[ k ];
	pos_argv[ pos_argc ] = NULL;
	if ( ( pos_argc < 3 ) || ( pos_argc > 4 ) ) {
		gim_error->app( GIM_ERROR_WARNING , "gkdp" , "wrong number of arguments (%d): gkdp GKP_file_name path [password]" , pos_argc - 1 );
			usage();
		exit( -1 );
	}
	gim_gkdp_obj	* gkdp = new gim_gkdp_obj;
	
	_gim_string	gkp_file_name( pos_argv[1] );
	_gim_string	source( pos_argv[2] );
	_gim_string	passw;
	_gim_string	gkp_system( "gimstat " );
	_gim_flag	skip_stat = __GIM_NO;
	
	IF_EXIST_KEY( gim->conf , "gkdp" , "skip_gimstat" )
		skip_stat = gim->conf->GetKeyFLAG( "gkdp" , "skip_gimstat" );

	IF_EXIST_KEY( gim->conf , "gkdp" , "quiet" )
		if ( gim->conf->GetKeyFLAG( "gkdp" , "quiet" ) == __GIM_YES )
			g_quiet = 1;   /* the configuration can turn quiet on; -q already did, and never turns it off */

	hand_shake();   /* after the quiet setting is known */
	/* the log is always complete: -q silences only the screen */
	gim_error->app( GIM_ERROR_MESSAGE , "gkdp" , "start: %d argument(s), quiet %s, skip_gimstat %s" , pos_argc - 1 , g_quiet ? "YES" : "NO" , skip_stat == __GIM_YES ? "YES" : "NO" );

	if ( GKDP_DEBUG == __GIM_YES ) {
	   	gim->conf->ChangeKeyFlag( "debug", "f_debug" , __GIM_YES );
		gim->conf->AddKeyComment( "debug", "f_debug" , PRSR_AFTER , "Forced YES by main program" );
	}
  	gim->conf->ChangeKey( "crypt", "iterations" , 5 );
  	IF_NOT_EXIST_SECTION( gim->conf , "gkdp" )
  		gim->conf->AddSection( "gkdp" );
  	IF_NOT_EXIST_KEY( gim->conf , "gkdp" , "skip_gimstat" )
  		gim->conf->AddKeyFlag( "gkdp" , "skip_gimstat" , __GIM_NO );
  	IF_NOT_EXIST_KEY( gim->conf , "gkdp" , "quiet" )
  		gim->conf->AddKeyFlag( "gkdp" , "quiet" , __GIM_NO );
  	gim->conf->Write();

	if ( pos_argc > 1 ) {
	
		if ( pos_argc == 4 ) {
			passw.set( pos_argv[3] );
			gkdp->set_password( passw.c_str() );
			mode = __GIM_YES;
			if ( !g_quiet ) puts("Crypt activated");
		}
		if ( pos_argc == 3 ) {
			mode = __GIM_NO;
			if ( !g_quiet ) puts("Crypt NOT activated");
		}
	
		if ( !g_quiet ) printf("Generating %s..." , gkp_file_name.c_str() );
		fflush( stdout );
		gkdp->New( gkp_file_name.c_str() , mode );
		gkdp->set_prg( "Gkdp_creator" , 0 , 3 , 1 );
		gkdp->new_path( source.c_str() );
		gkdp->write();
		gim_error->app( GIM_ERROR_MESSAGE , "gkdp" , "package written: %s (source %s, crypt %s)" , gkp_file_name.c_str() , source.c_str() , mode == __GIM_YES ? "YES" : "NO" );
		if ( !g_quiet ) puts( "Done!\n");
		
		/* gimstat is a quality check of the encrypted content: a plain package is not checked, and skip_gimstat = YES skips it */
		bool do_stat = ( mode == __GIM_YES ) && ( skip_stat == __GIM_NO );
		if ( !do_stat )
			gim_error->app( GIM_ERROR_MESSAGE , "gkdp" , "gimstat not run: %s" , ( mode == __GIM_YES ) ? "skip_gimstat = YES" : "plain package, no check" );
		if ( do_stat ) {
			gkp_system.cat( gkp_file_name.c_str() );
		if ( g_quiet )
			gkp_system.cat( GKDP_NULL );
			stat = system( gkp_system.c_str() );
				score = gkaos_score( stat );
				gim_error->app( GIM_ERROR_MESSAGE , "gkdp" , "gimstat result: exit code %d, GKaos %d (threshold %d)" , stat , score , __GIM_KAOS_TRSHLD );
		}

//		printf("%d - %d\n" , stat , stat/256);
		if ( do_stat && score < 0 ) {
			gim_error->app( GIM_ERROR_MESSAGE , "gkdp" , "GKaos not evaluated: the package is too small for the analysis" );
			if ( !g_quiet ) puts( "GKaos: not evaluated (file too small)" );
		}
		if ( do_stat && score >= 0 && score < __GIM_KAOS_TRSHLD ) {
			gim_error->app( GIM_ERROR_WARNING , "gkdp" , "low gKaos score %d (< %d): the content could be violated, repeat with a password" , score , __GIM_KAOS_TRSHLD );
			if ( !g_quiet ) puts("\n  WARNING:");
			if ( !g_quiet ) puts("      The content of this file does not have a high level of gKaos and could be violated.\n      We recommend to repeat now this command with the encryption option activated simply typing a password after the file name and the path.");
			if ( !g_quiet ) puts( "\n          gkfp nome.gkp path [password]\n" );
		}
	}
	
	gim_error->app( GIM_ERROR_MESSAGE , "gkdp" , "end: exit 0" );
	delete gkdp;
	delete gim;
	
	return 0;
}
