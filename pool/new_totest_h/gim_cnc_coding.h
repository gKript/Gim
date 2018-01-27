/***************************************************************************
 *            gim_cnc_coding.h
 *
 *  Sun Apr 27 19:09:11 2008
 *  Copyright  2008  asyntote
 *  <asyntote@<host>>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
 
 
/*!
	\file		gim_cnc_coding.h
	\version	0.3-5
	\brief		Copyright (C) 2002, 2006<BR>
				under the terms of the GNU General Public License<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)
*/ 		

#ifndef _CNC_CODING_H
#define _CNC_CODING_H

	#include "gim_file.h"

	/*! \class 		cnc_encoding_class
		\brief		CnC internal class to encode
	*/
	class  gim_cnc_encoding_obj {
		public:
			/*! \brief	Use this function to encode a buffer 
				\param	buffer			Is the poiter to the buffer you want to encode<BR>
										\a buffer != NULL
				\param	buffer_lenght	Initially is the buffer lenght<BR>
			 							At the end will be the lenght of the encoded buffer<BR>
										\a buffer_lenght > 0
				\return The pointer of the encoded buffer or NULL if some errors occurred
				\see	Fog_a_byte()
			*/
			_gim_buffer 	Fog_a_buffer( _gim_buffer buffer , _gim_Ulong * buffer_lenght );
			/*! \brief	Use this function to encode a byte 
				\param	c		Is the character to encode<BR>
								( ( \a c >= 0 ) && ( \a c < 256 ) )
				\param	base	It indicates tne base in which the character will be encoded<BR>
								( ( \a base >= \a MIN_BASE ) && ( \a base <= \a MAX_BASE ) )
				\param	digit	It indicates how many characters will compose the encode character<BR>
								\a digit >= 2
				\return The pointer of the string of the encoded character or NULL if some errors occurred
				\see	Fog_a_buffer()
			*/
			char *	Fog_a_byte( _gim_Uchar c , _gim_Uint32 base , _gim_Uint8 digit );
			/*! \brief	Use this function to calculate the necessary minimum number of characters<BR>
						to encode a file
				\param	filename	Is the name of the file within to which trying the higest value to encode<BR>
						\a filename != NULL
				\param	base	It indicates tne base in which the character will be encoded<BR>
								( ( \a base >= \a MIN_BASE ) && ( \a base <= \a MAX_BASE ) )
				\return The minimum number of characters to encode the file
			*/
			_gim_Uint32	min_digit( char * filename , _gim_int8 base );
			/*! \brief	Use this function to calculate the necessary minimum number of characters<BR>
						to encode a buffer
				\param	buffer			Is the buffer within to which trying the higest value to encode<BR>
										\a buffer != NULL
				\param	buffer_lenght	Is the lenght of the buffer to encode<BR>
										\a buffer_lenght > 0
				\param	base			It indicates tne base in which the character will be encoded<BR>
										( ( \a base >= \a MIN_BASE ) && ( \a base <= \a MAX_BASE ) )
				\return The minimum number of characters to encode the buffer
			*/
			_gim_Uint32	min_digit( _gim_buffer buffer , _gim_Ulong buffer_lenght , _gim_int8 base );
		private:
			int	tmp_crt( _gim_int8 , _gim_int8 , _gim_Uchar );
		};

	/*! \class 		cnc_decoding_class
		\brief		CnC internal class to decode
	*/
	class  gim_cnc_decoding_obj {
		public:
			/*! \brief	Use this function to decode a buffer 
				\param	buffer			Is the poiter to the buffer you want to decode<BR>
										\a buffer != NULL
				\param	buffer_lenght	Initially is the buffer lenght<BR>
			 							At the end will be the lenght of the decoded buffer<BR>
										\a buffer_lenght > 0
				\return The pointer of the decoded buffer or NULL if some errors occurred
				\see	Fog_a_byte()
			*/

			_gim_buffer	Fog_a_buffer( _gim_buffer buffer , _gim_Ulong * buffer_lenght );
			/*! \brief	Use this function to encode a byte
				\param	resti	The pointer of the string of the encoded character<BR>
								\a resti != NULL
				\return the result of decodes
				\see	Fog_a_buffer()
			*/
			int	Fog_a_byte( char * resti );
		};

#endif /* _CNC_CODING_H */
