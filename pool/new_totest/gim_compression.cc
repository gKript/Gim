/***************************************************************************
 *            gkp_compression.cc
 *
 *  Sat May 13 17:58:59 2006
 *  Copyright  2006  AsYntote
 *  asyntote@gkript.org
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "../include/gim_compression.h"

gim_compression_obj * gim_compression;

gim_compression_obj::gim_compression_obj() {
/*	gim_error->set( "gim_compress_obj::gim_compress_obj" , "Start self test" );
	if ( self_test() == __GIM_OK ) {
		gim_error->set( "gim_compress_obj::gim_compress_obj" , "OK... Self test succesfully passed" );
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_compress_obj::gim_compress_obj" , "Hmmm...Sorry but the self test has failed" , __GIM_ERROR );
	}
*/}

_gim_buffer	gim_compression_obj::Compress( _gim_flag comp_type , _gim_buffer soub , _gim_Uint32 soulen , _gim_Uint32 * destlen ) {
	int	bz_len ;
	_gim_buffer	resb = NULL;
	char message[256];
	gim_error->set( "gim_compress_obj::compress" , "Entering..." );
	switch ( comp_type ) {
		case __GIM_BZ2 : {
			gim_error->set( "gim_compress_obj::compress" , "Bzip2" );
			bz_len = soulen + ( soulen / 100 ) + ( sizeof( char ) * 600 ) ;
			sprintf( message , "%d ORIGINAL -  %d for BZ2" , soulen , bz_len  );
			gim_error->set( "gim_compress_obj::compress" , message );
			resb = (_gim_buffer) gim_memory->Alloc_unlocked( (size_t)( bz_len * sizeof(char) ) );
			//resb = Encode_BZ2( soub, soulen. destlen);
/*			if ( BZ2_bzBuffToBuffCompress( resb , destlen , soub , soulen , __GIM_BZ_BLOCKSIZE100K , __GIM_BZ_VERBOSITY , __GIM_BZ_WORKFACTOR ) == BZ_OK )
				gim_error->set( "gim_compress_obj::compress" , "Bzip2 succesfull" );
			else
				gim_error->set( "gim_compress_obj::compress" , "Bzip2 UNSUCCESFULL" );*/
			break;
		}
		case __GIM_LZH  : {
//			lzh_len = soulen + ( soulen / 10  ) +   sizeof( char ) ;
			gim_error->set( GIM_ERROR_MESSAGE , "gim_compress_obj::compress" , "LZH" , 0 );
//			resb = (_gim_buffer) gim_memory->Alloc( ( gz_len * sizeof(char) ) );
			resb = (_gim_buffer)EncodeLzHuf( (unsigned char *)soub , soulen , (long unsigned int *)destlen );
//			compress( (Bytef *)resb ,(uLongf *)destlen , (Bytef *)soub , soulen );
			break;
		}
		default : {
			gim_error->set( GIM_ERROR_FATAL , "gim_compress_obj::compress" , "UNKNOWN compression type" , __GIM_ERROR );
			break;
		}
	}
	if ( *destlen >= soulen ) {
		sprintf( message , "%d original - %d compresssed   : RETURNING ORIGINAL" , soulen , *destlen );
		gim_error->set( GIM_ERROR_MESSAGE , "gim_compress_obj::compress" , message , 0 );
		gim_memory->Free( resb );
		*destlen = 0;
		return soub;
	}
	else {
		sprintf( message , "%d original - %d compresssed" , soulen , *destlen );
		gim_error->set( GIM_ERROR_MESSAGE , "gim_compress_obj::compress" , message , 0 );
		sprintf( message , "INDEX : %9x original - %9x compresssed" , (unsigned long long)soub , (unsigned long long)resb );
		gim_error->set( GIM_ERROR_MESSAGE , "gim_compress_obj::compress" , message , 0 );
		gim_memory->Free( soub );
		return resb;
	}
	return resb;
}


_gim_buffer	gim_compression_obj::Uncompress	( _gim_flag comp_type , _gim_buffer soub , _gim_Uint32 soulen , _gim_Uint32   destlen ) {
	_gim_buffer	destb = NULL;
	char	message[256];
	gim_error->set( GIM_ERROR_MESSAGE , "gim_compress_obj::Uncompress" , "Entering" , 0 );
	switch ( comp_type ) {
		case __GIM_BZ2 : {
			gim_error->set( GIM_ERROR_MESSAGE , "gim_compress_obj::Uncompress" , "Bzip2" , 0 );
			destb = (_gim_buffer) gim_memory->Alloc_unlocked( (size_t)( destlen * sizeof(char) ) );
			BZ2_bzBuffToBuffDecompress( (_gim_buffer)destb , & destlen , (_gim_buffer)soub , soulen , 1 , 0 );
			break;
		}
		case __GIM_LZH  : {
			gim_error->set( "gim_compress_obj::Uncompress" , "Gzip" );
//			destb = (_gim_buffer) gim_memory->Alloc( ( destlen * sizeof(char) ) );
//			uncompress( (Bytef *)destb , (uLongf *) & destlen , (Bytef *)soub , soulen);
			DecodeLzHuf((unsigned char *)soub , (unsigned char *)destb , (unsigned long)destlen , (unsigned long)soulen );
//			DecodeLzHuf(unsigned char *Sdata, unsigned char *OData, unsigned long textsize,unsigned long sourcesize);
			break;
		}
	}
	sprintf( message , "%d original - %d Ucompresssed" , soulen , destlen );
	gim_error->set( GIM_ERROR_MESSAGE , "gim_compress_obj::Uncompress" , message , 0 );
	return destb;
}



_gim_flag	gim_compression_obj::self_test( void ) {
/*	_gim_buffer	to_test , compressed , uncompressed;
	_gim_rand	mt;
	_gim_Uint32	count = 0 , comp_len;
	char 		md5_comp[256] , md5_ori[256] , message[256];
	__GIM_CLEAR( message , 256 , char );
	gim_error->set( "gim_compress_obj::self_test" , "Preparing the buffer for the test ( 1 MB of random sequence ) " );
	to_test = (_gim_buffer)gim_memory->Alloc_unlocked( (size_t)( sizeof(char) * 1024 * 1024 ) );
	for ( ; count < ( 1024 * 1024 ) ; count++ ) {
		to_test[count] = mt.randInt( 256 - 1 );
		if ( count < 25 ) 
			sprintf( message , "%s%d " , message , (unsigned int)to_test[count] );
	}
	gim_error->set( "gim_compress_obj::self_test" , message );
	compressed = (_gim_buffer)Compress( __GIM_BZ2 , to_test , (sizeof(char) * 1024 * 1024 ) , &comp_len );
*/	//~ strcpy( md5_comp , gim_checksum->sha1( compressed , comp_len ) );
}

