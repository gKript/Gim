//
// gim_platform.h
//
// Gim  -  Generic Information Manager static library
// Platform abstraction layer: on POSIX systems it just pulls in the native
// headers, on Windows it provides small replacements for the POSIX calls
// used by the library (mmap, uname, getpwuid, statvfs, getrusage, scandir...)
//
// Released under the terms of the GNU General Public License

#ifndef _GIM_PLATFORM_H
#define _GIM_PLATFORM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef environ
	#undef environ
#endif

#if defined( _WIN32 )

	#define GIM_PLATFORM_WINDOWS	1

	#include <io.h>
	#include <direct.h>
	#include <process.h>
	#include <sys/time.h>

	#ifdef __cplusplus
	extern "C" {
	#endif

	/* ---- file mode helpers -------------------------------------------- */
	#ifndef S_ISDIR
		#define S_ISDIR(m)	(((m) & _S_IFMT) == _S_IFDIR)
	#endif
	#ifndef S_ISREG
		#define S_ISREG(m)	(((m) & _S_IFMT) == _S_IFREG)
	#endif
	#ifndef S_ISCHR
		#define S_ISCHR(m)	(((m) & _S_IFMT) == _S_IFCHR)
	#endif
	#ifndef S_ISBLK
		#define S_ISBLK(m)	(0)
	#endif
	#ifndef S_ISLNK
		#define S_ISLNK(m)	(0)
	#endif

	#define lstat(path,buf)		stat( (path) , (buf) )
	#define mkdir(path,mode)	_mkdir( (path) )
	#define chmod(path,mode)	_chmod( (path) , (mode) & ( _S_IREAD | _S_IWRITE ) )
	#ifndef O_BINARY
		#define O_BINARY 0x8000
	#endif

	/* ---- identity ------------------------------------------------------ */
	typedef int	uid_t;

	struct utsname {
		char	sysname		[ 65 ];
		char	nodename	[ 65 ];
		char	release		[ 65 ];
		char	version		[ 65 ];
		char	machine		[ 65 ];
	};
	int		uname( struct utsname * u );

	struct passwd {
		char	pw_name		[ 256 ];
		char	pw_dir		[ 512 ];
		char	pw_gecos	[ 256 ];
		int		pw_uid;
	};
	uid_t			getuid( void );
	struct passwd *	getpwuid( uid_t uid );

	/* ---- filesystem info ------------------------------------------------ */
	struct statvfs {
		unsigned long	f_namemax;
		unsigned long long	f_bsize;
		unsigned long long	f_blocks;
		unsigned long long	f_bfree;
		unsigned long long	f_bavail;
	};
	int		statvfs( const char * path , struct statvfs * buf );

	/* ---- resource usage ------------------------------------------------- */
	#define RUSAGE_SELF	0
	struct rusage {
		struct timeval	ru_utime;
		struct timeval	ru_stime;
	};
	int		getrusage( int who , struct rusage * usage );

	/* ---- memory mapped files -------------------------------------------- */
	#define PROT_READ		1
	#define MAP_PRIVATE		2
	#define MAP_FAILED		( (void *) -1 )
	void *	mmap( void * addr , size_t length , int prot , int flags , int fd , long long offset );
	int		munmap( void * addr , size_t length );

	/* ---- directories ---------------------------------------------------- */
	#define DT_UNKNOWN	0
	#define DT_DIR		4
	#define DT_REG		8
	#define DT_BLK		6
	#define DT_CHR		2
	#define DT_FIFO		1
	#define DT_SOCK		12
	#define DT_LNK		10
	struct dirent {
		unsigned char	d_type;
		char			d_name[ 260 ];
	};
	int		scandir( const char * dir , struct dirent *** namelist , int (*filter)( const struct dirent * ) , int (*compar)( const struct dirent ** , const struct dirent ** ) );
	int		alphasort( const struct dirent ** a , const struct dirent ** b );

	/* ---- signals / threads ---------------------------------------------- */
	#ifndef SIGQUIT
		#define SIGQUIT	SIGBREAK
	#endif
	#ifndef SIGKILL
		#define SIGKILL	SIGTERM
	#endif
	char *	strsignal( int sig );

	long	gim_gettid( void );
	int	gim_is_main_thread( void );   /* TRUE on the thread that started the process */
	long	gethostid( void );

	/* ---- misc ------------------------------------------------------------ */
	int		getloadavg( double loadavg[] , int nelem );
	char *	get_current_dir_name( void );

	/* ---- host information (replaces /proc/cpuinfo and /proc/meminfo) ---- */
	int		gim_win_cpu_count( void );
	int		gim_win_cpu_info( int index , char * name , char * vendor , float * mhz , int * family , int * model , int * stepping );
	int		gim_win_mem_info( unsigned long long * total , unsigned long long * avail );
	int		gim_win_cache_kb( void );
	char *	strcasestr( const char * haystack , const char * needle );

	/* ---- getopt (short options, GNU-like argument permutation) ----------- */
	extern char *	gim_optarg;
	extern int		gim_optind , gim_opterr , gim_optopt;
	int				gim_getopt( int argc , char * const argv[] , const char * optstring );
	#define optarg	gim_optarg
	#define optind	gim_optind
	#define opterr	gim_opterr
	#define optopt	gim_optopt
	#define getopt	gim_getopt
	#define	gim_getpid()	( (long) _getpid() )

	#ifdef __cplusplus
	}
	#endif

#else /* ------------------------------------------------------------ POSIX */

	#include <sys/mman.h>
	#include <unistd.h>
	#include <dirent.h>
	#include <pwd.h>
	#include <sys/utsname.h>
	#include <sys/statvfs.h>
	#include <sys/time.h>
	#include <sys/resource.h>
	#include <sys/syscall.h>

	#ifndef O_BINARY
		#define O_BINARY 0
	#endif

	#ifdef SYS_gettid
		#define	gim_gettid()	( (long) syscall( SYS_gettid ) )
	#else
		#define	gim_gettid()	( (long) getpid() )
	#define	gim_is_main_thread()	( gim_gettid() == gim_getpid() )
	#endif
	#define	gim_getpid()	( (long) getpid() )

#endif

#endif /* _GIM_PLATFORM_H */
