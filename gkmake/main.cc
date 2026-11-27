/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cc
 * Copyright (C) 2017-2026 asyntote <asyntote@viprox>
 * 
 * gKmake is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gKmake is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gkm_main.h"

_gim_flag				do_system = __GIM_YES;
_gim_flag				install = __GIM_NO;
_gim_flag				autotools = __GIM_NO;
_gim_flag				writec = __GIM_YES;

gkmake_st				gkmake;
old_gkmake_st			gkmake_old;
_gim_Uint32				running_time;

gim_ascii_file_obj *	header;

//!< Build directory of the current project: the section named after the project in ~/.gim/gKmake/build.conf (per machine).
/* Build directory per project: ~/.gim/gKmake/build.conf, one section per project name (as in .gkmake_conf),
   key dir. Read lazily, on the first build step, when the project name is known. A project with no
   entry uses build-<host> in the project root: one folder per machine, never a shared "build" folder,
   so a CMake cache written on another PC (or under another drive letter) is never reused. */
static char build_dir[BUFF_DIM] = "";
static int  build_dir_loaded = 0;
static int  build_dir_is_default = 0;	//!< 1 when build_dir is the default build-<host>, owned by gkmake
static int  build_dir_quiet = 0;		//!< 1 with -q: the status report says it, not ensure_build_dir()

/* A folder inside the project root is stored relative to it (gkmake always runs from the root), so the
   entry survives a change of drive letter or of the folder that holds the projects. A folder outside
   the root is kept as given. Separators become '/'; on Windows the comparison ignores case. */
static void relative_to_root( const char * dir , char * out , size_t len ) {
	gim_utils_obj	utils;
	char			root[BUFF_DIM] , d[BUFF_DIM];
	snprintf( d , sizeof( d ) , "%s" , dir );
	Lexical.char_subst( d , '\\' , '/' );
	snprintf( out , len , "%s" , d );
	snprintf( root , sizeof( root ) , "%s" , utils.pwd() );		// pwd(): static buffer, gkmake is single-threaded
	Lexical.char_subst( root , '\\' , '/' );
	size_t n = strlen( root );
	if ( ( n == 0 ) || ( n + 1 >= sizeof( root ) ) )
		return;
	if ( root[ n - 1 ] != '/' ) {		// the prefix must end on a folder boundary
		root[ n++ ] = '/';
		root[ n ] = '\0';
	}
	if ( strlen( d ) <= n )			// the root itself, or shorter: keep it as given
		return;
	char * r_cmp = root;
	char * d_cmp = d;
#ifdef _WIN32
	r_cmp = Lexical.str_down( root , strlen( root ) );	// Windows paths ignore case
	d_cmp = Lexical.str_down( d , strlen( d ) );
#endif
	if ( utils.str_subtraction( d_cmp , r_cmp ) == d_cmp + n )	// d starts with the root
		snprintf( out , len , "%s" , d + n );
#ifdef _WIN32
	free( r_cmp );		// str_down returns a strdup() copy
	free( d_cmp );
#endif
}

static bool is_absolute( const char * p ) {
	return ( p[ 0 ] == '/' ) || ( p[ 0 ] == '\\' ) || ( p[ 0 ] && ( p[ 1 ] == ':' ) );
}

/* -K <folder>: records the build folder of the current project in ~/.gim/gKmake/build.conf, written by the Gim parser (Lex B) */
static void save_build_dir( const char * given ) {
	char path[BUFF_DIM] , section[BUFF_DIM] , dir[BUFF_DIM];
	relative_to_root( given , dir , sizeof( dir ) );
	_gim_prsr * bc = new _gim_prsr;
	sprintf( path , "%s/.gim/gKmake/build.conf" , gim_identity->home() );
	strncpy( section , gkmake.prj_name , BUFF_DIM - 1 );
	section[ BUFF_DIM - 1 ] = '\0';
	for ( char * q = section ; *q ; q++ )
		*q = (char) toupper( (unsigned char) *q );
	bc->SetLex( __LEX_B );
	if ( bc->Read( path ) != __GIM_OK ) {
		bc->Up( path , "gKmake build folders" );
		bc->SetLex( __LEX_B );
	}
	if ( bc->ExistSection( section ) != __GIM_EXIST )
		bc->AddSection( section );
	_gim_flag	set_rc;
	if ( bc->ExistKey( section , "dir" ) == __GIM_EXIST )
		set_rc = bc->ChangeKey( section , "dir" , dir );
	else
		set_rc = bc->AddKey( section , "dir" , dir );
	if ( set_rc != __GIM_OK ) {
		/* the parser refuses the value (e.g. a string value is limited to 63 characters): stop, do not build in the old folder */
		printf( "\n  ERROR: the build folder '%s' was not saved in %s:\n" , dir , path );
		printf( "         the configuration parser refused it (string values must be shorter than 64 characters).\n" );
		printf( "         A folder inside the project is saved as a short relative path.\n\n" );
		delete bc;
		exit( 1 );
	}
	if ( bc->Write() != __GIM_OK )
		printf( "  ERROR: cannot write %s\n" , path );
	else {
		printf( "  build folder of project '%s' saved in %s: %s\n" , gkmake.prj_name , path , dir );
		if ( is_absolute( dir ) )
			printf( "  note: the folder is outside the project, so it is saved as an absolute path (not portable)\n" );
	}
	delete bc;
}
static void ensure_build_dir( void ) {
	char path[BUFF_DIM];
	char section[BUFF_DIM];
	_gim_prsr * bc;
	if ( build_dir_loaded )
		return;
	build_dir_loaded = 1;
	sprintf( path , "%s/.gim/gKmake/build.conf" , gim_identity->home() );
	strncpy( section , gkmake.prj_name , BUFF_DIM - 1 );
	section[ BUFF_DIM - 1 ] = '\0';
	bc = new _gim_prsr;
	for ( char * p = section ; *p ; p++ )
		*p = (char) toupper( (unsigned char) *p );
	bc->SetLex( __LEX_B );
	/* build.conf is created by the first -K: when it does not exist there is nothing to read (and the parser
	   would log an error at every run) */
	if ( ( ! gkmake_exist( path ) ) || ( bc->Read( path ) != __GIM_OK ) || ( bc->ExistSection( section ) != __GIM_EXIST ) || ( bc->ExistKey( section , "dir" ) != __GIM_EXIST ) ) {
		/* no entry: default build-<host>, relative to the project root (gkmake runs there); -K overrides it */
		char host[BUFF_DIM] = "local";
		const char * node = gim_identity->node();
		if ( ( node != NULL ) && ( node[ 0 ] != '\0' ) ) {
			char * low = Lexical.str_down( node , strlen( node ) );
			snprintf( host , sizeof( host ) , "%s" , low );
			free( low );						// str_down returns a strdup() copy
			Lexical.char_subst( host , '.' , '-' );		// e.g. "proxima.lan" -> "proxima-lan"
			Lexical.char_subst( host , ' ' , '-' );
		}
		snprintf( build_dir , BUFF_DIM , "build-%s" , host );
		build_dir_is_default = 1;
		if ( ! build_dir_quiet )
			printf( "  build folder of project '%s': %s (default for this machine, -K to change it)\n" , gkmake.prj_name , build_dir );
		delete bc;
		return;
	}
	strncpy( build_dir , bc->GetKeySTR( section , "dir" ) , BUFF_DIM - 1 );
	build_dir[ BUFF_DIM - 1 ] = '\0';
	delete bc;
}

static int build_cache_exists( void ) {
	ensure_build_dir();
	char p[BUFF_DIM];
	sprintf( p , "%s/CMakeCache.txt" , build_dir );
	return gkmake_exist( p );
}

static const char * cmake_build_cmd( void ) {
	ensure_build_dir();
	static char c[BUFF_DIM];
	sprintf( c , "cmake --build \"%s\"" , build_dir );
	return c;
}

static const char * cmake_clean_cmd( void ) {
	ensure_build_dir();
	static char c[BUFF_DIM];
	sprintf( c , "cmake --build \"%s\" --target clean" , build_dir );
	return c;
}

static const char * cmake_rm_cmd( void ) {
	ensure_build_dir();
	static char c[BUFF_DIM];
	sprintf( c , "cmake -E rm -rf \"%s\"" , build_dir );
	return c;
}

static const char * cmake_install_cmd( void ) {
	ensure_build_dir();
	static char c[BUFF_DIM];
	sprintf( c , "cmake --install \"%s\"" , build_dir );
	return c;
}

/* 1 when the program `name` can be run (it answers --version): used for cmake and ninja */
static int tool_found( const char * name ) {
	char cmd[BUFF_DIM];
#ifdef _WIN32
	snprintf( cmd , sizeof( cmd ) , "%s --version >NUL 2>&1" , name );
#else
	snprintf( cmd , sizeof( cmd ) , "%s --version >/dev/null 2>&1" , name );
#endif
	return system( cmd ) == 0;
}

/* Generator of a new build folder: Ninja when it is installed (the Gim builds use it); otherwise CMake's
   default (on Windows that is NMake, which needs Visual Studio). */
static const char * cmake_generator( void ) {
	return tool_found( "ninja" ) ? " -G Ninja" : "";
}

/* Two folder names are the same folder: '\' and '/' are equal, a trailing '/' does not count, and on Windows
   the case is ignored. */
static int same_folder( const char * a , const char * b ) {
	char x[BUFF_DIM] , y[BUFF_DIM];
	snprintf( x , sizeof( x ) , "%s" , a );
	snprintf( y , sizeof( y ) , "%s" , b );
	Lexical.char_subst( x , '\\' , '/' );
	Lexical.char_subst( y , '\\' , '/' );
	for ( size_t n = strlen( x ) ; ( n > 1 ) && ( x[ n - 1 ] == '/' ) ; n-- )
		x[ n - 1 ] = '\0';
	for ( size_t n = strlen( y ) ; ( n > 1 ) && ( y[ n - 1 ] == '/' ) ; n-- )
		y[ n - 1 ] = '\0';
#ifdef _WIN32
	char * lx = Lexical.str_down( x , strlen( x ) );
	char * ly = Lexical.str_down( y , strlen( y ) );
	_gim_flag same = Lexical.str_equal( lx , ly );
	free( lx );							// str_down returns a strdup() copy
	free( ly );
	return same == __GIM_YES;
#else
	return Lexical.str_equal( x , y ) == __GIM_YES;
#endif
}

/* Value of a key of the CMake cache of the build folder, e.g. key "CMAKE_HOME_DIRECTORY:INTERNAL=": the text
   up to the end of the line, in out. Returns 1 when the cache exists and has the key (a key starts a line). */
static int cmake_cache_value( const char * key , char * out , size_t len ) {
	char cache[BUFF_DIM];
	out[ 0 ] = '\0';
	sprintf( cache , "%s/CMakeCache.txt" , build_dir );
	if ( ! gkmake_exist( cache ) )
		return 0;
	_gim_file	f( cache , __GIM_FILE_POINTER , __GIM_READ );
	char *		buf = f.load();			// ends with '\0' (gim_file_obj::load)
	int			found = 0;
	if ( buf != NULL ) {
		/* standard library: Gim has no substring search on a plain buffer (gim_ascii_file_obj is incomplete, issue #4) */
		for ( char * p = strstr( buf , key ) ; p != NULL ; p = strstr( p + 1 , key ) ) {
			if ( ( p == buf ) || ( p[ -1 ] == '\n' ) ) {
				p += strlen( key );
				snprintf( out , len , "%.*s" , (int) strcspn( p , "\r\n" ) , p );
				found = 1;
				break;
			}
		}
	}
	f.close();							// frees the loaded buffer: no unload()
	return found;
}

static const char * cmake_configure_cmd( void ) {
	ensure_build_dir();
	static char c[BUFF_DIM];
	sprintf( c , "cmake -S . -B \"%s\"%s -DCMAKE_BUILD_TYPE=Release" , build_dir , cmake_generator() );
	return c;
}
//!< Removes the CMake cache of the build folder (CMakeCache.txt and CMakeFiles): the next step configures again.
static void remove_build_cache( void ) {
	char rm[BUFF_DIM * 2];
	sprintf( rm , "cmake -E rm -rf \"%s/CMakeCache.txt\" \"%s/CMakeFiles\"" , build_dir , build_dir );
	system( rm );
}

/* The CMake cache of the build folder must belong to this source tree: a cache written for another path
   (another PC, another drive letter) makes every cmake call fail. The default folder build-<host> is
   gkmake's own: its cache is removed and configured again. A folder chosen with -K is not touched: stop. */
static void check_build_cache( void ) {
	static int checked = 0;
	if ( checked )
		return;
	checked = 1;
	if ( ! build_cache_exists() )
		return;
	gim_utils_obj	utils;
	char			source[BUFF_DIM] , here[BUFF_DIM];
	if ( ! cmake_cache_value( "CMAKE_HOME_DIRECTORY:INTERNAL=" , source , sizeof( source ) ) || ( source[ 0 ] == '\0' ) )
		return;							// no source folder recorded: let cmake decide
	snprintf( here , sizeof( here ) , "%s" , utils.pwd() );
	if ( same_folder( here , source ) )
		return;
	Lexical.char_subst( here , '\\' , '/' );		// for the messages below
	Lexical.char_subst( source , '\\' , '/' );
	if ( build_dir_is_default ) {
		printf( "  the CMake cache in %s was written for %s:\n  it is removed and the folder is configured again\n\n" , build_dir , source );
		remove_build_cache();
		return;
	}
	printf( "\n  ERROR: the CMake cache in %s was written for another source folder:\n         %s\n" , build_dir , source );
	printf( "         This project is %s. Choose another folder with -K, or remove the cache by hand.\n\n" , here );
	exit( 1 );
}

#ifdef _WIN32
	#define GKM_EXE		".exe"
	#define GKM_PATHSEP	";"
#else
	#define GKM_EXE		""
	#define GKM_PATHSEP	":"
#endif

/* -q: where things are and what does not add up, in one report. Nothing is changed or built.
   Returns the number of problems found (0 = all fine). */
static int status_report( void ) {
	gim_utils_obj	utils;
	char			here[BUFF_DIM] , value[BUFF_DIM] , copy_dir[BUFF_DIM] = "" , generator[BUFF_DIM] = "" , build_abs[BUFF_DIM];
	int				problems = 0;
	int				use_cmake = gkmake_exist( "./CMakeLists.txt" );

	build_dir_quiet = 1;
	ensure_build_dir();
	snprintf( here , sizeof( here ) , "%s" , utils.pwd() );
	Lexical.char_subst( here , '\\' , '/' );
	if ( is_absolute( build_dir ) )
		snprintf( build_abs , sizeof( build_abs ) , "%s" , build_dir );
	else
		snprintf( build_abs , sizeof( build_abs ) , "%s/%s" , here , build_dir );

	puts( "\n  GKMAKE STATUS\n" );
	printf( "  Machine             : %s (user %s)\n" , gim_identity->node() , gim_identity->login() );
	printf( "  gkmake              : %s\n" , GKMAKE_VERSION );
	printf( "  Project             : %s\n" , gkmake.prj_name );
	printf( "  Project folder      : %s\n" , here );
	printf( "  Build system        : %s\n" , use_cmake ? "CMake" : ( gkmake_exist( "./Makefile" ) ? "make" : "none found" ) );
	printf( "  Clean before build  : %s\n" , ( gkmake.clean == __GIM_YES ) ? "YES (saved setting: \"gkmake -C NO\" for an incremental build)" : "NO (incremental build)" );

	if ( use_cmake ) {
		const int cm = tool_found( "cmake" ) , nj = tool_found( "ninja" );
		printf( "  cmake / ninja       : %s / %s\n" , cm ? "found" : "NOT FOUND" , nj ? "found" : "not found" );
		if ( ! cm ) {
			problems++;
			puts( "  !  cmake is not in the PATH: gkmake cannot build this project." );
		}
		printf( "  Build folder        : %s (%s)\n" , build_dir , build_dir_is_default ? "default for this machine" : "chosen with -K, in ~/.gim/gKmake/build.conf" );
		if ( ! cmake_cache_value( "CMAKE_HOME_DIRECTORY:INTERNAL=" , value , sizeof( value ) ) ) {
			puts( "  CMake cache         : none yet: the first build configures the folder" );
		}
		else {
			Lexical.char_subst( value , '\\' , '/' );
			if ( same_folder( here , value ) )
				printf( "  CMake cache         : ok (source folder %s)\n" , value );
			else {
				problems++;
				printf( "  CMake cache         : written for another folder: %s\n" , value );
				puts( build_dir_is_default ? "  !  the next build removes the cache and configures the folder again." :
											 "  !  the build stops: choose another folder with -K, or remove the cache by hand." );
			}
			if ( cmake_cache_value( "CMAKE_GENERATOR:INTERNAL=" , generator , sizeof( generator ) ) ) {
				printf( "  Generator           : %s\n" , generator );
				if ( ( ! nj ) && ( Lexical.str_equal( generator , "Ninja" ) == __GIM_YES ) ) {
					problems++;
					puts( "  !  the cache uses Ninja, which is not in the PATH." );
				}
			}
			/* the tools copy folder exists only in projects that define it (Gim): generic projects skip these lines */
			if ( cmake_cache_value( "GIM_GKMAKE_COPY_DIR:PATH=" , copy_dir , sizeof( copy_dir ) ) ) {
				Lexical.char_subst( copy_dir , '\\' , '/' );
				if ( copy_dir[ 0 ] == '\0' )
					puts( "  Tools copy folder   : none (copy disabled)" );
				else {
					printf( "  Tools copy folder   : %s\n" , copy_dir );
					if ( same_folder( copy_dir , build_abs ) ) {
						problems++;
						puts( "  !  the copy folder is the build folder itself: the copy is skipped." );
					}
				}
			}
		}
	}

	/* PATH: standard library (getenv, strtok). Gim has no access to the environment, and Lexical.tokenizer is
	   made for "key = value" lines: it would alter the paths. */
	const char * env = getenv( "PATH" );
	int in_path = 0 , found = 0;
	char first[BUFF_DIM] = "";
	char listed[16][BUFF_DIM];			// folders already listed: a folder repeated in the PATH counts once
	if ( env != NULL ) {
		char * list = strdup( env );
		for ( char * dir = strtok( list , GKM_PATHSEP ) ; dir != NULL ; dir = strtok( NULL , GKM_PATHSEP ) ) {
			char exe[BUFF_DIM];
			if ( ( copy_dir[ 0 ] != '\0' ) && same_folder( dir , copy_dir ) )
				in_path = 1;
			snprintf( exe , sizeof( exe ) , "%s/gkmake%s" , dir , GKM_EXE );
			if ( ! gkmake_exist( exe ) )
				continue;
			int repeated = 0;
			for ( int k = 0 ; ( k < found ) && ( k < 16 ) ; k++ )
				if ( same_folder( listed[ k ] , dir ) )
					repeated = 1;
			if ( repeated )
				continue;
			if ( found < 16 )
				snprintf( listed[ found ] , BUFF_DIM , "%s" , dir );
			found++;
			if ( found == 1 )
				snprintf( first , sizeof( first ) , "%s" , dir );
			printf( "  gkmake in PATH      : %d. %s%s\n" , found , dir , ( found == 1 ) ? "   <- the one that runs" : "" );
		}
		free( list );
	}
	if ( found == 0 )
		puts( "  gkmake in PATH      : none" );
	else if ( found > 1 )
		puts( "  (more than one gkmake in the PATH: the first one runs; the others are not updated by the build)" );
	if ( copy_dir[ 0 ] != '\0' ) {
		printf( "  Copy folder in PATH : %s\n" , in_path ? "yes" : "NO" );
		if ( ! in_path ) {
			problems++;
			puts( "  !  the tools are copied there, but that folder is not in the PATH: they are not found." );
		}
		else if ( ( found > 0 ) && ( ! same_folder( first , copy_dir ) ) ) {
			problems++;
			puts( "  !  the gkmake that runs is not the one in the copy folder: a newer build is not used." );
		}
	}

	if ( problems == 0 )
		puts( "\n  No problems found.\n" );
	else
		printf( "\n  %d problem(s) found.\n\n" , problems );
	return problems;
}

//!< Clean the build tree: cmake target clean (only if configured) or make clean.
static void gk_clean( bool cmake ) {
	if ( cmake ) {
		check_build_cache();
		if ( build_cache_exists() )
			system( cmake_clean_cmd() );
	}
	else
		system( "make clean" );
}

int main( int argc , char *argv[] ) {
	int ciclo = 1, feedback = 0 , makedoc = __GIM_NO;
	time_t	tp;
	char cur_path[BUFF_DIM];
	char cur_gkmake_file[BUFF_DIM];
	char gkmake_makefile[BUFF_DIM];
	char cur_old_gkmake_file[BUFF_DIM];
	char command_t1[BUFF_DIM];
	char command[BUFF_DIM];
	char include_gkmake_file[BUFF_DIM];
	char thread_command[16];

	memset( cur_gkmake_file		, 0, BUFF_DIM );
	memset( command				, 0, BUFF_DIM );
	memset( command_t1			, 0, BUFF_DIM );
	memset( gkmake_makefile		, 0, BUFF_DIM );
	memset( include_gkmake_file , 0, BUFF_DIM );

	sprintf ( cur_gkmake_file, "%s/%s", get_current_dir_name(), GKMAKE_FILE_COUNTER );
	sprintf ( gkmake_makefile, "%s/%s", get_current_dir_name(), MAKEFILE );
	sprintf ( cur_path, "%s" , get_current_dir_name() );
	
	if ( gim_check_version(2,5,0) != __GIM_OK ) {
		printf( "%s\n\n" , gim_version() );
		printf( "Gim is not updated at the required version.\n" );
		printf( "For %s is necessary Gim >= 2.5-0\n" , argv[0] );
		exit( -1 );
	}

	hand_shake();

	gim_set_application_name( "gKmake" );
	gim_set_application_version( GKMAKE_VERSION );
	gim_obj * gim = new gim_obj;
	_gim_time * runT = new _gim_time;
	prsr_lexical_class * lex = new prsr_lexical_class;
	runT->start();
	
	gim->error->set( "main" , "gkmake start here" );

	gim_getopt_obj * gkopt = new gim_getopt_obj;
	gkopt->scanopt( argc , argv , ":RN:A:P:I:S:D:F:X:M:H:B:T:C:E:t:K:coeshidanzq" );

	// CMake is preferred whenever CMakeLists.txt exists (build tree in ./build)
		const bool use_cmake = ( gkmake_exist( "./CMakeLists.txt" ) != 0 );

		if ( ( ! use_cmake ) && ( ! gkmake_exist( "./Makefile" ) ) && ( gkopt->search( 'a' ) == __GIM_NO ) ) {
		if ( ( ( gkmake_exist( "./autogen.sh" ) ) || ( gkmake_exist( "./configure" ) ) ) && ( gkmake_exist( "./Makefile.in" ) ) ) {
			puts( "  No Makefile found, but...\n" );
			if ( gkmake_exist( "./autogen.sh" ) )
				puts( "  \"autogen.sh\" found" );
			else
				puts( "  \"autogen.sh\" NOT found" );					
			if ( gkmake_exist( "./configure" ) )
				puts( "  \"configure\" found" );
			else
				puts( "  \"configure\" NOT found" );
			if ( gkmake_exist( "./Makefile.in" ) )
				puts( "  \"Makefile.in\" found" );
			else
				puts( "  \"Makefile.in\" NOT found" );
			puts( "\n  Try to run gkmake with \'-a\' option\n" );
		}
		else
		    puts( "  No Makefile found.\n  This one not seems a project directory\n" );
		delete runT;
		delete lex;
		delete gkopt;
		delete gim;
		exit( -1 );
	}
	
	if ( gkopt->errors_present() == __GIM_YES ) {
		puts( "  Errors on option:" );
		GIM_GETOPT_ERROR( gkopt ) {
			if ( gkopt->geterror()->status == __GIM_OPT_NO_ARG )
				printf( "   A mandatory argument for otion \033[1m-%c\033[0m is missing\n" , gkopt->geterror()->option  );
			if ( gkopt->geterror()->status == __GIM_OPT_UNKNOWN )
				printf( "   Option \033[1m-%c\033[0m is unknown\n" , gkopt->geterror()->option );
			if ( gkopt->geterror()->status == __GIM_OPT_NOT_OK )
				printf( "   Some error in \033[1m-%c\033[0m option\n" , gkopt->geterror()->option );
		}
		puts("");
		print_help();
		delete runT;
		delete lex;
		delete gkopt;
		delete gim;
		exit( __GIM_ERROR );
	}

	if ( ( gkopt->opt_members() ) && \
	   ( ( gkopt->search( 'N' ) == __GIM_YES ) || \
	     ( gkopt->search( 'A' ) == __GIM_YES ) || \
	     ( gkopt->search( 'P' ) == __GIM_YES ) ) ) {
		remove( cur_gkmake_file );
	}
	else {
		if ( ! gkmake_exist( ".gkmake_conf" ) ) {
			writec = __GIM_NO;
		}
	}
	
	_gim_prsr * gkconf = new _gim_prsr;
	header = new gim_ascii_file_obj;
	header->set_dimension( 20 * __GIM_KB ); 
	
	if ( gkconf->Read( cur_gkmake_file ) != __GIM_OK ) {
//		puts(" NON Letto !");
		gkconf->Up( cur_gkmake_file , "gKmake configuration file" );
		gkconf->SetLex( __LEX_C );
		gkconf->AddSection( gim->identity->login() );
		gkconf->AddKey( gim->identity->login() , "coder" , gim->identity->login() );
		gkconf->AddKey( gim->identity->login() , "host" , gim->identity->node() );
		gkconf->AddKey( gim->identity->login() , "architecture" , gim->identity->arch() );
		gkconf->AddKey( gim->identity->login() , "gim_rel" , gim_version_mini() );
		gkconf->AddKey( gim->identity->login() , "gkmake_rel" , GKMAKE_VERSION );
	}
	else {
		gkconf->SetLex( __LEX_C );
		strcpy( gkmake.login		, gim->identity->login() );
		strcpy( gkmake.node			, gim->identity->node() );
//		strcpy( gkmake.arch			, gim->identity->arch() );
//		strcpy( gkmake.gim_rel		, gim_version_mini() );
		strcpy( gkmake.gkmake_rel	, GKMAKE_VERSION );
		
		IF_EXIST_SECTION( gkconf , gkmake.login ) {
			gkmake.coder_exist = __GIM_EXIST;
			gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Coder found: %s" , gkmake.login );
		}
		else {
			gkmake.coder_exist = __GIM_NOT_EXIST;
			gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Coder NOT FOUND: %s" , gkmake.login );
			gkconf->AddSection( gkmake.login );
			gkconf->AddKey( gim->identity->login() , "coder" , gkmake.login );
			gkconf->AddKey( gim->identity->login() , "host" , gkmake.node );
			gkconf->AddKey( gim->identity->login() , "architecture" , gkmake.arch );
			gkconf->AddKey( gim->identity->login() , "gim_rel" , gkmake.gim_rel );
			gkconf->AddKey( gim->identity->login() , "gkmake_rel" , gkmake.gkmake_rel );
			gkconf->AddKey( gim->identity->login() , "total_build_number" , 0 );
			gkconf->AddKey( gim->identity->login() , "succesful_build_number" , 0 );
			gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Coder added to the list file" );
			puts  ( "  WARNING" );
			printf( "    Hey %s, you were added into the configuration file \".gkmake_conf\" as new coder.\n" , gkmake.login );
			printf( "    You can proceed in your gkmake personalization starting by typing \"gkmake -h\". \n" );
			printf( "    Enjoy your coding time!\n\n" );
			do_system = __GIM_NO;
			printf( "    Run again to make the project.\n\n" );
		}
		
		for ( _gim_Uint8 s = 0 ; s < gkconf->GetHowManySection() ; s++ ) {
			char sectname[64];
			__GIM_CLEAR( sectname , 64 , char );
//			strcpy( gkmake.prj_name , gkconf->GetSectionName( s ) );
			strcpy( sectname , gkconf->GetSectionName( s ) );
//			printf( "working on section: %s\n" , sectname );

			_gim_Uint8 c = 0, count = gkconf->GetHowManyKey( sectname );
			for ( c = 0 ; c < count ; c++ ) {
				char	tmpkeyname[64];
				__GIM_CLEAR( tmpkeyname , 64 , char );
				strcpy( tmpkeyname , gkconf->GetKeyName( sectname , c ) );
				
/*STRINGS */	if ( lex->str_equal( "prefix" , tmpkeyname ) == __GIM_YES ) {
					strcpy( gkmake.prefix , gkconf->GetKeySTR( sectname , "prefix" ) );
					gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.prefix );
				} 
				else if ( lex->str_equal( "project_name" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.prj_name , gkconf->GetKeySTR( sectname , "project_name" ) );
					gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.prj_name );
				} 
				else if ( lex->str_equal( "author" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.author , gkconf->GetKeySTR( sectname , "author" ) );
					gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.author );
				} 
				else if ( lex->str_equal( "include_path" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.include_path , gkconf->GetKeySTR( sectname , "include_path" ) );
					gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.include_path );
				} 
				else if ( lex->str_equal( "gim_rel" , tmpkeyname ) == __GIM_YES ) { 
					if ( lex->str_equal( sectname , Lexical.str_up( gkmake.login , strlen( gkmake.login ) ) ) == __GIM_YES ) {
						strcpy( gkmake.gim_rel , gkconf->GetKeySTR( sectname , "gim_rel" ) );
						gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.gim_rel );
						if ( lex->str_equal( gkmake.gim_rel , gim_version_mini() ) == __GIM_NO ) {
							strcpy( gkmake.gim_rel , gim_version_mini() );
							gkconf->ChangeKey( sectname , "gim_rel" , gkmake.gim_rel );
							gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "gim release updated to: %s" ,gkmake.gim_rel );
						}
					}
				} 
				else if ( lex->str_equal( "gkmake_rel" , tmpkeyname ) == __GIM_YES ) { 
					if ( lex->str_equal( sectname , Lexical.str_up( gkmake.login , strlen( gkmake.login ) ) ) == __GIM_YES ) {
						strcpy( gkmake.gkmake_rel , gkconf->GetKeySTR( sectname , "gkmake_rel" ) );
						gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.gkmake_rel );
						if ( lex->str_equal( gkmake.gkmake_rel , GKMAKE_VERSION ) == __GIM_NO ) {
							strcpy( gkmake.gkmake_rel , GKMAKE_VERSION );
							gkconf->ChangeKey( sectname , "gkmake_rel" , gkmake.gkmake_rel );
							gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "gkmake release updated to: %s" ,gkmake.gkmake_rel );
						}
					}
				} 
				else if ( lex->str_equal( "documentation_path" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.documentation , gkconf->GetKeySTR( sectname , "documentation_path" ) );
					gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.documentation );
				} 
				else if ( lex->str_equal( "documentation_command" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.doc_command , gkconf->GetKeySTR( sectname , "documentation_command" ) );
					gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.doc_command );
				} 
				else if ( lex->str_equal( "editor_command" , tmpkeyname ) == __GIM_YES ) {
					if ( lex->str_equal( sectname , Lexical.str_up( gkmake.login , strlen( gkmake.login ) ) ) == __GIM_YES ) {
						strcpy( gkmake.editor_command , gkconf->GetKeySTR( sectname , "editor_command" ) );
						gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.editor_command );
					}
				}
				else if ( lex->str_equal( "header_file_name" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.header_file_name , gkconf->GetKeySTR( sectname , "header_file_name" ) );
					gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.header_file_name);
				}
/*FLAGS */		else if ( lex->str_equal( "clean" , tmpkeyname ) == __GIM_YES ) {
					if ( lex->str_equal( sectname , Lexical.str_up( gkmake.login , strlen( gkmake.login ) ) ) == __GIM_YES ) {
						gkmake.clean = gkconf->GetKeyFLAG( sectname , "clean" ); 
						gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.clean );
					}
				}
				else if ( lex->str_equal( "use_sudo" , tmpkeyname ) == __GIM_YES ) {
					if ( lex->str_equal( sectname , Lexical.str_up( gkmake.login , strlen( gkmake.login ) ) ) == __GIM_YES ) { 
						gkmake.sudo = gkconf->GetKeyFLAG( sectname , "use_sudo" ); 
						gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.sudo );
					}
				}
				else if ( lex->str_equal( "project_header_generation" , tmpkeyname ) == __GIM_YES ) { 
					gkmake.header_file = gkconf->GetKeyFLAG( sectname , "project_header_generation" );
					gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.header_file );
				} 
				else if ( lex->str_equal( "multi_threads_enable" , tmpkeyname ) == __GIM_YES ) {
			 		if ( lex->str_equal( sectname , Lexical.str_up( gkmake.login , strlen( gkmake.login ) ) ) == __GIM_YES ) { 
						gkmake.parallel = gkconf->GetKeyFLAG( sectname , "multi_threads_enable" ); 
						gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.parallel );
						if ( gkmake.parallel == __GIM_YES ) {
							IF_EXIST_KEY( gkconf , sectname , "number_of_threads" ) {
								gkmake.num_core = gkconf->GetKeyINT( sectname , "number_of_threads" );
								gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key number_of_threads - Value %d" , sectname , gkmake.num_core );
								if ( gkmake.num_core == 0 )
									gkmake.num_core = gim->identity->n_proc() + 1;
							}
							else {
								gkmake.num_core = gim->identity->n_proc() + 1;
								IF_NOT_EXIST_KEY( gkconf , sectname , "number_of_threads" )
									gkconf->AddKey( sectname , "number_of_threads" , gkmake.num_core );
							}
						}
					}
				}
/*INT VALUES */	else if ( lex->str_equal( "total_build_number" , tmpkeyname ) == __GIM_YES ) {
			 		if ( lex->str_equal( sectname , Lexical.str_up( gkmake.login , strlen( gkmake.login ) ) ) == __GIM_YES ) {
						gkmake.tot_build = gkconf->GetKeyINT( sectname , "total_build_number" );
						gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.tot_build );
					}
					gkmake.prj_sum_total_build += gkconf->GetKeyINT( sectname , "total_build_number" );
				} 
				else if ( lex->str_equal( "succesful_build_number" , tmpkeyname ) == __GIM_YES ) { 
			 		if ( lex->str_equal( sectname , Lexical.str_up( gkmake.login , strlen( gkmake.login ) ) ) == __GIM_YES ) {
						gkmake.ok_build = gkconf->GetKeyINT( sectname , "succesful_build_number" ); 
						gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main" , "Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.ok_build );
					}
					gkmake.prj_sum_ok_build += gkconf->GetKeyINT( sectname , "succesful_build_number" );
				}
			}
			
		}
	}
	gkopt->rewind();
	
	GIM_GETOPT_OK( gkopt ) {
		switch ( gkopt->getoption()->option ) {
			case 'N' : {
				if ( !gkopt->search( 'A' )  || !gkopt->search( 'P' ) ) {
					puts( "  Errors on option:" );
					puts( "    If you want to set a new project you have to set at least also the Author and the Prefix\n" );
					print_help();
					gkconf->Down();
					delete header;
					delete gkconf;
					delete runT;
					delete lex;
					delete gkopt;
					delete gim;
					exit( __GIM_ERROR );
				}
				strcpy( gkmake.prj_name , gkopt->getoption()->argument );
				gkconf->AddSection( gkmake.prj_name );
				gkconf->AddSectionComment( gkmake.prj_name , PRSR_BEFORE , "This is the Project name" );
				gkconf->AddKey( gkmake.prj_name , "project_name" , gkmake.prj_name );
				break;
			}
			case 'A' : {
				if ( !gkopt->search( 'N' ) || !gkopt->search( 'P' ) ) {
					puts( "  Errors on option:" );
					puts( "   If you want to set a new project you have to set at least also the Project name and the Prefix" );
					print_help();
					gkconf->Down();
					delete header;
					delete gkconf;
					delete runT;
					delete lex;
					delete gkopt;
					delete gim;
					exit( __GIM_ERROR );
				}
				strcpy( gkmake.author , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "author" )
						gkconf->AddKey( gkmake.prj_name , "author" , gkmake.author );
				}
				break;
			}
			case 'P' : {
				if ( !gkopt->search( 'N' ) || !gkopt->search( 'A' ) ) {
					puts( "  Errors on option:" );
					puts( "   If you want to set a new project you have to set at least also the Project name and the Author" );
					print_help();
					gkconf->Down();
					delete header;
					delete gkconf;
					delete runT;
					delete lex;
					delete gkopt;
					delete gim;
					exit( __GIM_ERROR );
				}
				strcpy( gkmake.prefix , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "prefix" ) 
						gkconf->AddKey( gkmake.prj_name , "prefix" , gkmake.prefix );
				}
				break;
			}
			case 'I' : {
				strcpy( gkmake.include_path , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "include_path" )
						gkconf->AddKey( gkmake.prj_name , "include_path" , gkmake.include_path );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "include_path" )
						gkconf->ChangeKey( gkmake.prj_name , "include_path" , gkmake.include_path );
				}
				break;
			}
			case 'D' : {
				strcpy( gkmake.documentation , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_path" )
						gkconf->AddKey( gkmake.prj_name , "documentation_path" , gkmake.documentation );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_path" )
						gkconf->ChangeKey( gkmake.prj_name , "documentation_path" , gkmake.documentation );
				}
				break;
			}
			case 'E' : {
				strcpy( gkmake.editor_command , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "editor_command" )
						gkconf->AddKey( gim->identity->login() , "editor_command" , gkmake.editor_command );
					IF_EXIST_KEY( gkconf , gim->identity->login() , "editor_command" )
						gkconf->ChangeKey( gim->identity->login() , "editor_command" , gkmake.editor_command );
				}
				break;
			}
			case 'X' : {
				strcpy( gkmake.doc_command , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_command" )
						gkconf->AddKey( gkmake.prj_name , "documentation_command" , gkmake.doc_command );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_command" )
						gkconf->ChangeKey( gkmake.prj_name , "documentation_command" , gkmake.doc_command );
				}
				break;
			}
			case 'S' : {
				if ( lex->str_equal( "YES" , gkopt->getoption()->argument ) == __GIM_YES )
					gkmake.sudo = __GIM_YES;
				else 
					gkmake.sudo = __GIM_NO;
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "use_sudo" )
						gkconf->AddKeyFlag( gim->identity->login() , "use_sudo" , gkmake.sudo );
				}
				break;
			}
			case 'C' : {
				if ( lex->str_equal( "YES" , gkopt->getoption()->argument ) == __GIM_YES )
					gkmake.clean = __GIM_YES;
				else 
					gkmake.clean = __GIM_NO;
				
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "clean" ) 
						gkconf->AddKeyFlag( gim->identity->login() , "clean" , gkmake.clean );
					IF_EXIST_KEY( gkconf , gim->identity->login() , "clean" )
						gkconf->ChangeKeyFlag( gim->identity->login() , "clean" , gkmake.clean );
				}
				break;
			}
			case 'H' : {
				if ( lex->str_equal( "YES" , gkopt->getoption()->argument ) == __GIM_YES )
					gkmake.header_file = __GIM_YES;
				else 
					gkmake.header_file = __GIM_NO;
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "project_header_generation" ) 
						gkconf->AddKeyFlag( gkmake.prj_name , "project_header_generation" , gkmake.header_file );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "project_header_generation" )
						gkconf->ChangeKeyFlag( gkmake.prj_name , "project_header_generation" , gkmake.header_file );
				}
				break; 
			}
			case 'F' : {
				strcpy( gkmake.header_file_name , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "header_file_name" )
						gkconf->AddKey( gkmake.prj_name , "header_file_name" , gkmake.header_file_name );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "header_file_name" )
						gkconf->ChangeKey( gkmake.prj_name , "header_file_name" , gkmake.header_file_name );
				}
				break;
			}
			case 'T' : {
				gkmake.parallel = __GIM_YES;
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "multi_threads_enable" ) {
						gkconf->AddKeyFlag( gim->identity->login() , "multi_threads_enable" , gkmake.parallel );
					}
				}
				gkmake.num_core = atoi( gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "number_of_threads" ) {
						gkconf->AddKey( gim->identity->login() , "number_of_threads" , gkmake.num_core );
					}
				}
				break;
			}
			case 'M' : {
				gkmake.tot_build = atoi( gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "total_build_number" ) 
						gkconf->AddKey( gim->identity->login() , "total_build_number" , gkmake.tot_build );
				}
				break;
			}
			case 'K' : {
				save_build_dir( gkopt->getoption()->argument );
				break;
			}
			case 'B' : {
				gkmake.ok_build = atoi( gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "succesful_build_number" ) 
						gkconf->AddKey( gim->identity->login() , "succesful_build_number" , gkmake.ok_build );
				}
				break;
			}
			case 't' : {
				if( ! strncmp( gkopt->getoption()->argument , "auto" , 4 ) )
					gkmake.num_core = gim->identity->n_proc();
				else	 
					gkmake.num_core = atoi( gkopt->getoption()->argument );
				sprintf( thread_command , " -j%d" , gkmake.num_core );
				gkmake.parallel = __GIM_YES;
				printf( "  MULTI THREADS build activated (%d threads)...\n" , gkmake.num_core );
				break;
			}
			case 'c' : {
				puts( "  CLEANING NOW...\n");
				if ( do_system == __GIM_YES )
					gk_clean( use_cmake );
				puts("");
				break;
			}
			case 'n' : {
				puts( "  DISTCLEAN FOR THE PROJECT NOW...\n");
				if ( do_system == __GIM_YES )
					system( use_cmake ? cmake_rm_cmd() : "make distclean" );
				puts("");
				do_system = __GIM_NO;
				writec = __GIM_NO;
				break;
			}
			case 'd' : {
				makedoc = __GIM_YES;
				break;
			}
			case 'h' : {
				do_system = __GIM_NO;
				writec = __GIM_NO;
				print_help();
				break;
			}
			case 'q' : {
				do_system = __GIM_NO;
				writec = __GIM_NO;
				exit( status_report() ? 1 : 0 );		// 0 = no problems, 1 = problems found
			}
			case 'a' : {
				writec = __GIM_NO;
				puts( "  SERCHING FOR AUTOTOOLS...\n");
				if ( gkmake_exist( "./autogen.sh" ) )
					puts( "  \"autogen.sh\" found" );
				else
					puts( "  \"autogen.sh\" NOT found" );					
				if ( gkmake_exist( "./configure" ) )
					puts( "  \"configure\" found" );
				else
					puts( "  \"configure\" NOT found" );
				if ( gkmake_exist( "./Makefile.in" ) )
					puts( "  \"Makefile.in\" found" );
				else
					puts( "  \"Makefile.in\" NOT found" );

				if ( ( ( gkmake_exist( "./autogen.sh" ) ) || ( gkmake_exist( "./configure" ) ) ) && ( gkmake_exist( "./Makefile.in" ) ) ) {
					puts( "  \nOK, I TRY TO CONFIGURE THIS PROJECT...\n" );
					if ( gkmake_exist( "./autogen.sh" ) )
						system( "./autogen.sh" );
					else if ( gkmake_exist( "./configure" ) )
						system( "./configure" );
					
				}
				else 
					puts( "\n  INCOMPLETE AUTOTOOLS" );
					gkconf->Down();
					delete header;
					delete gkconf;
					delete runT;
					delete lex;
					delete gkopt;
					delete gim;
					exit( 0 );
			}			
			case 'z' : {
				writec = __GIM_NO;
				puts( "\n  BUILD DISTRIBUITION PACKAGE..." );
				if ( ! system( "make" ) ) {
					system( "make dist" );
				}
				if ( runT->get() > 2 ) {
					printf( "\n  GKMAKE ran for %d seconds\n" , runT->get() );
					printf(   "  GKMAKE built by %s@%s on %s\n" , gim->identity->login() , gim->identity->node() , gkmake.last_build );
				}
				puts("");
				gkconf->Down();
				delete header;
				delete gkconf;
				delete runT;
				delete lex;
				delete gkopt;
				delete gim;
				exit( 0 );
			}
			case 'o' : {
				if ( gkmake_exist( ".gkmake_conf" ) ) {
					puts( "\n  PRINT CONFIGURATION FILE...\n" );
					puts( gkconf->WriteOnBuffer() );
					puts("");
				}
				else {
					puts("\n  ERROR: configuration file not found. Project not yet configured");
				}
				gkconf->Down();
				delete header;
				delete gkconf;
				delete runT;
				delete lex;
				delete gkopt;
				delete gim;
				exit( 0 );
			}
			case 'e' : {
				char mess[64];
				writec = __GIM_NO;
				do_system = __GIM_NO;
				IF_EXIST_SECTION( gkconf , gkmake.login ) {
					IF_EXIST_KEY( gkconf , gkmake.login , "editor_command" ) {
						puts( "\n  EDIT CONFIGURATION FILE...\n" );
						sprintf( mess, "%s .gkmake_conf" , gkmake.editor_command ); 
						system( mess );
					}
					else
						puts("\n  ERROR: No editor command configured");
				}
				gkconf->Down();
				delete header;
				delete gkconf;
				delete runT;
				delete lex;
				delete gkopt;
				delete gim;
				exit( 0 );
			}
			case 'i' : {
				install = __GIM_YES;
				if ( gkmake.sudo == __GIM_YES ) 
					sprintf( command_t1 , "sudo %s" , use_cmake ? cmake_install_cmd() : "make install" );
				else
					sprintf( command_t1 , "%s" , use_cmake ? cmake_install_cmd() : "make install" );
				break;
			}
		}		
	}
	
//	gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::gkmake_status" , "  gkmake struct dumped to std out" );
//	gkmake_status();	

	if ( ( gkopt->opt_members() ) && \
	   ( ( gkopt->search( 'N' ) == __GIM_YES ) || \
	     ( gkopt->search( 'A' ) == __GIM_YES ) || \
	     ( gkopt->search( 'P' ) == __GIM_YES ) || \
	     ( gkopt->search( 'I' ) == __GIM_YES ) || \
	     ( gkopt->search( 'D' ) == __GIM_YES ) || \
	     ( gkopt->search( 'S' ) == __GIM_YES ) || \
	     ( gkopt->search( 'C' ) == __GIM_YES ) || \
		 ( gkopt->search( 'H' ) == __GIM_YES ) || \
		 ( gkopt->search( 'F' ) == __GIM_YES ) || \
		 ( gkopt->search( 'E' ) == __GIM_YES ) || \
		 ( gkopt->search( 'T' ) == __GIM_YES ) || \
	     ( gkopt->search( 'a' ) == __GIM_YES ) ) ) {

		do_system = __GIM_NO;

	if ( writec == __GIM_YES )
		gkconf->Write();
		puts( "  New configuration file wrote\n" );
	}

	time( &tp );
	strftime( gkmake.last_build , 256 , "%A %d-%m-%Y %H:%M:%S" , localtime ( &tp ) );


	_gim_flag   stop = __GIM_YES;
	
	if ( do_system == __GIM_YES ) {
		if ( gkmake.clean == __GIM_YES ) {
			/* the saved setting 'clean = YES' (set with -C YES) is the only reason for a clean here: say it */
			puts( "  CLEANING NOW (saved setting 'clean = YES': \"gkmake -C NO\" builds incrementally)...\n");
			gk_clean( use_cmake );
			puts("");
		}
	}
	
	if ( use_cmake && ( do_system == __GIM_YES ) )
		check_build_cache();
	if ( use_cmake && ( do_system == __GIM_YES ) && ( ! build_cache_exists() ) ) {
			puts( "  CONFIGURING WITH CMAKE...\n" );
			if ( system( cmake_configure_cmd() ) ) {
				/* a failed configure leaves an incomplete CMakeCache.txt: remove it, or the next run would not configure again */
				remove_build_cache();
				printf( "\n  ERROR: CMake configuration of '%s' failed (see the messages above).\n\n" , build_dir );
				exit( 1 );
			}
		}
		sprintf( command , "%s" , use_cmake ? cmake_build_cmd() : GKMAKE_COMMAND );

	if ( gkmake.parallel == __GIM_YES ) {
		char tmp[16];
		sprintf( tmp , use_cmake ? " --parallel %d" : " -j%d" , gkmake.num_core );
		strcat( command , tmp );
	}

	if ( do_system == __GIM_YES ) {
		if ( writec == __GIM_YES ) {
			IF_EXIST_KEY( gkconf , gkmake.login , "succesful_build_number" )
				{ gkmake.tot_build++; gkmake.prj_sum_total_build++; printf( "  BUILDING (#%d) THE PROJECT ( %s )...\n\n" , gkmake.prj_sum_total_build , command ); }
			else
				printf( "  BUILDING THE PROJECT ( %s )...\n\n" , command ); 
		}
		else 
			printf( "  BUILDING THE PROJECT ( %s )...\n\n" , command );
		
		if ( ! system( command ) ) {
			IF_EXIST_KEY( gkconf , gkmake.login , "succesful_build_number" ) { 
				gkconf->ChangeKey( gkmake.login , "succesful_build_number" , ++gkmake.ok_build );
				gkmake.prj_sum_ok_build++;
				gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "Upgrading Succesfull Build to %d" , gkmake.ok_build );
			}
			else 
				gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "Succesfull Build NOT FOUND!" );
			if ( writec == __GIM_YES ) {
				printf( "\n  BUILT SUCCESFULLY ( %d )\n" , gkmake.ok_build );
				printf( "      Your personal succesful build rate is: %3d%%\n"   ,  (_gim_Uint8)( ( (float)((float)gkmake.ok_build / (float)gkmake.tot_build ) * 100 ) ) );
				printf( "      Total Project succesful build rate is: %3d%%\n\n" ,  (_gim_Uint8)( ( (float)((float)gkmake.prj_sum_ok_build / (float)gkmake.prj_sum_total_build ) * 100 ) ) );
			}
			else
				printf( "\n  BUILT SUCCESFULLY\n" );
		}
		else
			do_system = __GIM_NO;
	}
	IF_EXIST_KEY( gkconf , gkmake.login , "total_build_number" ) {
		gkconf->ChangeKey( gkmake.login , "total_build_number" , gkmake.tot_build );
		gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "Upgrading Total Build to %d" , gkmake.tot_build );
	}
	else 
		gim->error->app( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "Total Build NOT FOUND!" );
	
	if ( writec == __GIM_YES )
		gkconf->Write();
	
	if ( do_system == __GIM_YES ) {
		if ( install == __GIM_YES ) {
			printf( "\n  INSTALLING THE PROJECT  ( %s )...\n\n" , command_t1 );
		if ( system( command_t1 ) ) 
			printf( "\n  ERROR INSTALLING THE PROJECT...\n\n" );
		}
	}


	if ( do_system == __GIM_YES ) {
		if ( makedoc == __GIM_YES ) {
			IF_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_path" ) { 
				IF_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_command" ) {
					puts( "\n  GENERATING DOCUMENTATION...\n\n" );
					if ( chdir( gkconf->GetKeySTR( gkmake.prj_name , "documentation_path" ) ) ) {
						puts( "\n  ERROR : Some error with the Documentation path...\n\n" );
					}
					else {
						const char * doc_cmd = gkconf->GetKeySTR( gkmake.prj_name , "documentation_command" );
#ifdef _WIN32
							// make_doc.sh is a shell script: on Windows run make_doc.bat, which does the same steps
							if ( gkmake_exist( "./make_doc.bat" ) )
								doc_cmd = ".\\make_doc.bat";
#endif
							system( doc_cmd );
						chdir( cur_path );
					}
				}
			}
		}
	}

	if ( ( gkmake.header_file == __GIM_YES ) && ( do_system == __GIM_YES ) ) {
		_gim_handler * headf;
		if ( chdir( gkconf->GetKeySTR( gkmake.prj_name , "include_path" ) ) ) {
			puts( "\n  ERROR : Some error with the include path...\n\n" );
		}
		else {
			printf( "\n  CREATING HEADER FILE ( %s%s )...\n\n" , gkmake.include_path , gkmake.header_file_name );
			make_header();
			headf = gim->file_manager->open( gkmake.header_file_name , __GIM_FILE_POINTER , __GIM_WRITE );
			fputs( header->get_buffer() , headf->fp );
			gim->file_manager->close( headf );
			chdir( cur_path );
		}
	}

	
	if ( runT->get() > 2 ) {
		printf( "\n  GKMAKE ran for %d seconds\n" , runT->get() );
		printf(   "  GKMAKE built by %s@%s on %s\n" , gim->identity->login() , gim->identity->node() , gkmake.last_build );
	}
	puts("");

	gkconf->Down();
	delete header;
	delete gkconf;
	delete runT;
	delete lex;
	delete gkopt;
	delete gim;

	exit( feedback );

}



