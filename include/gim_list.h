//
//  gim_list.h
//
//  Gim - Generic Information Manager
//  Version 2.4-1  -  Milestone 2.5
//  Creator - asyntote
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
// Gim is developed with Anjuta version 1.2.4  - 2.32.1.1
//

/*!     \file        gim_list.cc
        \version     2.5
	\brief       Copyright (C) 2002, 2003<BR>
		     Danilo Zannoni (AsYntote)<BR>
		     Released under the terms of the GNU General Public License v2<BR>
		     [ http://www.gkript.org/gkript-gpl.html ]
*/

/*!     \class       gim_list_obj
        \brief       "Put your brief here"
        \author      asyntote
        \version     2.5
        \date        2003-2011
*/

#ifndef _GIM_LIST_OBJ_H_
#define _GIM_LIST_OBJ_H_


	struct gim_list_item {
		_gim_Uint32			index;
		void *				item;
		_gim_list_item *	nextitem;
		_gim_list_item *	previtem;
	};

	

    class gim_list_obj {
		public:
			_gim_int32			size			( void );
			void				size			( _gim_int32 mem_size );
			_gim_flag			add_item		( void * member );
			_gim_flag			del_item		( _gim_Uint32 index );
			void *				get_item		( void );
			void *				get_item		( _gim_Uint32 index );
			_gim_flag			next_item		( void );
			_gim_flag			rewind			( void );
			_gim_flag			seek			( _gim_Uint32 index );
			
		private:
			_gim_int32			item_size;
			_gim_Uint32			item_counter;
			_gim_list_item *	startlist;
			_gim_list_item *	endlist;
			_gim_list_item *	currentlist;

			void				destroy_list	( void );

		public:

			/*!gim_list_obj constructor
		    */
		    inline gim_list_obj() {
				gim_error->set( "gim_list_obj::gim_list_obj()" , "A new list allocated" );
				startlist = NULL;
				endlist = NULL;
				currentlist = NULL;
				item_size = 0;
				item_counter = 0;
		    };

			/*!gim_list_obj constructor
		    */
		    inline gim_list_obj( _gim_int32 mem_size ) {
				gim_error->Set( "gim_list_obj::gim_list_obj()" , "A new list allocated [ Item size : %d ]" , mem_size );
				startlist = NULL;
				endlist = NULL;
				currentlist = NULL;
				item_size = mem_size;
				item_counter = 0;
		    };

		    /*!gim_list_obj distructor
		    */
		    inline ~gim_list_obj() {
				destroy_list();
		    };
    };

#endif // _GIM_LIST_OBJ_H_

