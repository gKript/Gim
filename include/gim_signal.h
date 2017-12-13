//
// gim_signal.h
//
// Gim  -  Generic Information Manager static library
// Version 1.0-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%201.0?by=version]
//
/*#############################################################################

	Copyright (C) 2002, 2003 
	Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)

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
//
// Gim is developed with Anjuta version 1.2.4
//

/*!	\file		gim_signal.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_SIGNAL_H_
#define _GIM_SIGNAL_H_

#include "gim_base_header.h"

	/*!	\fn		void	gim_signal_received( int sig )
		\warning	\n <b>It is not possible to use directly this function.</b>
		\internal
		\brief		This funtion is automatically called by Gim when a signal will be received.
	*/
	void	gim_signal_received( int sig );

	/*!	\fn		void	gim_signal_handler( void )
		\warning	\n <b>It is not possible to use directly this function.</b>
		\internal
		\brief		This funtion is automatically called by Gim at the constructor. It set the default action when a signal will be received
					
	*/
	void	gim_signal_handler( void );

	/*!	\fn		void	gim_forever( void )
		\brief	If you call this function Gim will enter in a forever loop. You can only type ctrl-c to exit. 
				It is useful to test the signal handler.
	*/
	void	gim_forever( void );

	/*!	\fn		void	gim_fatal_test( void )
		\brief	If you call this function Gim generates a \a FATAL occurrence. It is useful to test the \a FATAL error
	*/
	void	gim_fatal_test( void );

#endif // _GIM_SIGNAL_H_
