//
// gim_db_structures.h
//
// Gim  -  Generic Information Manager static library
// Version 2.5-0
// AsYntote
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

/*!	\file		gim_db_structures.h
	\version	2.5-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/

#ifndef _GIM_DB_STRUCTURES_H_
#define _GIM_DB_STRUCTURES_H_

 	extern char		gim_application_name[256];

	#define GIM_DB_MODE				"mode"
	#define GIM_DB_TYPE				"type"

	#define	GDBS_SEPARATOR_STR		" ;"
	#define	GDBS_STOP_CHAR			'#'
	

	enum	gim_db_type {
		GIM_DB_VOLATILE = 1,		/*!<	*/
		GIM_DB_PERMANENT,			/*!<	*/
	};

	enum	gim_db_mode {
		GIM_DB_SAVE_MEMORY = 3,		/*!<	*/
		GIM_DB_PERFORMANCE,			/*!<	*/
		GIM_DB_BALANCED,			/*!<	*/
	};

	enum	gim_db_field_data_type {
		GIM_DB_TYPE_INT = 1,		/*!<	*/
		GIM_DB_TYPE_FLOAT,			/*!<	*/
		GIM_DB_TYPE_PERCENTAGE,		/*!<	*/
		GIM_DB_TYPE_BOOL,			/*!<	*/
		GIM_DB_TYPE_FLAG,			/*!<	*/
		GIM_DB_TYPE_STRING,			/*!<	*/
		GIM_DB_TYPE_DATE,			/*!<	*/
		GIM_DB_TYPE_TIME,			/*!<	*/
		GIM_DB_TYPE_TIMESTAMP,		/*!<	*/
	};

	enum	gim_db_table_properities {
		GIM_DB_IS_INDEX = 1,		/*!<	*/
		GIM_DB_IS_NOT_INDEX,		/*!<	*/
		GIM_DB_IS_KEY,				/*!<	*/
		GIM_DB_IS_NOT_KEY,			/*!<	*/
		GIM_DB_TB_VOLATILE,			/*!<	*/
		GIM_DB_TB_PERMANENT,		/*!<	*/
	};

	enum	gim_db_status {
		GIM_DB_NOT_EXIST = 1,		/*!<	*/
		GIM_DB_CONF_NOT_EXIST,		/*!<	*/
		GIM_DB_GDB_NOT_EXIST,		/*!<	*/
		GIM_DB_OK,					/*!<	*/
		GIM_DB_READ,				/*!<	*/
		GIM_DB_NEW,					/*!<	*/
	};

 	typedef union	gim_db_value_u	{
		int			db_int;
		float		db_float;
		_gim_flag	db_bool;
		_gim_flag	db_flag;
		char		db_char[512];
	} _gim_db_value_u;


 	struct _gim_db_value {
		_gim_flag			type;
		_gim_db_value_u		value;
	};


 	struct _gim_db_field {
		char				name[64];
		_gim_flag			type;
	};


 	struct _gim_db_virtual_table {
		char			name[64];
		char			comment[512];
//		_gim_flag		type;		//	Virtual table is always GIM_DB_TB_VOLATILE
		_gim_Uint8		n_fields;
		_gim_Uint8		n_records;
		_gim_Uint32		sizeof_per_record;
	};


 	struct _gim_db_table {
		char			name[64];
		char			comment[512];
		_gim_flag		type;				//  Permanent, volatile or virtual 
		_gim_Uint8		items;
		_gim_Uint8		fields_number;
		_gim_int16		key_field_number;
		_gim_Uint32		sizeof_per_record;

		_gim_list *		fields;
		_gim_list *		records;
		 
		_gim_prsr		* tbl;

	};


 	struct _gim_db_main {
		_gim_flag		mode;
		_gim_flag		type;
		_gim_Uint8		tables_number;

		_gim_prsr		* conf;
		 
		_gim_list		* tables;
	};

#endif // _GIM_DB_STRUCTURES_H_
