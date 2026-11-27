//
//  gim_rpi_gpio.cc
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
//  GPIO: Linux sysfs backend (Raspberry Pi); Windows has no GPIO and reports an error.

/*!     \file       gim_rpi_gpio.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include "../include/gim_rpi_gpio.h"
#include "../include/gim_error.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32

	_gim_flag	gim_rpi_gpio_obj::export_pin( int pin , _gim_flag output ) {
		(void) pin; (void) output;
		gim_error->Set( GIM_ERROR_CRITICAL , "gim_rpi_gpio_obj::export_pin" , "GPIO is not available on Windows" );
		return __GIM_NOT_OK;
	}

	_gim_flag	gim_rpi_gpio_obj::unexport_pin( int pin ) {
		(void) pin;
		return __GIM_NOT_OK;
	}

	_gim_flag	gim_rpi_gpio_obj::write( int pin , int value ) {
		(void) pin; (void) value;
		return __GIM_NOT_OK;
	}

	int			gim_rpi_gpio_obj::read( int pin ) {
		(void) pin;
		return -1;
	}

#else

	#include <fcntl.h>
	#include <unistd.h>
	#include <errno.h>
	#include <time.h>

	#define GIM_RPI_GPIO_SYSFS		"/sys/class/gpio"

	/* sysfs writes the new directory a little after the export: wait for it (up to 1 s) */
	static int gim_rpi_gpio_wait( const char * path ) {
		struct timespec ts;
		int i;
		ts.tv_sec = 0;
		ts.tv_nsec = 10000000L;
		for ( i = 0 ; i < 100 ; i++ ) {
			if ( access( path , F_OK ) == 0 )
				return 0;
			nanosleep( &ts , NULL );
		}
		return -1;
	}

	static int gim_rpi_gpio_write_file( const char * path , const char * text ) {
		int fd = open( path , O_WRONLY );
		ssize_t n;
		if ( fd < 0 )
			return -1;
		n = write( fd , text , strlen( text ) );
		close( fd );
		return ( n == (ssize_t) strlen( text ) ) ? 0 : -1;
	}

	_gim_flag	gim_rpi_gpio_obj::export_pin( int pin , _gim_flag output ) {
		char	path[128] , text[32];
		sprintf( text , "%d" , pin );
		sprintf( path , GIM_RPI_GPIO_SYSFS "/gpio%d/direction" , pin );
		if ( access( path , F_OK ) != 0 ) {
			if ( gim_rpi_gpio_write_file( GIM_RPI_GPIO_SYSFS "/export" , text ) != 0 ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_rpi_gpio_obj::export_pin" , "cannot export GPIO %d (%s): run as root or add the user to the gpio group" , pin , strerror( errno ) );
				return __GIM_NOT_OK;
			}
			if ( gim_rpi_gpio_wait( path ) != 0 ) {
				gim_error->Set( GIM_ERROR_CRITICAL , "gim_rpi_gpio_obj::export_pin" , "GPIO %d did not appear after export" , pin );
				return __GIM_NOT_OK;
			}
		}
		if ( gim_rpi_gpio_write_file( path , output == __GIM_YES ? "out" : "in" ) != 0 ) {
			gim_error->Set( GIM_ERROR_CRITICAL , "gim_rpi_gpio_obj::export_pin" , "cannot set the direction of GPIO %d" , pin );
			return __GIM_NOT_OK;
		}
		return __GIM_OK;
	}

	_gim_flag	gim_rpi_gpio_obj::unexport_pin( int pin ) {
		char text[32];
		sprintf( text , "%d" , pin );
		if ( gim_rpi_gpio_write_file( GIM_RPI_GPIO_SYSFS "/unexport" , text ) != 0 )
			return __GIM_NOT_OK;
		return __GIM_OK;
	}

	_gim_flag	gim_rpi_gpio_obj::write( int pin , int value ) {
		char path[128];
		sprintf( path , GIM_RPI_GPIO_SYSFS "/gpio%d/value" , pin );
		if ( gim_rpi_gpio_write_file( path , value ? "1" : "0" ) != 0 )
			return __GIM_NOT_OK;
		return __GIM_OK;
	}

	int			gim_rpi_gpio_obj::read( int pin ) {
		char	path[128] , c = 0;
		int		fd;
		ssize_t n;
		sprintf( path , GIM_RPI_GPIO_SYSFS "/gpio%d/value" , pin );
		fd = ::open( path , O_RDONLY );
		if ( fd < 0 )
			return -1;
		n = ::read( fd , &c , 1 );
		close( fd );
		if ( n != 1 )
			return -1;
		return ( c == '1' ) ? 1 : 0;
	}

#endif
