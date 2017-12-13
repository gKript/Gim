//
// gim_directory.h
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

/*!	\file		gim_directory.h
	\version	2.5-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]

/*!	\class		gim_directory_obj
	\brief		This class helps you to work with directory on a file system 
	\author		Danilo Zannoni  -  asyntote@gkript.org
	\version	2.5-0
	\date		2003-2008
*/

#ifndef _GIM_DIRECTORY_OBJ_H_
#define _GIM_DIRECTORY_OBJ_H_

	#include "gim_gkp_file_list.h"

	/*!	\struct gim_dir_file_list_struct
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief	This is a sigle member of the file list.
	*/
	struct gim_dir_file_list {
		char		name[256];				/*!<	The file name */
		char		path[1024];				/*!<	The file path */
		_gim_Uint32	size;					/*!<	The chunk size */
		_gim_Uint8	type;					/*!<	The type of the file	*/
		int			id;						/*!<	The file id. An incremental number  */
		struct stat	stat;					/*!<	The stat of the file	*/

		gim_dir_file_list * link;		/*!<	To find the next member in memory or \a NULL if is reached the end of the file list  */
	};


	typedef struct gim_dir_item {
		char		name[2048];				/*!<	The file name with absolute path */
		_gim_Uint32	size;					/*!<	The size */
		_gim_Uint8	type;					/*!<	The type of the file	*/
		int			id;						/*!<	The file id. An incremental number  */
	} _gim_dir_item;

				 
	enum gim_dir_type {
		GIM_DIR_RECURSIVE = 1,				/*!<	*/
		GIM_DIR_NOT_RECURSIVE,				/*!<	*/
		GIM_DIR_INCREMENT,					/*!<	*/
	};


	class gim_directory_obj {
		public:
			_gim_Uint32				make( const char * dir_name );
			_gim_Uint32				refresh( void );
			_gim_flag				recursion( _gim_flag rec );
			void					also_hide( _gim_flag _hide );
			_gim_Uint32				items( void );
			void					reset_position( void );
			_gim_dir_item *			get_item( _gim_flag inc = 0 );
			_gim_dir_item *			get_item_id( _gim_Uint32 id );
			_gim_flag				free_item( _gim_dir_item * to_free );

		private:
			_gim_Uint32				p_make( const char * dir_name , _gim_Uint16 level );
			gim_dir_file_list *		add( void );
			void					destroy( void );

			_gim_flag				recursive;
			_gim_flag				hide;

			_gim_Uint32				item;
			char					path[1024];

			gim_dir_file_list		* startlist;
			gim_dir_file_list		* currentlist;
			gim_dir_file_list		* endlist;

			_gim_mempage			* memory;

		public:
			/*! gim_directory_obj constructor
			*/
			inline gim_directory_obj() {
				gim_error->set( "gim_directory_obj::~gim_directory_obj" , "Start constructor" );
				item = 0;
				startlist	= NULL;
				currentlist	= NULL;
				endlist		= NULL;
				recursive   = GIM_DIR_NOT_RECURSIVE;
				hide		= __GIM_NO;
				memory = new _gim_mempage;
				gim_error->set( "gim_directory_obj::~gim_directory_obj" , "End constructor" );
			};

			/*! gim_directory_obj constructor
			*/
			inline gim_directory_obj( const char * dir , _gim_flag rec , _gim_flag _hide = __GIM_NO ) {
				gim_error->set( "gim_directory_obj::~gim_directory_obj" , "Start constructor" );
				item = 0;
				startlist	= NULL;
				currentlist	= NULL;
				endlist		= NULL;
				recursive   = rec;
				hide		= _hide;
				memory = new _gim_mempage;
				p_make( dir , __GIM_ROOT );
				reset_position();
				gim_error->set( "gim_directory_obj::~gim_directory_obj" , "End constructor" );
			};

			/*! gim_directory_obj destructor
			*/
			inline ~gim_directory_obj() {
				gim_error->set( "gim_directory_obj::~gim_directory_obj" , "Start destructor" );
				destroy();
				delete memory;
				gim_error->set( "gim_directory_obj::~gim_directory_obj" , "End destructor" );
			};
	};

	typedef class	gim_directory_obj		_gim_directory;

#endif //_GIM_DIRECTORY_OBJ_H_

