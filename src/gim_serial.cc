//
//  gim_serial.cc
//
//  Gim - Generic Information Manager
//  Serial port: Windows (COMn, Win32 API) and Linux / Raspberry Pi (termios).
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

#include "../include/gim_serial.h"
#include <string.h>
#include <stdio.h>

#ifdef _WIN32

	#include <windows.h>

	gim_serial_obj::gim_serial_obj() {
		opened = __GIM_NO;
		handle = NULL;
		fd = -1;
	}

	gim_serial_obj::~gim_serial_obj() {
		close();
	}

	_gim_flag	gim_serial_obj::open( const char * port , _gim_Uint32 baud ) {
		char			name[64];
		DCB				dcb;
		COMMTIMEOUTS	to;
		HANDLE			h;
		close();
		/* COM10 and above need the \\.\ prefix */
		sprintf( name , "\\\\.\\%s" , port );
		h = CreateFileA( name , GENERIC_READ | GENERIC_WRITE , 0 , NULL , OPEN_EXISTING , 0 , NULL );
		if ( h == INVALID_HANDLE_VALUE ) {
			printf( "gim_serial: cannot open %s (error %lu)\n" , port , (unsigned long) GetLastError() );
			return __GIM_NOT_OK;
		}
		memset( &dcb , 0 , sizeof( dcb ) );
		dcb.DCBlength = sizeof( dcb );
		if ( ! GetCommState( h , &dcb ) ) {
			CloseHandle( h );
			return __GIM_NOT_OK;
		}
		dcb.BaudRate = (DWORD) baud;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.fBinary = TRUE;
		dcb.fParity = FALSE;
		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		if ( ! SetCommState( h , &dcb ) ) {
			CloseHandle( h );
			return __GIM_NOT_OK;
		}
		memset( &to , 0 , sizeof( to ) );
		SetCommTimeouts( h , &to );
		handle = (void *) h;
		opened = __GIM_YES;
		flush();
		return __GIM_OK;
	}

	void		gim_serial_obj::close( void ) {
		if ( opened == __GIM_YES && handle != NULL )
			CloseHandle( (HANDLE) handle );
		handle = NULL;
		opened = __GIM_NO;
	}

	_gim_flag	gim_serial_obj::is_open( void ) {
		return opened;
	}

	int			gim_serial_obj::write( const unsigned char * buf , int len ) {
		DWORD done = 0;
		if ( opened != __GIM_YES )
			return -1;
		if ( ! WriteFile( (HANDLE) handle , buf , (DWORD) len , &done , NULL ) )
			return -1;
		FlushFileBuffers( (HANDLE) handle );
		return (int) done;
	}

	int			gim_serial_obj::read( unsigned char * buf , int len , int timeout_ms ) {
		COMMTIMEOUTS	to;
		DWORD			got = 0;
		if ( opened != __GIM_YES )
			return -1;
		/* return as soon as one byte is there; otherwise wait timeout_ms */
		memset( &to , 0 , sizeof( to ) );
		to.ReadIntervalTimeout = MAXDWORD;
		to.ReadTotalTimeoutMultiplier = MAXDWORD;
		to.ReadTotalTimeoutConstant = (DWORD) timeout_ms;
		SetCommTimeouts( (HANDLE) handle , &to );
		if ( ! ReadFile( (HANDLE) handle , buf , (DWORD) len , &got , NULL ) )
			return -1;
		return (int) got;
	}

	void		gim_serial_obj::flush( void ) {
		if ( opened == __GIM_YES )
			PurgeComm( (HANDLE) handle , PURGE_RXCLEAR | PURGE_TXCLEAR );
	}

#else

	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>
	#include <sys/select.h>
	#include <sys/time.h>
	#include <errno.h>

	gim_serial_obj::gim_serial_obj() {
		opened = __GIM_NO;
		handle = NULL;
		fd = -1;
	}

	gim_serial_obj::~gim_serial_obj() {
		close();
	}

	static speed_t	gim_serial_speed( _gim_Uint32 baud , int * ok ) {
		*ok = 1;
		switch ( baud ) {
			case 9600:		return B9600;
			case 19200:		return B19200;
			case 38400:		return B38400;
			case 57600:		return B57600;
			case 115200:	return B115200;
		}
		*ok = 0;
		return B9600;
	}

	_gim_flag	gim_serial_obj::open( const char * port , _gim_Uint32 baud ) {
		struct termios	t;
		speed_t			sp;
		int				ok = 0;
		close();
		sp = gim_serial_speed( baud , &ok );
		if ( ! ok ) {
			printf( "gim_serial: speed %u not supported\n" , (unsigned int) baud );
			return __GIM_NOT_OK;
		}
		fd = ::open( port , O_RDWR | O_NOCTTY );
		if ( fd < 0 ) {
			printf( "gim_serial: cannot open %s (%s)\n" , port , strerror( errno ) );
			return __GIM_NOT_OK;
		}
		if ( tcgetattr( fd , &t ) != 0 ) {
			::close( fd );
			fd = -1;
			return __GIM_NOT_OK;
		}
		cfmakeraw( &t );
		cfsetispeed( &t , sp );
		cfsetospeed( &t , sp );
		t.c_cflag &= ~( PARENB | CSTOPB | CRTSCTS | CSIZE );
		t.c_cflag |= CS8 | CLOCAL | CREAD;
		t.c_cc[ VMIN ] = 0;
		t.c_cc[ VTIME ] = 0;
		if ( tcsetattr( fd , TCSANOW , &t ) != 0 ) {
			::close( fd );
			fd = -1;
			return __GIM_NOT_OK;
		}
		handle = NULL;
		opened = __GIM_YES;
		flush();
		return __GIM_OK;
	}

	void		gim_serial_obj::close( void ) {
		if ( opened == __GIM_YES && fd >= 0 )
			::close( fd );
		fd = -1;
		opened = __GIM_NO;
	}

	_gim_flag	gim_serial_obj::is_open( void ) {
		return opened;
	}

	int			gim_serial_obj::write( const unsigned char * buf , int len ) {
		int done = 0;
		if ( opened != __GIM_YES )
			return -1;
		while ( done < len ) {
			ssize_t n = ::write( fd , buf + done , (size_t) ( len - done ) );
			if ( n < 0 ) {
				if ( errno == EINTR )
					continue;
				return -1;
			}
			done += (int) n;
		}
		tcdrain( fd );
		return done;
	}

	int			gim_serial_obj::read( unsigned char * buf , int len , int timeout_ms ) {
		fd_set			rd;
		struct timeval	tv;
		int				r;
		if ( opened != __GIM_YES )
			return -1;
		FD_ZERO( &rd );
		FD_SET( fd , &rd );
		tv.tv_sec = timeout_ms / 1000;
		tv.tv_usec = ( timeout_ms % 1000 ) * 1000;
		do {
			r = select( fd + 1 , &rd , NULL , NULL , &tv );
		} while ( r < 0 && errno == EINTR );
		if ( r < 0 )
			return -1;
		if ( r == 0 )
			return 0;
		{
			ssize_t n;
			do {
				n = ::read( fd , buf , (size_t) len );
			} while ( n < 0 && errno == EINTR );
			return (int) n;
		}
	}

	void		gim_serial_obj::flush( void ) {
		if ( opened == __GIM_YES )
			tcflush( fd , TCIOFLUSH );
	}

#endif
