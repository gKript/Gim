//           gim_cnc_alfabeto.cc
//  Sun Apr 27 19:09:49 2008
//  Copyright (C) 2008-2026 asyntote
//
//  This file is part of Gim library. Released under the terms of the GNU General Public License v3,
//  as the rest of Gim (https://www.gnu.org/licenses/gpl-3.0.html).
//  <asyntote@<host>>


#include "../include/gim_cnc.h"

void	gim_cnc_alfabeto_obj::init	( void ) {
	memset ( alfabeto	, NL , MAX_BASE );
	memset ( rotazione	, 0  , MAX_BASE );
}

_gim_flag	gim_cnc_alfabeto_obj::control	( int res ) {
	_gim_Uint8	q;
	for ( q = 0; q < MAX_BASE; q++ )
		if ( alfabeto[q] == res )
			return ( __GIM_NO );
	return __GIM_YES;
}

void	gim_cnc_alfabeto_obj::make	( void ) {
	int	c, res, s,stack;
	char message[256];
	_gim_rand	mt;
	stack = cnc_feature.VariableAB;
	cnc_feature.VariableAB = __GIM_OFF;
	cnc_password->calc();
	mt.seed( cnc_password->seed_main );
	gim_error->set( "gim_cnc_alfabeto_obj::make" , "Randomize the FOG alphabet..." );
	for (c = 0, s = 0; s < MAX_BASE; c++)	{
		res = cnc_generic.num2char( mt.randInt( MAX_BASE ) );
		if (control( res ) == __GIM_YES ) {
			cnc_alfabeto.alfabeto[s] = res;
			cnc_alfabeto.posizione[s] = s;
			s++;
		}
	}
	sprintf( message , "Done %d attempts on %d FOG char", c, s);
	gim_error->set( "gim_cnc_alfabeto_obj::make" , message );
	cnc_feature.VariableAB = stack;
}
