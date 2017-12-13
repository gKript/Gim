//           gim_cnc_generic.cc
//  Sun Apr 27 19:44:38 2008
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

#include	"../include/gim_cnc.h"

int	gim_cnc_generic_obj::elev( int num ) {
	int			ele = 1 ; 
	_gim_Uint32	n = 1 ;
	for ( ; n < ( cnc_data.coding_digit - num ) ; n++ )
		ele *= cnc_data.coding_base;
	return ele;
}

_gim_flag	gim_cnc_generic_obj::instr( char * str1 , char * str2 ) {
	int flag = 0 ;
	char * start_str2 = str2;
	char * start_str1 = str1;
	if ( strlen( str1 ) < strlen( str2 ) )
		return __GIM_NO;
	for ( ; *str1 != '\0' ; str1++ )
		if ( ( *str1 ) == ( *str2 ) )
			start_str1 = str1, str2++, flag++;
	if ( flag == (int)strlen( start_str2 ) )
		return __GIM_YES;
	else
		return __GIM_NO;
}

void	gim_cnc_generic_obj::del_ext( char * str1 , char * str2 ) {
	int pos = strlen( str1 ) - strlen( str2 );
	str1[pos] = '\0';
}

char *	gim_cnc_generic_obj::del_path( char * str ) {
	for ( char * s2 = str ; *s2 != '\0' ; s2++ )
		if ( *s2 == '/' )
			str = s2;
	return ++str;
}

char	gim_cnc_generic_obj::num2char( int resto ) {
	char ctmp = '\0';
	cnc_VariableAB( __GIM_YES )
		ctmp = cnc_alfabeto.alfabeto[resto];
	cnc_VariableAB( __GIM_NO )
		ctmp = resto + MIN_CH;
	return ctmp;
}

_gim_int8	gim_cnc_generic_obj::char2num( char dato ) {
	_gim_int8 ris=0;
	cnc_VariableAB( __GIM_YES ) {
		for ( int c = 0 ; c < MAX_BASE ; c++ )
			if ( cnc_alfabeto.alfabeto[c] == dato )
				return c;
		}
	cnc_VariableAB( __GIM_NO )
		ris = dato - MIN_CH;
	return ris;
}


_gim_flag	gim_cnc_generic_obj::isPrime( _gim_Ulong num ) {
	_gim_Ulong 	c = 0;
	if ( num > 3 ) {
		for ( c = 2 ; c > num ; c++ ) { 
			if ( ! ( num % c ) ) {
				return __GIM_NO;
			}
		}
	}
	return __GIM_YES;
}
