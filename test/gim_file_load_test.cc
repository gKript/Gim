//
// gim_file_load_test.cc
//
// Tests of gim_file_obj::load(): the loaded buffer has the file content and always ends with '\0'
// (one byte more than size()), with the three open methods. Before 2026-11-26 the buffer had exactly
// size() bytes and no terminator, so it could not be searched as a string.
// load() allocates the buffer in gim_memory and close() frees it: no unload() after close().
// Build: see the comment at the end of this file.

#include "gim.h"
#include <stdio.h>
#include <string.h>

gim_obj * gim;

static int failures = 0;

static void check( int ok , const char * what ) {
	if ( ok )
		printf( "ok:   %s\n" , what );
	else {
		failures++;
		printf( "FAIL: %s\n" , what );
	}
}

int main( int argc , char ** argv ) {
	const char * path = ( argc > 1 ) ? argv[ 1 ] : "gim_file_load_test.txt";
	char text[] = "first line\nCMAKE_HOME_DIRECTORY:INTERNAL=I:/GitHub Asy/Gim\nlast line";
	gim = new gim_obj;

	// flush() writes exactly the given bytes (put() with __GIM_FILE_POINTER appends a '\n')
	_gim_file out( path , __GIM_FILE_POINTER , __GIM_WRITE );
	check( out.flush( text , (int) strlen( text ) ) == __GIM_OK , "write the test file" );
	out.close();

	const _gim_flag methods[] = { __GIM_FILE_POINTER , __GIM_FILE_DESCRIPTOR , __GIM_FILE_MAPPING };
	const char * names[] = { "pointer" , "descriptor" , "mapping" };
	for ( int m = 0 ; m < 3 ; m++ ) {
		char what[128];
		_gim_file in( path , methods[ m ] , __GIM_READ );
		char * buf = in.load();
		sprintf( what , "%s: load returns a buffer" , names[ m ] );
		check( buf != NULL , what );
		if ( buf != NULL ) {
			sprintf( what , "%s: size is the text length (%u / %u)" , names[ m ] , (unsigned) in.size() , (unsigned) strlen( text ) );
			check( in.size() == strlen( text ) , what );
			sprintf( what , "%s: content is the text" , names[ m ] );
			check( memcmp( buf , text , strlen( text ) ) == 0 , what );
			sprintf( what , "%s: buffer ends with '\\0' at size()" , names[ m ] );
			check( buf[ in.size() ] == '\0' , what );
			sprintf( what , "%s: strstr finds the key" , names[ m ] );
			check( strstr( buf , "CMAKE_HOME_DIRECTORY:INTERNAL=" ) != NULL , what );
		}
		in.close();
	}

	remove( path );
	printf( "\n%s (%d failures)\n" , failures ? "FAILED" : "ALL OK" , failures );
	delete gim;
	return failures ? 1 : 0;
}

/*
	Compile as for gim_list_test.cc (MinGW, Windows), with the libgim.a of this machine's build folder:
	  g++ -std=c++17 -I include test/gim_file_load_test.cc build-<host>/libgim.a -static \
	      -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 \
	      -o <folder>/gim_file_load_test.exe
	Run it with the path of a temporary file as argument (default: gim_file_load_test.txt in the current folder).
*/
