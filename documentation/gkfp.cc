//
// gkfp.cc
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

#define	GKFP_MAJOR			0
#define GKFP_MINOR			3
#define	GKFP_SUBMINOR		1
#define	GKFP_VERSION		"0.3-1"

#define GKFP_DEBUG			__GIM_YES
#define __GIM_KAOS_TRSHLD	55

#define _LINE				puts("")

/* progress bar of the packing: one line, updated for each file, with percentage and estimated time left */
/* quiet: no status and no progress; set by the -q flag or by gkfp::quiet in the Gim configuration */
static int g_quiet = 0;
#ifdef _WIN32
#define GKFP_NULL " >NUL 2>&1"
#else
#define GKFP_NULL " >/dev/null 2>&1"
#endif

static void show_progress( int done , int total , const char * name ) {
	if ( g_quiet )
		return;
	static clock_t t0 = 0;
	char bar[31] , eta[16] = "--:--";
	int filled = ( total > 0 ) ? ( done * 30 / total ) : 30 , pct = ( total > 0 ) ? ( done * 100 / total ) : 100 , i;
	if ( done <= 1 )
		t0 = clock();
	if ( done > 0 && done < total ) {
		double elapsed = (double) ( clock() - t0 ) / CLOCKS_PER_SEC;
		long left = (long) ( elapsed / done * ( total - done ) + 0.5 );
		sprintf( eta , "%02ld:%02ld" , left / 60 , left % 60 );
	}
	for ( i = 0 ; i < 30 ; i++ )
		bar[ i ] = ( i < filled ) ? '#' : '-';
	bar[ 30 ] = 0;
	printf( "\r[%s] %3d%%  %4d/%-4d  eta %s  %-30.30s" , bar , pct , done , total , eta , name );
	if ( done >= total ) puts( "" );
	fflush( stdout );
}



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

static void usage_short( void ) {
	puts( "gkfp creator V0.3 usage :" );
	puts( "gkfp -c out.gkp path [password]  |  gkfp -x in.gkp [password]\n" );
	puts( "gkfp coded by AsYntote" );
	puts( "asyntote@gkript.org" );
	puts( "esco subito" );
}

int main ( int argc , char *argv[] ) {
	gim_set_application_name( "Gkfp" );
	gim_set_application_version( GKFP_VERSION );

	gim_obj		* gim = new gim_obj;

	/* options through the Gim scanner (gim_getopt_obj). -q makes this call quiet. The scanner puts the options
	   first: the positional arguments then start at optind */
	gim_getopt_obj * gkopt = new gim_getopt_obj;
	gkopt->scanopt( argc , argv , ":qcx" );
	if ( gkopt->search( 'q' ) == __GIM_YES )
		g_quiet = 1;
	char * pos_argv[ 8 ];
	int pos_argc = 1 , k;
	pos_argv[ 0 ] = argv[ 0 ];
	for ( k = optind ; k < argc && pos_argc < 7 ; k++ )
		pos_argv[ pos_argc++ ] = argv[ k ];
	pos_argv[ pos_argc ] = NULL;
	if ( pos_argc < 2 ) {
		usage_short();
		exit( -1 );
	}
	_gim_gkfp	* gkp = new _gim_gkfp;

	int stat=0 , score=-1;
	_gim_flag	skip_stat = __GIM_NO;
	
	_gim_string	gkp_file_name( pos_argv[1] );
	_gim_string	gkp_system( "gimstat " );
	
	if ( GKFP_DEBUG == __GIM_YES ) {
	   	gim->conf->ChangeKeyFlag( "debug", "f_debug" , __GIM_YES );
		gim->conf->AddKeyComment( "debug", "f_debug" , PRSR_AFTER , "Forced YES by main program" );
	}
  	gim->conf->ChangeKey( "crypt", "iterations" , 5 );
  	  	IF_NOT_EXIST_SECTION( gim->conf , "gkfp" )
  		gim->conf->AddSection( "gkfp" );
  	IF_NOT_EXIST_KEY( gim->conf , "gkfp" , "skip_gimstat" )
  		gim->conf->AddKeyFlag( "gkfp" , "skip_gimstat" , __GIM_NO );
  	IF_NOT_EXIST_KEY( gim->conf , "gkfp" , "quiet" )
  		gim->conf->AddKeyFlag( "gkfp" , "quiet" , __GIM_NO );
  	gim->conf->Write();

  	IF_EXIST_KEY( gim->conf , "gkfp" , "skip_gimstat" )
		skip_stat = gim->conf->GetKeyFLAG( "gkfp" , "skip_gimstat" );

	IF_EXIST_KEY( gim->conf , "gkfp" , "quiet" )
		if ( gim->conf->GetKeyFLAG( "gkfp" , "quiet" ) == __GIM_YES )
			g_quiet = 1;   /* the configuration can turn quiet on; -q never turns it off */
	
	/* the log is always complete: -q silences only the screen */
	gim_error->app( GIM_ERROR_MESSAGE , "gkfp" , "start: %d argument(s), quiet %s, skip_gimstat %s" , pos_argc - 1 , g_quiet ? "YES" : "NO" , skip_stat == __GIM_YES ? "YES" : "NO" );
	if ( ( gkopt->search( 'c' ) == __GIM_YES ) && ( gkopt->search( 'x' ) == __GIM_YES ) ) {
		gim_error->app( GIM_ERROR_WARNING , "gkfp" , "-c and -x together: not allowed" );
		usage_short();
		exit( -1 );
	}
	_gim_flag	want_create;
	if ( gkopt->search( 'c' ) == __GIM_YES )
		want_create = __GIM_YES;
	else if ( gkopt->search( 'x' ) == __GIM_YES )
		want_create = __GIM_NO;
	else {
		/* no flag: an existing package (as given, or with .gkp added) is extracted, otherwise a new one is created */
		_gim_string	with_ext( gkp_file_name.c_str() );
		if ( with_ext.find( ".gkp" ) == __GIM_NO )
			with_ext.cat( ".gkp" );
		if ( ( gim->file_manager->exist( gkp_file_name.c_str() ) == __GIM_EXIST ) || ( gim->file_manager->exist( with_ext.c_str() ) == __GIM_EXIST ) )
			want_create = __GIM_NO;
		else
			want_create = __GIM_YES;
	}
	if ( want_create == __GIM_NO ) {
		if ( ( gim->file_manager->exist( gkp_file_name.c_str() ) == __GIM_NOT_EXIST ) && ( gkp_file_name.find( ".gkp" ) == __GIM_NO ) )
			gkp_file_name.cat( ".gkp" );
	}
	gim_error->app( GIM_ERROR_MESSAGE , "gkfp" , "mode: %s , file %s" , want_create == __GIM_YES ? "create" : "extract" , gkp_file_name.c_str() );
	if ( want_create == __GIM_YES ) {
		
		if ( ( pos_argc < 3 ) || ( pos_argc > 4 ) ) {
			puts( "gkfp creator V0.3 usage :" );
			puts( "gkfp -c out.gkp path [password]  |  gkfp -x in.gkp [password]\n" );
			puts( "gkfp coded by AsYntote" );
			puts( "asyntote@gkript.org" );
			gim_error->app( GIM_ERROR_WARNING , "gkfp" , "create: missing arguments (need out.gkp and path)" );
			puts( "esco" );
			exit( -1 );
		}
		if ( !g_quiet ) puts( "Generating mode" );
		if ( gkp_file_name.find( ".gkp" ) == __GIM_NO )
			gkp_file_name.cat( ".gkp" );
		if ( pos_argc == 4 ) {
			gkp->New( gkp_file_name.c_str() , __GIM_YES , __GIM_FOG , __GIM_YES , __GIM_YES );
			gkp->set_password( pos_argv[3] );
			if ( !g_quiet ) puts("Crypt ACTIVATED");
		}
		else {
			gkp->New( gkp_file_name.c_str() , __GIM_YES , __GIM_SUN , __GIM_YES , __GIM_YES );
			if ( !g_quiet ) puts("Crypt NOT acivated");
		}
		if ( !g_quiet ) printf("Generating %s..." , gkp_file_name.c_str() );
		fflush( stdout );
		gkp->Set_prg( "Gkfp_creator" , 0 , 1 , 0 );
		gkp->New_path( pos_argv[2] );
		gim_gkfp_progress = show_progress;
		gkp->Write();
		gim_gkfp_progress = NULL;
		gim_error->app( GIM_ERROR_MESSAGE , "gkfp" , "package written: %s (source %s, crypt %s)" , gkp_file_name.c_str() , pos_argv[2] , pos_argc == 4 ? "YES" : "NO" );
			if ( !g_quiet ) puts( "Done!");
		/* gimstat is a quality check of the encrypted content: a plain package is not checked; gkfp::skip_gimstat = YES skips it */
		bool do_stat = ( pos_argc == 4 ) && ( skip_stat == __GIM_NO );
			if ( !do_stat )
				gim_error->app( GIM_ERROR_MESSAGE , "gkfp" , "gimstat not run: %s" , ( pos_argc == 4 ) ? "skip_gimstat = YES" : "plain package, no check" );
		if ( do_stat ) {
			gkp_system.cat( gkp_file_name.c_str() );
			if ( g_quiet )
				gkp_system.cat( GKFP_NULL );
			stat = system( gkp_system.c_str() );
				score = gkaos_score( stat );
				gim_error->app( GIM_ERROR_MESSAGE , "gkfp" , "gimstat result: exit code %d, GKaos %d (threshold %d)" , stat , score , 60 );
		}
//		printf("%d - %d\n" , stat , stat/256);
		if ( do_stat && score < 0 ) {
				gim_error->app( GIM_ERROR_MESSAGE , "gkfp" , "GKaos not evaluated: the package is too small for the analysis" );
				if ( !g_quiet ) puts( "GKaos: not evaluated (file too small)" );
			}
			if ( do_stat && score >= 0 && score < 60 ) {
			gim_error->app( GIM_ERROR_WARNING , "gkfp" , "low gKaos score %d (< %d): the content could be violated, repeat with a password" , score , 60 );
			if ( !g_quiet ) puts("WARNING:");
			if ( !g_quiet ) puts("The content of this file does not have a high level of gKaos and could be violated.\nWe recommend to repeat now this command with the encryption option activated simply typing a password after the file name and the path.");
			if ( !g_quiet ) puts( "\ngkfp nome.gkp path [password]\n" );
		}
	}
	else {
		if ( ( pos_argc < 2 ) || ( pos_argc > 3 ) ) {
			puts( "gkfp creator V0.3 usage :" );
			puts( "gkfp -c out.gkp path [password]  |  gkfp -x in.gkp [password]\n" );
			puts( "gkfp coded by AsYntote" );
			puts( "asyntote@gkript.org" );
			exit( -1 );
		}
		if ( !g_quiet ) puts( "Extracting mode\n" );
		if ( pos_argc == 3 ) {
			gkp->set_password( pos_argv[2] );
			if ( !g_quiet ) puts("Crypt ACTIVATED");
		}
		gkp->Extract( gkp_file_name.c_str() );
		gim_error->app( GIM_ERROR_MESSAGE , "gkfp" , "extracted: %s (crypt %s)" , gkp_file_name.c_str() , pos_argc == 3 ? "YES" : "NO" );
		if ( !g_quiet ) puts("\ndone!!!");
	}

	gim_error->app( GIM_ERROR_MESSAGE , "gkfp" , "end: exit 0" );
	delete gkp;
	delete gim;
	
	return 0;
}
