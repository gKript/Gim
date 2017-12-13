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

_gim_buffer gim_cnc_encoding_obj::Fog_a_buffer( _gim_buffer buffer , _gim_Ulong  * buffer_lenght ) {
    _gim_Uint32	i=0;
	char 	*resti;
	_gim_Ulong	t=0,total=0;
	_gim_Uchar	c=0;
	_gim_buffer	encoded;
	_gim_rand	mt;
	if ( ! cnc_data.coding_digit )
		cnc_data.coding_digit = min_digit( buffer , *(buffer_lenght) , cnc_data.coding_base ) ;
	cnc_NoSkema( __GIM_YES ) {
		cnc_password->calc();
		mt.seed( cnc_password->seed_md5 );
	}
	encoded = (_gim_buffer)gim_memory->Alloc( ( (*(buffer_lenght) + sizeof ( char )) * cnc_data.coding_digit ) );
	if ( ! encoded )
		gim_error->set( GIM_ERROR_CRITICAL , "cnc_encoding_class::Fog_a_buffer" , "No more free memory" , __GIM_ERROR );
    for (t = 0; t < *(buffer_lenght) ; t++) {
		c =  *(buffer + t );
		cnc_NoSkema( __GIM_YES )
			c ^= ( mt.randInt( 255 ) );
		resti = Fog_a_byte( c , cnc_data.coding_base , cnc_data.coding_digit );
		for ( i=0 ; i < cnc_data.coding_digit ; i++ ,total++ ) {
			*(encoded + total) = *(resti+i);
		}
    }
	*(buffer_lenght) *= cnc_data.coding_digit;
	gim_memory->Free( buffer );
	buffer = encoded;
    return encoded;
}

char * gim_cnc_encoding_obj::Fog_a_byte( _gim_Uchar c , _gim_Uint32 base , _gim_Uint8 digit ) {
	int	i=0, j=0 ;
	_gim_Uint32	ch=0;
	int			reverse[MAX_DIGIT];
	static char resti[MAX_DIGIT];
	memset ( resti , 0 , MAX_DIGIT );
	ch = c & 0xff;
	//~ cnc_Additive( __GIM_ON )
		//~ ch += cnc_data->password_seed_sha512;
	//~ cnc_Multiplier( __GIM_ON )
		//~ ch *= cnc_data->password_seed_md5;
	for (j = (digit - 1) ; j >= 0 ; j-- ) {
		reverse[j] = c % base;
		c /= base;
	}
	for (i = 0; i < digit; i++ )
		resti[i] = cnc_generic.num2char(reverse[i]);
	return resti;
}

_gim_Uint32	gim_cnc_encoding_obj::min_digit( char * filename , _gim_int8 base ) {
	int			count = 0, max = 0;
	_gim_Uchar	ch = 0;
	char		* p;
	_gim_int8	mbit = MIN_DIGIT;
	_gim_file	fin( filename );
	cnc_NoSkema( __GIM_NO ) {
		if ( fin.check() == __GIM_NOT_EXIST )
			return __GIM_NO;
		p = fin.fm();
		while ( count < fin.size() )
		{
			ch = *( p + count ) & 0xff ;
			if (ch > max)
				max = ch;
			count++;
			if ( max == 255 )
				break;
		}
		fin.close();
	}
	cnc_NoSkema( __GIM_YES )
		max = 255;
	for ( ; tmp_crt( base, mbit, max ) ; mbit++ );
	return mbit;
}

_gim_Uint32	gim_cnc_encoding_obj::min_digit( _gim_buffer buffer , _gim_Ulong buffer_lenght , _gim_int8 base ) {
	_gim_Ulong	count = 0 , max = 0;
	_gim_Uchar	ch = 0;
	_gim_int8	mbit = MIN_DIGIT;
	cnc_NoSkema( __GIM_NO ) {
		while ( count < buffer_lenght ) {
			ch = *(buffer + count) ;
			if (ch > max)
				max = ch;
			count++;
			if ( max == 255 )
				break;
		}
	}
	cnc_NoSkema( __GIM_YES )
		max = 255;
	for ( ; tmp_crt(base, mbit, max) ; mbit++ );
	return mbit;
}

int	gim_cnc_encoding_obj::tmp_crt( _gim_int8 base, _gim_int8 bit, _gim_Uchar word ) {
	int ind = 0, char_num = 0;
	char_num = word & 0xff;
	cnc_password->calc();
	//~ cnc_Additive(ON)	char_num += cnc_Kernel_data->password_seed_sha512;
	//~ cnc_Multiplier(ON)	char_num *= cnc_Kernel_data->password_seed_md5;
	for (ind = (bit - 1); ind >= 0; ind--) {
		char_num /= base;
	}
	return char_num;
}
