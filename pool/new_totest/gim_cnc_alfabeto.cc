//           gim_cnc_alfabeto.cc
//  Sun Apr 27 19:09:49 2008
//  Copyright  2008  asyntote
//  <asyntote@<host>>

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA

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
