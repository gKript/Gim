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
	\brief		Basic types, return codes and forward declarations used by all Gim modules.
				Copyright (C) 2002, 2003<BR>
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
		__GIM_ERROR = -1 ,	/*!< generic error */
		__GIM_NO,			/*!< false / no */
		__GIM_YES,			/*!< true / yes */
		__GIM_AUTO,			/*!< automatic choice */
		__GIM_WARNING,		/*!< warning */
		__GIM_OPEN,			/*!< opened */
		__GIM_CLOSE,		/*!< closed */
		__GIM_EXIST,		/*!< exists */
		__GIM_NOT_EXIST,	/*!< does not exist */
		__GIM_OK,			/*!< success */
		__GIM_NOT_OK,		/*!< not successful */
		__GIM_DISABLE,		/*!< disabled */
		__GIM_ENABLE,		/*!< enabled */
		__GIM_OFF,			/*!< off */
		__GIM_ON,			/*!< on */
		__GIM_FOUND,		/*!< found */
		__GIM_NOT_FOUND,	/*!< not found */
		__GIM_TRUE,			/*!< true */
		__GIM_FALSE,		/*!< false */
		__GIM_READY,		/*!< ready */
		__GIM_NOT_READY,	/*!< not ready */
		__GIM_RUNNING,		/*!< running */
		__GIM_NOT_RUNNING,	/*!< not running */
		__GIM_UNKNOWN,		/*!< unknown */
		__LEX_UNKNOWN,		/*!< lexical analysis: unknown token */
		__LEX_A,			/*!< lexical analysis: syntax A */
		__LEX_B,			/*!< lexical analysis: syntax B */
		__LEX_C,			/*!< lexical analysis: syntax C */
	};

	/*! \enum	GimThState
				States of the Gim working threads.
	*/
	enum GimThState {
		__GIM_TH_ERROR	= -1,	/*!< error */
		__GIM_TH_IDLE	= 1,	/*!< idle */
		__GIM_TH_INIT,			/*!< initializing */
		__GIM_TH_WAIT,			/*!< waiting */
		__GIM_TH_WORKING,		/*!< working */
		__GIM_TH_CRYPT,			/*!< encrypting */
		__GIM_TH_MCRYPT_FL,		/*!< multi-file encryption: file list */
		__GIM_TH_MCRYPT_FD,		/*!< multi-file encryption: file data */
		__GIM_TH_DECRYPT,		/*!< decrypting */
		__GIM_TH_EVAL,			/*!< evaluating */
		__GIM_TH_DISP_EN,		/*!< dispatching encryption */
		__GIM_TH_THWORK,		/*!< thread work */
		__GIM_TH_THCOMP,		/*!< thread compute */
		__GIM_TH_THWRITE,		/*!< thread write */
		__GIM_TH_THWAITSYNKSTART,	/*!< waiting to start synchronization */
		__GIM_TH_THWAITSYNKSTOP,	/*!< waiting to stop synchronization */
		__GIM_TH_THFLUSH,		/*!< flushing */
		__GIM_TH_STOP,			/*!< stop requested */
		__GIM_TH_STOPPED,		/*!< stopped */
		__GIM_TH_EXIT,			/*!< exit */
		__GIM_TH_THSUM,			/*!< thread checksum */
		__GIM_TH_THWAITSYNK,	/*!< waiting for synchronization */
		__GIM_TH_FINISH,		/*!< finished */
	};


	/*! Flag type: holds one of the _gim_return_value constants. */
	typedef	char							_gim_flag;

	/*! Unsigned char. */
	typedef unsigned char					_gim_Uchar;

	/*! Signed 8 bit integer. */
	typedef char							_gim_int8;
	/*! Signed 16 bit integer. */
	typedef short							_gim_int16;
	/*! Signed 32 bit integer. */
	typedef int								_gim_int32;

	/*! Unsigned 8 bit integer. */
	typedef unsigned char					_gim_Uint8;
	/*! Unsigned 16 bit integer. */
	typedef unsigned short					_gim_Uint16;
	/*! Unsigned 32 bit integer. */
	typedef unsigned int					_gim_Uint32;

	/*! Long integer. */
	typedef int								_gim_long;
	/*! Unsigned long integer. */
	typedef unsigned int					_gim_Ulong;
	/*! Signed 64 bit integer. */
	typedef long long						_gim_Llong;
	/*! Unsigned 64 bit integer. */
	typedef unsigned long long				_gim_ULlong;
	/*! Memory address. */
	typedef unsigned long long				_gim_address;


	/*! Pointer to unsigned bytes. */
	typedef	unsigned char *					_gim_Ubuffer;
	/*! Pointer to a character buffer. */
	typedef	char *							_gim_buffer;
	/*! Pointer to a character map. */
	typedef char *							_gim_map;

	typedef	class	gim_html_obj			_gim_html;			/*!< HTML handler */

	typedef class	MTRand					_gim_rand;			/*!< random number generator */
	typedef class	gim_file_obj			_gim_file;			/*!< file handler */
	typedef	class	gim_time_obj			_gim_time;			/*!< time handler */
	typedef class	gim_prsr_obj			_gim_kvalue;		/*!< key/value parser */
	typedef class	gim_prsr_obj			_gim_prsr;			/*!< key/value parser */
	typedef	struct	gim_global_item_handler	_gim_handler;		/*!< file handler of the global item list */
	typedef struct	gim_file_header_struct	_gim_gkp_header;	/*!< GKP file header */
	typedef struct	gim_file_chunk_struct	_gim_gkp_chunk;		/*!< GKP file chunk */
	typedef struct	gim_file_list_struct	_gim_gkp_flist;		/*!< GKP file list */
	typedef struct	gim_gkdp_data_struct	_gim_gkdp;			/*!< GKDP data */
	typedef struct	gim_gkfp_obj			_gim_gkfp;			/*!< GKFP object */
	typedef	struct	gim_processor_struct 	_gim_processor;		/*!< processor description */
	typedef	struct	gim_memory_struct 		_gim_memory;		/*!< memory manager */
	typedef	struct	gim_memory_page_obj 	_gim_mempage;		/*!< memory page */
	typedef struct	gim_interface_data		_gim_interface;		/*!< interface data */
	typedef 		_gim_flag				_gim_th_state;		/*!< thread state */
	typedef	class	gim_string_obj			_gim_string;		/*!< string */
	typedef class	gim_personal_log_obj	_gim_log;			/*!< log */
	typedef	struct	gim_list_item 			_gim_list_item;		/*!< list item */
	typedef	struct	gim_option_item			_gim_option;		/*!< command line option */
	typedef	class	gim_list_obj 			_gim_list;			/*!< list */
	typedef	class	prsr_lexical_class		_gim_lex;			/*!< lexical analyzer */
	typedef class	gim_ascii_file_obj		_gim_ascii_buffer;	/*!< ASCII buffer */
	typedef class	gim_checksum_obj		_gim_checksum;		/*!< checksum calculator */


	//	WORD_VAL, WORD_BITS, 	PicGIM code based	//

	/*! 16 bit value, seen as one word or as two bytes (PicGIM code). */
	typedef union _WORD_VAL {
		_gim_Uint16		Val;		/*!< the whole word */
		_gim_Uint8		V[ 2 ];		/*!< the two bytes */
		struct {
			_gim_Uint8	LB;			/*!< low byte */
			_gim_Uint8	HB;			/*!< high byte */
		} byte;

	} _gim_Uint16_VAL, _gim_Uint16_BITS;


	/*! 32 bit value, seen as a double word, two words or four bytes (PicGIM code). */
	typedef union _DWORD_VAL {
		_gim_Uint32			Val;		/*!< the whole double word */
		_gim_Uint16			W[ 2 ];		/*!< the two words */
		_gim_Uint8			V[ 4 ];		/*!< the four bytes */
		struct {
			_gim_Uint16		LW;			/*!< low word */
			_gim_Uint16		HW;			/*!< high word */
		} word;
		struct {
			_gim_Uint8		LB;			/*!< lowest byte */
			_gim_Uint8		HB;			/*!< second byte */
			_gim_Uint8		UB;			/*!< third byte */
			_gim_Uint8		MB;			/*!< highest byte */
		} byte;
		struct {
			_gim_Uint16_VAL	low;		/*!< low word */
			_gim_Uint16_VAL high;		/*!< high word */
		} wordUnion;
	} _gim_Uint32_VAL, _gim_Uint32_BITS;


#endif /* _GIM_CUSTOM_TYPE_H_ */
