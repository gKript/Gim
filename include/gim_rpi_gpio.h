//
//  gim_rpi_gpio.h
//
//  Gim - Generic Information Manager
//  GPIO pins (Raspberry Pi and other Linux boards). Digital input / output only.
//
//  Linux backend: sysfs (/sys/class/gpio). Pin numbers are the BCM / global GPIO numbers.
//  On Windows there are no GPIO pins: every call returns an error.
//
/*#############################################################################

	This file is part of Gim library.

	Gim is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Gim is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

#############################################################################*/

#ifndef _GIM_RPI_GPIO_H_
#define _GIM_RPI_GPIO_H_

	#include "gim_custom_type.h"

	/*!	\class		gim_rpi_gpio_obj
		\brief		Digital GPIO pins: export a pin as input or output, then read or write it.
	*/
	class gim_rpi_gpio_obj {
		public:
			/*! Exports a pin and sets its direction.
				\param	pin		the GPIO number
				\param	output	__GIM_YES for an output pin, __GIM_NO for an input pin
				\return	__GIM_OK on success, __GIM_NOT_OK otherwise (the reason is printed)
			*/
			_gim_flag	export_pin( int pin , _gim_flag output );

			/*! Releases a pin previously exported.
				\param	pin		the GPIO number
				\return	__GIM_OK on success
			*/
			_gim_flag	unexport_pin( int pin );

			/*! Writes an output pin.
				\param	pin		the GPIO number
				\param	value	0 (low) or 1 (high)
				\return	__GIM_OK on success, __GIM_NOT_OK otherwise
			*/
			_gim_flag	write( int pin , int value );

			/*! Reads a pin (input or output).
				\param	pin		the GPIO number
				\return	0 (low), 1 (high), -1 on error
			*/
			int			read( int pin );
	};

#endif /* _GIM_RPI_GPIO_H_ */
