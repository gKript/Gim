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
//
// Gim is developed with Anjuta version 1.2.4  - 2.32.1.1
//

/*!     \file        gim_list.h
        \version     2.5
	\brief       Generic list of pointers used by Gim (gim_list_obj).
		     Copyright (C) 2002-2026<BR>
		     Danilo Zannoni (AsYntote)<BR>
		     Released under the terms of the GNU General Public License v3<BR>
		     [ https://www.gnu.org/licenses/gpl-3.0.html ]
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
			/*! Returns the size of the items, as set with the constructor or size(mem_size). The list does not use it: see the lists page.
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
			/*! Moves the current item to the item with the given index. NOTE: when not found it returns NULL, that is __GIM_NO: see the lists page.
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

			/*! \brief	Frees all the items of the list (called by the destructor).
				Ownership: a member is freed here only when it was allocated with gim_memory (checked for each member, one level only).
				Members allocated elsewhere, and the members of the members, are not freed: the caller frees them first.
			*/
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

/*!	\page	gimdoc_list	Lists

	A list of generic pointers, kept in order, with a current position. \n
	Class: \ref gim_list_obj. Header: gim_list.h

	\tableofcontents

	\section	gimdoc_list_what	What it is for

	The list keeps any number of pointers, each one in its own item, in the order they were added. The items are numbered
	from 1, and the list remembers a current item that is moved with rewind() and next_item(). It is the structure used
	by Gim for the lists of the database tables and of the records, for the options of the command line and for the
	entries of the memory and of the file managers.

	The list does not own what it points to: it stores the pointers and never copies the data. The memory of the items
	is managed by the list, the memory of the data is managed by the caller, with one exception described below.

	\section	gimdoc_list_example	Examples

	Storing a pointer per item and walking the list from the beginning:
	\code{.cpp}
	_gim_list * names = new _gim_list;
	names->add_item( (void *) "anna" );
	names->add_item( (void *) "bruno" );

	names->rewind();
	for ( _gim_Uint32 i = 1 ; i <= names->items() ; i++ ) {
		printf( "%u: %s\n" , names->get_id() , (char *) names->get_item() );
		names->next_item();
	}
	delete names;
	\endcode

	Reading an item by its number, without moving the current position:
	\code{.cpp}
	char * second = (char *) names->get_item( 2 );
	\endcode

	Removing an item. The items after it are renumbered:
	\code{.cpp}
	names->del_item( 1 );      // "bruno" is now item 1
	\endcode

	Saving the current position around a search:
	\code{.cpp}
	names->backup();
	// ... move the current item ...
	names->restore();
	\endcode

	\section	gimdoc_list_methods	The methods, by purpose

	| Purpose | Methods |
	|---------|---------|
	| Add and remove | add_item(), del_item() |
	| Read | get_item() (current item or by number), items(), get_id() |
	| Move the current item | rewind(), next_item(), seek() |
	| Save the current item | backup(), restore() |
	| Item size | size() (get and set) |

	The details of each one are in \ref gim_list_obj.

	\section	gimdoc_list_ownership	Ownership of the items

	- add_item() stores the pointer. The list allocates only the item node, with \ref gim_memory_obj.
	- del_item() frees the node, not the pointer: the caller frees the data first, or keeps it.
	- The destructor frees the node of each item, and also the data of each item that was allocated with \ref gim_memory_obj
	  (one level only). Data allocated in another way is not freed: free it before deleting the list.
	- A pointer must not be in two lists that are destroyed: the second destruction would free it again.

	\section	gimdoc_list_returns	Return values

	| Method | Returns |
	|--------|---------|
	| add_item() | __GIM_OK, or __GIM_ERROR when the memory is not available |
	| del_item() | __GIM_OK, or __GIM_NOT_OK when the index is 0 or not found |
	| rewind() | __GIM_OK, or __GIM_NOT_OK when the list is empty |
	| next_item() | __GIM_OK, or __GIM_NOT_OK when the current item is already the last |
	| get_item() | the pointer, or NULL when there is no current item or the index is not found |
	| seek() | __GIM_OK when found; when not found it returns NULL, that is __GIM_NO (see the warnings) |
	| items() | the number of items |
	| get_id() | the number of the current item, or 0 when there is none |

	\section	gimdoc_list_threads	Threads

	Not thread-safe: the methods do not take the global lock of Gim. Use a list from one thread, or protect every call
	with a lock of your own.

	\section	gimdoc_list_warnings	Warnings and limits

	- \b seek() \b returns NULL when the item is not found. NULL is the same value as __GIM_NO, so the result is not an error
	  code: check the current item with get_id() instead. An index of 0 also returns NULL. When the item is not found the
	  current position is left after the last item.
	- \b restore() \b can point to a deleted item. backup() saves the current item; if that item is removed with del_item()
	  before restore(), the current position points to freed memory.
	- \b size() \b does nothing. The size of the items is stored, but the list never uses it: the items are pointers.
	- A list built with the constructor without a size does not set the size: the value read by size() is not defined.
	- Indexes are renumbered on every del_item(): an index kept from before the removal may point to another item.
	- Every access by number walks the list from the beginning: for long lists, use the current position with next_item().
	- The destructor warns "The list is already empty" for an empty list. The message is only a warning.
	- A failed add_item() (memory not available) returns __GIM_ERROR and leaves the list unchanged.
*/


#endif // _GIM_LIST_OBJ_H_

