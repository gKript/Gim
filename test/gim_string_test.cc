/*
 * gim_string_test.cc - test of gim_string_obj (REWRITTEN 2026-11-27, NOT RUN YET).
 * Build: link with libgim (see the test folder of Gim). Run: the exit code is 0 when all checks pass.
 * Every check prints PASS or FAIL with its line, so a failure can be found in the log.
 */

#include <stdio.h>
#include <string.h>
#include <gim/gim.h>

static int g_fail = 0;
static int g_pass = 0;

#define CHECK( cond ) do { if ( cond ) { g_pass++; } else { g_fail++; printf( "FAIL line %d: %s\n" , __LINE__ , #cond ); } } while ( 0 )

int main( void ) {
	gim_set_application_name( "gim_string_test" );
	gim_obj * gim = new gim_obj;

	/* empty and set */
	{
		gim_string_obj s;
		CHECK( s.empty() == __GIM_YES );
		CHECK( s.size() == 0 );
		CHECK( strcmp( s.c_str() , "" ) == 0 );		/* c_str() is never NULL */
		s.set( "hello" );
		CHECK( s.size() == 5 );
		CHECK( s.length() == 5 );
		CHECK( s.empty() == __GIM_NO );
		CHECK( strcmp( s.c_str() , "hello" ) == 0 );
		s.set( NULL );
		CHECK( s.size() == 0 );
	}

	/* cat, also on an empty text (the old code did nothing) */
	{
		gim_string_obj s;
		s.cat( "abc" );
		CHECK( strcmp( s.c_str() , "abc" ) == 0 );
		CHECK( s.size() == 3 );
		s.cat( "def" );
		CHECK( strcmp( s.c_str() , "abcdef" ) == 0 );
		CHECK( s.size() == 6 );
		s.cat( s.c_str() );					/* cat of itself: the buffer may move while copying */
		CHECK( strcmp( s.c_str() , "abcdefabcdef" ) == 0 );
		CHECK( s.size() == 12 );
	}

	/* clean keeps the buffer and empties the text */
	{
		gim_string_obj s( "xyz" );
		s.clean();
		CHECK( s.size() == 0 );
		CHECK( s.empty() == __GIM_YES );
		CHECK( strcmp( s.c_str() , "" ) == 0 );
		s.cat( "new" );
		CHECK( strcmp( s.c_str() , "new" ) == 0 );
	}

	/* deep copy: copy construction and assignment */
	{
		gim_string_obj a( "one" );
		gim_string_obj b( a );
		b.cat( "two" );
		CHECK( strcmp( a.c_str() , "one" ) == 0 );		/* a is not changed */
		CHECK( strcmp( b.c_str() , "onetwo" ) == 0 );
		gim_string_obj c;
		c = b;
		c.set( "x" );
		CHECK( strcmp( b.c_str() , "onetwo" ) == 0 );
		CHECK( strcmp( c.c_str() , "x" ) == 0 );
		c = c;								/* self assignment */
		CHECK( strcmp( c.c_str() , "x" ) == 0 );
	}

	/* long text: more than the old fixed 32 KB */
	{
		gim_string_obj s;
		for ( int i = 0 ; i < 100000 ; i++ )
			s.cat( "a" );
		CHECK( s.size() == 100000 );
		CHECK( strlen( s.c_str() ) == 100000 );
	}

	/* compare and equal */
	{
		gim_string_obj a( "abc" ) , b( "abd" ) , c( "abc" );
		CHECK( a.equal( "abc" ) == __GIM_YES );
		CHECK( a.equal( "abd" ) == __GIM_NO );
		CHECK( a.equal( b ) == __GIM_NO );
		CHECK( a.equal( c ) == __GIM_YES );
		CHECK( a == c );
		CHECK( a != b );
		CHECK( a.compare( "abc" ) == 0 );
		CHECK( a.compare( b ) < 0 );
		CHECK( b.compare( a ) > 0 );
	}

	/* find, pos_of, rpos_of, left_find, right_find */
	{
		gim_string_obj s( "abcabc" );
		CHECK( s.find( "bca" ) == __GIM_YES );
		CHECK( s.find( "xyz" ) == __GIM_NO );
		CHECK( s.pos_of( "bc" ) == 1 );
		CHECK( s.rpos_of( "bc" ) == 4 );
		CHECK( s.pos_of( "zz" ) == -1 );
		CHECK( s.left_find( 'c' ) == s.c_str() + 2 );
		CHECK( s.right_find( 'c' ) == s.c_str() + 5 );
		CHECK( s.left_find( "ca" ) == s.c_str() + 2 );
		CHECK( s.right_find( "ab" ) == s.c_str() + 3 );
		CHECK( s.left_find( "nope" ) == NULL );
		gim_string_obj e;
		CHECK( e.left_find( 'a' ) == NULL );
		CHECK( e.pos_of( "a" ) == -1 );
	}

	/* trim, upper, lower */
	{
		gim_string_obj s( "  \t hi there \r\n" );
		s.trim();
		CHECK( strcmp( s.c_str() , "hi there" ) == 0 );
		gim_string_obj t( "  x  " );
		t.trim_left();
		CHECK( strcmp( t.c_str() , "x  " ) == 0 );
		t.trim_right();
		CHECK( strcmp( t.c_str() , "x" ) == 0 );
		gim_string_obj u( "MiXeD 1" );
		u.upper();
		CHECK( strcmp( u.c_str() , "MIXED 1" ) == 0 );
		u.lower();
		CHECK( strcmp( u.c_str() , "mixed 1" ) == 0 );
	}

	/* replace, including a replacement that uses the text itself */
	{
		gim_string_obj s( "a-b-c" );
		s.replace( "-" , "++" );
		CHECK( strcmp( s.c_str() , "a++b++c" ) == 0 );
		s.replace( "a" , "" );
		CHECK( strcmp( s.c_str() , "++b++c" ) == 0 );
		gim_string_obj r( "xx" );
		r.replace( "x" , "[x]" );
		CHECK( strcmp( r.c_str() , "[x][x]" ) == 0 );
		gim_string_obj q( "ab" );
		q.replace( "" , "z" );					/* empty old text: nothing */
		CHECK( strcmp( q.c_str() , "ab" ) == 0 );
	}

	/* fields and field */
	{
		gim_string_obj s( "a;b;;c" );
		CHECK( s.fields( ';' ) == 4 );
		gim_string_obj f;
		CHECK( s.field( ';' , 0 , f ) == __GIM_YES );
		CHECK( strcmp( f.c_str() , "a" ) == 0 );
		CHECK( s.field( ';' , 2 , f ) == __GIM_YES );
		CHECK( strcmp( f.c_str() , "" ) == 0 );
		CHECK( s.field( ';' , 3 , f ) == __GIM_YES );
		CHECK( strcmp( f.c_str() , "c" ) == 0 );
		CHECK( s.field( ';' , 4 , f ) == __GIM_NO );
		gim_string_obj e;
		CHECK( e.fields( ';' ) == 0 );
		CHECK( s.field( ';' , 1 , s ) == __GIM_YES );	/* out is the same object */
		CHECK( strcmp( s.c_str() , "b" ) == 0 );
	}

	/* substr, insert, erase */
	{
		gim_string_obj s( "abcdef" );
		gim_string_obj p = s.substr( 2 , 3 );
		CHECK( strcmp( p.c_str() , "cde" ) == 0 );
		gim_string_obj tail = s.substr( 4 , 100 );
		CHECK( strcmp( tail.c_str() , "ef" ) == 0 );
		CHECK( s.substr( 9 , 2 ).size() == 0 );
		s.insert( 3 , "XY" );
		CHECK( strcmp( s.c_str() , "abcXYdef" ) == 0 );
		s.insert( 0 , s.c_str() );				/* insert of itself */
		CHECK( strcmp( s.c_str() , "abcXYdefabcXYdef" ) == 0 );
		s.erase( 0 , 9 );
		CHECK( strcmp( s.c_str() , "abcXYdef" ) == 0 );
		s.erase( 100 , 5 );						/* past the end: nothing */
		CHECK( strcmp( s.c_str() , "abcXYdef" ) == 0 );
		CHECK( s.at( 0 ) == 'a' );
		CHECK( s.at( 100 ) == '\0' );
	}

	/* starts_with, ends_with */
	{
		gim_string_obj s( "gkfp.gkp" );
		CHECK( s.starts_with( "gkfp" ) == __GIM_YES );
		CHECK( s.starts_with( "gkdp" ) == __GIM_NO );
		CHECK( s.ends_with( ".gkp" ) == __GIM_YES );
		CHECK( s.ends_with( ".gk" ) == __GIM_NO );
		CHECK( s.ends_with( "longer text than the string" ) == __GIM_NO );
	}

	/* formats and numbers */
	{
		gim_string_obj s;
		s.set_format( "%s-%03d" , "file" , 7 );
		CHECK( strcmp( s.c_str() , "file-007" ) == 0 );
		s.cat_format( "[%d]" , 42 );
		CHECK( strcmp( s.c_str() , "file-007[42]" ) == 0 );
		gim_string_obj n;
		n.set_int( -1234567890123LL );
		CHECK( n.to_int() == -1234567890123LL );
		n.set( "  12abc" );
		CHECK( n.to_int() == 12 );
		n.set_float( 1.5 );
		CHECK( n.to_float() == 1.5 );
		n.set( "junk" );
		CHECK( n.to_int() == 0 );
	}

	/* reserve keeps the text */
	{
		gim_string_obj s( "keep" );
		CHECK( s.reserve( 1000 ) == __GIM_OK );
		CHECK( s.capacity() >= 1000 );
		CHECK( strcmp( s.c_str() , "keep" ) == 0 );
	}

	printf( "gim_string_test: %d passed, %d failed\n" , g_pass , g_fail );
	delete gim;
	return ( g_fail == 0 ) ? 0 : 1;
}
