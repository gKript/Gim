//
// gim_list_test.cc
//
// Tests of gim_list_obj through its public API: item indexes after del_item and add_item.
// destroy_list() is private: the destructor is the only way to free a list.
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

// the values of the list, in order; the indexes are copied to idx
static int dump( gim_list_obj * l , int * values , int * idx , int max ) {
	int n = 0;
	if ( l->rewind() != __GIM_OK )
		return 0;
	while ( l->get_item() != NULL && n < max ) {
		values[ n ] = *(int *) l->get_item();
		idx[ n ] = (int) l->get_id();
		n++;
		l->next_item();
	}
	return n;
}

static int unique_indexes( const int * idx , int n ) {
	for ( int i = 0 ; i < n ; i++ )
		for ( int j = i + 1 ; j < n ; j++ )
			if ( idx[ i ] == idx[ j ] )
				return 0;
	return 1;
}

int main( int argc , char ** argv ) {
	gim_set_application_name( "ListTest" );
	gim_set_application_version( "0.0" );
	gim = new gim_obj;

	static int v[ 4 ] = { 1 , 2 , 3 , 4 };
	int values[ 16 ] , idx[ 16 ];

	// 1. after del_item the indexes stay unique when new items are added
	{
		gim_list_obj * l = new gim_list_obj;
		l->add_item( &v[ 0 ] );
		l->add_item( &v[ 1 ] );
		l->add_item( &v[ 2 ] );
		l->del_item( 2 );
		l->add_item( &v[ 3 ] );
		int n = dump( l , values , idx , 16 );
		check( n == 3 , "after del_item(2) and add_item: 3 items" );
		check( unique_indexes( idx , n ) , "after del_item(2) and add_item: every index is unique" );
		check( l->items() == 3 , "items() is 3" );
		check( *(int *) l->get_item( 1 ) == 1 && *(int *) l->get_item( 2 ) == 3 && *(int *) l->get_item( 3 ) == 4 ,
			"get_item(k) is the k-th item after the delete (positions 1,2,3 hold 1,3,4)" );
		delete l;
	}

	// 1b. deleting the first item moves every index down
	{
		gim_list_obj * l = new gim_list_obj;
		l->add_item( &v[ 0 ] );
		l->add_item( &v[ 1 ] );
		l->add_item( &v[ 2 ] );
		l->del_item( 1 );
		check( l->items() == 2 && *(int *) l->get_item( 1 ) == 2 && *(int *) l->get_item( 2 ) == 3 ,
			"del_item(1): positions 1,2 hold 2,3" );
		l->add_item( &v[ 3 ] );
		check( *(int *) l->get_item( 3 ) == 4 , "add_item after del_item(1) goes to position 3" );
		delete l;
	}

	printf( "\n%s (%d failures)\n" , failures ? "FAILED" : "ALL OK" , failures );
	delete gim;
	return failures ? 1 : 0;
}

/*
	Compile as for gim_kvalue_delsection_test.cc (MinGW, Windows):
	  g++ -std=c++17 -I include test/gim_list_test.cc build_e/libgim.a -static \
	      -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 \
	      -o <folder>/gim_list_test.exe
*/
