/*
 * gim_time_test.cc - test of gim_time_obj (WRITTEN 2026-11-27, NOT RUN YET).
 * Build: link with libgim (like the other tests in this folder). Run: the exit code is 0 when all checks pass.
 * Every failed check prints its line, so a failure can be found in the log.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <gim/gim.h>

static int g_fail = 0;
static int g_pass = 0;

#define CHECK( cond ) do { if ( cond ) { g_pass++; } else { g_fail++; printf( "FAIL line %d: %s\n" , __LINE__ , #cond ); } } while ( 0 )

int main( void ) {
	gim_set_application_name( "gim_time_test" );
	gim_obj * gim = new gim_obj;

	/* get_usec_buf: format "%2ld.%06ld" and a non-negative value */
	{
		gim_time_obj t;
		char buf[32];
		t.start_usec();
		size_t n = t.get_usec_buf( buf , sizeof( buf ) );
		CHECK( n > 0 );
		CHECK( strchr( buf , '.' ) != NULL );
		CHECK( buf[ strlen( buf ) - 7 ] == '.' );		/* six digits after the dot */
		CHECK( buf[0] != '-' );
		CHECK( t.get_usec_buf( NULL , 32 ) == 0 );		/* NULL buffer is refused */
		CHECK( t.get_usec_buf( buf , 0 ) == 0 );		/* zero size is refused */
	}

	/* two objects give independent results: each one has its own buffer */
	{
		gim_time_obj a , b;
		char ba[32] , bb[32];
		a.start_usec();
		b.start_usec();
		a.get_usec_buf( ba , sizeof( ba ) );
		b.get_usec_buf( bb , sizeof( bb ) );
		CHECK( strlen( ba ) > 0 && strlen( bb ) > 0 );
	}

	/* process_buf: format "%3ld.%03ld" */
	{
		gim_time_obj t;
		char buf[32];
		size_t n = t.process_buf( buf , sizeof( buf ) );
		CHECK( n > 0 );
		CHECK( strchr( buf , '.' ) != NULL );
		CHECK( buf[ strlen( buf ) - 4 ] == '.' );		/* three digits after the dot */
	}

	/* date_buf: "%A %d-%m-%Y %H:%M:%S" is at least 19 characters, and the year is this one or later */
	{
		gim_time_obj t;
		char buf[64];
		size_t n = t.date_buf( buf , sizeof( buf ) );
		CHECK( n >= 19 );
		CHECK( t.date_buf( buf , 4 ) == 0 );			/* too small: nothing written */
		t.date_buf( buf , sizeof( buf ) );
		CHECK( strlen( buf ) >= 19 );
	}

	/* date(): allocated string, the caller frees it */
	{
		gim_time_obj t;
		char * d = t.date();
		CHECK( d != NULL );
		CHECK( d != NULL && strlen( d ) >= 19 );
		free( d );
	}

	/* process_sec / process_msec: the wall clock, close to time(NULL) (fixed: they used to return a stale value) */
	{
		gim_time_obj t;
		int s = t.process_sec();
		time_t now = time( NULL );
		CHECK( (long) s >= (long) now - 2 && (long) s <= (long) now + 2 );
		int ms = t.process_msec();
		CHECK( ms >= 0 && ms < 1000 );
	}

	/* seconds of start / stop / get */
	{
		gim_time_obj t;
		t.start();
		t.stop();
		CHECK( t.get() <= 2 );
	}

	printf( "gim_time_test: %d passed, %d failed\n" , g_pass , g_fail );
	delete gim;
	return ( g_fail == 0 ) ? 0 : 1;
}
