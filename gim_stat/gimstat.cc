//
// gimstat.cc
//
// Gim  -  Generic Information Manager static library
// Version 2.5-x
// AsYntote
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%201.0?by=version]
//
/*#############################################################################

	Copyright (C) 2002-2026 
	Danilo Zannoni (AsYntote)

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

#include <stdarg.h>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <gim/gim.h>
#include <gim/gim_thread.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define MIN_SIZE	256
/* files at least this big get one thread per test and a progress bar on stderr */
#define GIM_PARALLEL_MIN_BYTES	( 1L << 20 )

/* buffers come from Gim's memory manager (its ceiling, its lock and its list of allocations), not from malloc; hidden from the log */
static void * gim_mem_alloc( size_t size ) {
	return gim_memory->Alloc( size , (_gim_Uint32) __GIM_MEM_GIMSTAT_BUF , __GIM_HIDE );
}

static void gim_mem_free( void * p ) {
	if ( p != NULL )
		gim_memory->Free( p );
}

/* ---- statistical tests with p-values ----------------------------------------
   Each test prints its p-value and PASS (p >= 0.01) or FAIL. The tests with many
   sub-tests (lags, bit positions) use a Bonferroni threshold: 0.01 / number of sub-tests. */

static double gim_gammq( double a , double x ) {
	/* regularized upper incomplete gamma Q(a,x) */
	double gln , ap , del , sum , b , c , d , h;
	int n , i;
	if ( x <= 0.0 )
		return 1.0;
	gln = lgamma( a );
	if ( x < a + 1.0 ) {
		ap = a;
		sum = del = 1.0 / a;
		for ( n = 0 ; n < 10000 ; n++ ) {
			ap += 1.0;
			del *= x / ap;
			sum += del;
			if ( fabs( del ) < fabs( sum ) * 1e-14 )
				break;
		}
		return 1.0 - sum * exp( -x + a * log( x ) - gln );
	}
	b = x + 1.0 - a;
	c = 1.0 / 1e-300;
	d = 1.0 / b;
	h = d;
	for ( i = 1 ; i < 10000 ; i++ ) {
		double an = -i * ( i - a );
		b += 2.0;
		d = an * d + b;
		if ( fabs( d ) < 1e-300 ) d = 1e-300;
		c = b + an / c;
		if ( fabs( c ) < 1e-300 ) c = 1e-300;
		d = 1.0 / d;
		del = d * c;
		h *= del;
		if ( fabs( del - 1.0 ) < 1e-14 )
			break;
	}
	return exp( -x + a * log( x ) - gln ) * h;
}

static double gim_chi2_p( double chi , int df ) {
	return gim_gammq( df / 2.0 , chi / 2.0 );
}

static double gim_normal_p( double z ) {
	return erfc( fabs( z ) / sqrt( 2.0 ) );
}

/* Each test writes its text and its p-values into its own context. Big files run one thread per test;
   the main thread prints the texts in test order, so the output keeps the same order as before. */
struct gim_test_ctx {
	double				pv[ 32 ];
	int					npv;
	int					quiet;
	std::string			out;
	std::atomic<long>	progress;	/* 0..1000 */
	std::atomic<bool>	done;
	gim_test_ctx() : npv( 0 ) , quiet( 0 ) , progress( 0 ) , done( false ) {}
};
static thread_local gim_test_ctx * gim_ctx = NULL;

static void gim_out( const char * fmt , ... ) {
	char line[ 1024 ];
	va_list ap;
	if ( gim_ctx == NULL || gim_ctx->quiet )
		return;
	va_start( ap , fmt );
	vsnprintf( line , sizeof( line ) , fmt , ap );
	va_end( ap );
	gim_ctx->out += line;
}

static void gim_tick( long done , long total ) {
	if ( gim_ctx != NULL && total > 0 )
		gim_ctx->progress.store( (long) ( (long long) done * 1000 / total ) );
}
static int gim_quiet = 0;

static void gim_report( const char * name , double p , double threshold , const char * detail ) {
	if ( gim_ctx->npv < 32 )
		gim_ctx->pv[ gim_ctx->npv++ ] = p;
	gim_out( "%-30s p = %-10.4g %-34s %s\n" , name , p , detail , ( p >= threshold ) ? "PASS" : "FAIL" );
}

/* test 1: chi-square of the 256 byte values (255 degrees of freedom) */
static void gim_test_bytes( const unsigned char * buf , long n ) {
	double expected = (double) n / 256.0 , chi = 0.0;
	unsigned long cnt[256];
	long i;
	char detail[64];
	memset( cnt , 0 , sizeof( cnt ) );
	for ( i = 0 ; i < n ; i++ )
		{ cnt[ buf[ i ] ]++; if ( ( i & 0xFFFF ) == 0 ) gim_tick( i , n ); }
	for ( i = 0 ; i < 256 ; i++ ) {
		double d = (double) cnt[ i ] - expected;
		chi += d * d / expected;
	}
	sprintf( detail , "chi2 = %.1f  (df 255)" , chi );
	gim_report( "Byte chi-square" , gim_chi2_p( chi , 255 ) , 0.01 , detail );
}

/* test 2: frequency of each bit position (monobit per position), Bonferroni over 8 positions */
static void gim_test_bits( const unsigned char * buf , long n ) {
	long ones[8];
	double pmin = 1.0 , worst = 0.0;
	int b , bit;
	long i;
	char detail[96];
	memset( ones , 0 , sizeof( ones ) );
	for ( i = 0 ; i < n ; i++ )
		for ( bit = 0 ; bit < 8 ; bit++ )
			ones[ bit ] += ( buf[ i ] >> bit ) & 1;
	for ( b = 0 ; b < 8 ; b++ ) {
		double z = ( 2.0 * (double) ones[ b ] - (double) n ) / sqrt( (double) n );
		double p = gim_normal_p( z );
		double dev = fabs( (double) ones[ b ] / (double) n - 0.5 );
		if ( p < pmin )
			pmin = p;
		if ( dev > worst )
			worst = dev;
	}
	sprintf( detail , "worst |ones - 50%%| = %.4f%%, x8" , worst * 100.0 );
	gim_report( "Bit frequency (per position)" , ( pmin * 8.0 > 1.0 ) ? 1.0 : pmin * 8.0 , 0.01 , detail );
}

/* test 3: serial correlation at lags 1..maxlag (Bonferroni over the lags) */
static void gim_serial( const char * name , const double * x , long n , int maxlag ) {
	double mean = 0.0 , var = 0.0 , pmin = 1.0 , rworst = 0.0;
	int worst = 0 , lag;
	long i;
	char detail[96];
	for ( i = 0 ; i < n ; i++ )
		mean += x[ i ];
	mean /= (double) n;
	for ( i = 0 ; i < n ; i++ )
		var += ( x[ i ] - mean ) * ( x[ i ] - mean );
	for ( lag = 1 ; lag <= maxlag ; lag++ ) {
		double num = 0.0 , r , z , p;
		for ( i = 0 ; i + lag < n ; i++ )
			num += ( x[ i ] - mean ) * ( x[ i + lag ] - mean );
		r = num / var;
		z = r * sqrt( (double) ( n - lag ) );
		p = gim_normal_p( z );
		if ( p < pmin ) {
			pmin = p;
			worst = lag;
			rworst = r;
		}
	}
	sprintf( detail , "worst lag %d: r = %+.5f, x%d" , worst , rworst , maxlag );
	gim_report( name , ( pmin * maxlag > 1.0 ) ? 1.0 : pmin * maxlag , 0.01 , detail );
}

/* test 4: rank of 32x32 binary matrices built from 32 consecutive 32-bit words
   (probabilities of rank 32 / 31 / <=30 for random 32x32 matrices) */
static int gim_rank32( unsigned int * rows ) {
	unsigned int m[32];
	int r = 0 , col , i;
	memcpy( m , rows , sizeof( m ) );
	for ( col = 31 ; col >= 0 ; col-- ) {
		int piv = -1 ;
		unsigned int t;
		for ( i = r ; i < 32 ; i++ )
			if ( ( m[ i ] >> col ) & 1u ) {
				piv = i;
				break;
			}
		if ( piv < 0 )
			continue;
		t = m[ piv ];
		m[ piv ] = m[ r ];
		m[ r ] = t;
		for ( i = 0 ; i < 32 ; i++ )
			if ( i != r && ( ( m[ i ] >> col ) & 1u ) )
				m[ i ] ^= m[ r ];
		r++;
	}
	return r;
}

static void gim_test_rank( const unsigned char * buf , long n ) {
	long words = n / 4 , matrices = words / 32 , k;
	double e32 = 0.2888 * matrices , e31 = 0.5776 * matrices , e30 = 0.1336 * matrices;
	double chi;
	long c32 = 0 , c31 = 0 , cless = 0;
	char detail[80];
	unsigned int rows[32];
	for ( k = 0 ; k < matrices ; k++ ) {
		int j , rk;
		for ( j = 0 ; j < 32 ; j++ ) {
			long o = ( k * 32 + j ) * 4;
			rows[ j ] = (unsigned int) buf[ o ] | ( (unsigned int) buf[ o + 1 ] << 8 ) |
			            ( (unsigned int) buf[ o + 2 ] << 16 ) | ( (unsigned int) buf[ o + 3 ] << 24 );
		}
		rk = gim_rank32( rows );
		if ( rk == 32 ) c32++;
		else if ( rk == 31 ) c31++;
		else cless++;
	}
	chi = ( c32 - e32 ) * ( c32 - e32 ) / e32 + ( c31 - e31 ) * ( c31 - e31 ) / e31 + ( cless - e30 ) * ( cless - e30 ) / e30;
	sprintf( detail , "%ld matrices, rank32 %ld rank31 %ld <=30 %ld" , matrices , c32 , c31 , cless );
	gim_report( "Binary matrix rank (GF2)" , gim_chi2_p( chi , 2 ) , 0.01 , detail );
}

/* test 5: runs of bits (NIST SP 800-22, 2.3) */
static void gim_test_runs( const unsigned char * buf , long n ) {
	long nbits = n * 8 , ones = 0 , runs = 0 , i;
	int bit , prev = -1 , b;
	double pi , tau , num , den , p;
	char detail[80];
	for ( i = 0 ; i < n ; i++ )
		for ( b = 0 ; b < 8 ; b++ )
			ones += ( buf[ i ] >> b ) & 1;
	pi = (double) ones / (double) nbits;
	tau = 2.0 / sqrt( (double) nbits );
	if ( fabs( pi - 0.5 ) >= tau ) {
		sprintf( detail , "monobit prerequisite failed (pi %.5f)" , pi );
		gim_report( "Runs of bits" , 0.0 , 0.01 , detail );
		return;
	}
	for ( i = 0 ; i < n ; i++ )
		for ( b = 0 ; b < 8 ; b++ ) {
			bit = ( buf[ i ] >> b ) & 1;
			if ( bit != prev )
				runs++;
			prev = bit;
			if ( b == 7 && ( i & 0xFFFF ) == 0 ) gim_tick( i , n );
		}
	num = fabs( (double) runs - 2.0 * (double) nbits * pi * ( 1.0 - pi ) );
	den = 2.0 * sqrt( 2.0 * (double) nbits ) * pi * ( 1.0 - pi );
	p = erfc( num / den );
	sprintf( detail , "runs %ld (exp ~%.0f), pi-1/2 = %+.5f" , runs , 2.0 * (double) nbits * pi * ( 1.0 - pi ) + 1.0 , pi - 0.5 );
	gim_report( "Runs of bits" , p , 0.01 , detail );
}

/* test 6: Shannon entropy of the bytes (ideal 8 bits); PASS when H >= 7.999 */
static void gim_test_entropy( const unsigned char * buf , long n ) {
	unsigned long cnt[256];
	double h = 0.0;
	long i;
	int k;
	char detail[64];
	memset( cnt , 0 , sizeof( cnt ) );
	for ( i = 0 ; i < n ; i++ )
		{ cnt[ buf[ i ] ]++; if ( ( i & 0xFFFF ) == 0 ) gim_tick( i , n ); }
	for ( k = 0 ; k < 256 ; k++ )
		if ( cnt[ k ] ) {
			double p = (double) cnt[ k ] / (double) n;
			h -= p * log( p ) / log( 2.0 );
		}
	sprintf( detail , "H = %.5f bits (max 8)" , h );
	gim_out( "%-30s %-44s %s\n" , "Shannon entropy (bytes)" , detail , ( h >= 7.999 ) ? "PASS" : "FAIL" );
}

/* linear complexity (Berlekamp-Massey over GF(2), bits packed in 64-bit words) */
static void gim_bs_shl1( unsigned long long * r , int w ) {
	int j;
	for ( j = w - 1 ; j > 0 ; j-- )
		r[ j ] = ( r[ j ] << 1 ) | ( r[ j - 1 ] >> 63 );
	r[ 0 ] <<= 1;
}

static int gim_bs_parity_and( const unsigned long long * a , const unsigned long long * b , int w ) {
	unsigned long long x = 0;
	int j;
	for ( j = 0 ; j < w ; j++ )
		x ^= a[ j ] & b[ j ];
	return (int) ( __builtin_popcountll( x ) & 1 );
}

static void gim_bs_xor_shl( unsigned long long * c , const unsigned long long * b , int k , int w ) {
	int q = k / 64 , r = k % 64 , j;
	for ( j = w - 1 ; j >= q ; j-- ) {
		unsigned long long v = b[ j - q ] << r;
		if ( r && ( j - q - 1 ) >= 0 )
			v |= b[ j - q - 1 ] >> ( 64 - r );
		c[ j ] ^= v;
	}
}

static long gim_linear_complexity( const unsigned char * s , long N ) {
	int w = (int) ( N / 64 + 2 );
	unsigned long long * C = (unsigned long long *) gim_mem_alloc( w * sizeof( unsigned long long ) );
	unsigned long long * B = (unsigned long long *) gim_mem_alloc( w * sizeof( unsigned long long ) );
	unsigned long long * T = (unsigned long long *) gim_mem_alloc( w * sizeof( unsigned long long ) );
	unsigned long long * R = (unsigned long long *) gim_mem_alloc( w * sizeof( unsigned long long ) );
	long L = 0 , m = -1 , n;
	C[ 0 ] = 1;
	B[ 0 ] = 1;
	for ( n = 0 ; n < N ; n++ ) {
		gim_bs_shl1( R , w );
		R[ 0 ] |= s[ n ];
		if ( gim_bs_parity_and( C , R , w ) ) {
			memcpy( T , C , w * sizeof( unsigned long long ) );
			gim_bs_xor_shl( C , B , (int) ( n - m ) , w );
			if ( 2 * L <= n ) {
				L = n + 1 - L;
				memcpy( B , T , w * sizeof( unsigned long long ) );
				m = n;
			}
		}
	}
	gim_mem_free( C );
	gim_mem_free( B );
	gim_mem_free( T );
	gim_mem_free( R );
	return L;
}

#define GIM_LC_M	40000

/* test 7: linear complexity of bit 0 of each 32-bit word, in blocks of GIM_LC_M bits
   (NIST SP 800-22, 2.10). A linear generator such as the original Mersenne Twister
   has linear complexity 19937, far from M/2: the test fails. */
static void gim_test_linear( const unsigned char * buf , long n ) {
	long nw = n / 4 , blocks = nw / GIM_LC_M , b , j;
	if ( blocks > 16 ) blocks = 16;   /* sample: the cost is quadratic in the block size */
	double mu = GIM_LC_M / 2.0 + 2.0 / 9.0 - ( GIM_LC_M / 3.0 + 2.0 / 9.0 ) / pow( 2.0 , GIM_LC_M );
	static const double pi[ 7 ] = { 0.010417 , 0.03125 , 0.125 , 0.5 , 0.25 , 0.0625 , 0.020833 };
	long v[ 7 ];
	double chi = 0.0 , lmean = 0.0;
	unsigned char * seq;
	char detail[80];
	int k;
	memset( v , 0 , sizeof( v ) );
	if ( blocks < 1 ) {
		gim_out( "Linear complexity          too few words for one block\n" );
		return;
	}
	seq = (unsigned char *) gim_mem_alloc( GIM_LC_M );
	for ( b = 0 ; b < blocks ; b++ ) {
		long L;
		for ( j = 0 ; j < GIM_LC_M ; j++ )
			seq[ j ] = buf[ ( b * GIM_LC_M + j ) * 4 ] & 1;
		L = gim_linear_complexity( seq , GIM_LC_M );
		gim_tick( b , blocks );
		lmean += (double) L;
		{
			double T = (double) L - mu + 2.0 / 9.0;
			int bin;
			if ( T <= -2.5 ) bin = 0;
			else if ( T <= -1.5 ) bin = 1;
			else if ( T <= -0.5 ) bin = 2;
			else if ( T <= 0.5 ) bin = 3;
			else if ( T <= 1.5 ) bin = 4;
			else if ( T <= 2.5 ) bin = 5;
			else bin = 6;
			v[ bin ]++;
		}
	}
	gim_mem_free( seq );
	for ( k = 0 ; k < 7 ; k++ ) {
		double e = blocks * pi[ k ];
		chi += ( v[ k ] - e ) * ( v[ k ] - e ) / e;
	}
	sprintf( detail , "%ld blocks of %d bits, mean L %.0f (expected ~%.0f)" , blocks , GIM_LC_M , lmean / blocks , mu );
	gim_report( "Linear complexity (bit 0)" , gim_chi2_p( chi , 6 ) , 0.01 , detail );
}

static int gim_stderr_is_terminal( void ) {
#ifdef _WIN32
	return _isatty( _fileno( stderr ) );
#else
	return isatty( fileno( stderr ) );
#endif
}

/* -q (minimal): only the GKaos line reaches the screen, as a single line. stdout and stderr go to the null device
   and the line is written to the original stdout. The Gim log is not affected: it stays complete. */
static int gs_minimal = 0;
static int gs_out = 1;

static void gs_write_out( const char * text ) {
#ifdef _WIN32
	_write( gs_out , text , (unsigned int) strlen( text ) );
#else
	(void) write( gs_out , text , strlen( text ) );
#endif
}

static void gim_stat_say( const char * text ) {
	if ( !gs_minimal ) {
		puts( text );
		return;
	}
	gs_write_out( text );
	gs_write_out( "\n" );
}

static void gim_stat_minimal_on( void ) {
	fflush( stdout );
	fflush( stderr );
#ifdef _WIN32
	gs_out = _dup( 1 );
	int nul = _open( "NUL" , _O_WRONLY );
	_dup2( nul , 1 );
	_dup2( nul , 2 );
	_close( nul );
#else
	gs_out = dup( 1 );
	int nul = open( "/dev/null" , O_WRONLY );
	dup2( nul , 1 );
	dup2( nul , 2 );
	close( nul );
#endif
}

static void gim_stat_minimal_result( double score ) {
	char line[ 64 ];
	if ( !gs_minimal )
		return;
	if ( score < 0 )
		snprintf( line , sizeof( line ) , "GKaos : n/a (file too small for the analysis)" );
	else
		snprintf( line , sizeof( line ) , "GKaos : %5.1f / 100" , score );
	gim_stat_say( line );
}

static void gim_draw_bar( const std::vector< gim_test_ctx > & ctx ) {
	long sum = 0;
	char bar[ 41 ];
	double frac;
	int i , filled;
	for ( size_t k = 0 ; k < ctx.size() ; k++ )
		sum += ctx[ k ].progress.load();
	frac = (double) sum / ( 1000.0 * (double) ctx.size() );
	filled = (int) ( frac * 40.0 );
	for ( i = 0 ; i < 40 ; i++ )
		bar[ i ] = ( i < filled ) ? '#' : '.';
	bar[ 40 ] = '\0';
	fprintf( stderr , "\r  [%s] %3.0f%%  " , bar , frac * 100.0 );
	fflush( stderr );
}

static void gim_draw_progress( double frac , const char * label ) {
	char bar[ 41 ];
	int i , filled = (int) ( frac * 40.0 );
	for ( i = 0 ; i < 40 ; i++ )
		bar[ i ] = ( i < filled ) ? '#' : '.';
	bar[ 40 ] = '\0';
	fprintf( stderr , "\r  [%s] %3.0f%%  %s  " , bar , frac * 100.0 , label );
	fflush( stderr );
}

static void gim_clear_bar( void ) {
	fprintf( stderr , "\r%70s\r" , "" );
	fflush( stderr );
}
static thread_local int gim_block_worker = 0;   /* 1 inside a block worker: its tests run in turn */

/* how many threads may run at once: never more than the logical processors, and never more than the memory
   budget allows: free RAM x memory::limit (the same ceiling Gim applies to its own allocations).
   per_job is the memory one job needs at its peak. */
static int gim_pool_size( int max_jobs , long long per_job ) {
	int hw = gim_identity->n_proc();
	int pct = 75;
	long long budget , fit;
	int n;
	if ( hw < 1 )
		hw = (int) std::thread::hardware_concurrency();
	if ( hw < 1 )
		hw = 1;
	IF_EXIST_KEY_IN_CONF( "memory" , "limit" )
		pct = gim_conf->GetKeyINT( "memory" , "limit" );
	budget = (long long) gim_identity->mi->free * pct / 100;
	fit = ( per_job > 0 ) ? budget / per_job : (long long) max_jobs;
	n = max_jobs;
	if ( hw < n )
		n = hw;
	if ( fit < n )
		n = (int) ( fit < 1 ? 1 : fit );
	if ( n < 1 )
		n = 1;
	static int last_max = -1 , last_n = -1;   /* log only when the sizing changes */
	static long long last_per = -1;
	if ( max_jobs != last_max || per_job != last_per || n != last_n ) {
		gim_error->app( GIM_ERROR_MESSAGE , "gimstat" , "threads: %d for %d jobs (logical processors %d, memory budget %lld MB, %lld MB per job)" , n , max_jobs , hw , budget / 1048576 , per_job / 1048576 );
		last_max = max_jobs;  last_per = per_job;  last_n = n;
	}
	return n;
}
static const char * const gim_test_names[ 8 ] = { "byte chi-square" , "bit frequency" , "serial correlation (bytes)" , "serial correlation (words)" , "binary matrix rank" , "runs of bits" , "shannon entropy" , "linear complexity" };

/* threads are used for big files only if the configuration allows the class (thread::gim_stat = YES, default).
   Decided once, in the main thread, so that the log gets one message and not one per block. */
static int gim_stat_threads_allowed( void ) {
	static int allowed = -1;
	if ( allowed < 0 )
		allowed = ( gim_thread_allowed( "gim_stat" ) == __GIM_YES ) ? 1 : 0;
	return allowed;
}
static double gim_stat_tests( const unsigned char * buf , long n ) {
		int npv = 0;  double pv[ 32 ];
	if ( !gim_quiet ) puts( "Statistical tests (p-value, PASS if p >= 0.01)" );
	if ( n < 8192 ) {
		if ( !gim_quiet ) puts( "file too small for the tests (at least 8192 bytes)\n" );
		return 0.0;
	}
	{
		const int NJ = 8;
		std::vector< std::function< void() > > jobs;
		std::vector< gim_test_ctx > ctx( NJ );
		std::vector< std::thread > th;
		bool parallel = ( n >= GIM_PARALLEL_MIN_BYTES ) && !gim_block_worker && gim_stat_threads_allowed();
		int workers = parallel ? gim_pool_size( NJ , 11LL * n ) : 1;
		parallel = ( workers > 1 );
		bool show = parallel && !gim_quiet && gim_stderr_is_terminal();
		int k;

		jobs.push_back( [ buf , n ]() { gim_test_bytes( buf , n ); } );
		jobs.push_back( [ buf , n ]() { gim_test_bits( buf , n ); } );
		jobs.push_back( [ buf , n ]() {
			double * x = (double *) gim_mem_alloc( sizeof( double ) * n );
			for ( long i = 0 ; i < n ; i++ )
				x[ i ] = (double) buf[ i ] / 256.0;
			gim_serial( "Serial correlation (bytes)" , x , n , 8 );
			gim_mem_free( x );
		} );
		jobs.push_back( [ buf , n ]() {
			long nw = n / 4 , i;
			double * x = (double *) gim_mem_alloc( sizeof( double ) * nw );
			unsigned int * w = (unsigned int *) gim_mem_alloc( sizeof( unsigned int ) * nw );
			for ( i = 0 ; i < nw ; i++ ) {
				long o = i * 4;
				w[ i ] = (unsigned int) buf[ o ] | ( (unsigned int) buf[ o + 1 ] << 8 ) |
				         ( (unsigned int) buf[ o + 2 ] << 16 ) | ( (unsigned int) buf[ o + 3 ] << 24 );
				x[ i ] = (double) w[ i ] / 4294967296.0;
			}
			gim_serial( "Serial correlation (words)" , x , nw , 4 );
			gim_mem_free( x );
			gim_mem_free( w );
		} );
		jobs.push_back( [ buf , n ]() { gim_test_rank( buf , n ); } );
		jobs.push_back( [ buf , n ]() { gim_test_runs( buf , n ); } );
		jobs.push_back( [ buf , n ]() { gim_test_entropy( buf , n ); } );
		jobs.push_back( [ buf , n ]() { gim_test_linear( buf , n ); } );

		for ( k = 0 ; k < NJ ; k++ )
			ctx[ k ].quiet = gim_quiet;

		auto run_job = [ &jobs , &ctx , n ]( int j ) {
			gim_ctx = &ctx[ j ];
			gim_error->app( GIM_ERROR_MESSAGE , "gimstat_test" , "%s: start (%ld bytes)" , gim_test_names[ j ] , n );
			jobs[ j ]();
			gim_error->app( GIM_ERROR_MESSAGE , "gimstat_test" , "%s: end" , gim_test_names[ j ] );
			gim_ctx = NULL;
			ctx[ j ].progress.store( 1000 );
			ctx[ j ].done.store( true );
		};
		if ( parallel ) {
		std::atomic<int> nxt( 0 );
			for ( int w = 0 ; w < workers ; w++ )
			th.emplace_back( [ & ]() { int j; while ( ( j = nxt++ ) < NJ ) run_job( j ); } );
		}
		/* results are printed in test order, as soon as each test (and the ones before it) is finished */
		for ( k = 0 ; k < NJ ; k++ ) {
			if ( parallel ) {
				while ( !ctx[ k ].done.load() ) {
					if ( show )
						gim_draw_bar( ctx );
					std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
				}
			}
			else
				run_job( k );
			if ( show )
				gim_clear_bar();
			if ( !gim_quiet )
				fputs( ctx[ k ].out.c_str() , stdout );
			for ( int p = 0 ; p < ctx[ k ].npv && npv < 32 ; p++ )
				pv[ npv++ ] = ctx[ k ].pv[ p ];
		}
		for ( auto & x : th )
			x.join();
		fflush( stdout );
	}
	/* single score: Fisher combination of all the p-values above.
	   100 = no anomaly; it falls linearly with the orders of magnitude of the combined p, reaching 0 at p = 1e-300 */
	{
		double logsum = 0.0 , pcomb , l10 , score;
		int k;
		for ( k = 0 ; k < npv ; k++ )
			logsum += -2.0 * log( pv[ k ] > 1e-300 ? pv[ k ] : 1e-300 );
		pcomb = gim_chi2_p( logsum , 2 * npv );
		l10 = -log10( pcomb > 1e-300 ? pcomb : 1e-300 );
		score = 100.0 * ( 1.0 - ( l10 > 300.0 ? 300.0 : l10 ) / 300.0 );
		if ( !gim_quiet ) puts( "" );
		if ( !gim_quiet ) printf( "Score (whole file)    : %5.1f / 100   combined p = %.3g   %s\n\n" , score , pcomb ,
		        ( score >= 90.0 ) ? "PASS" : ( ( score >= 50.0 ) ? "WARNING" : "FAIL" ) );
		fflush( stdout );
		return score;
	}
}


static int gim_cmp_double( const void * a , const void * b );

/* shared output of the block analysis: sc[] = scores in file order, nb blocks of bs bytes */
static double gim_print_blocks( double * sc , int nb , long long bs ) {
	double mean = 0.0 , mn = 100.0 , median;
	int pass = 0 , i;
	double * sorted = (double *) gim_mem_alloc( sizeof( double ) * nb );
	for ( i = 0 ; i < nb ; i++ ) {
		mean += sc[ i ];
		if ( sc[ i ] < mn ) mn = sc[ i ];
		if ( sc[ i ] >= 90.0 ) pass++;
		sorted[ i ] = sc[ i ];
	}
	mean /= nb;
	if ( nb == 100 ) {
		/* map of the blocks: one character per block, in file order; density = criticality */
		static const char ramp[] = " .:-=+*#%@";
		int r , c;
		printf( "\nMap of the blocks (10 x 10, file order; ' ' = score 90-100, '@' = score 0-10)\n" );
		for ( r = 0 ; r < 10 ; r++ ) {
			printf( "  " );
			for ( c = 0 ; c < 10 ; c++ ) {
				int lev = (int) ( ( 100.0 - sc[ r * 10 + c ] ) / 10.0 );
				if ( lev < 0 ) lev = 0;
				if ( lev > 9 ) lev = 9;
				putchar( ramp[ lev ] );
			}
			putchar( '\n' );
		}
		printf( "  legend:  ' ' 90-100   '.' 80-90   ':' 70-80   '-' 60-70   '=' 50-60\n" );
		printf( "           '+' 40-50   '*' 30-40   '#' 20-30   '%%' 10-20   '@' 0-10\n" );
	}
	qsort( sorted , nb , sizeof( double ) , gim_cmp_double );
	median = sorted[ nb / 2 ];
	gim_mem_free( sorted );
	printf( "\nBlock analysis: %d blocks of %lld bytes\n" , nb , bs );
	printf( "  mean score    : %5.1f / 100\n" , mean );
	printf( "  median score  : %5.1f / 100\n" , median );
	printf( "  min score     : %5.1f / 100\n" , mn );
	printf( "  blocks PASS   : %d / %d  (%.0f%%)   score >= 90\n" , pass , nb , 100.0 * pass / nb );
	printf( "\nGKaos          : %5.1f / 100\n\n" , mean );
	fflush( stdout );
	return mean;
}

/* GKaos: the file is split in nb blocks, every block gets its own score (the same tests);
   the GKaos is the mean of the block scores. */
static int gim_cmp_double( const void * a , const void * b ) {
	double x = *(const double *) a , y = *(const double *) b;
	return ( x < y ) ? -1 : ( x > y ) ? 1 : 0;
}

/* runs work(0..nb-1) on a pool of threads; the main thread only draws the progress bar */
static void gim_run_blocks( int nb , int workers , bool show , const std::function< void( int ) > & work ) {
	std::atomic<int> next( 0 ) , finished( 0 );
	std::vector< std::thread > th;
	if ( workers < 1 )
		workers = 1;
	if ( workers > nb )
		workers = nb;
	auto loop = [ & ]() {
		int i;
		gim_block_worker = 1;
		while ( ( i = next++ ) < nb ) {
			work( i );
			finished++;
		}
		gim_block_worker = 0;
	};
	for ( int w = 0 ; w < workers ; w++ )
		th.emplace_back( loop );
	if ( show ) {
		while ( finished.load() < nb ) {
			gim_draw_progress( (double) finished.load() / (double) nb , "blocks" );
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
		}
		gim_clear_bar();
	}
	for ( auto & x : th )
		x.join();
}
/* Block analysis. Below GIM_LC_MIN_BYTES the blocks have a fixed size of GIM_BLOCK_MIN_BYTES, spread evenly over the file;
   their number is n / GIM_BLOCK_MIN_BYTES, at most nb. From GIM_LC_MIN_BYTES on the file is cut into nb contiguous blocks,
   so that each block is big enough for the linear complexity test (gim_test_linear needs 40000 words per block). */
#define GIM_BLOCK_MIN_BYTES	8192
#define GIM_LC_MIN_BYTES	( 16L * 1024 * 1024 )
static double gim_stat_blocks( const unsigned char * buf , long n , int nb_req ) {
	double * sc;
	long bs , stride;
	int nb;
	double mean;
	if ( n < GIM_BLOCK_MIN_BYTES ) {
		puts( "file too small for the block analysis" );
		return -1.0;   /* not evaluable: -1 (exit code 255), not a score */
	}
	if ( n >= GIM_LC_MIN_BYTES ) {
		nb = nb_req;
		bs = n / nb;
		stride = bs;
	}
	else {
		nb = (int) ( n / GIM_BLOCK_MIN_BYTES );
		if ( nb > nb_req )
			nb = nb_req;
		bs = GIM_BLOCK_MIN_BYTES;
		stride = n / nb;
	}
	sc = (double *) gim_mem_alloc( sizeof( double ) * nb );
	gim_stat_threads_allowed();   /* decided here, in the main thread */
	gim_quiet = 1;
	gim_run_blocks( nb , gim_pool_size( nb , 11LL * bs ) , gim_stderr_is_terminal() != 0 , [ & ]( int j ) {
		gim_error->app( GIM_ERROR_MESSAGE , "gimstat_block" , "block %d of %d: start (%ld bytes)" , j + 1 , nb , bs );
		sc[ j ] = gim_stat_tests( buf + (long) j * stride , bs );
		gim_error->app( GIM_ERROR_MESSAGE , "gimstat_block" , "block %d of %d: end, score %.1f" , j + 1 , nb , sc[ j ] );
	} );
	gim_quiet = 0;
	mean = gim_print_blocks( sc , nb , bs );
	gim_mem_free( sc );
	return mean;
}

/* files too big to be loaded: nb samples of up to 128 MB, spread over the whole file, read from disk */
static long long gim_file_size64( const char * path ) {
	FILE * f = fopen( path , "rb" );
	long long size;
	if ( f == NULL )
		return -1;
	fseeko64( f , 0 , SEEK_END );
	size = (long long) ftello64( f );
	fclose( f );
	return size;
}

static double gim_stat_big( const char * path , long long size ) {
	const int nb = 100;
	const long long maxb = 128LL * 1024 * 1024;
	long long stride = size / nb;
	long bs = (long) ( stride < maxb ? stride : maxb );
	unsigned char * buf = (unsigned char *) gim_mem_alloc( bs );
	double * sc = (double *) gim_mem_alloc( sizeof( double ) * nb );
	FILE * f = fopen( path , "rb" );
	int i;
	bool show = gim_stderr_is_terminal() != 0;
	printf( "file of %lld bytes: %d samples of %ld bytes spread over the file\n" , size , nb , bs );
	fflush( stdout );
	for ( i = 0 ; i < nb ; i++ ) {
		size_t got;
		fseeko64( f , (off64_t) ( i * stride ) , SEEK_SET );
		got = fread( buf , 1 , (size_t) bs , f );
		gim_error->app( GIM_ERROR_MESSAGE , "gimstat_sample" , "sample %d of %d: start at offset %lld (%lu bytes)" , i + 1 , nb , (long long) ( i * stride ) , (unsigned long) got );
		gim_quiet = 1;
		sc[ i ] = gim_stat_tests( buf , (long) got );
		gim_quiet = 0;
		gim_error->app( GIM_ERROR_MESSAGE , "gimstat_sample" , "sample %d of %d: end, score %.1f" , i + 1 , nb , sc[ i ] );
		if ( show )
			gim_draw_progress( (double) ( i + 1 ) / nb , "samples" );
	}
	if ( show )
		gim_clear_bar();
	fclose( f );
	gim_mem_free( buf );
	{
		double mean = gim_print_blocks( sc , nb , bs );
		gim_mem_free( sc );
		return mean;
	}
}

int main ( int argc , char *argv[] ) {
	if ( argc < 2 ) {
		puts( "gimstat V0.1 usage :" );
		puts( "gimstat filename \n" );
		puts( "gimstat -q filename   (only the GKaos score, one line; the log stays complete)\n" );
		puts( "gimstat coded by asyntote" );
		puts( "asyntote@gkript.org" );
		exit( -1 );
	}

	gim_set_application_name( "Gimstat" );
	gim_set_application_version( "0.1" );

	gim_obj		* gim = new gim_obj;

	/* options through the Gim scanner (gim_getopt_obj). The scanner puts the options first: the file name is then argv[ optind ] */
	gim_getopt_obj * gsopt = new gim_getopt_obj;
	gsopt->scanopt( argc , argv , ":q" );
	gs_minimal = ( gsopt->search( 'q' ) == __GIM_YES );
	if ( optind > 1 ) {
		argv[ optind - 1 ] = argv[ 0 ];
		argv += optind - 1;
		argc -= optind - 1;
	}
	delete gsopt;
	if ( argc < 2 ) {
		puts( "gimstat filename  |  gimstat -q filename" );
		delete gim;
		exit( -1 );
	}
	if ( gs_minimal && ( argc > 2 ) ) {
		puts( "gimstat -q works on one file only" );
		delete gim;
		exit( -1 );
	}
	if ( gs_minimal )
		gim_stat_minimal_on();

	gim_error->app( GIM_ERROR_MESSAGE , "gimstat" , "run start: %s (%lld bytes)%s" , argv[ 1 ] , gim_file_size64( argv[ 1 ] ) , gs_minimal ? ", minimal output" : "" );
	auto gim_t0 = std::chrono::steady_clock::now();

	/* files over 1 GiB: sampled from disk; done after the Gim environment is up (gim_error, conf and memory are used) */
	{
		long long fsz = gim_file_size64( argv[1] );
		if ( fsz > (long long) 1 << 30 ) {
			double g = gim_stat_big( argv[1] , fsz );
			gim_error->app( GIM_ERROR_MESSAGE , "gimstat" , "run end: exit %d, %.1f s" , (int) g , std::chrono::duration< double >( std::chrono::steady_clock::now() - gim_t0 ).count() );
			delete gim;
			gim_stat_minimal_result( g );
			exit( (int) g );
		}
	}

	
	gim_utils_obj   *util = new gim_utils_obj;
	char	*ch;
	unsigned char nch=0;
	_gim_handler	* source;

	unsigned int gstat[256];
	unsigned int rsize=0;
	int gvarianza[256];

	float gqcaos_extension=0.0;
	float gqcaos_distribuition=0.0;
	float gqcaos_levelling=0.0;
	float gqcaos=0.0;
	
	int qgvarianza[4];
	int qgmedie[4];
	int gcaos[4];

	_gim_Uint32 c = 0;
		
	unsigned int gmin=0,gmax=0,gused=0,gdelta=0;
	char chmax , chmin;
	unsigned int cntrl=0,graph=0,space=0,others=0;

	_gim_flag   multisin=0;

	if( gim->file_manager->exist( argv[1]) == __GIM_NOT_EXIST ) {
		gim_stat_say( "File not exists" );
		delete gim;
		exit(1);
	}

	if ( argc > 2 ) {
		multisin = 1;

		for ( _gim_Uint32 e = 1 ; e < argc ; e++ ) {

			if( util->isregular( argv[e] ) ) {
			
				memset( gstat , 0 , 256*sizeof(unsigned int) );
				memset( gvarianza , 0 , 256*sizeof(int) );
				memset( qgvarianza , 0 , 4*sizeof( int) );
				memset( qgmedie , 0 , 4*sizeof( int) );
				memset( gcaos , 0 , 4*sizeof( int) );

				cntrl=0;
				graph=0;
				space=0;
				others=0;

				gused = 0;
				gmax = 0;
				gmin = 0;
	
				source = gim->file_manager->open( argv[e] , __GIM_FILE_MAPPING , __GIM_READ );

				printf("%60s" , Lexical.string_trunc( argv[e] , 60 ));
				fflush(stdout);

				ch = source->fm;	

				for ( c = 0 ; c < source->size ; c++) {
					nch = *ch++;
					gstat[ nch ]++;
				} 

				for ( c = 0 ; c < 256 ;c++) {
					rsize += gstat[c];  
				}

				for ( c = 0 ; c < 256 ;c++) {
					gvarianza[ c ] = (source->size / 256) - gstat[c];
					if ( iscntrl(c) ) cntrl += gstat[c];
					if ( isgraph(c) ) graph += gstat[c];
					if ( c == ' ' ) space += gstat[c];
					if ( ( ! iscntrl(c) ) && ( ! isgraph(c) ) && ( c != ' ' ) ) others += gstat[c];
				}

				for ( c = 0 ; c < 256 ;c++) {
					  if ( gstat[c]!=0 )
						gused++;
				}
	
				for ( c = 0 ; c < 256 ; c++) {
					 if ( gstat[c] > gmax ) {
						gmax = gstat[c];
					 }
				}

				gmin = gmax;
	
				for ( c = 0 ; c < 256 ; c++) {
					if (( gstat[c] > 0 ) && (gstat[c] < gmin)) {
						gmin = gstat[c];
					}
				}

				gdelta = gmax - gmin;

				for ( c = 0 ; c < 64 ; c++ ) {
					qgvarianza[0] += gvarianza[c];
					qgvarianza[1] += gvarianza[c+64];
					qgvarianza[2] += gvarianza[c+128];
					qgvarianza[3] += gvarianza[c+192];
					qgmedie[0] += gstat[c];
					qgmedie[1] += gstat[c+64];
					qgmedie[2] += gstat[c+128];
					qgmedie[3] += gstat[c+192];
				}

				qgvarianza[0] = abs((int)(qgvarianza[0]/64) - ((int)(source->size/4) / (int)gused));
				qgvarianza[1] = abs((int)(qgvarianza[1]/64) - ((int)(source->size/4) / (int)gused));
				qgvarianza[2] = abs((int)(qgvarianza[2]/64) - ((int)(source->size/4) / (int)gused));
				qgvarianza[3] = abs((int)(qgvarianza[3]/64) - ((int)(source->size/4) / (int)gused));

				qgmedie[0] /= 64;
				qgmedie[1] /= 64;
				qgmedie[2] /= 64;
				qgmedie[3] /= 64;
	
/*				gcaos[0] = abs((qgmedie[0]*100)/abs((source->size) / gused));
				gcaos[1] = abs((qgmedie[1]*100)/abs((source->size) / gused));
				gcaos[2] = abs((qgmedie[2]*100)/abs((source->size) / gused));
				gcaos[3] = abs((qgmedie[3]*100)/abs((source->size) / gused));
*/		 
				gcaos[0] = abs( (int)100 - abs((int)(qgmedie[0]*100)/abs((int)(source->size) / (int)gused)));
				gcaos[1] = abs( (int)100 - abs((int)(qgmedie[1]*100)/abs((int)(source->size) / (int)gused)));
				gcaos[2] = abs( (int)100 - abs((int)(qgmedie[2]*100)/abs((int)(source->size) / (int)gused)));
				gcaos[3] = abs( (int)100 - abs((int)(qgmedie[3]*100)/abs((int)(source->size) / (int)gused)));

				gqcaos_extension = (float)((float)gused*100.00)/256.00;
				gqcaos_distribuition = (float)(100.00 - (float)((float)( (float)gdelta / (float)gmax ) * 100.0));
				gqcaos_levelling = (float)abs( 100.00 - (float)( (float)( (float)gcaos[0] + (float)gcaos[1] + (float)gcaos[2] + (float)gcaos[3]) / 4.00) );
				gqcaos = (float)((float)((float)gqcaos_extension + (float)gqcaos_distribuition + (float)gqcaos_levelling) / 3.00);

				printf("  -  gKaos : %3.1f%%\n" , gqcaos);

				if( source->size < MIN_SIZE) {
					puts("    WARNING:The file size is too small. The result for this file is not reliable");
				}

				gim->file_manager->close (source);
			}
		}
	}
	else { 
		multisin = -1;

		printf("analyzing ststistics for %s...\n\n" , argv[1]);
		fflush (stdout);

		memset( gstat , 0 , 256*sizeof(unsigned int) );
		memset( gvarianza , 0 , 256*sizeof(int) );
		memset( qgvarianza , 0 , 4*sizeof( int) );
		memset( qgmedie , 0 , 4*sizeof( int) );
		memset( gcaos , 0 , 4*sizeof( int) );
	
		source = gim->file_manager->open( argv[1] , __GIM_FILE_MAPPING , __GIM_READ );

		if( source->size < MIN_SIZE) {
			puts("WARNING:");
			puts("The file size is too small");
			puts("The results are not reliable\n");
		}

		printf("File size     : %d\n\n" , source->size );
		fflush (stdout);

	//	gim->file_manager->load ( source );
	
	//	ch = source->loadp;
		ch = source->fm;	

		for ( c = 0 ; c < source->size ; c++) {
			nch = *ch++;
			gstat[ nch ]++;
		} 

		for ( c = 0 ; c < 256 ;c++) {
			rsize += gstat[c];  
		}

		for ( c = 0 ; c < 256 ;c++) {
			gvarianza[ c ] = (source->size / 256) - gstat[c];
			if ( iscntrl(c) ) cntrl += gstat[c];
			if ( isgraph(c) ) graph += gstat[c];
			if ( c == ' ' ) space += gstat[c];
			if ( ( ! iscntrl(c) ) && ( ! isgraph(c) ) && ( c != ' ' ) ) others += gstat[c];
		}

	/*	for ( c = 0 ; c < 256 ;c++) {
			if ( iscntrl(c) ) cntrl += gstat[c];  
		}

		for ( c = 0 ; c < 256 ;c++) {
			if ( isgraph(c) ) graph += gstat[c];
		}

		for ( c = 0 ; c < 256 ;c++) {
			if ( c == ' ' ) space += gstat[0];
		}

		for ( c = 0 ; c < 256 ;c++) {
			if ( ( ! iscntrl(c) ) && ( ! isgraph(c) ) && ( c != ' ' ) ) others += gstat[c];
		}
	*/
		for ( c = 0 ; c < 256 ;c++) {
			  if ( gstat[c]!=0 )
				gused++;
		}
	
		for ( c = 0 ; c < 256 ; c++) {
			if ( gstat[c] > gmax ) {
				gmax = gstat[c];
				chmax = c;
	//			printf("M %u\n",gmax);
			 }
		}

		gmin = gmax;
	
		for ( c = 0 ; c < 256 ; c++) {
			if (( gstat[c] > 0 ) && (gstat[c] < gmin)) {
				gmin = gstat[c];
				chmin = c;
	//			printf("m %u\n",gmin);
			}
		}

		gdelta = gmax - gmin;

	//	unsigned int	cntrl=0 , graph=0 , space=0 , others=0;
		puts("Character statistics");
		printf("Control       : (%3.1f%%) %d\n",   (float)((float)cntrl/(float)source->size)*100.00 , cntrl );
		printf("Printable     : (%3.1f%%) %d\n",   (float)((float)graph/(float)source->size)*100.00 ,graph );
		printf("Spaces        : (%3.1f%%) %d\n",   (float)((float)space/(float)source->size)*100.00 ,space );
		printf("Others        : (%3.1f%%) %d\n\n", (float)((float)others/(float)source->size)*100.00 , others);

		puts("Extension and Distribuition statistics");
		printf("Used          : %10u\nMin times     : %10u - %3d\nMax times     : %10u - %3d\nDelta         : %10u\n\n"  , gused, gmin , (unsigned char)chmin , gmax , (unsigned char)chmax , gdelta);

		fflush (stdout);

		for ( c = 0 ; c < 64 ; c++ ) {
			qgvarianza[0] += gvarianza[c];
			qgvarianza[1] += gvarianza[c+64];
			qgvarianza[2] += gvarianza[c+128];
			qgvarianza[3] += gvarianza[c+192];
			qgmedie[0] += gstat[c];
			qgmedie[1] += gstat[c+64];
			qgmedie[2] += gstat[c+128];
			qgmedie[3] += gstat[c+192];
		}

		qgvarianza[0] = abs((int)(qgvarianza[0]/64) - ((int)(source->size/4) / (int)gused));
		qgvarianza[1] = abs((int)(qgvarianza[1]/64) - ((int)(source->size/4) / (int)gused));
		qgvarianza[2] = abs((int)(qgvarianza[2]/64) - ((int)(source->size/4) / (int)gused));
		qgvarianza[3] = abs((int)(qgvarianza[3]/64) - ((int)(source->size/4) / (int)gused));

		qgmedie[0] /= 64;
		qgmedie[1] /= 64;
		qgmedie[2] /= 64;
		qgmedie[3] /= 64;
	
		gcaos[0] = abs((int)(qgmedie[0]*100)/abs((int)(source->size) / (int)gused));
		gcaos[1] = abs((int)(qgmedie[1]*100)/abs((int)(source->size) / (int)gused));
		gcaos[2] = abs((int)(qgmedie[2]*100)/abs((int)(source->size) / (int)gused));
		gcaos[3] = abs((int)(qgmedie[3]*100)/abs((int)(source->size) / (int)gused));

	//	CH used : 256 - CH min : 125286 - CH max : 126860 - CH delta : 1574
	//	Blk 1   : 099%- Blk 2  99%   - Blk 3 100%   - Blk 4  99%
	//  Blk 4    :
	//	Mean value :
		puts("Leveling statistics");
		printf("Average value : %u\n" , abs((int)(source->size) / (int)gused));
		printf("Blk 1         : %3.2f%%\n", (float)((float)qgmedie[0]*100.00)/(float)abs((source->size) / (float)gused) );
		printf("Blk 2         : %3.2f%%\n", (float)((float)qgmedie[1]*100.00)/(float)abs((source->size) / (float)gused) );
		printf("Blk 3         : %3.2f%%\n", (float)((float)qgmedie[2]*100.00)/(float)abs((source->size) / (float)gused) );
		printf("Blk 4         : %3.2f%%\n\n", (float)((float)qgmedie[3]*100.00)/(float)abs((source->size) / (float)gused) );
		fflush (stdout);
	 
		gcaos[0] = abs( (int)100 - abs((int)(qgmedie[0]*100)/abs((int)(source->size) / (int)gused)));
		gcaos[1] = abs( (int)100 - abs((int)(qgmedie[1]*100)/abs((int)(source->size) / (int)gused)));
		gcaos[2] = abs( (int)100 - abs((int)(qgmedie[2]*100)/abs((int)(source->size) / (int)gused)));
		gcaos[3] = abs( (int)100 - abs((int)(qgmedie[3]*100)/abs((int)(source->size) / (int)gused)));

		gqcaos_extension = (float)((float)gused*100.00)/256.00;
		gqcaos_distribuition = (float)(100.00 - (float)((float)( (float)gdelta / (float)gmax ) * 100.0));
		gqcaos_levelling = (float)abs( 100.00 - (float)( (float)( (float)gcaos[0] + (float)gcaos[1] + (float)gcaos[2] + (float)gcaos[3]) / 4.00) );
		gqcaos = (float)((float)((float)gqcaos_extension + (float)gqcaos_distribuition + (float)gqcaos_levelling) / 3.00);

	//	        Average value :
		puts("Chaos parameters");
		printf("Extension     : %3.1f%%\n" , gqcaos_extension);
		printf("Distribuition : %3.1f%%\n" , gqcaos_distribuition);
		printf("Leveling      : %3.1f%%\n\n" , gqcaos_levelling);

		gim_stat_tests( (const unsigned char *) source->fm , (long) source->size );
		if ( source->size > 16777216 )
			puts( "note: for files this large the verdict is the GKaos of the blocks; the whole-file score is only informative" );
		gqcaos = (float) gim_stat_blocks( (const unsigned char *) source->fm , (long) source->size , 100 );
		gim->file_manager->close (source);
		
	}
	
	gim_error->app( GIM_ERROR_MESSAGE , "gimstat" , "run end: exit %d, %.1f s" , (int) gqcaos , std::chrono::duration< double >( std::chrono::steady_clock::now() - gim_t0 ).count() );
	delete gim;
	
	gim_stat_minimal_result( gqcaos );
	exit( (int)gqcaos);
}

