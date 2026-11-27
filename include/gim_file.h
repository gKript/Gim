//
// gim_file.h
//
// Gim  -  Generic Information Manager static library
// Version 2.0-0
// AsYntote - SkyMatrix
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

/*!	\file		gim_file.h
	\version	2.0-0
	\brief		Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ http://gkript.org/gk_gpl_v2.php ]
*/ 		

#ifndef _GIMFILEOBJ_H_
#define _GIMFILEOBJ_H_

	#include "gim_base_header.h"
	#include "gim_gkp_utils.h"
	#include "gim_lexical.h"
	
	
	/*!
		\enum	item_type
		\brief	The list of file type you can find on a filesystem. \n
	*/	
	enum item_type {
		__GIM_FILE_UNKNOWN = -1 ,	/*!<	This is an unknown file */
		__GIM_REGULAR_FILE = 1,		/*!<	This is a regular file */
		__GIM_EMPTY_FILE,			/*!<	This is an empty regular file */
		__GIM_REGULAR_NODE,			/*!<	This is a directory */
		__GIM_LINK,					/*!<	This is a link */
		__GIM_SOCK,					/*!<	This is a socket */
		__GIM_CHR,					/*!<	This is a character */
		__GIM_FIFO,					/*!<	This is a fifo */
		__GIM_BLK,					/*!<	This is a block */
		__GIM_DEVICE				/*!<	This is a device [ __GIM_BLK || __GIM_CHR ] */
	};

	/*!
		\enum	open_method
		\brief	The list of methods to menage a file. \n
				To know the use of these methods please consult the documentation of the class gim_file_manager_obj. \n
	*/	
	enum open_method {
		__GIM_FILE_POINTER = 1,	/*!<	Use this method if you want to use buffered file */
		__GIM_FILE_DESCRIPTOR,	/*!<	Use this method if you want to use unbuffered file */
		__GIM_FILE_MAPPING		/*!<	Use this method if you want to use mapped file */
	};
	
	/*!
		\enum	open_mode
		\brief	The list of the modes to open a file. 
				To know the use of these mode please consult the documentation of the class gim_file_manager_obj 
				or of the class gim_file_obj \n
	*/	
	enum open_mode {
		__GIM_WRITE = 1,	/*!<	Use this if you want to open the file in write mode*/
		__GIM_READ,			/*!<	Use this if you want to open the file in read mode*/
		__GIM_APPEND		/*!<	Use this if you want to open the file in append mode*/
	};
	
	/*!	\struct		gim_file_manager_list
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is used by Gim to hold trace of your opearation on files.
					Please read the documentation on gim_file_manager_obj.
	*/
	struct gim_file_manager_list {
		_gim_handler	* hand;
		_gim_flag		method;
		_gim_flag		mode;
		struct stat		st;

		gim_file_manager_list 	* next;
	};
	
	/*!	\struct	gim_global_item_handler
		\brief	This struct is the gim file descriptor, for example is returnd
				by gim_file_manager_obj::open or used by gim_file_manager_obj::close. 
				For example when you type : \n \code _gim_handler * hand; \endcode \n you 
				say to the compiler to use one of this struct
	*/
	struct gim_global_item_handler {
		char		name[1024];	/*!<	This is the file name */
		_gim_Uint32	size;		/*!<	This is the file size */
		FILE		* fp;		/*!<	If you open the file in buffered this contain the file pointer */
		char		* fm;		/*!<	If you open the file in mapping this contain the pointer */
		int		  	fd;			/*!<	If you open the file in unbuffered this contain the file descriptor */
		char		* loadp;	/*!<	If you open the file and you have loaded the file this contain the 
										pointer to the buffer */
	};

	/*! \class 		gim_file_manager_obj
		\brief		The Gim's file manager is useful to hold trace of file you have used, and is very easy \n
					to use files in advanced mode like file mapping.
					In this example we create a backup of our precious /etc/X11/xorg.conf  \n
					\code 
	
	#include <stdio.h>
	#include <unistd.h>				
	#include <gim/gim.h>
	int main( int argc , char **argv ) {
		gim_obj * gim = new gim_obj;
		_gim_handler * source;
		_gim_handler * destination;
		source = gim->file_manager->open( "/etc/X11/xorg.conf" , __GIM_FILE_MAPPING , __GIM_READ );
		destination = gim->file_manager->open( "/etc/X11/xorg.conf.backup" , __GIM_FILE_POINTER , __GIM_WRITE );
		gim->file_manager->flush( destination , source->fm , source->size );
		gim->file_manager->close( source );
		gim->file_manager->close( destination );
		delete gim;
		return 0;
	}
		
					\endcode	

		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008
	*/
	class gim_file_manager_obj {
		public:

			/*! \fn		_gim_handler * open( char * filename , _gim_flag method , _gim_flag mode )
				\brief	Use this method to open a file. You can open a file in Read/Write/Append and you \n
						can use three different methods as buffered, unbuffered and mapped. These methods \n
						are described in ::open_method
				\param	filename	The name of th file you want to open\n
				\param	method		The method in which you want to open the file and this must be : \n
									\a __GIM_FILE_POINTER		for buffered file \n
									\a __GIM_FILE_DESCRIPTOR	for unbuffered file \n
									\a __GIM_FILE_MAPPING		for mapped file \n
				\param	mode		With this parameter you specify to gim in which mode you want to open \n
									the file. Mode must be : \n
									\a __GIM_READ	to open the file in read mode \n
									\a __GIM_WRITE	to open the file in write mode \n
									\a __GIM_APPEND	to open the file in append mode \n
				\return	a pointer to a _gim_handler. See the documentation of gim_global_item_handler to know is it.
			*/
			_gim_handler *			open	( const char * filename , _gim_flag method , _gim_flag mode );
		
			/*!	\fn		void close( _gim_handler * hand )
				\brief	This method is used to close a file opened with this file manager.
				\param	hand	is the pointer to a _gim_handler previously obtained by an open call.
			*/
			void					close	( _gim_handler * hand );
			
			/*! \fn		void close( char * name )
				\brief	If you have not the _gim_handler pointer to the opened file, with this method \n
						you can close a file with the filename.
				\param	name	The name of the file you want to close.
			*/
			void					close	( const char * name );
			
			/*!	\fn		_gim_flag exist( char * fname )
				\brief	To know if a file exist or not on filesystem
				\param	fname	The file name
				\return \a __GIM_EXIST if exist and \a __GIM_NOT_EXIST if not
			*/
			_gim_flag				exist	( const char * fname );
			
			/*!	\fn		_gim_buffer load( _gim_handler * hand )
				\brief	Use this method to load an opened file in \a __GIM_READ to a buffer.
				\param	hand	The _gim_handler pointer of the opened file.
				\return	A poiter to the buffer or \a NULL if error.
			*/
			_gim_buffer				load	( _gim_handler * hand );
			
			/*!	\fn		_gim_flag flush( _gim_handler * hand , _gim_buffer data , int data_len )
				\brief	Use this method if you want tu flush a buffer into a file opened in \a __GIM_WRITE
				\param	hand		The _gim_handler pointer to the file in which yo want to flush the buffer
				\param	data		A pointer to the buffer to flush
				\param	data_len	The size of the buffer in bytes
				\return	\a __GIM_OK if ok and \a __GIM_NOT_OK if some error happens
			*/
			_gim_flag				flush	( _gim_handler * hand , _gim_buffer data , int data_len );
			
			/*!	\fn		int length( char * fname )
				\brief	Useful to know the size of a file not yet opened
				\param	fname	The name of the file
				\return	The file size if ok or \a __GIM_ERROR if not
			*/
			int						length	( const char * fname );
			
			/*! \fn		_gim_flag DELETE( char * fname )
				\brief	This method is in capital in order placing greater attention \n
						on the meant of it's name. 
				\warning	This method remove a file from your filesystem.
				\param	fname	The name of the file you want to remove
				\return	\a __GIM_OK if the file is removed or __GIM_NOT_OK if not
			*/
			_gim_flag				DELETE  ( const char * fname );

		private:
	friend	class gim_file_obj;
	friend	class gim_obj;
	friend	class gim_gkl_obj;

			void					open_fp				( gim_file_manager_list * item , _gim_flag mode );
			void					open_fm				( gim_file_manager_list * item );
			void					open_fd				( gim_file_manager_list * item , _gim_flag mode );
	
			gim_file_manager_list *	item				( const char * fname );

			void 					init				( void );
			void					release				( void );
			gim_file_manager_list *	new_item			( void );
			_gim_flag				del_item			( gim_file_manager_list * to_del );
			_gim_Uint32				release_item_list	( void );
			const char *			mode_str			( _gim_flag mode );
			const char *			method_str			( _gim_flag method );
	
			gim_file_manager_list *	startlist;
			gim_file_manager_list *	currentlist;
			gim_file_manager_list *	endlist;

		public:
			/*!	gim_file_manager_obj constructor
			*/
			inline gim_file_manager_obj() {
				init();
			};

			/*!	gim_file_manager_obj destructor
			*/
			inline ~gim_file_manager_obj() {
				release();
			};
	};


	/*! \class 		gim_file_obj
		\brief		The Gim's file object class is useful in order to use the files like objects fastly and easily. 
					_gim_file is a type defined as this class. 
					In this example we create a backup of our precious /etc/X11/xorg.conf  \n
					\code 
	
	#include <gim/gim.h>
	int main( int argc , char **argv ) {
		gim_obj * gim = new gim_obj;
		_gim_file	source( "/etc/X11/xorg.conf" );
		_gim_file	destination( "/etc/X11/xorg.conf.backup" , __GIM_FILE_POINTER , __GIM_WRITE );
		destination.flush( source.fm() , source.size() );
		source.close();
		destination.close();
		delete gim;
		return 0;
	}

					\endcode
		
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008
	*/
	class gim_file_obj {
		public:
			/*!	\fn	gim_file_obj( char * fname )
					gim_file_obj constructor
				\brief	Defining the _gim_file object the name of the files can be set up to open. 
						Only giving the name it can be only opened in reading and file mapping.
				\param	fname	The file name you want to open in file mapping and in read mode
			*/
			gim_file_obj( const char * fname );

			/*!	\fn	gim_file_obj( char * fname , _gim_flag Method )
					gim_file_obj constructor
				\brief	Defining the _gim_file object the name of the files and the method can be set up. 
						It can be only opened in reading .
				\param	fname	The file name you want to open in file mapping and in read mode
				\param	Method	The method you want to use. Please to make reference to ::open_method
			*/
			gim_file_obj( const char * fname , _gim_flag Method );

			/*!	\fn	gim_file_obj( char * fname , _gim_flag Method , _gim_flag Mode )
					gim_file_obj constructor
				\brief	Defining the _gim_file object the name of the files, the method and the mode can be set up. 
				\param	fname	The file name you want to open in file mapping and in read mode
				\param	Method	The method you want to use. Please to make reference to ::open_method
				\param	Mode	The mode in which you want to open a file. Please to make reference to ::open_mode
			*/
			gim_file_obj( const char * fname , _gim_flag Method , _gim_flag Mode );
		
			/*!	\fn		void name( char * filename )
				\brief	NOT IMPLEMENTED: declared only, a call does not link. To set the file name
				\param	The name of the file
			*/
			void			name		( const char * filename );

			/*!	\fn		char * name( void )
				\brief	To get the setted file name
				\return	The file name setted
			*/
			char *			name		( void );

			/*!	\fn		_gim_Uint32 size( void )
				\brief	To get the size of an opened file
				\return	The file size
			*/
			_gim_Uint32		size		( void );
			
			/*!	\fn		_gim_Uint32 size( _gim_Uint32 offset )
				\brief	To get the size of an opened file with an offset... \n
						( it will return ( size - offset ) useful in some occasion )
				\return	The file size
			*/
			_gim_Uint32	size( _gim_Uint32 offset );

			/*! \todo	maybe present in future releases
				\return	Now, if you call it, return always \a NULL
			*/
			struct stat	*	get_stat	( void );

			/*! \fn		_gim_handler * open( _gim_flag method , _gim_flag mode );
				\brief	Use it to open the file previously setted.
				\param	method	The method you want to use. Please to make reference to ::open_method
				\param	mode	The mode in which you want to open a file. Please to make reference to ::open_mode
				\return The _gim_handler pointer of the file or NULL if any problem.
			*/
			_gim_handler *	open		( _gim_flag method , _gim_flag mode );
			
			/*!	\fn		void close( void )
				\brief	Use this to close a file previously opened.
			*/
			void 			close		( void );
			
			/*!	\fn		_gim_handler * get_handler( void )
				\brief	With this method you can get the relative _gim_handler pointer of the file opened using gim_file_obj
				\return	The _gim_handler pointer of the file.
			*/
			_gim_handler *	get_handler	( void );
			
			/*!	\fn		_gim_flag check( void )
				\brief	Use this method to check if the file exist or not.
				\return	\a __GIM_EXIST if exist or \a __GIM_NOT_EXIST if not
			*/
			_gim_flag		check		( void );
			
			/*!	\fn		_gim_flag error( void )
				\brief	Use this method to verify if all it is ok or some error occurred.
				\return	\a __GIM_OK if allit's ok or \a __GIM_ERROR if not
			*/
			_gim_flag		error		( void );

			/*! \fn		_gim_flag type( void )
				\brief	NOT RELIABLE: it uses a member that is never set. See the files page.
				\return	One of define listed in \a ::item_type enum.
			*/
			_gim_flag		type		( void );
			
			/*!	\fn		_gim_buffer load( void )
				\brief	Use this method to load an opened file in \a __GIM_READ to a buffer.
				\return	A poiter to the buffer or \a NULL if error.
			*/
			_gim_buffer		load		( void );

			/*!	\fn		void unload( void )
				\brief	Use this method to free the buffer previously load.
			*/
			void			unload		( void );
			
			/*!	\fn		_gim_flag flush( _gim_buffer data , int data_len )
				\brief	Use this method if you want tu flush a buffer into a file opened in \a __GIM_WRITE
				\param	data		A pointer to the buffer to flush
				\param	data_len	The size of the buffer in bytes
				\return	\a __GIM_OK if ok and \a __GIM_NOT_OK if some error happens
			*/
			_gim_flag		flush		( char * data , int data_len );


			/*!	\fn		_gim_flag put( const char * string )
				\brief	Use this method if you want tu put a string into a file opened in \a __GIM_WRITE. A newline is added with the buffered method only.
				\param	string		The string to append in the opened file
				\return	\a __GIM_OK if ok and \a __GIM_NOT_OK if some error happens
			*/
			_gim_flag	put( const char * string );
			
			/*! \fn		FILE * fp( void )
				\brief	With this method you can obtain the file pointer of the file only if you have opened 
						the file in \a __GIM_FILE_POINTER
				\return	The file pointer
			*/
			FILE *			fp( void );

			/*! \fn		int fd( void )
				\brief	With this method you can obtain the file descriptor of the file only if you have opened 
						the file in \a __GIM_FILE_DESCRIPTOR
				\return	The file descriptor
			*/
			int				fd( void );

			/*! \fn		char * fm( void )
				\brief	With this method you can obtain the pointer of the file only if you have opened 
						the file in \a __GIM_FILE_MAPPING
				\return	The pointer
			*/
			char *			fm( void );
			
			/*! \fn		_gim_buffer	buffer( void )
				\brief	With this method you can obtain the pointer of the buffer loaded
				\return	The pointer of the buffer
			*/
			_gim_buffer		buffer( void );
			
		private:
			friend	class gim_file_manager_obj;	
				
			char *			load_gch	( void );

			_gim_flag		errors;
			_gim_flag		exist;
			struct stat		* st;
			_gim_handler	* handler;	
			_gim_flag		method;
			_gim_flag		mode;
			char	 		* fname;
			_gim_buffer		f_buffer;
	};
	
	
	extern gim_file_obj				* gim_file;
	extern gim_file_manager_obj		* gim_file_manager;

/*!	\page	gimdoc_file	Files

	Opening, reading, writing and removing files through one manager that keeps track of every open file. \n
	Classes: \ref gim_file_manager_obj (global object \c gim_file_manager) and \ref gim_file_obj (a file used as an object,
	type \c _gim_file). Header: gim_file.h

	\tableofcontents

	\section	gimdoc_file_what	What it is for

	A program that works on files needs the same few things again and again: open a file in the right mode, read all of it
	into memory, write some text, know whether a file exists and how big it is, and close everything at the end. Gim gathers
	these operations in one place. Every file opened through the manager is recorded in a list, so the manager can close
	whatever is still open when the program ends, and every access to the list is under the global lock of Gim.

	Use it for the configuration and data files of a program, for files that must be read whole (for example a model or a
	package), and for large files that are only read, which can be mapped in memory instead of being read in pieces.

	\section	gimdoc_file_before	Before you start

	- Create the ::gim_obj first: the manager is created with it, and \ref gim_file_obj stops with a FATAL error without it.
	- Every path is a plain C string. Gim does not expand \c ~ or environment variables.
	- The methods of the manager are thread-safe because they take the global lock. The objects of \ref gim_file_obj are not:
	  use one object from one thread.

	\section	gimdoc_file_example	Examples

	Reading a whole file with the object, and checking its size first:
	\code{.cpp}
	_gim_file config( "settings.conf" );
	if ( config.check() == __GIM_EXIST ) {
		_gim_buffer text = config.load();
		if ( text != NULL )
			printf( "%u bytes read\n" , config.size() );
	}
	config.close();   // close() frees the buffer: do not call unload() as well (see the warnings)
	\endcode

	Writing a text file. put() appends the string and a newline, and the file is created or truncated when it is opened:
	\code{.cpp}
	_gim_file log( "report.txt" , __GIM_FILE_POINTER , __GIM_WRITE );
	if ( log.error() == __GIM_OK ) {
		log.put( "first line" );
		log.put( "second line" );
	}
	log.close();
	\endcode

	The manager directly, for a copy of a file with a buffer read in mapping:
	\code{.cpp}
	_gim_handler * src = gim_file_manager->open( "data.bin" , __GIM_FILE_MAPPING , __GIM_READ );
	_gim_handler * dst = gim_file_manager->open( "copy.bin" , __GIM_FILE_POINTER , __GIM_WRITE );
	if ( ( src != NULL ) && ( dst != NULL ) )
		gim_file_manager->flush( dst , src->fm , src->size );
	if ( src != NULL ) gim_file_manager->close( src );
	if ( dst != NULL ) gim_file_manager->close( dst );
	\endcode

	Removing a file, after checking that it exists:
	\code{.cpp}
	if ( gim_file_manager->exist( "old.tmp" ) == __GIM_EXIST )
		gim_file_manager->DELETE( "old.tmp" );
	\endcode

	\section	gimdoc_file_methods	The methods, by purpose

	| Purpose | Methods |
	|---------|---------|
	| Open and close | open(), close() (by handle or by name) |
	| Read | load(), unload() (on \ref gim_file_obj), load() of the manager |
	| Write | flush(), put() (on \ref gim_file_obj) |
	| Information | exist(), length(), size(), check(), error(), type() |
	| Remove | DELETE() |
	| Low level | fp(), fd(), fm(), buffer(), get_handler() |

	The details of each one are in \ref gim_file_manager_obj and \ref gim_file_obj.

	\section	gimdoc_file_methods_open	How a file is opened

	| Method | Read | Write | Append | Notes |
	|--------|------|-------|--------|-------|
	| \c __GIM_FILE_POINTER (buffered) | yes | yes | yes | write mode creates the file, or empties it if it exists |
	| \c __GIM_FILE_DESCRIPTOR (unbuffered) | yes | yes, only if the file exists | yes, only if the file exists | does not create files: see the warnings |
	| \c __GIM_FILE_MAPPING | yes | no | no | read-only, memory mapped; the file must not be empty |

	A file that does not exist is refused in read mode. The handle returned by open() is a \ref _gim_handler : its \c name,
	\c size and \c fp, \c fd or \c fm fields give access to the file.

	\section	gimdoc_file_buffers	Buffers

	load() reads the whole file into a buffer of \c size + 1 bytes, and writes a terminating zero after the last byte, so a
	text file can be used as a string. The buffer belongs to the file: unload() frees it. close() frees it too, so the
	buffer must be unloaded \b or closed with the file, not both (see the warnings).

	\section	gimdoc_file_returns	Return values

	| Method | Returns |
	|--------|---------|
	| open() | the handle, or NULL when the file cannot be opened |
	| exist() | __GIM_EXIST or __GIM_NOT_EXIST |
	| length() | the size in bytes, or __GIM_ERROR when the file cannot be opened |
	| flush(), put() | __GIM_OK, or __GIM_NOT_OK / __GIM_ERROR |
	| DELETE() | __GIM_OK when the file existed; the removal itself is not checked (see the warnings) |
	| load() | the buffer, or NULL |
	| type() | one of the values of \ref item_type (not reliable: see the warnings) |

	\section	gimdoc_file_threads	Threads

	The methods of \ref gim_file_manager_obj take the global lock of Gim, so the list of the open files is consistent. The
	objects of \ref gim_file_obj and the buffers they return are not protected: one object, one thread. Two threads that
	use the same file through different handles must coordinate with a lock of their own.

	\section	gimdoc_file_warnings	Warnings and limits

	- \b Closing \b a \b file \b by \b name \b frees its handle twice. close( name ) removes the entry from the list, which
	  already frees the handle, and then frees the same handle again: the memory can be reused by another block, and the
	  second free then releases it by mistake. Corrected in the source on 2026-11-27, not compiled yet: close the file with its handle until this is verified on a build.
	- \b unload() \b does \b not \b clear the buffer pointer. After unload(), a new load() returns the freed buffer, and a
	  close() after unload() frees it a second time. Corrected on 2026-11-27, not compiled: unload() now clears the pointer, so a later load() reads the file again. Do not use the buffer after unload().
	- \b Unbuffered \b write \b does \b not \b create \b files: the file must exist. The error is not reported: open() returns a
	  handle with a wrong descriptor. Corrected on 2026-11-27, not compiled: the unbuffered write now creates the file and empties it, as the buffered one does.
	- For the unbuffered method, open() checked the descriptor against 0 instead of -1: corrected on 2026-11-27, not compiled.
	- A file can be opened only once per name: the manager finds the entry by name, so a second open of the same file is
	  confused with the first when one of them is closed.
	- Opening a file in write mode with the buffered method empties it. There is no append-with-create on the unbuffered
	  method either.
	- A file of zero bytes cannot be mapped: open() with \c __GIM_FILE_MAPPING fails. gim_file_obj then reports "File not
	  found", which is misleading.
	- Using a \ref gim_file_obj after a failed open (name(), size(), load(), flush(), close()) dereferences a NULL handle.
	  Check error() or check() first. close() on a failed object crashes.
	- type() used a member that is never set: corrected on 2026-11-27, not compiled, it now uses the name of the handle. get_stat() always returns NULL.
	- DELETE() returns __GIM_OK even when the system does not remove the file.
	- length() opens the file in text mode: on Windows the size of a file with CRLF line endings can differ from the bytes on disk.
	- put() writes the string followed by a newline, for the buffered method only. The unbuffered method writes the string without the newline.
	- flush() of the manager dereferences the entry of the handle without a check: a handle that is not in the list crashes the program (corrected on 2026-11-27, not compiled: now it returns an error).
	- Writes are not checked: flush() and put() return __GIM_OK even if the system write fails.
	- The name setter of \ref gim_file_obj is declared but not defined: calling it does not link.
*/


#endif	//_GIMFILEOBJ_H_
