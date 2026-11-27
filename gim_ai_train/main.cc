/*!	\file		gim_ai_train/main.cc
	\brief		Command line tool: gimaitrain. Same options and exit codes as scripts/gim_ai_train.py.
				Exit codes: 0 accepted, 2 rejected by a check, 1 bad input.

	Usage:
		gimaitrain --dataset data.csv --out pgim_ai_model.h [--report r.txt]
				   [--out-bits 8|16] [--hidden 8,8] [--epochs 200] [--lr 0.05]
				   [--test-frac 0.25] [--seed 1] [--tol 0.02] [--test-vectors 16]
		gimaitrain --synthetic-out synth.csv [--synthetic-n 600] [--seed 1]

	Copyright (C) 2002-2026
	Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)

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
*/

#include "../include/gim_ai_train.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// After the standard headers: gim_sha512.h defines a min() macro.
#include "../include/gim.h"

namespace {

void usage() {
	std::cerr << "usage: gimaitrain --dataset CSV --out HEADER [options]\n"
			  << "       gimaitrain --synthetic-out CSV [--synthetic-n N] [--seed S]\n"
			  << "options: --report FILE --out-bits 8|16 --hidden 8,8 --epochs N --lr X\n"
			  << "         --test-frac X --seed S --tol X --test-vectors N\n";
}

std::vector< int > parse_hidden( const std::string & s ) {
	std::vector< int > out;
	std::stringstream ss( s );
	for ( std::string item ; std::getline( ss , item , ',' ) ; ) {
		if ( item.empty() ) continue;
		out.push_back( std::atoi( item.c_str() ) );
	}
	return out;
}

}	// namespace

int main( int argc , char ** argv ) {
	/* Gim log: errors and results go through gim_error; the usage text and the report stay on the console. */
	gim_set_application_name( "gimaitrain" );
	gim_obj gim;
	std::string dataset , out , report , synth_out;
	int synth_n = 600;
	gim_ai::Options opt;

	for ( int i = 1 ; i < argc ; i++ ) {
		std::string a = argv[ i ];
		auto next = [ & ]() -> std::string {
			if ( i + 1 >= argc ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gimaitrain" , "missing value for %s" , a.c_str() );
				std::exit( 1 );
			}
			return argv[ ++i ];
		};
		if      ( a == "--dataset" )        dataset = next();
		else if ( a == "--out" )            out = next();
		else if ( a == "--report" )         report = next();
		else if ( a == "--out-bits" )       opt.out_bits = std::atoi( next().c_str() );
		else if ( a == "--hidden" )         opt.hidden = parse_hidden( next() );
		else if ( a == "--epochs" )         opt.epochs = std::atoi( next().c_str() );
		else if ( a == "--lr" )             opt.lr = std::atof( next().c_str() );
		else if ( a == "--test-frac" )      opt.test_frac = std::atof( next().c_str() );
		else if ( a == "--seed" )           opt.seed = ( uint32_t ) std::strtoul( next().c_str() , nullptr , 10 );
		else if ( a == "--tol" )            opt.tol = std::atof( next().c_str() );
		else if ( a == "--test-vectors" )   opt.test_vectors = std::atoi( next().c_str() );
		else if ( a == "--synthetic-out" )  synth_out = next();
		else if ( a == "--synthetic-n" )    synth_n = std::atoi( next().c_str() );
		else if ( a == "-h" || a == "--help" ) { usage(); return 0; }
		else {
			gim_error->Set( GIM_ERROR_CRITICAL , "gimaitrain" , "unknown option %s" , a.c_str() );
			usage();
			return 1;
		}
	}

	try {
		if ( !synth_out.empty() ) {
			gim_ai::write_synthetic( synth_out , synth_n , opt.seed );
			gim_error->Set( GIM_ERROR_MESSAGE , "gimaitrain" , "synthetic dataset written %s" , synth_out.c_str() );
			std::cout << "synthetic dataset written " << synth_out << "\n";
			return 0;
		}
		if ( dataset.empty() || out.empty() ) {
			usage();
			return 1;
		}
		gim_ai::Dataset ds = gim_ai::read_dataset( dataset );
		gim_ai::Outcome res = gim_ai::run_pipeline( ds , opt );
		std::cout << res.report;
		if ( !report.empty() ) {
			std::ofstream rf( report.c_str() , std::ios::binary );
			rf << res.report;
		}
		if ( !res.accepted ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gimaitrain" , "REJECTED: integer accuracy %.4f below float %.4f minus tolerance" , res.int_acc , res.float_acc );
			std::cout << "REJECTED: integer accuracy " << res.int_acc
					  << " below float " << res.float_acc << " minus tolerance\n";
			return 2;
		}
		std::ofstream hf( out.c_str() , std::ios::binary );
		if ( !hf ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gimaitrain" , "cannot write %s" , out.c_str() );
			return 1;
		}
		hf << res.header;
		gim_error->Set( GIM_ERROR_MESSAGE , "gimaitrain" , "written %s" , out.c_str() );
		std::cout << "written " << out << "\n";
		return 0;
	} catch ( const gim_ai::ModelError & e ) {
		gim_error->Set( GIM_ERROR_CRITICAL , "gimaitrain" , "REJECTED: %s" , e.what() );
		return 2;
	} catch ( const std::exception & e ) {
		gim_error->Set( GIM_ERROR_CRITICAL , "gimaitrain" , "input error: %s" , e.what() );
		return 1;
	}
}
