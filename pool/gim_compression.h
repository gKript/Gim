//
// gim_compression.h
//
// Gim  -  Generic Information Manager static library
// Version 2.5.x
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.5]
//
/*#############################################################################

	Copyright (C) 2002, 2003 
	Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)
	and
	Adriano Zannoni (Tatti)

	This file is part of Gim library.

	Gim is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Gim is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Gim; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#############################################################################*/
		
/*!	\file		gim_compression.h
	\version	2.5-x
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
	//~\warning	This file is not yet included by any Gim header. It is documented only for completeness.
				//~ You cannot use the code inside this header.
	//~\todo		The compression is planned on the Gim 2.5 Milestone on Trac. You can visit this link if you want more datils
				//~ [ http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.5 ]
*/ 		



/**********************************************************************************	
 *	BZ2
*/
/*-------------------------------------------------------------*/
/*--- Public header file for the library.                   ---*/
/*---                                               bzlib.h ---*/
/*-------------------------------------------------------------*/
/*--
  This file is a part of bzip2 and/or libbzip2, a program and
  library for lossless, block-sorting data compression.

  Copyright (C) 1996-2002 Julian R Seward.  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

  2. The origin of this software must not be misrepresented; you must 
     not claim that you wrote the original software.  If you use this 
     software in a product, an acknowledgment in the product 
     documentation would be appreciated but is not required.

  3. Altered source versions must be plainly marked as such, and must
     not be misrepresented as being the original software.

  4. The name of the author may not be used to endorse or promote 
     products derived from this software without specific prior written 
     permission.

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Julian Seward, Cambridge, UK.
  jseward@acm.org
  bzip2/libbzip2 version 1.0 of 21 March 2000

  This program is based on (at least) the work of:
     Mike Burrows
     David Wheeler
     Peter Fenwick
     Alistair Moffat
     Radford Neal
     Ian H. Witten
     Robert Sedgewick
     Jon L. Bentley

  For more information on these sources, see the manual.
--*/



/**********************************************************************************	
 *	LZH
*/
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
		
#ifndef _GIM_COMPRESSION_H_
#define _GIM_COMPRESSION_H_

	#include "gim_base_header.h"
	#include "gim_checksum.h"
	#include "gim_mersennetwister.h"

	#define		GIM_BZ2_COMPRESSION			__GIM_DISABLE
	#define		GIM_GZ_COMPRESSION			__GIM_DISABLE
	#define		GIM_LZH_COMPRESSION			__GIM_DISABLE

	/*!
		\enum	gim_compression_type
		\brief	The list of compression type you can choice. \n
	*/	
	enum gim_compression_type {
		__GIM_UNKNOWN_COMPRESSION = -1,	/*!<	This is an unknown compression type */
		__GIM_BZ2 = 1,					/*!<	This is for a BZ2 compression */
		__GIM_GZIP,						/*!<	This is for a BZ2 compression */
		__GIM_LZH						/*!<	This is for a LZH compression */
	};
	
	/*!	\def	__GIM_BZ_BLOCKSIZE100K
				It is a number with you can balance the compress factor... 0 bigger result but faster  -  9 smaller result but slower
		 		Per default is \a 9
		\def	__GIM_BZ_VERBOSITY
				It is the \a verbosity for the BZ2 compression. Gim want a BZ2 silent compresison so per default is 0
		\def	__GIM_BZ_WORKFACTOR
				

	*/

	#define 	__GIM_BZ_BLOCKSIZE100K		9		// 0 - 9    0 bigger and faster  -  9 smaller and slower
	#define		__GIM_BZ_VERBOSITY			4		// 0 		0 Not verbose    -   for more infos please refer to the official bzlib site
	#define		__GIM_BZ_WORKFACTOR			30		// 0 - 255
	
	
	/*! \class 		gim_compression_obj
		\brief		With this useful cleass you can compress with different algorithm a buffer
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008
		\warning	This class is not yet included by any Gim header. It is documented only for completeness.
					You cannot use this class.
		\todo		The compression is planned on the Gim 2.5 Milestone on Trac. You can visit this link if you want more datils
					[ http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.5 ]
	*/
	class gim_compression_obj {
		public:
			/*!	\fn	Compress( _gim_flag comp_type , _gim_buffer soub , _gim_Uint32 soulen , _gim_Uint32 * destlen )

				\brief	This function compress a \a buffer into an other buffer.
				\param	comp_type	One of a compression type defined in the gim_compression_type enum.
				\param	soub		The source buffer (uncompressed).
				\param	soulen		The source buffer lenght.
				\param	destlen		At the end will be the lenght of the compressed buffer.
				\return	The compressed buffer
			*/
/*			_gim_buffer	Compress	( _gim_flag comp_type , _gim_buffer soub , _gim_Uint32 soulen , _gim_Uint32 * destlen );
*/			
			/*!	\fn	Uncompress( _gim_flag comp_type , _gim_buffer soub , _gim_Uint32 soulen , _gim_Uint32 destlen );

				\brief	This function uncompress a \a buffer into an other buffer.
				\param	comp_type	One of a compression type defined in the gim_compression_type enum.
				\param	soub		The source buffer (compressed).
				\param	soulen		The source buffer lenght.
				\param	destlen		At the end will be the lenght of the uncompressed buffer.
				\return	The uncompressed buffer
			*/
/*			_gim_buffer	Uncompress	( _gim_flag comp_type , _gim_buffer soub , _gim_Uint32 soulen , _gim_Uint32 destlen );
*/		
		private:
/*			_gim_flag	self_test( void );
*/		
		public:
			gim_compression_obj();

	};

/*	extern gim_compression_obj	* gim_compression;
*/
#endif // _GIM_COMPRESSION_H_
