//
//  gim_picgim.h
//
//  Gim - Generic Information Manager
//  Version 2.X-x  -  Milestone 3.0
//
//  [http://www.gkript.org]
//
/*#############################################################################

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

#############################################################################*/
//
// Gim is developed with Anjuta version 1.2.4  - 3.18.2
//

/*!     \file       gim_picgim.h
        \version    3.0
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (asyntote)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

/*!     \class       gim_picgim_obj
        \brief       "A Picgim infrastructure"
        \author      asyntote
        \version     3.0
        \date        2003-2017
*/

#ifndef _GIM_PICGIM_OBJ_H_
#define _GIM_PICGIM_OBJ_H_


		
    class gim_picgim_obj {
		public:
			
		private:

		public:

			/*!	\brief	CRC-8 used by SSP (SYNC/COMM_CONTRACT.md, sez. 1).
					Dallas/Maxim: poly 0x8C (0x31 reflected), init 0x00, reflected, no xorout.
					Check: crc8("123456789") == 0xA1.
				*/
				_gim_Uint8	crc8_ssp	( const _gim_Uint8 * data , _gim_Uint32 length );

				/*!	\brief	CRC-32 IEEE 802.3 used by GCP (SYNC/COMM_CONTRACT.md, sez. 2.4).
					Poly 0xEDB88320 reflected, init and xorout 0xFFFFFFFF.
					Check: crc32("123456789") == 0xCBF43926. On the wire the 4 bytes go LSB first.
				*/
				_gim_Uint32	crc32_gcp	( const _gim_Uint8 * data , _gim_Uint32 length );

				/*!gim_picgim_obj constructor
		    */
		    inline gim_picgim_obj() {
				gim_error->set( "gim_picgim_obj::gim_picgim_obj()" , "A new Picgim infrastructure allocated" );
		    };


		    /*!gim_picgim_obj distructor
		    */
		    inline ~gim_picgim_obj() {

		    };
    };

#endif // _GIM_PICGIM_OBJ_H_

