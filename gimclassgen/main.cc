// main.cc
//
// Copyright (C) 2020 - Danilo
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#include <stdio.h>
#include <unistd.h>             
#include <gim/gim.h>
#include <gcg_main_header.h>

#define	ITERS	1000
#define	TEST	100000
#define	PP		54.0


void hand_shake( void ) {
  puts  ( "  GimClassGen    -   gKript Gim Class Generator\n" );
  printf( "  Version-   -   -   %s\n" , GCG_VERSION );  
  puts  ( "  Author -   -   -   asyntote  [gKript.org]" );
  puts  ( "  Author -   -   -   Skymatrix [gKript.org]" );
}


int main( int argc , char **argv ) {
	
	if ( gim_check_version(2,8,7) != __GIM_OK ) {
		printf( "%s\n\n" , gim_version() );
		printf( "Gim is not updated at the required version.\n" );
		printf( "For %s is necessary Gim >= 2.8-7\n" , argv[0] );
		exit( -1 );
	}

	gim_set_application_name( "GimClassGen" );
    gim_obj *				gim = new gim_obj;
    gim_ascii_file_obj *	src_templ  = new gim_ascii_file_obj;
    src_templ->set_dimension( 1 *  __GIM_KB );
    gim_ascii_file_obj *	head_templ = new gim_ascii_file_obj;
    head_templ->set_dimension( 1 *  __GIM_KB );
   	gim_getopt_obj * 		opt = new gim_getopt_obj;
   	
   	_gim_rand mt;
   	
   	_gim_flag	v[TEST];
   	
   	gim->conf->ChangeKeyFlag( "debug", "f_debug" , __GIM_YES );
	gim->conf->AddKeyComment( "debug", "f_debug" , PRSR_AFTER , "Forced YES by main program" );
	gim->conf->Write();
	

	hand_shake();
	_LINE;
	printf( "  I am %s on %s\n" , gim->identity->login() , gim->identity->node() );
	_LINE;
	int s = 0, it = 0;
	float p[ITERS];
	float pm = 0.0;
	float em = 0.0;
	float e[ITERS];	
	float sume = 0.0;
	float sump = 0.0;
	for( int r = 0 ; r < 10 ; r++ ) {
		s = 0;
		it = 0;
		pm = 0.0;
		em = 0.0;
		sume = 0.0;
		sump = 0.0;
		int rn = mt.randInt( 100 ) + 1;
		for( it = 0 ; it < ITERS ; it++ ) {	
			s = 0;
			for( int c = 0 ; c < TEST ; c++ ) {
				v[c] = mt.stat_distr_over_100_percentage( rn );
				if ( v[c] == __GIM_YES )
					s++;
			}
			p[it] = (float)( (float)( (float)s / TEST ) * 100.0 );
			e[it] = (float)( p[it] - rn ) ;
//			printf( "it: %d: s: %d - P: %2.3f - E: %2.3f\n" , it , s , p[it] , e[it] );
		}
		sume = 0.0;
		sump = 0.0;
		for( it = 0 ; it < ITERS ; it++ ) {	
			sume += e[it];
			sump += p[it];
//			printf( "Sume %2.3f - Sump%2.3f\n" , sume , sump );
		}
		em = (float)( ( sume / (float)ITERS ) );
		pm = (float)( ( sump / (float)ITERS ) );
		printf( "request: %2.3f%%\n" , (float)rn ); 
		printf( "perc   : %2.3f%%\n" , pm ); 
		printf( "error  : %2.3f%%\n---------------------\n\n" , em ); 
	}

	delete opt;
	delete src_templ;
	delete head_templ;
    delete gim;
    
    exit( 0 );
}

