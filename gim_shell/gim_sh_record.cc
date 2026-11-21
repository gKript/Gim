//
// gim_sh_record.cc
//
// Gim Shell - "insert" and "select" commands (phases 3-4).
//   insert <table> <field> <value> [<field> <value> ...]
//       every field of the table must be given once; quote values that contain spaces
//   select <table>
//   select <table> show <field>[,<field>...]     prints only these fields
//   select <table> where <field> <op> <value> [and <field> <op> <value> ...]
//          [order by <field> [asc|desc]] [limit <n> | last <n>]
//   select <table> where <field> between <a> <b>     same as >= a and <= b
//       op: =  !=  <  <=  >  >=
//       numbers (INT, FLOAT) are compared as numbers, the other types as text
//       keywords are case insensitive; 'and' only (no 'or')
//
/*#############################################################################

	Released under the terms of the GNU General Public License v2, as the rest of Gim.

#############################################################################*/

#include "gim_shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SEL_CONDS_MAX	32
#define SEL_SHOW_MAX	16

struct sel_cond {
	const char *	field;
	_gim_flag		type;
	const char *	op;
	const char *	value;
};

struct sel_query {
	int				nshow;			// 0 = all the fields
	const char *	show[ SEL_SHOW_MAX ];
	int				ncond;
	sel_cond		cond[ SEL_CONDS_MAX ];
	const char *	order_field;
	_gim_flag		order_type;
	int				order_desc;
	int				limit;			// 0 = no limit
	int				limit_last;		// 1 = last n, 0 = first n
};

struct sel_hit {
	int		section;
	char *	key;					// copy of the order-by value (NULL when no order by)
};

static _gim_flag sel_sort_type;
static int sel_sort_desc;

static int record_is_numeric( _gim_flag t ) {
	return ( t == GIM_DB_TYPE_INT ) || ( t == GIM_DB_TYPE_FLOAT );
}

static int insert_usage( void ) {
	puts( "usage: insert <table> <field> <value> [<field> <value> ...]" );
	return GIM_SH_CMD_ERROR;
}

static int select_usage( void ) {
	puts( "usage: select <table>" );
	puts( "       select <table> [show <field>[,<field>...]] [where <field> <op> <value> [and ...]]" );
	puts( "       select <table> where <field> between <a> <b>      same as >= a and <= b" );
	puts( "       optional at the end: order by <field> [asc|desc]   limit <n>   last <n>" );
	puts( "       op: =  !=  <  <=  >  >=     values with spaces: use double quotes" );
	return GIM_SH_CMD_ERROR;
}

int cmd_insert( int argc , char ** argv ) {
	// insert <table> then (field, value) pairs
	if ( argc < 4 || ( argc - 2 ) % 2 != 0 )
		return insert_usage();

	gim_db_obj * db = gim_sh_current_db();
	if ( db == NULL ) {
		puts( "no database selected (use 'db use <name>' or 'db create')" );
		return GIM_SH_CMD_ERROR;
	}

	int n = ( argc - 2 ) / 2;
	const char * fields[ 255 ];
	const char * values[ 255 ];
	for ( int k = 0 ; k < n ; k++ ) {
		fields[ k ] = argv[ 2 + 2 * k ];
		values[ k ] = argv[ 3 + 2 * k ];
	}

	if ( db->insert_record( argv[ 1 ] , (_gim_Uint8) n , fields , values ) != __GIM_OK ) {
		printf( "insert: record not added to '%s' (see the messages above)\n" , argv[ 1 ] );
		return GIM_SH_CMD_ERROR;
	}
	printf( "record added to '%s'\n" , argv[ 1 ] );
	return GIM_SH_CMD_OK;
}

// compares the stored value with the one of the query: returns 1 when the record matches
static int record_matches( _gim_flag type , const char * stored , const char * op , const char * wanted ) {
	int cmp;
	if ( record_is_numeric( type ) ) {
		double a = atof( stored );
		double b = atof( wanted );
		cmp = ( a < b ) ? -1 : ( a > b ) ? 1 : 0;
	}
	else
		cmp = strcmp( stored , wanted );

	if ( strcmp( op , "=" ) == 0 )  return cmp == 0;
	if ( strcmp( op , "!=" ) == 0 ) return cmp != 0;
	if ( strcmp( op , "<" ) == 0 )  return cmp < 0;
	if ( strcmp( op , "<=" ) == 0 ) return cmp <= 0;
	if ( strcmp( op , ">" ) == 0 )  return cmp > 0;
	if ( strcmp( op , ">=" ) == 0 ) return cmp >= 0;
	return 0;
}

static int op_is_valid( const char * op ) {
	return strcmp( op , "=" ) == 0 || strcmp( op , "!=" ) == 0 || strcmp( op , "<" ) == 0 ||
		   strcmp( op , "<=" ) == 0 || strcmp( op , ">" ) == 0 || strcmp( op , ">=" ) == 0;
}

// keyword check: 'kw' is lowercase, 'word' may be in any case
static int kw_is( const char * word , const char * kw ) {
	for ( ; *word && *kw ; word++ , kw++ )
		if ( tolower( (unsigned char) *word ) != *kw )
			return 0;
	return *word == '\0' && *kw == '\0';
}

static int find_field( _gim_db_table * t , const char * name , _gim_flag * type ) {
	t->fields->rewind();
	for ( int i = 0 ; i < t->fields->items() ; i++ ) {
		_gim_db_field * f = (_gim_db_field *) t->fields->get_item();
		if ( strcmp( f->name , name ) == 0 ) {
			*type = f->type;
			return 1;
		}
		t->fields->next_item();
	}
	return 0;
}

static int add_cond( _gim_db_table * t , sel_query * q , const char * field , const char * op , const char * value ) {
	if ( q->ncond >= SEL_CONDS_MAX ) {
		puts( "select: too many conditions" );
		return 1;
	}
	sel_cond * c = &q->cond[ q->ncond ];
	if ( !find_field( t , field , &c->type ) ) {
		printf( "select: table '%s' has no field '%s'\n" , t->name , field );
		return 1;
	}
	c->field = field;
	c->op = op;
	c->value = value;
	q->ncond++;
	return 0;
}

// parses argv[2..argc-1]: returns 0 on success, 1 after printing the error
static int parse_select( int argc , char ** argv , _gim_db_table * t , sel_query * q ) {
	int i = 2;
	q->nshow = 0;
	q->ncond = 0;

	// show <field>[,<field>...]  -> only these fields are printed
	if ( i < argc && kw_is( argv[ i ] , "show" ) ) {
		if ( i + 1 >= argc ) {
			select_usage();
			return 1;
		}
		char * p = argv[ i + 1 ];
		for ( ;; ) {
			if ( q->nshow >= SEL_SHOW_MAX ) {
				puts( "select: too many fields in 'show'" );
				return 1;
			}
			char * comma = strchr( p , ',' );
			if ( comma != NULL )
				*comma = '\0';
			_gim_flag ftype;
			if ( !find_field( t , p , &ftype ) ) {
				printf( "select: table '%s' has no field '%s'\n" , t->name , p );
				return 1;
			}
			q->show[ q->nshow++ ] = p;
			if ( comma == NULL )
				break;
			p = comma + 1;
		}
		i += 2;
	}
	q->order_field = NULL;
	q->order_desc = 0;
	q->limit = 0;
	q->limit_last = 0;

	// where <field> <op> <value> [and ...]
	if ( i < argc && kw_is( argv[ i ] , "where" ) ) {
		i++;
		for ( ;; ) {
			if ( i + 2 >= argc ) {
				select_usage();
				return 1;
			}
			const char * field = argv[ i ];
			const char * op = argv[ i + 1 ];
			if ( kw_is( op , "between" ) ) {
				if ( i + 3 >= argc ) {
					select_usage();
					return 1;
				}
				if ( add_cond( t , q , field , ">=" , argv[ i + 2 ] ) ||
					 add_cond( t , q , field , "<=" , argv[ i + 3 ] ) )
					return 1;
				i += 4;
			}
			else {
				if ( !op_is_valid( op ) ) {
					select_usage();
					return 1;
				}
				if ( add_cond( t , q , field , op , argv[ i + 2 ] ) )
					return 1;
				i += 3;
			}
			if ( i < argc && kw_is( argv[ i ] , "and" ) ) {
				i++;
				continue;
			}
			break;
		}
	}

	// order by <field> [asc|desc]
	if ( i < argc && kw_is( argv[ i ] , "order" ) ) {
		if ( i + 2 >= argc || !kw_is( argv[ i + 1 ] , "by" ) ) {
			select_usage();
			return 1;
		}
		q->order_field = argv[ i + 2 ];
		if ( !find_field( t , q->order_field , &q->order_type ) ) {
			printf( "select: table '%s' has no field '%s'\n" , t->name , q->order_field );
			return 1;
		}
		i += 3;
		if ( i < argc && kw_is( argv[ i ] , "desc" ) ) {
			q->order_desc = 1;
			i++;
		}
		else if ( i < argc && kw_is( argv[ i ] , "asc" ) )
			i++;
	}

	// limit <n> (first n)  or  last <n> (last n)
	if ( i < argc && ( kw_is( argv[ i ] , "limit" ) || kw_is( argv[ i ] , "last" ) ) ) {
		if ( i + 1 >= argc ) {
			select_usage();
			return 1;
		}
		q->limit = atoi( argv[ i + 1 ] );
		if ( q->limit <= 0 ) {
			printf( "select: '%s' needs a positive number\n" , argv[ i ] );
			return 1;
		}
		q->limit_last = kw_is( argv[ i ] , "last" );
		i += 2;
	}

	if ( i != argc ) {
		select_usage();
		return 1;
	}
	return 0;
}

// sort of the hits by the order-by value (ties keep the table order)
static int sel_hit_cmp( const void * pa , const void * pb ) {
	const sel_hit * a = (const sel_hit *) pa;
	const sel_hit * b = (const sel_hit *) pb;
	int c;
	if ( record_is_numeric( sel_sort_type ) ) {
		double x = atof( a->key );
		double y = atof( b->key );
		c = ( x < y ) ? -1 : ( x > y ) ? 1 : 0;
	}
	else
		c = strcmp( a->key , b->key );
	if ( sel_sort_desc )
		c = -c;
	if ( c == 0 )
		c = ( a->section < b->section ) ? -1 : ( a->section > b->section ) ? 1 : 0;
	return c;
}

int cmd_select( int argc , char ** argv ) {
	if ( argc < 2 )
		return select_usage();

	gim_db_obj * db = gim_sh_current_db();
	if ( db == NULL ) {
		puts( "no database selected (use 'db use <name>' or 'db create')" );
		return GIM_SH_CMD_ERROR;
	}
	_gim_db_table * t = db->seek_table( argv[ 1 ] );
	if ( t == NULL ) {
		printf( "table '%s' not found\n" , argv[ 1 ] );
		return GIM_SH_CMD_ERROR;
	}

	sel_query q;
	if ( parse_select( argc , argv , t , &q ) != 0 )
		return GIM_SH_CMD_ERROR;

	// filter: collect the matching records
	int n = ( t->Tdata != NULL && t->Tdata->ParserIsUp() == __GIM_YES ) ? t->Tdata->GetHowManySection() : 0;
	sel_hit * hits = (sel_hit *) malloc( sizeof( sel_hit ) * ( n > 0 ? n : 1 ) );
	if ( hits == NULL ) {
		puts( "select: out of memory" );
		return GIM_SH_CMD_ERROR;
	}
	int nhit = 0;
	for ( int s = 0 ; s < n ; s++ ) {
		char * sec = t->Tdata->GetSectionName( (_gim_int16) s );
		if ( sec == NULL )
			continue;
		int ok = 1;
		for ( int c = 0 ; c < q.ncond && ok ; c++ ) {
			char * stored = t->Tdata->GetKeySTR( sec , q.cond[ c ].field );
			ok = ( stored != NULL ) && record_matches( q.cond[ c ].type , stored , q.cond[ c ].op , q.cond[ c ].value );
		}
		if ( !ok )
			continue;
		hits[ nhit ].section = s;
		hits[ nhit ].key = NULL;
		if ( q.order_field != NULL ) {
			char * k = t->Tdata->GetKeySTR( sec , q.order_field );
			hits[ nhit ].key = strdup( k ? k : "" );
		}
		nhit++;
	}

	// order by
	if ( q.order_field != NULL && nhit > 1 ) {
		sel_sort_type = q.order_type;
		sel_sort_desc = q.order_desc;
		qsort( hits , nhit , sizeof( sel_hit ) , sel_hit_cmp );
	}

	// limit / last: choose the range to show
	int from = 0 , to = nhit;
	if ( q.limit > 0 && q.limit < nhit ) {
		if ( q.limit_last )
			from = nhit - q.limit;
		else
			to = q.limit;
	}

	for ( int k = from ; k < to ; k++ ) {
		char * sec = t->Tdata->GetSectionName( (_gim_int16) hits[ k ].section );
		printf( "  [%d] " , hits[ k ].section + 1 );
		if ( q.nshow > 0 ) {
			for ( int j = 0 ; j < q.nshow ; j++ ) {
				char * kv = t->Tdata->GetKeySTR( sec , q.show[ j ] );
				printf( "%s%s=%s" , j ? ", " : "" , q.show[ j ] , kv ? kv : "?" );
			}
		}
		else {
			int keys = t->Tdata->GetHowManyKey( sec );
			for ( int j = 0 ; j < keys ; j++ ) {
				char * kn = t->Tdata->GetKeyName( sec , (_gim_int16) j );
				char * kv = ( kn != NULL ) ? t->Tdata->GetKeySTR( sec , kn ) : NULL;
				printf( "%s%s=%s" , j ? ", " : "" , kn ? kn : "?" , kv ? kv : "?" );
			}
		}
		puts( "" );
	}

	int shown = to - from;
	printf( "%d record(s) of %d in '%s'" , shown , n , t->name );
	if ( nhit != shown )
		printf( " (%d matching)" , nhit );
	puts( "" );

	for ( int k = 0 ; k < nhit ; k++ )
		free( hits[ k ].key );
	free( hits );
	return GIM_SH_CMD_OK;
}
