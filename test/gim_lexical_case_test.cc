//
// gim_lexical_case_test.cc
//
// Tests of prsr_lexical_class::str_up and str_down with strings longer than 255 characters.
// Before 2026-11-26 the loop index was _gim_Uint8: with len > 255 it wrapped and the loop never ended.
// Both functions return a strdup() copy: the caller frees it.
// Build: see the comment at the end of this file.

#include "gim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

static void check( int ok , const char * what , int len ) {
	if ( ok )
		printf( "ok:   %s (len %d)\n" , what , len );
	else {
		failures++;
		printf( "FAIL: %s (len %d)\n" , what , len );
	}
}

// all the len characters of s are c, and the string ends there
static int all_equal( const char * s , int len , char c ) {
	for ( int i = 0 ; i < len ; i++ )
		if ( s[ i ] != c )
			return 0;
	return s[ len ] == '\0';
}

int main( void ) {
	const int lens[] = { 1 , 255 , 256 , 300 , 1000 };
	for ( int k = 0 ; k < (int)( sizeof( lens ) / sizeof( lens[ 0 ] ) ) ; k++ ) {
		int len = lens[ k ];
		char * src = (char *) malloc( len + 1 );
		memset( src , 'A' , len );
		src[ len ] = '\0';
		char * down = Lexical.str_down( src , len );
		check( all_equal( down , len , 'a' ) , "str_down" , len );
		char * up = Lexical.str_up( down , len );
		check( all_equal( up , len , 'A' ) , "str_up" , len );
		free( up );
		free( down );
		free( src );
	}
	printf( "\n%s (%d failures)\n" , failures ? "FAILED" : "ALL OK" , failures );
	return failures ? 1 : 0;
}

/*
	Compile as for gim_list_test.cc (MinGW, Windows), with the libgim.a of this machine's build folder:
	  g++ -std=c++17 -I include test/gim_lexical_case_test.cc build-<host>/libgim.a -static \
	      -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 \
	      -o <folder>/gim_lexical_case_test.exe
*/
