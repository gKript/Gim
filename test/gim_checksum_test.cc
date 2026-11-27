/*
 * gim_checksum_test.cc - test of gim_checksum_obj (WRITTEN 2026-11-27, NOT RUN YET).
 * Build: link with libgim (like the other tests in this folder). Run: the exit code is 0 when all checks pass.
 * The expected values are the standard test vectors of the algorithms: MD5 (RFC 1321), SHA-1 and SHA-2 (FIPS 180-4),
 * all for the input "abc". Every failed check prints its line, so a failure can be found in the log.
 */

#include <stdio.h>
#include <string.h>
#include <gim/gim.h>

static int g_fail = 0;
static int g_pass = 0;

#define CHECK( cond ) do { if ( cond ) { g_pass++; } else { g_fail++; printf( "FAIL line %d: %s\n" , __LINE__ , #cond ); } } while ( 0 )

int main( void ) {
	gim_set_application_name( "gim_checksum_test" );
	gim_obj * gim = new gim_obj;
	gim_checksum_obj ck;
	char out[ 256 ];
	char abc[] = "abc";

	/* thread-safe buffer forms: known vectors for "abc" */
	CHECK( ck.md5_buf( out , sizeof( out ) , abc , 3 ) == 32 );
	CHECK( strcmp( out , "900150983cd24fb0d6963f7d28e17f72" ) == 0 );

	CHECK( ck.sha1_buf( out , sizeof( out ) , abc , 3 ) == 40 );
	CHECK( strcmp( out , "a9993e364706816aba3e25717850c26c9cd0d89d" ) == 0 );

	CHECK( ck.sha256_buf( out , sizeof( out ) , abc , 3 ) == 64 );
	CHECK( strcmp( out , "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" ) == 0 );

	CHECK( ck.sha512_buf( out , sizeof( out ) , abc , 3 ) == 128 );
	CHECK( strcmp( out , "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f" ) == 0 );

	/* too small output: nothing written, 0 returned */
	{
		char small[ 16 ];
		CHECK( ck.md5_buf( small , sizeof( small ) , abc , 3 ) == 0 );
		CHECK( ck.md5_buf( NULL , 64 , abc , 3 ) == 0 );
	}

	/* the static-buffer form gives the same value (still not thread-safe) */
	CHECK( strcmp( ck.md5( abc , 3 ) , "900150983cd24fb0d6963f7d28e17f72" ) == 0 );
	CHECK( strcmp( ck.sha256( abc , 3 ) , "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" ) == 0 );

	/* a file that does not exist: 0 from the buffer form, NULL from the static form */
	CHECK( ck.md5_file_buf( out , sizeof( out ) , (char *) "gim_checksum_no_such_file.bin" ) == 0 );
	CHECK( ck.sha256( (char *) "gim_checksum_no_such_file.bin" ) == NULL );

	printf( "gim_checksum_test: %d passed, %d failed\n" , g_pass , g_fail );
	delete gim;
	return ( g_fail == 0 ) ? 0 : 1;
}
