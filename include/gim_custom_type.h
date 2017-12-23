//
// gim_custom_type.h
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

/*!	\file		gim_custom_type.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_CUSTOM_TYPE_H_
#define _GIM_CUSTOM_TYPE_H_
	
	/*! \enum	_gim_return_value
				This enumerator describe the possible return value from gim's metohods.
	*/
	enum _gim_return_value {
		__GIM_ERROR = -1 ,
		__GIM_NO,
		__GIM_YES,
		__GIM_WARNING,
		__GIM_OPEN,
		__GIM_CLOSE,
		__GIM_EXIST,
		__GIM_NOT_EXIST,
		__GIM_OK,
		__GIM_NOT_OK,
		__GIM_DISABLE,
		__GIM_ENABLE,
		__GIM_OFF,
		__GIM_ON,
		__GIM_FOUND,
		__GIM_NOT_FOUND,
		__GIM_TRUE,
		__GIM_FALSE,
		__LEX_UNKNOW,
		__LEX_A,
		__LEX_B,
	};
	
	enum GimThState {
		__GIM_TH_ERROR	= -1,
		__GIM_TH_IDLE	= 1,
		__GIM_TH_INIT,
		__GIM_TH_WAIT,
		__GIM_TH_WORKING,
		__GIM_TH_CRYPT,
		__GIM_TH_MCRYPT_FL,
		__GIM_TH_MCRYPT_FD,
		__GIM_TH_DECRYPT,
		__GIM_TH_EVAL,
		__GIM_TH_DISP_EN,
		__GIM_TH_THWORK,
		__GIM_TH_THCOMP,
		__GIM_TH_THWRITE,
		__GIM_TH_THWAITSYNKSTART,
		__GIM_TH_THWAITSYNKSTOP,
		__GIM_TH_THFLUSH,
		__GIM_TH_STOP,
		__GIM_TH_STOPPED,
		__GIM_TH_EXIT,
		__GIM_TH_THSUM,
		__GIM_TH_THWAITSYNK,
		__GIM_TH_FINISH,
	};

	
	typedef	char							_gim_flag;
	
	typedef unsigned char					_gim_Uchar;

	typedef char							_gim_int8;
	typedef short							_gim_int16;
	typedef int								_gim_int32;
	
	typedef unsigned char					_gim_Uint8;
	typedef unsigned short					_gim_Uint16;
	typedef unsigned int					_gim_Uint32;
	
	typedef int								_gim_long;
	typedef unsigned int					_gim_Ulong;
	typedef long long						_gim_Llong;
	
	typedef	unsigned char *					_gim_Ubuffer;
	typedef	char *							_gim_buffer;
	typedef char *							_gim_map;

	typedef	class	gim_html_obj			_gim_html;
	
	typedef class	MTRand					_gim_rand;
	typedef class	gim_file_obj			_gim_file;
	typedef	class	gim_time_obj			_gim_time;	
	typedef class	gim_prsr_obj			_gim_kvalue;
	typedef class	gim_prsr_obj			_gim_prsr;
	typedef	struct	gim_global_item_handler	_gim_handler;
	typedef struct	gim_file_header_struct	_gim_gkp_header;
	typedef struct	gim_file_chunk_struct	_gim_gkp_chunk;
	typedef struct	gim_file_list_struct	_gim_gkp_flist;
	typedef struct	gim_gkdp_data_struct	_gim_gkdp;
	typedef struct	gim_gkfp_obj			_gim_gkfp;
	typedef	struct	gim_processor_struct 	_gim_processor;
	typedef	struct	gim_memory_struct 		_gim_memory;
	typedef	struct	gim_memory_page_obj 	_gim_mempage;
	typedef struct	gim_interface_data		_gim_interface;
	typedef 		_gim_flag				_gim_th_state;
	typedef	class	gim_string_obj			_gim_string;
	typedef class	gim_personal_log_obj	_gim_log;
	typedef	struct	gim_list_item 			_gim_list_item;
	typedef	struct	gim_option_item			_gim_option;
	typedef	class	gim_list_obj 			_gim_list;


#endif /* _GIM_CUSTOM_TYPE_H_ */
