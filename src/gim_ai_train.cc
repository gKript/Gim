/*!	\file		gim_ai_train.cc
	\brief		Gim side of the PicGIM AI model. See gim_ai_train.h and SYNC/COMM_CONTRACT.md sez. 4.4 - 4.6.
				Same algorithm and checks as scripts/gim_ai_train.py.

	Released under the terms of the GNU General Public License v2, as the rest of Gim.
*/

#include "../include/gim_ai_train.h"
#include "../include/gim_sha256.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <numeric>
#include <random>
#include <sstream>

namespace gim_ai {

namespace {

const int64_t kInt31 = 2147483648LL;          // 2^31: int32 accumulator bound (sez. 4.5)
const int64_t kMaxScale = 65535;              // scale fits uint16 (sez. 4.2)
const int kMaxChannels = 16;                  // PGIM_AI_MAX_CHANNELS upper limit

int64_t out_max_for( int bits ) {
	if ( bits == 8 )  return 127;
	if ( bits == 16 ) return 32767;
	throw std::invalid_argument( "out-bits must be 8 or 16" );
}

//!< Channel configuration, same fields as pg_ai_channel_cfg (pgim_ai_input.h).
struct ChCfg {
	int64_t		in_min;
	int64_t		in_max;
	int64_t		scale;
	int			shift;
	int64_t		fallback;
	int			enabled;
};

//!< Bit-exact copy of pg_ai_input_normalize (pgim_ai_input.c).
std::vector<int64_t> normalize_int( const std::vector<ChCfg> & cfg , const std::vector<uint16_t> & raw ,
									const std::vector<uint8_t> & ok , int64_t out_max ) {
	std::vector<int64_t> out( cfg.size() );
	for ( size_t c = 0 ; c < cfg.size() ; c++ ) {
		const ChCfg & k = cfg[ c ];
		if ( k.enabled != 1 || ok[ c ] != 1 ) {
			out[ c ] = k.fallback;
			continue;
		}
		int64_t r = std::min( std::max< int64_t >( raw[ c ] , k.in_min ) , k.in_max );
		int64_t diff = r - k.in_min;
		int64_t v = ( diff * k.scale ) >> k.shift;
		if ( v > 2 * out_max ) v = 2 * out_max;
		out[ c ] = v - out_max;
	}
	return out;
}

//!< Float layer used for training. Hidden layers: ReLU saturated at 1.
struct FLayer {
	int							n_in;
	int							n_out;
	bool						relu;
	std::vector< std::vector< double > >	W;	//!< W[o][i]
	std::vector< double >		B;
};

FLayer make_flayer( int n_in , int n_out , bool relu , std::mt19937 & rng ) {
	FLayer L;
	L.n_in = n_in;
	L.n_out = n_out;
	L.relu = relu;
	std::normal_distribution< double > nd( 0.0 , std::sqrt( ( relu ? 2.0 : 1.0 ) / n_in ) );
	L.W.assign( n_out , std::vector< double >( n_in ) );
	for ( int o = 0 ; o < n_out ; o++ )
		for ( int i = 0 ; i < n_in ; i++ )
			L.W[ o ][ i ] = nd( rng );
	L.B.assign( n_out , 0.0 );
	return L;
}

//!< Forward pass keeping activations (acts[l] = input of layer l) and pre-activations (zs[l]).
void forward_train( const std::vector< FLayer > & layers , const std::vector< double > & x ,
					std::vector< std::vector< double > > & acts , std::vector< std::vector< double > > & zs ) {
	acts.assign( 1 , x );
	zs.clear();
	const size_t last = layers.size() - 1;
	for ( size_t l = 0 ; l < layers.size() ; l++ ) {
		const FLayer & L = layers[ l ];
		const std::vector< double > & a = acts.back();
		std::vector< double > z( L.n_out );
		for ( int o = 0 ; o < L.n_out ; o++ ) {
			double s = L.B[ o ];
			for ( int i = 0 ; i < L.n_in ; i++ )
				s += L.W[ o ][ i ] * a[ i ];
			z[ o ] = s;
		}
		std::vector< double > next( L.n_out );
		for ( int o = 0 ; o < L.n_out ; o++ )
			next[ o ] = ( l < last ) ? std::min( std::max( z[ o ] , 0.0 ) , 1.0 ) : z[ o ];
		zs.push_back( z );
		acts.push_back( next );
	}
}

int argmax_first( const std::vector< double > & v ) {
	int best = 0;
	for ( int i = 1 ; i < ( int ) v.size() ; i++ )
		if ( v[ i ] > v[ best ] ) best = i;
	return best;
}

int float_predict( const std::vector< FLayer > & layers , const std::vector< double > & x ) {
	std::vector< std::vector< double > > acts , zs;
	forward_train( layers , x , acts , zs );
	return argmax_first( acts.back() );
}

void train_float( std::vector< FLayer > & layers , const std::vector< std::vector< double > > & xs ,
				  const std::vector< int > & ys , int epochs , double lr , std::mt19937 & rng ) {
	std::vector< size_t > order( xs.size() );
	std::iota( order.begin() , order.end() , 0 );
	const int last = ( int ) layers.size() - 1;
	std::vector< std::vector< double > > acts , zs;
	for ( int ep = 0 ; ep < epochs ; ep++ ) {
		std::shuffle( order.begin() , order.end() , rng );
		for ( size_t k : order ) {
			forward_train( layers , xs[ k ] , acts , zs );
			const std::vector< double > & zl = zs.back();
			double m = *std::max_element( zl.begin() , zl.end() );
			std::vector< double > g( zl.size() );
			double s = 0.0;
			for ( size_t o = 0 ; o < zl.size() ; o++ ) {
				g[ o ] = std::exp( zl[ o ] - m );
				s += g[ o ];
			}
			for ( size_t o = 0 ; o < zl.size() ; o++ )
				g[ o ] /= s;
			g[ ys[ k ] ] -= 1.0;
			for ( int l = last ; l >= 0 ; l-- ) {
				FLayer & L = layers[ l ];
				if ( l < last )
					for ( int o = 0 ; o < L.n_out ; o++ )
						if ( !( 0.0 < zs[ l ][ o ] && zs[ l ][ o ] < 1.0 ) ) g[ o ] = 0.0;
				const std::vector< double > & a_prev = acts[ l ];
				std::vector< double > g_prev( L.n_in , 0.0 );
				for ( int o = 0 ; o < L.n_out ; o++ )
					for ( int i = 0 ; i < L.n_in ; i++ )
						g_prev[ i ] += L.W[ o ][ i ] * g[ o ];
				for ( int o = 0 ; o < L.n_out ; o++ ) {
					for ( int i = 0 ; i < L.n_in ; i++ )
						L.W[ o ][ i ] -= lr * g[ o ] * a_prev[ i ];
					L.B[ o ] -= lr * g[ o ];
				}
				g = g_prev;
			}
		}
	}
}

//!< Scale the last layer so its training logits lie in [-1, 1]. Argmax is unchanged.
void scale_output_layer( std::vector< FLayer > & layers , const std::vector< std::vector< double > > & xs ) {
	double m = 0.0;
	std::vector< std::vector< double > > acts , zs;
	for ( const auto & x : xs ) {
		forward_train( layers , x , acts , zs );
		for ( double v : zs.back() ) m = std::max( m , std::fabs( v ) );
	}
	if ( m <= 0.0 ) return;
	FLayer & L = layers.back();
	for ( auto & row : L.W )
		for ( auto & w : row ) w /= m;
	for ( auto & b : L.B ) b /= m;
}

//!< Integer layer: weights and bias already in the accumulator scale (sez. 4.4 passo 5).
struct ILayer {
	int									n_in;
	int									n_out;
	int									shift;
	bool								relu;
	std::vector< std::vector< int64_t > >	W;
	std::vector< int64_t >				B;
};

//!< Largest shift q (16..0) with |w| <= OUT_MAX and the int32 bound of sez. 4.5 on every neuron.
ILayer quantize_layer( const FLayer & L , int64_t out_max ) {
	for ( int q = 16 ; q >= 0 ; q-- ) {
		double sc = ( double ) ( 1LL << q );
		ILayer il;
		il.n_in = L.n_in;
		il.n_out = L.n_out;
		il.shift = q;
		il.relu = L.relu;
		il.W.assign( L.n_out , std::vector< int64_t >( L.n_in ) );
		il.B.assign( L.n_out , 0 );
		bool in_range = true;
		for ( int o = 0 ; o < L.n_out && in_range ; o++ ) {
			for ( int i = 0 ; i < L.n_in ; i++ ) {
				il.W[ o ][ i ] = std::llround( L.W[ o ][ i ] * sc );
				if ( std::llabs( il.W[ o ][ i ] ) > out_max ) in_range = false;
			}
			il.B[ o ] = std::llround( L.B[ o ] * sc * ( double ) out_max );
		}
		if ( !in_range ) continue;
		bool fits = true;
		for ( int o = 0 ; o < L.n_out && fits ; o++ ) {
			int64_t sum = 0;
			for ( int i = 0 ; i < L.n_in ; i++ ) sum += std::llabs( il.W[ o ][ i ] );
			int64_t bound = sum * out_max + std::llabs( il.B[ o ] ) + ( 1LL << q );
			if ( bound >= kInt31 ) fits = false;
		}
		if ( fits ) return il;
	}
	std::ostringstream msg;
	msg << "layer " << L.n_in << "x" << L.n_out << " cannot be represented without int32 overflow";
	throw ModelError( msg.str() );
}

//!< Bit-exact copy of pg_ai_net_infer / pg_ai_net_layer (pgim_ai_net.c). Returns the class.
int int_forward( const std::vector< ILayer > & layers , const std::vector< int64_t > & x , int64_t out_max ,
				 std::vector< int64_t > & scores ) {
	std::vector< int64_t > a = x;
	for ( const ILayer & il : layers ) {
		std::vector< int64_t > y( il.n_out );
		for ( int o = 0 ; o < il.n_out ; o++ ) {
			int64_t acc = il.B[ o ];
			for ( int i = 0 ; i < il.n_in ; i++ )
				acc += il.W[ o ][ i ] * a[ i ];
			if ( il.shift > 0 ) acc += 1LL << ( il.shift - 1 );
			acc >>= il.shift;                       // arithmetic shift (gcc / msvc)
			if ( il.relu && acc < 0 ) acc = 0;
			if ( acc > out_max ) acc = out_max;
			if ( acc < -out_max ) acc = -out_max;
			y[ o ] = acc;
		}
		a = y;
	}
	scores = a;
	int best = 0;
	for ( int i = 1 ; i < ( int ) scores.size() ; i++ )
		if ( scores[ i ] > scores[ best ] ) best = i;
	return best;
}

std::string join_ints( const std::vector< int64_t > & v ) {
	std::ostringstream s;
	for ( size_t i = 0 ; i < v.size() ; i++ ) {
		if ( i ) s << ", ";
		s << v[ i ];
	}
	return s.str();
}

std::string today_iso() {
	std::time_t t = std::time( nullptr );
	char buf[ 16 ] = { 0 };
	std::tm * tm = std::localtime( &t );
	if ( tm ) std::strftime( buf , sizeof( buf ) , "%Y-%m-%d" , tm );
	return buf;
}

struct TestItem {
	std::vector< int64_t >	raw;
	std::vector< int64_t >	ok;
	std::vector< int64_t >	x;
	std::vector< int64_t >	scores;
	int						cls;
};

//!< Same layout as the Python writer (pgim_ai_model.h of PicGim).
std::string build_header( const Options & opt , const std::string & digest , const std::string & arch ,
						  double float_acc , double int_acc , const std::vector< ChCfg > & cfg ,
						  const std::vector< ILayer > & layers , const std::vector< TestItem > & tests ,
						  int64_t out_max ) {
	( void ) out_max;
	const int n_in = ( int ) cfg.size();
	const int n_cls = layers.back().n_out;
	int max_width = n_in;
	for ( const ILayer & il : layers ) max_width = std::max( max_width , il.n_out );
	std::ostringstream h;
	h << std::fixed << std::setprecision( 4 );
	h << "/*\n"
	  << "\tFile name :\t\tpgim_ai_model.h\n"
	  << "\tProject :\t\tPicGim\n\n"
	  << "\t\\file\t\tpgim_ai_model.h\n"
	  << "\t\\brief\t\tGENERATED by Gim (gim_ai_train). Do not edit by hand.\n"
	  << "\tgenerated  : " << today_iso() << "\n"
	  << "\tgenerator  : gim_ai_train (C++) 1.0\n"
	  << "\tdataset    : sha256 " << digest << "\n"
	  << "\tnetwork    : " << arch << "\n"
	  << "\tseed/epochs: " << opt.seed << " / " << opt.epochs << ", lr " << opt.lr << "\n"
	  << "\tfloat acc  : " << float_acc << "   integer acc: " << int_acc << "   (test split)\n"
	  << "*/\n\n"
	  << "#ifndef _PGIM_AI_MODEL_H_\n"
	  << "\t#define _PGIM_AI_MODEL_H_\n\n"
	  << "\t#include \"pgim_ai_net.h\"\n\n"
	  << "\t#if ( PGIM_AI_OUT_BITS != " << opt.out_bits << " )\n"
	  << "\t\t#error \"model generated for " << opt.out_bits << " bit output\"\n"
	  << "\t#endif\n\n"
	  << "\t#define PG_AI_NUM_INPUTS\t\t" << n_in << "\n"
	  << "\t#define PG_AI_NUM_LAYERS\t\t" << layers.size() << "\n"
	  << "\t#define PG_AI_NUM_CLASSES\t\t" << n_cls << "\n"
	  << "\t#define PG_AI_MAX_WIDTH\t\t" << max_width << "\n"
	  << "\t#define PG_AI_NUM_TESTS\t\t" << tests.size() << "\n"
	  << "\t#define PG_AI_RAM_BYTES\t\t(2 * PG_AI_MAX_WIDTH * sizeof(pg_ai_out_t))\n\n"
	  << "\t//---[ Channel configuration ]---\n"
	  << "\trom const pg_ai_channel_cfg PG_AI_CHANNELS[ PG_AI_NUM_INPUTS ] = {\n";
	for ( int c = 0 ; c < n_in ; c++ ) {
		const ChCfg & k = cfg[ c ];
		h << "\t\t{ " << k.in_min << ", " << k.in_max << ", " << k.scale << ", " << k.shift << ", "
		  << k.fallback << ", " << k.enabled << " },\t// ch" << c << "\n";
	}
	h << "\t};\n\n";
	for ( size_t l = 0 ; l < layers.size() ; l++ ) {
		const ILayer & il = layers[ l ];
		std::vector< int64_t > flat;
		for ( const auto & row : il.W ) flat.insert( flat.end() , row.begin() , row.end() );
		h << "\t//---[ Layer " << l << " ]---\n"
		  << "\trom const pg_ai_w_t PG_AI_L" << l << "_W[ " << flat.size() << " ] = { " << join_ints( flat ) << " };\n"
		  << "\trom const _pg_int32 PG_AI_L" << l << "_B[ " << il.n_out << " ] = { " << join_ints( il.B ) << " };\n";
	}
	h << "\n\trom const pg_ai_layer PG_AI_LAYERS[ PG_AI_NUM_LAYERS ] = {\n";
	for ( size_t l = 0 ; l < layers.size() ; l++ ) {
		const ILayer & il = layers[ l ];
		h << "\t\t{ " << il.n_in << ", " << il.n_out << ", PG_AI_L" << l << "_W, PG_AI_L" << l << "_B, "
		  << il.shift << ", " << ( il.relu ? "PG_YES" : "PG_NO" ) << " }"
		  << ( l + 1 < layers.size() ? "," : "" ) << "\n";
	}
	h << "\t};\n\n"
	  << "\t//---[ Test vectors: raw input -> normalized input -> expected output ]---\n";
	auto emit_rows = [ & ]( const char * decl , const char * type , const char * field , int cols ) {
		h << "\t" << "rom const " << type << " " << decl << "[ PG_AI_NUM_TESTS ][ " << cols << " ] = {\n";
		for ( size_t t = 0 ; t < tests.size() ; t++ ) {
			const std::vector< int64_t > & v = std::string( field ) == "raw" ? tests[ t ].raw
											 : std::string( field ) == "ok"  ? tests[ t ].ok
											 : std::string( field ) == "x"   ? tests[ t ].x
											 : tests[ t ].scores;
			h << "\t{ " << join_ints( v ) << " }" << ( t + 1 < tests.size() ? "," : " };" ) << "\n";
		}
	};
	emit_rows( "PG_AI_TEST_RAW" , "_pg_Uint16" , "raw" , n_in );
	emit_rows( "PG_AI_TEST_OK" , "_pg_Uint8" , "ok" , n_in );
	emit_rows( "PG_AI_TEST_X" , "pg_ai_out_t" , "x" , n_in );
	emit_rows( "PG_AI_TEST_SCORES" , "pg_ai_out_t" , "scores" , n_cls );
	h << "\trom const _pg_Uint8 PG_AI_TEST_CLASS[ PG_AI_NUM_TESTS ] = { ";
	for ( size_t t = 0 ; t < tests.size() ; t++ )
		h << tests[ t ].cls << ( t + 1 < tests.size() ? ", " : " };\n" );
	h << "\n#endif\n";
	return h.str();
}

std::vector< std::string > split_line( const std::string & line ) {
	std::vector< std::string > out;
	std::string cur;
	for ( char ch : line ) {
		if ( ch == ',' ) {
			out.push_back( cur );
			cur.clear();
		} else if ( ch != '\r' ) {
			cur += ch;
		}
	}
	out.push_back( cur );
	return out;
}

int64_t parse_int( const std::string & s , int line_no ) {
	try {
		size_t used = 0;
		long long v = std::stoll( s , &used );
		if ( used != s.size() ) throw std::invalid_argument( "trailing characters" );
		return v;
	} catch ( const std::exception & ) {
		std::ostringstream m;
		m << "line " << line_no << ": not an integer '" << s << "'";
		throw std::invalid_argument( m.str() );
	}
}

}	// namespace

//------------------------------------------------------------------------------------------------

std::string sha256_hex( const std::string & bytes ) {
	gim_sha256_obj sha;
	sha2_context ctx;
	sha.SHA256_init( &ctx );
	sha.SHA256_update( &ctx , ( unsigned char * ) bytes.data() , ( int ) bytes.size() );
	unsigned char out[ 32 ];
	sha.SHA256_final( &ctx , out );
	static const char hex[] = "0123456789abcdef";
	std::string s;
	for ( unsigned char b : out ) {
		s += hex[ b >> 4 ];
		s += hex[ b & 15 ];
	}
	return s;
}

Dataset read_dataset( const std::string & path ) {
	std::ifstream f( path.c_str() , std::ios::binary );
	if ( !f ) throw std::invalid_argument( "cannot open " + path );
	std::string bytes( ( std::istreambuf_iterator< char >( f ) ) , std::istreambuf_iterator< char >() );

	Dataset ds;
	ds.sha256 = sha256_hex( bytes );

	std::vector< std::string > lines;
	std::istringstream in( bytes );
	for ( std::string line ; std::getline( in , line ) ; )
		lines.push_back( line );
	if ( lines.empty() ) throw std::invalid_argument( "dataset is empty" );

	std::vector< std::string > header = split_line( lines[ 0 ] );
	if ( header.empty() || header[ 0 ] != "label" )
		throw std::invalid_argument( "header must start with 'label'" );
	const size_t n_cols = header.size() - 1;
	if ( n_cols == 0 || n_cols % 2 != 0 )
		throw std::invalid_argument( "after 'label' expect pairs <name>,<name>_ok" );
	for ( size_t k = 1 ; k < header.size() ; k += 2 ) {
		if ( header[ k + 1 ] != header[ k ] + "_ok" )
			throw std::invalid_argument( "column " + header[ k ] + " must be followed by " + header[ k ] + "_ok" );
		ds.names.push_back( header[ k ] );
	}
	const int n = ( int ) ds.names.size();
	if ( n > kMaxChannels ) throw std::invalid_argument( "too many channels (max 16)" );

	int max_label = -1;
	for ( size_t ln = 1 ; ln < lines.size() ; ln++ ) {
		std::vector< std::string > r = split_line( lines[ ln ] );
		bool blank = true;
		for ( const auto & c : r ) if ( !c.empty() ) blank = false;
		if ( blank ) continue;
		const int line_no = ( int ) ln + 1;
		if ( r.size() != 1 + 2 * ( size_t ) n )
			throw std::invalid_argument( "line " + std::to_string( line_no ) + ": wrong number of fields" );
		Sample s;
		s.label = ( int ) parse_int( r[ 0 ] , line_no );
		if ( s.label < 0 ) throw std::invalid_argument( "line " + std::to_string( line_no ) + ": negative label" );
		for ( int c = 0 ; c < n ; c++ ) {
			int64_t v = parse_int( r[ 1 + 2 * c ] , line_no );
			if ( v < 0 || v > 65535 )
				throw std::invalid_argument( "line " + std::to_string( line_no ) + ": raw value out of 0..65535" );
			int64_t st = parse_int( r[ 2 + 2 * c ] , line_no );
			if ( st != 0 && st != 1 )
				throw std::invalid_argument( "line " + std::to_string( line_no ) + ": ok must be 0 or 1" );
			s.raw.push_back( ( uint16_t ) v );
			s.ok.push_back( ( uint8_t ) st );
		}
		max_label = std::max( max_label , s.label );
		ds.samples.push_back( s );
	}
	if ( ds.samples.empty() ) throw std::invalid_argument( "dataset has no samples" );
	ds.n_classes = max_label + 1;
	if ( ds.n_classes < 2 ) throw std::invalid_argument( "need at least 2 classes" );
	std::vector< bool > present( ds.n_classes , false );
	for ( const auto & s : ds.samples ) present[ s.label ] = true;
	for ( bool p : present )
		if ( !p ) throw std::invalid_argument( "labels must be 0..K-1, all present" );
	return ds;
}

Outcome run_pipeline( const Dataset & ds , const Options & opt ) {
	const int64_t out_max = out_max_for( opt.out_bits );
	const int n = ( int ) ds.names.size();
	if ( opt.hidden.empty() ) throw std::invalid_argument( "at least one hidden layer is needed" );

	// split (plain shuffle, not stratified) ------------------------------------------------
	std::vector< size_t > idx( ds.samples.size() );
	std::iota( idx.begin() , idx.end() , 0 );
	std::mt19937 split_rng( opt.seed );
	std::shuffle( idx.begin() , idx.end() , split_rng );
	size_t n_test = ( size_t ) std::floor( idx.size() * opt.test_frac + 0.5 );
	if ( n_test < 1 ) n_test = 1;
	if ( n_test >= idx.size() ) throw std::invalid_argument( "test fraction leaves no training samples" );
	std::vector< const Sample * > test , train;
	for ( size_t k = 0 ; k < idx.size() ; k++ )
		( k < n_test ? test : train ).push_back( &ds.samples[ idx[ k ] ] );

	// channel configuration from the training split only (sez. 4.2) -------------------------
	std::vector< ChCfg > cfg( n );
	for ( int c = 0 ; c < n ; c++ ) {
		int64_t lo = 0 , hi = -1;
		bool any = false;
		for ( const Sample * s : train ) {
			if ( s->ok[ c ] != 1 ) continue;
			int64_t v = s->raw[ c ];
			if ( !any || v < lo ) lo = v;
			if ( !any || v > hi ) hi = v;
			any = true;
		}
		if ( !any ) throw ModelError( "channel " + std::to_string( c ) + ": no valid training sample" );
		if ( hi <= lo ) throw ModelError( "channel " + std::to_string( c ) + ": constant in training" );
		const int64_t span = hi - lo;
		int shift = 0;
		int64_t scale = 0;
		for ( shift = 16 ; shift >= 0 ; shift-- ) {          // largest shift with scale <= 65535
			int64_t num = ( 2 * out_max ) << shift;
			scale = ( num + span - 1 ) / span;               // ceil
			if ( scale <= kMaxScale ) break;
		}
		cfg[ c ] = ChCfg{ lo , hi , scale , shift , 0 , 1 };
	}

	std::vector< std::vector< double > > xs_train;
	std::vector< int > ys_train;
	for ( const Sample * s : train ) {
		std::vector< int64_t > v = normalize_int( cfg , s->raw , s->ok , out_max );
		std::vector< double > x( n );
		for ( int c = 0 ; c < n ; c++ ) x[ c ] = ( double ) v[ c ] / ( double ) out_max;
		xs_train.push_back( x );
		ys_train.push_back( s->label );
	}

	// float training (sez. 4.4 passo 4) ----------------------------------------------------
	std::vector< int > sizes;
	sizes.push_back( n );
	for ( int h : opt.hidden ) sizes.push_back( h );
	sizes.push_back( ds.n_classes );
	std::mt19937 rng( opt.seed );
	std::vector< FLayer > layers;
	for ( size_t l = 0 ; l + 1 < sizes.size() ; l++ )
		layers.push_back( make_flayer( sizes[ l ] , sizes[ l + 1 ] , l + 2 < sizes.size() , rng ) );
	train_float( layers , xs_train , ys_train , opt.epochs , opt.lr , rng );
	scale_output_layer( layers , xs_train );

	std::vector< std::vector< int64_t > > test_int;
	std::vector< int > ys_test;
	for ( const Sample * s : test ) {
		test_int.push_back( normalize_int( cfg , s->raw , s->ok , out_max ) );
		ys_test.push_back( s->label );
	}
	int float_correct = 0;
	for ( size_t t = 0 ; t < test_int.size() ; t++ ) {
		std::vector< double > x( n );
		for ( int c = 0 ; c < n ; c++ ) x[ c ] = ( double ) test_int[ t ][ c ] / ( double ) out_max;
		if ( float_predict( layers , x ) == ys_test[ t ] ) float_correct++;
	}
	const double float_acc = ( double ) float_correct / ( double ) test_int.size();

	// quantization and checks (sez. 4.4 passo 5-6) ------------------------------------------
	std::vector< ILayer > ilayers;
	for ( const FLayer & L : layers )
		ilayers.push_back( quantize_layer( L , out_max ) );

	int int_correct = 0;
	for ( size_t t = 0 ; t < test_int.size() ; t++ ) {
		std::vector< int64_t > scores;
		if ( int_forward( ilayers , test_int[ t ] , out_max , scores ) == ys_test[ t ] ) int_correct++;
	}
	const double int_acc = ( double ) int_correct / ( double ) test_int.size();

	std::ostringstream rep;
	rep << std::fixed << std::setprecision( 4 );
	rep << "channels: " << n << ", classes: " << ds.n_classes << ", train " << train.size()
		<< ", test " << test.size() << "\n";
	for ( int c = 0 ; c < n ; c++ )
		rep << "  " << ds.names[ c ] << ": in [" << cfg[ c ].in_min << ", " << cfg[ c ].in_max << "] scale "
			<< cfg[ c ].scale << " shift " << cfg[ c ].shift << "\n";
	for ( size_t l = 0 ; l < ilayers.size() ; l++ )
		rep << "  layer " << l << ": " << ilayers[ l ].n_in << " -> " << ilayers[ l ].n_out << ", shift "
			<< ilayers[ l ].shift << ", relu " << ( ilayers[ l ].relu ? "True" : "False" ) << "\n";
	rep << "float accuracy (test): " << float_acc << "\n";
	rep << "integer accuracy (test): " << int_acc << "\n";
	const bool accepted = int_acc >= float_acc - opt.tol;
	rep << "tolerance " << opt.tol << " -> " << ( accepted ? "ACCEPTED" : "REJECTED" ) << "\n";

	Outcome out;
	out.accepted = accepted;
	out.float_acc = float_acc;
	out.int_acc = int_acc;
	out.report = rep.str();
	if ( !accepted ) return out;

	// test vectors (sez. 4.4 passo 7) and header ---------------------------------------------
	std::vector< TestItem > tests;
	const size_t m = std::min< size_t >( ( size_t ) opt.test_vectors , test.size() );
	for ( size_t t = 0 ; t < m ; t++ ) {
		const Sample * s = test[ t ];
		TestItem item;
		item.raw.assign( s->raw.begin() , s->raw.end() );
		item.ok.assign( s->ok.begin() , s->ok.end() );
		item.x = normalize_int( cfg , s->raw , s->ok , out_max );
		item.cls = int_forward( ilayers , item.x , out_max , item.scores );
		tests.push_back( item );
	}
	std::string arch;
	for ( size_t k = 0 ; k < sizes.size() ; k++ )
		arch += ( k ? " -> " : "" ) + std::to_string( sizes[ k ] );
	out.header = build_header( opt , ds.sha256 , arch , float_acc , int_acc , cfg , ilayers , tests , out_max );
	return out;
}

void write_synthetic( const std::string & path , int n , uint32_t seed ) {
	const int n_ch = 8 , n_cls = 3;
	std::mt19937 rng( seed );
	std::uniform_real_distribution< double > u01( 0.0 , 1.0 );
	std::vector< double > mid( n_ch ) , sigma( n_ch ) , step( n_ch );
	for ( int c = 0 ; c < n_ch ; c++ ) {
		mid[ c ] = 500.0 + 2500.0 * u01( rng );
		sigma[ c ] = 40.0 + 110.0 * u01( rng );
		double sign = ( u01( rng ) < 0.5 ) ? -1.0 : 1.0;
		step[ c ] = sigma[ c ] * sign * ( 0.3 + 0.9 * u01( rng ) );
	}
	std::ofstream f( path.c_str() , std::ios::binary );
	if ( !f ) throw std::invalid_argument( "cannot write " + path );
	f << "label";
	for ( int c = 0 ; c < n_ch ; c++ ) f << ",ch" << c << ",ch" << c << "_ok";
	f << "\n";
	for ( int k = 0 ; k < n ; k++ ) {
		int label = ( int ) ( u01( rng ) * n_cls );
		if ( label >= n_cls ) label = n_cls - 1;
		f << label;
		for ( int c = 0 ; c < n_ch ; c++ ) {
			double mu = mid[ c ] + ( label - ( n_cls - 1 ) / 2.0 ) * step[ c ];
			std::normal_distribution< double > nd( mu , sigma[ c ] );
			double v = std::min( std::max( nd( rng ) , 0.0 ) , 65535.0 );
			int ok = ( u01( rng ) < 0.02 ) ? 0 : 1;
			f << "," << ( int ) v << "," << ok;
		}
		f << "\n";
	}
}

}	// namespace gim_ai
