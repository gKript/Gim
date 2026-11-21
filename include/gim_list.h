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

/*!     \file        gim_list.h
        \version     2.5
	\brief       Generic list of pointers used by Gim (gim_list_obj).
		     Copyright (C) 2002, 2003<BR>
		     Danilo Zannoni (AsYntote)<BR>
		     Released under the terms of the GNU General Public License v2<BR>
		     [ http://www.gkript.org/gkript-gpl.html ]
*/

/*!     \class       gim_list_obj
        \brief       A list of generic pointers. Items are numbered from 1; the list keeps a
                     current position that is moved with rewind() and next_item().
        \author      asyntote
        \version     2.5
        \date        2003-2011
*/

#ifndef _GIM_LIST_OBJ_H_
#define _GIM_LIST_OBJ_H_


	/*! One item of the list. */
	struct gim_list_item {
		_gim_Uint32			index;		/*!< position of the item, starting from 1 */
		void *				item;		/*!< the pointer stored in the item */
		_gim_list_item *	nextitem;	/*!< the next item */
		_gim_list_item *	previtem;	/*!< the previous item */
	};



    class gim_list_obj {
		public:
			/*! Returns the size of the items, as set with the constructor or size(mem_size).
				\return	the size
			*/
			_gim_int32			size			( void );
			/*! Sets the size of the items.
				\param	mem_size	the size
			*/
			void				size			( _gim_int32 mem_size );
			/*! Adds an item at the end of the list.
				\param	member	the pointer to store
				\return	__GIM_OK on success, __GIM_ERROR when the memory is not available
			*/
			_gim_flag			add_item		( void * member );
			/*! Deletes the item with the given index (see the todo of the class).
				\param	index	the index of the item, starting from 1
				\return	__GIM_OK on success, __GIM_NOT_OK otherwise
			*/
			_gim_flag			del_item		( _gim_Uint32 index );
			/*! Returns the pointer of the current item.
				\return	the pointer, or NULL when there is no current item
			*/
			void *				get_item		( void );
			/*! Returns the pointer of the item with the given index. It does not change the current item.
				\param	index	the index of the item, starting from 1
				\return	the pointer, or NULL when the item is not found
			*/
			void *				get_item		( _gim_Uint32 index );
			/*! Moves the current item to the next one.
				\return	__GIM_OK, or __GIM_NOT_OK at the end of the list
			*/
			_gim_flag			next_item		( void );
			/*! Moves the current item to the first one.
				\return	__GIM_OK, or __GIM_NOT_OK when the list is empty
			*/
			_gim_flag			rewind			( void );
			/*! Moves the current item to the item with the given index (see the todo of the class).
				\param	index	the index of the item, starting from 1
				\return	__GIM_OK when found
			*/
			_gim_flag			seek			( _gim_Uint32 index );
			/*! Returns the number of items in the list.
				\return	the number of items
			*/
			_gim_Uint32			items			( void );
			/*! Returns the index of the current item.
				\return	the index of the current item
			*/
			_gim_Uint32			get_id			( void );
			/*! Remembers the current item, to be restored with restore().
			*/
			void				backup			( void );
			/*! Makes current again the item saved by backup().
			*/
			void				restore			( void );

		private:
	friend	class	gim_getopt_obj;
	friend	class	gim_db_obj;

			/*! \brief	Size of the items. */
			_gim_int32			item_size;
			/*! \brief	Number of items added so far. */
			_gim_Uint32			item_counter;
			/*! \brief	First item of the list. */
			_gim_list_item *	startlist;
			/*! \brief	Last item of the list. */
			_gim_list_item *	endlist;
			/*! \brief	Current item of the list. */
			_gim_list_item *	currentlist;
			/*! \brief	Item saved by backup(). */
			_gim_list_item *	savelist;

			/*! \brief	Frees all the items of the list. */
			void				destroy_list	( void );

		public:

			/*!gim_list_obj constructor
		    */
		    inline gim_list_obj() {
				startlist = NULL;
				endlist = NULL;
				currentlist = NULL;
				savelist = NULL;
//				item_size = 0;
				item_counter = 0;
				gim_error->set( "gim_list_obj::gim_list_obj" , "A new list allocated and inizialized" );
		    };

			/*!gim_list_obj constructor
				\param	mem_size	the size of the items
		    */
		    inline gim_list_obj( _gim_int32 mem_size ) {
				startlist = NULL;
				endlist = NULL;
				currentlist = NULL;
				item_size = mem_size;
				item_counter = 0;
				gim_error->set( "gim_list_obj::gim_list_obj" , "A new list allocated and inizialized" );
		    };

		    /*!gim_list_obj distructor
		    */
		    inline ~gim_list_obj() {
				destroy_list();
		    };
    };

#endif // _GIM_LIST_OBJ_H_

