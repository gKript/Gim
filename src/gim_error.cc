//
// gim_error.cc
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

#include "../include/gim.h"
#include <stdlib.h>

gim_error_obj		* gim_error;
gim_time_obj		error_time;
gim_error_list_str	gim_error_list [] = {
{	0	,	GIM_SEPARATOR		,	0	,	"Initialization ok"																		} ,
{	1	,	GIM_ERROR_MESSAGE	,	-1	,	"No memory list, starting initialization..." 											} ,
{	2	,	GIM_ERROR_CRITICAL	,	-1	,	"File not found" 																		} ,
{	3	,	GIM_ERROR_MESSAGE	,	0	,	"Start to use gim File Debug" 															} ,
{	4	,	GIM_ERROR_MESSAGE	,	0	,	"Stop  to use gim File Debug" 															} ,
{	5	,	GIM_ERROR_WARNING	,	-1	,	"The gim log file is already close" 													} ,
{	6	,	GIM_ERROR_FATAL		,	-1	,	"I cannot open in write mode the debug file on disk" 									} ,
{	7	,	GIM_ERROR_FATAL		,	-1	,	"Impossible to write on Fdebug file" 													} ,
{	8	,	GIM_ERROR_FATAL		,	-1	,	"I cannot open in write mode file on disk" 												} ,
{	9	,	GIM_ERROR_MESSAGE	,	0	,	"Memory allocation system is up" 														} ,
{	10	,	GIM_ERROR_MESSAGE	,	0	,	"Memory allocation system goes down..." 												} ,
{	11	,	GIM_ERROR_MESSAGE	,	0	,	"Memory allocation system goes down...Done" 											} ,
{	12	,	GIM_ERROR_FATAL		,	-1	,	"No more free memory" 																	} ,
{	13	,	GIM_ERROR_WARNING	,	-1	,	"No memory list, starting initialization..." 											} ,
{	14	,	GIM_ERROR_CRITICAL	,	-1	,	"No memory list" 																		} ,
{	15	,	GIM_ERROR_CRITICAL	,	-1	,	"Frame not found" 																		} ,
{	16	,	GIM_ERROR_MESSAGE	,	0	,	"Start to release list..." 																} ,
{	17	,	GIM_ERROR_OK		,	0	,	"Gim initialization ok" 																} ,
{	18	,	GIM_SEPARATOR		,	0	,	"Gim shutting down..." 																	} ,
{	19	,	GIM_ERROR_OK		,	0	,	"Gim shutting down...Done! bye" 														} ,
{	20	,	GIM_ERROR_CRITICAL	,	-1	,	"The gim lib is not the required version" 												} ,
{	21	,	GIM_ERROR_CRITICAL	,	-1	,	"We suggest to download from http://www.gkript.org" 									} ,
{	22	,	GIM_ERROR_CRITICAL	,	-1	,	"    and install the required version" 													} ,
{	23	,	GIM_ERROR_CRITICAL	,	-1	,	"Gim cannot crypt/decrypt and cannot hash anything but will work as best as possible" 	} ,
{	24	,	GIM_ERROR_CRITICAL	,	-1	,	"Some problem with stat/lstat" 															} ,
{	25	,	GIM_ERROR_MESSAGE	,	0	,	"It's a dir" 																			} ,
{	26	,	GIM_ERROR_MESSAGE	,	0	,	"It's NOT a dir" 																		} ,
{	27	,	GIM_ERROR_MESSAGE	,	0	,	"It's a regular file" 																	} ,
{	28	,	GIM_ERROR_MESSAGE	,	0	,	"It's NOT a regular file" 																} ,
{	29	,	GIM_ERROR_MESSAGE	,	0	,	"It's a Link" 																			} ,
{	30	,	GIM_ERROR_MESSAGE	,	0	,	"It's NOT a Link" 																		} ,
{	31	,	GIM_ERROR_MESSAGE	,	0	,	"It's a Device" 																		} ,
{	32	,	GIM_ERROR_MESSAGE	,	0	,	"It's NOT a Device" 																	} ,
{	33	,	GIM_ERROR_FATAL		,	-1	,	"I'm not understanding... You want to CREATE or EXTRACT ?" 								} ,
{	34	,	GIM_ERROR_MESSAGE	,	0	,	"List succesfully released" 															} ,
{	35	,	GIM_ERROR_MESSAGE	,	0	,	"The file manager is down" 																} ,
{	36	,	GIM_ERROR_MESSAGE	,	0	,	"The file manager is goning down..." 													} ,
{	37	,	GIM_ERROR_MESSAGE	,	0	,	"!!!" 																					} ,
{	38	,	GIM_ERROR_MESSAGE	,	0	,	"!!!" 																					} ,

};


void	gim_error_obj::set	( const char * type , const char * function , const char * message , int value ) {
	char mess[512];
	strcpy	( this->type , type );
	sprintf	( this->function , "%s()" , function );
	strcpy	( this->message	, message );
	this->value	= value;
	if  ( ! ( strcmp ( type , GIM_ERROR_CRITICAL ) ) )
		sprintf( mess , "ERROR : %s" , message );
	else
		strcpy( mess , message );
	if ( gim_log->is_open() == __GIM_YES ) {
		if ( first_in != NULL ) {
			lflush();
		}
		else {
			gim_log->addline_format( type , this->function , mess , value );
		}
	}
	else {
		if ( log_enable == __GIM_YES ) {
			if ( ladd( type , this->function , mess , value ) == __GIM_NOT_OK ) {
				puts( "errore" );
			}
		}
//		else 
//			lflush();
	}
	if ( autoget ) get ();
}


void	gim_error_obj::set	( pid_t cur_pid , const char * type , const char * function , const char * message , int value ) {
	strcpy	( this->type , type );
	sprintf	( this->function , "%s()" , function );
	strcpy	( this->message	, message );
	this->value	= value;
	if ( gim_log->is_open() == __GIM_YES ) {
		if ( first_in != NULL ) {
			lflush();
		}
		else {
			gim_log->addline_format( type , this->function , message , value );
		}
	}
	else {
		if ( log_enable == __GIM_YES ) {
			if ( ladd( type , this->function , message , value ) == __GIM_NOT_OK ) {
				puts( "errore" );
			}
		}
//		else 
//			lflush();
	}
	if ( autoget )
		get ();
}


void	gim_error_obj::set	( const char * function , const char * message ) {
	set( GIM_ERROR_MESSAGE , function , message , 0 );
}


void	gim_error_obj::Set	( const char * function , const char * format , ... ) {
	char	message[1024];
	va_list	VAList;
	va_start( VAList , format );
	vsprintf( message , format , VAList );
	va_end( VAList );
	set( GIM_ERROR_MESSAGE , function , message , 0 );
}


void	gim_error_obj::Set	( const char * type , const char * function , const char * format , ... ) {
	char	message[1024];
	if ( ! ( strcmp ( type , GIM_ERROR_FATAL ) ) )
		value = __GIM_ERROR;
	else
		value = 0;
	va_list	VAList;
	va_start( VAList , format );
	vsprintf( message , format , VAList );
	va_end( VAList );
	set( type , function , message , value );
}


void	gim_error_obj::set	( const char * function , int index ) {
	_gim_flag fatal = __GIM_NO;
	if  ( ! ( strcmp ( type , GIM_ERROR_FATAL ) ) )
		fatal = __GIM_YES;
	strcpy	( type , gim_error_list[index].type );
	sprintf	( this->function , "%s()" , function );
	strcpy	( message , gim_error_list[index].message );
	value = gim_error_list[index].value;
	if ( index != 7 ) {
		if ( gim_log->is_open() == __GIM_YES ) {
			if ( first_in != NULL ) {
				lflush();
			}
			else {
				gim_log->addline_format( type , this->function , message , value );
			}
		}
		else {
			if ( log_enable == __GIM_YES ) {
				if ( ladd( type , this->function , message , value ) == __GIM_NOT_OK ) {
					puts( "errore" );
				}
			}
//			else 
//				lflush();
		}
	}
	if ( autoget )
		get ();
	if ( ( fatal == __GIM_YES )  &&  ( on_fatal_exit == __GIM_YES ) ) {
		internal_gim->down( value );
	}
}

void	gim_error_obj::get	( void ) {
	if ( value || always_verbose ) {
		if ( ( ( ! ( strcmp ( type , GIM_ERROR_FATAL ) ) )  &&  ( on_fatal_verbose == __GIM_YES ) ) || ( always_verbose ) ) {
			fprintf ( stderr , "[%s] %s%-40s:#%2d: | %s \n" , error_time.process() , type , function , value , message );
			fflush( stderr );
			if ( ( ! ( strcmp ( type , GIM_ERROR_FATAL ) ) )  &&  ( on_fatal_exit == __GIM_YES ) )
				internal_gim->down( value );
		}
	}
}

void	gim_error_obj::set( _gim_int8 cmd , _gim_flag val ) {
	switch(cmd) {
		case GIM_ON_FATAL_EXIT : {
			on_fatal_exit = val;
			break;
		}
		case GIM_ON_FATAL_VERBOSE : {
			on_fatal_verbose = val;
			break;
		}
		case GIM_ALWAYSE_VERBOSE : {
			always_verbose = val;
			break;
		}
		case GIM_AUTOGET : {
			autoget = val;
			break;
		}
	}
}


_gim_flag	gim_error_obj::get( _gim_int8 cmd ) {
	switch(cmd) {
		case GIM_ON_FATAL_EXIT :	return ( on_fatal_exit );
		case GIM_ON_FATAL_VERBOSE :	return ( on_fatal_verbose );
		case GIM_ALWAYSE_VERBOSE :	return ( always_verbose );
		case GIM_AUTOGET :			return ( autoget );
		default :					return __GIM_ERROR;
	}
}


_gim_flag	gim_error_obj::ladd( const char * type , const char * function , const char * message , int value ) {
	gim_error_message * new_message;
	new_message = (gim_error_message *)malloc( sizeof( gim_error_message ) );
	if ( new_message == NULL )
		return __GIM_NOT_OK;
	new_message->next  = NULL;
	strcpy	( new_message->type , type );
	strcpy	( new_message->function , function );
	strcpy	( new_message->message	, message );
	new_message->value	= value;
	if ( first_in == NULL ) {
		last_in = new_message;
		first_in = new_message;
	}
	else {
		last_in->next = new_message;
		last_in = last_in->next;
	}
	return __GIM_OK;
}


void		gim_error_obj::lflush( void ) {
	int cnt = 0;
	char message[256];
	gim_error_message * cursor = first_in;;
	while( cursor ) {
		if ( log_enable == __GIM_YES ) 
			gim_log->addline_format( cursor->type , cursor->function , cursor->message , cursor->value );
		cursor = cursor->next;
		free( first_in );
		first_in = cursor;
		cnt++;
	}
	first_in = NULL;
	last_in  = NULL;
	if ( log_enable == __GIM_YES )
		sprintf( message , "Succesfully flushed %d messages" , cnt );
	else
		sprintf( message , "Succesfully deleted %d messages" , cnt );
	set( "gim_error_obj::lflush" , message );
}
