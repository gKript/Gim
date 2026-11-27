//
//  gim_platform_win32.cc
//
//  Gim - Generic Information Manager
//
//  [http://www.gkript.org]
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
//
//
// gim_platform_win32.cc
//
// Gim  -  Generic Information Manager static library
// Windows implementation of the POSIX replacements declared in gim_platform.h
//
// Released under the terms of the GNU General Public License

#if defined( _WIN32 )

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
/*!     \file       gim_platform_win32.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include <windows.h>
#include <lmcons.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "gim_platform.h"

/* ------------------------------------------------------------- identity */

static void gim_win_version( char * release , char * version , size_t n ) {
	typedef LONG ( WINAPI * rtl_get_version_t )( OSVERSIONINFOW * );
	OSVERSIONINFOW	vi;
	HMODULE			nt = GetModuleHandleA( "ntdll.dll" );
	rtl_get_version_t fn = nt ? (rtl_get_version_t) GetProcAddress( nt , "RtlGetVersion" ) : NULL;

	memset( &vi , 0 , sizeof( vi ) );
	vi.dwOSVersionInfoSize = sizeof( vi );
	if ( fn != NULL && fn( &vi ) == 0 ) {
		snprintf( release , n , "%lu.%lu.%lu" , vi.dwMajorVersion , vi.dwMinorVersion , vi.dwBuildNumber );
		snprintf( version , n , "build %lu" , vi.dwBuildNumber );
	} else {
		snprintf( release , n , "unknown" );
		snprintf( version , n , "unknown" );
	}
}

extern "C" int uname( struct utsname * u ) {
	DWORD		len = sizeof( u->nodename );
	SYSTEM_INFO	si;

	if ( u == NULL )
		return -1;
	memset( u , 0 , sizeof( *u ) );
	strcpy( u->sysname , "Windows" );
	if ( ! GetComputerNameExA( ComputerNameDnsHostname , u->nodename , &len ) )
		strcpy( u->nodename , "localhost" );
	gim_win_version( u->release , u->version , sizeof( u->release ) );
	GetNativeSystemInfo( &si );
	switch ( si.wProcessorArchitecture ) {
		case PROCESSOR_ARCHITECTURE_AMD64 :	strcpy( u->machine , "x86_64" );	break;
		case PROCESSOR_ARCHITECTURE_INTEL :	strcpy( u->machine , "i686" );		break;
		case PROCESSOR_ARCHITECTURE_ARM64 :	strcpy( u->machine , "aarch64" );	break;
		case PROCESSOR_ARCHITECTURE_ARM :	strcpy( u->machine , "arm" );		break;
		default :							strcpy( u->machine , "unknown" );
	}
	return 0;
}

extern "C" uid_t getuid( void ) {
	return 1000;
}

extern "C" struct passwd * getpwuid( uid_t uid ) {
	static struct passwd	pw;
	char					name[ UNLEN + 1 ];
	DWORD					len = sizeof( name );
	const char *			home;

	memset( &pw , 0 , sizeof( pw ) );
	pw.pw_uid = uid;
	if ( GetUserNameA( name , &len ) )
		strncpy( pw.pw_name , name , sizeof( pw.pw_name ) - 1 );
	else
		strcpy( pw.pw_name , "user" );
	strcpy( pw.pw_gecos , pw.pw_name );
	home = getenv( "USERPROFILE" );
	if ( home == NULL )
		home = getenv( "HOMEPATH" );
	strncpy( pw.pw_dir , ( home != NULL ) ? home : "." , sizeof( pw.pw_dir ) - 1 );
	/* Gim builds its paths with '/': use '/' in the home too (C:/Users/... instead of C:\Users\...) */
	for ( char * c = pw.pw_dir ; *c != '\0' ; c++ )
		if ( *c == '\\' )
			*c = '/';
	return &pw;
}

/* ------------------------------------------------------------ filesystem */

extern "C" int statvfs( const char * path , struct statvfs * buf ) {
	ULARGE_INTEGER	avail , total , freeb;

	if ( buf == NULL )
		return -1;
	memset( buf , 0 , sizeof( *buf ) );
	buf->f_namemax = 255;
	buf->f_bsize = 4096;
	if ( GetDiskFreeSpaceExA( path , &avail , &total , &freeb ) ) {
		buf->f_blocks = total.QuadPart / buf->f_bsize;
		buf->f_bfree  = freeb.QuadPart / buf->f_bsize;
		buf->f_bavail = avail.QuadPart / buf->f_bsize;
	}
	return 0;
}

/* -------------------------------------------------------- resource usage */

static long long gim_filetime_us( const FILETIME * ft ) {
	ULARGE_INTEGER u;
	u.LowPart  = ft->dwLowDateTime;
	u.HighPart = ft->dwHighDateTime;
	return (long long)( u.QuadPart / 10 );	/* 100ns -> us */
}

extern "C" int getrusage( int who , struct rusage * usage ) {
	FILETIME	create , exit_t , kernel , user;
	long long	u , k;

	if ( usage == NULL )
		return -1;
	memset( usage , 0 , sizeof( *usage ) );
	if ( ! GetProcessTimes( GetCurrentProcess() , &create , &exit_t , &kernel , &user ) )
		return -1;
	u = gim_filetime_us( &user );
	k = gim_filetime_us( &kernel );
	usage->ru_utime.tv_sec  = (long)( u / 1000000 );
	usage->ru_utime.tv_usec = (long)( u % 1000000 );
	usage->ru_stime.tv_sec  = (long)( k / 1000000 );
	usage->ru_stime.tv_usec = (long)( k % 1000000 );
	return 0;
}

/* ------------------------------------------------------------------ mmap */

extern "C" void * mmap( void * addr , size_t length , int prot , int flags , int fd , long long offset ) {
	HANDLE	file , mapping;
	void *	view;

	(void) addr; (void) prot; (void) flags;
	file = (HANDLE) _get_osfhandle( fd );
	if ( file == INVALID_HANDLE_VALUE )
		return MAP_FAILED;
	mapping = CreateFileMappingA( file , NULL , PAGE_READONLY , 0 , 0 , NULL );
	if ( mapping == NULL )
		return MAP_FAILED;
	view = MapViewOfFile( mapping , FILE_MAP_READ , (DWORD)( offset >> 32 ) , (DWORD)( offset & 0xffffffffu ) , length );
	CloseHandle( mapping );		/* the view keeps the mapping alive */
	return ( view != NULL ) ? view : MAP_FAILED;
}

extern "C" int munmap( void * addr , size_t length ) {
	(void) length;
	return UnmapViewOfFile( addr ) ? 0 : -1;
}

/* ----------------------------------------------------------- directories */

extern "C" int alphasort( const struct dirent ** a , const struct dirent ** b ) {
	return strcmp( (*a)->d_name , (*b)->d_name );
}

extern "C" int scandir( const char * dir , struct dirent *** namelist , int (*filter)( const struct dirent * ) , int (*compar)( const struct dirent ** , const struct dirent ** ) ) {
	char				pattern[ MAX_PATH * 2 ];
	size_t				len;
	WIN32_FIND_DATAA	fd;
	HANDLE				h;
	struct dirent **	list = NULL;
	int					count = 0 , cap = 0;

	len = strlen( dir );
	if ( len + 3 > sizeof( pattern ) )
		return -1;
	strcpy( pattern , dir );
	if ( len > 0 && pattern[ len - 1 ] != '/' && pattern[ len - 1 ] != '\\' )
		strcat( pattern , "\\" );
	strcat( pattern , "*" );

	h = FindFirstFileA( pattern , &fd );
	if ( h == INVALID_HANDLE_VALUE )
		return -1;
	do {
		struct dirent * e = (struct dirent *) malloc( sizeof( struct dirent ) );
		if ( e == NULL )
			break;
		memset( e , 0 , sizeof( *e ) );
		strncpy( e->d_name , fd.cFileName , sizeof( e->d_name ) - 1 );
		if ( fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT )
			e->d_type = DT_LNK;
		else if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			e->d_type = DT_DIR;
		else
			e->d_type = DT_REG;
		if ( filter != NULL && ! filter( e ) ) {
			free( e );
			continue;
		}
		if ( count == cap ) {
			int					ncap = cap ? cap * 2 : 32;
			struct dirent **	n = (struct dirent **) realloc( list , ncap * sizeof( *list ) );
			if ( n == NULL ) {
				free( e );
				break;
			}
			list = n;
			cap = ncap;
		}
		list[ count++ ] = e;
	} while ( FindNextFileA( h , &fd ) );
	FindClose( h );

	if ( compar != NULL && count > 1 )
		qsort( list , count , sizeof( *list ) , (int (*)( const void * , const void * )) compar );
	*namelist = list;
	return count;
}

/* --------------------------------------------------------- signals, tids */

extern "C" char * strsignal( int sig ) {
	switch ( sig ) {
		case SIGINT   : return (char *) "Interrupt";
		case SIGILL   : return (char *) "Illegal instruction";
		case SIGABRT  : return (char *) "Aborted";
		case SIGFPE   : return (char *) "Floating point exception";
		case SIGSEGV  : return (char *) "Segmentation fault";
		case SIGTERM  : return (char *) "Terminated";
		case SIGBREAK : return (char *) "Break";
		default       : return (char *) "Unknown signal";
	}
}

static const DWORD gim_main_thread_id = GetCurrentThreadId();

extern "C" int gim_is_main_thread( void ) {
	return ( GetCurrentThreadId() == gim_main_thread_id ) ? 1 : 0;
}

extern "C" long gim_gettid( void ) {
	return (long) GetCurrentThreadId();
}

extern "C" {
}

extern "C" long gethostid( void ) {
	DWORD serial = 0;
	if ( ! GetVolumeInformationA( "C:\\" , NULL , 0 , &serial , NULL , NULL , NULL , 0 ) )
		serial = 0;
	return (long) serial;
}


extern "C" int getloadavg( double loadavg[] , int nelem ) {
	int i;
	for ( i = 0 ; i < nelem ; i++ )
		loadavg[ i ] = 0.0;		/* not available on Windows */
	return nelem;
}

extern "C" char * strcasestr( const char * haystack , const char * needle ) {
	size_t n = strlen( needle );

	if ( n == 0 )
		return (char *) haystack;
	for ( ; *haystack ; haystack++ )
		if ( _strnicmp( haystack , needle , n ) == 0 )
			return (char *) haystack;
	return NULL;
}

/* ---------------------------------------------------------------- getopt */

extern "C" {

char *	gim_optarg = NULL;
int		gim_optind = 1;
int		gim_opterr = 1;
int		gim_optopt = 0;

static int gim_nextchar = 0;

static int gim_is_option( const char * a ) {
	return a != NULL && a[0] == '-' && a[1] != '\0';
}

int gim_getopt( int argc , char * const argv[] , const char * optstring ) {
	char **			av = (char **) argv;
	const char *	spec;
	int				quiet = ( optstring[0] == ':' );

	gim_optarg = NULL;
	if ( gim_nextchar == 0 ) {
		int i , j;
		/* permute: bring the next option in front of any non-option args */
		for ( i = gim_optind ; i < argc && ! gim_is_option( av[i] ) ; i++ )
			;
		if ( i >= argc )
			return -1;
		if ( strcmp( av[i] , "--" ) == 0 ) {
			gim_optind = i + 1;
			return -1;
		}
		if ( i != gim_optind ) {
			char * opt = av[i];
			for ( j = i ; j > gim_optind ; j-- )
				av[j] = av[j - 1];
			av[gim_optind] = opt;
		}
		gim_nextchar = 1;
	}
	gim_optopt = av[gim_optind][gim_nextchar++];
	spec = ( gim_optopt != ':' ) ? strchr( optstring , gim_optopt ) : NULL;
	if ( spec == NULL ) {
		if ( av[gim_optind][gim_nextchar] == '\0' ) {
			gim_optind++;
			gim_nextchar = 0;
		}
		if ( gim_opterr && ! quiet )
			fprintf( stderr , "%s: invalid option -- '%c'\n" , av[0] , gim_optopt );
		return '?';
	}
	if ( spec[1] == ':' ) {
		if ( av[gim_optind][gim_nextchar] != '\0' ) {
			gim_optarg = &av[gim_optind][gim_nextchar];
			gim_optind++;
		} else if ( gim_optind + 1 < argc ) {
			gim_optarg = av[gim_optind + 1];
			gim_optind += 2;
		} else {
			gim_optind++;
			gim_nextchar = 0;
			if ( gim_opterr && ! quiet )
				fprintf( stderr , "%s: option requires an argument -- '%c'\n" , av[0] , gim_optopt );
			return quiet ? ':' : '?';
		}
		gim_nextchar = 0;
	} else if ( av[gim_optind][gim_nextchar] == '\0' ) {
		gim_optind++;
		gim_nextchar = 0;
	}
	return gim_optopt;
}

}


extern "C" char * get_current_dir_name( void ) {
	return _getcwd( NULL , 0 );
}


/* ------------------------------------------------- host info (cpu / ram) */

extern "C" int gim_win_cpu_count( void ) {
	SYSTEM_INFO si;
	GetNativeSystemInfo( &si );
	return (int) si.dwNumberOfProcessors;
}

extern "C" int gim_win_cpu_info( int index , char * name , char * vendor , float * mhz , int * family , int * model , int * stepping ) {
	char	key[128] , ident[256] = "";
	HKEY	hk;
	DWORD	len , type , mhz_val = 0;
	char *	p;

	name[0] = vendor[0] = '\0';
	*mhz = 0; *family = *model = *stepping = 0;
	snprintf( key , sizeof( key ) , "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d" , index );
	if ( RegOpenKeyExA( HKEY_LOCAL_MACHINE , key , 0 , KEY_READ , &hk ) != ERROR_SUCCESS )
		return -1;
	len = 255; type = REG_SZ;
	if ( RegQueryValueExA( hk , "ProcessorNameString" , NULL , &type , (LPBYTE) name , &len ) == ERROR_SUCCESS )
		name[ len < 255 ? len : 255 ] = '\0';
	len = 255;
	if ( RegQueryValueExA( hk , "VendorIdentifier" , NULL , &type , (LPBYTE) vendor , &len ) == ERROR_SUCCESS )
		vendor[ len < 255 ? len : 255 ] = '\0';
	len = sizeof( mhz_val );
	if ( RegQueryValueExA( hk , "~MHz" , NULL , &type , (LPBYTE) &mhz_val , &len ) == ERROR_SUCCESS )
		*mhz = (float) mhz_val;
	len = 255;
	if ( RegQueryValueExA( hk , "Identifier" , NULL , &type , (LPBYTE) ident , &len ) == ERROR_SUCCESS ) {
		ident[ len < 255 ? len : 255 ] = '\0';
		if ( ( p = strstr( ident , "Family " ) ) != NULL )		*family = atoi( p + 7 );
		if ( ( p = strstr( ident , "Model " ) ) != NULL )		*model = atoi( p + 6 );
		if ( ( p = strstr( ident , "Stepping " ) ) != NULL )	*stepping = atoi( p + 9 );
	}
	RegCloseKey( hk );
	/* trim leading spaces of the brand string */
	for ( p = name ; *p == ' ' ; p++ )
		;
	if ( p != name )
		memmove( name , p , strlen( p ) + 1 );
	return 0;
}

extern "C" int gim_win_mem_info( unsigned long long * total , unsigned long long * avail ) {
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof( ms );
	if ( ! GlobalMemoryStatusEx( &ms ) )
		return -1;
	*total = ms.ullTotalPhys;
	*avail = ms.ullAvailPhys;
	return 0;
}

/* size in KB of the largest processor cache (usually L3, else L2): the Linux "cache size" */
extern "C" int gim_win_cache_kb( void ) {
	DWORD	len = 0;
	int		best = 0;
	GetLogicalProcessorInformation( NULL , &len );
	if ( len == 0 )
		return 0;
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buf = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *) malloc( len );
	if ( buf == NULL )
		return 0;
	if ( GetLogicalProcessorInformation( buf , &len ) ) {
		DWORD n = len / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
		for ( DWORD i = 0 ; i < n ; i++ ) {
			if ( buf[ i ].Relationship == RelationCache && (int)( buf[ i ].Cache.Size / 1024 ) > best )
				best = (int)( buf[ i ].Cache.Size / 1024 );
		}
	}
	free( buf );
	return best;
}

#endif /* _WIN32 */
