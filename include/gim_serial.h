//
//  gim_serial.h
//
//  Gim - Generic Information Manager
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
//  Serial port (RS-232 / USB-serial / UART of a Raspberry Pi).
//  Same interface on Windows (COMn) and on Linux (/dev/ttyXXX, /dev/serial0).
//  Used by the PC side of SSP and GCP (SYNC/COMM_CONTRACT.md).

/*!     \file       gim_serial.h
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#ifndef _GIM_SERIAL_H_
#define _GIM_SERIAL_H_

	#include "gim_custom_type.h"

	/*!	\class		gim_serial_obj
		\brief		A serial port: 8 data bits, no parity, 1 stop bit, no flow control, raw mode.
					Read and write are byte oriented.
	*/
	class gim_serial_obj {
		public:
			/*! Builds a closed serial object. */
			gim_serial_obj();
			/*! Closes the port if it is open. */
			~gim_serial_obj();

			/*! Opens and configures the port.
				\param	port	"COM3" on Windows, "/dev/serial0" or "/dev/ttyUSB0" on Linux
				\param	baud	speed in bit/s: 9600, 19200, 38400, 57600, 115200
				\return	__GIM_OK on success, __GIM_NOT_OK otherwise (the reason is logged)
			*/
			_gim_flag	open( const char * port , _gim_Uint32 baud );

			/*! Closes the port. */
			void		close( void );

			/*! Tells whether the port is open.
				\return	__GIM_YES or __GIM_NO
			*/
			_gim_flag	is_open( void );

			/*! Writes all the bytes (blocking until they are handed to the system).
				\param	buf		the bytes
				\param	len		how many
				\return	the number of bytes written, -1 on error
			*/
			int			write( const unsigned char * buf , int len );

			/*! Reads up to len bytes; waits at most timeout_ms for the first byte.
				It returns as soon as at least one byte is available.
				\param	buf			where to store the bytes
				\param	len			maximum number of bytes
				\param	timeout_ms	how long to wait for the first byte
				\return	the number of bytes read (0 on timeout), -1 on error
			*/
			int			read( unsigned char * buf , int len , int timeout_ms );

			/*! Discards the bytes waiting in the input and output buffers. */
			void		flush( void );

		private:
			/*! \brief	__GIM_YES while the port is open. */
			_gim_flag	opened;
			/*! \brief	Windows: HANDLE of the port (void * to keep <windows.h> out of this header). */
			void *		handle;
			/*! \brief	Linux: file descriptor of the port. */
			int			fd;
	};

#endif /* _GIM_SERIAL_H_ */
