//
// gim_kvalue_delsection_test.cc
//
// Test of gim_prsr_obj::DelSection() on a Lex A parser.
// For each position (first, middle, last, only) it builds a file with sections S1..Sn,
// deletes one section, writes the file, reads it back into a new parser and checks
// that the remaining sections and their keys are exactly the expected ones.
//
// Usage: gim_kvalue_delsection_test <folder for the test files>
//
// Build (not part of the library): see the comment at the end of this file.

#include "gim.h"
#include <stdio.h>
#include <stdlib.h>
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

// fills the parser with n sections, each with an INT key "a" and a text key "b"
static void build( _gim_prsr * p , const char * file , int n ) {
	p->Up( file , "ds_test" );
	p->SetLex( __LEX_A );
	for ( int i = 1 ; i <= n ; i++ ) {
		char sec[ 32 ];
		sprintf( sec , "S%d" , i );
		p->AddSection( sec );
		p->AddKey( sec , "a" , i );
		p->AddKey( sec , "b" , sec );
	}
}

// copies the section names in order; returns how many there are
static int get_names( _gim_prsr * p , char names[][ 32 ] , int max ) {
	int n = p->GetHowManySection();
	for ( int i = 0 ; i < n && i < max ; i++ ) {
		char * s = p->GetSectionName( (_gim_int16) i );		// shared buffer: copy it now
		strncpy( names[ i ] , ( s != NULL ) ? s : "" , 31 );
		names[ i ][ 31 ] = '\0';
	}
	return n;
}

// true when the parser holds exactly the sections in expected[], each with a = its number and b = its name
static int same_content( _gim_prsr * p , const char expected[][ 32 ] , int nexp , int original_index[] ) {
	char names[ 64 ][ 32 ];
	int n = get_names( p , names , 64 );
	if ( n != nexp )
		return 0;
	for ( int i = 0 ; i < n ; i++ ) {
		if ( strcmp( names[ i ] , expected[ i ] ) != 0 )
			return 0;
		if ( p->GetKeyINT( names[ i ] , "a" ) != original_index[ i ] )
			return 0;
		char * b = p->GetKeySTR( names[ i ] , "b" );
		if ( b == NULL || strcmp( b , expected[ i ] ) != 0 )
			return 0;
	}
	return 1;
}

// n sections, delete the one at position victim (1-based), check memory and the file after a round trip
static void run_case( const char * label , const char * folder , int n , int victim ) {
	char file[ 512 ];
	sprintf( file , "%s/ds_%s.kv" , folder , label );

	// expected content after the delete
	char expected[ 64 ][ 32 ];
	int original_index[ 64 ];
	int m = 0;
	for ( int i = 1 ; i <= n ; i++ ) {
		if ( i == victim )
			continue;
		sprintf( expected[ m ] , "S%d" , i );
		original_index[ m ] = i;
		m++;
	}

	char msg[ 256 ];

	_gim_prsr * p = new _gim_prsr;
	build( p , file , n );
	sprintf( msg , "[%s] built %d sections" , label , n );
	check( p->GetHowManySection() == n , msg );

	char victim_name[ 32 ];
	sprintf( victim_name , "S%d" , victim );
	check( p->DelSection( victim_name ) == __GIM_OK , "DelSection returns __GIM_OK" );
	check( p->ExistSection( victim_name ) == __GIM_NOT_EXIST , "deleted section no longer exists" );
	check( p->DelSection( victim_name ) != __GIM_OK , "deleting it again is refused" );
	sprintf( msg , "[%s] memory: %d sections left" , label , m );
	check( same_content( p , expected , m , original_index ) , msg );

	check( p->Write() == __GIM_OK , "Write() returns __GIM_OK" );
	p->Down();
	delete p;

	_gim_prsr * q = new _gim_prsr;
	check( q->Read( file ) == __GIM_OK , "Read() of the written file" );
	sprintf( msg , "[%s] file round trip keeps %d sections" , label , m );
	check( same_content( q , expected , m , original_index ) , msg );
	q->Down();
	delete q;
}

int main( int argc , char ** argv ) {
	const char * folder = ( argc > 1 ) ? argv[ 1 ] : ".";

	gim_set_application_name( "DelSectionTest" );
	gim_set_application_version( "0.0" );
	gim = new gim_obj;

	run_case( "first"  , folder , 5 , 1 );
	run_case( "middle" , folder , 5 , 3 );
	run_case( "last"   , folder , 5 , 5 );
	run_case( "only"   , folder , 1 , 1 );

	printf( "\n%s (%d failures)\n" , failures ? "FAILED" : "ALL OK" , failures );
	delete gim;
	return failures ? 1 : 0;
}

/*
	Compile with the same libraries as gkmake (MinGW, Windows):
	  g++ -std=c++17 -I include test/gim_kvalue_delsection_test.cc build_e/libgim.a -static \
	      -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 \
	      -o <folder>/gim_kvalue_delsection_test.exe
*/
