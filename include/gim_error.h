//
// gim_error.h
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

/*!	\file		gim_error.h
	\version	2.0-0
	\brief		Error management of Gim: messages, levels and the behavior on fatal errors.
				Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

/*! \class 		gim_error_obj
	\brief		The inner management of the errors
	\author		Danilo Zannoni  -  asyntote@gkript.org
	\version	2.0-0
	\date		2003-2008
*/

/*!	\struct		gim_error_list_str
	\brief		gim_error_list_str is used by Gim to store the error more common
	\internal	It is NOT possible to directly use this struct.
*/

/*!	\struct		gim_error_message
	\brief		It is a list used by Gim to store the error message before have opened a log file
	\internal	It is NOT possible to directly use this struct.
*/

#ifndef _GIM_ERROR_H_
#define _GIM_ERROR_H_

	#include <string.h>

	#include "gim_time.h"

	/*! Name of the application (used in the error messages). */
	extern char	gim_application_name[256];

	#define	GIM_SEPARATOR			" ==="		/*!< separator in the log */
	#define	GIM_ERROR_OK			" ok "		/*!< type: success */
	#define	GIM_ERROR_MESSAGE		"    "		/*!< type: plain message */
	#define	GIM_ERROR_WARNING		" w  "		/*!< type: warning */
	#define	GIM_ERROR_CRITICAL		" C  "		/*!< type: critical error */
	#define	GIM_ERROR_FATAL			" \033[1;31mF\033[0m  "	/*!< type: fatal error (red F on a terminal) */

	#define	GIM_ERROR_UNKNOWN		" UU "		/*!< type: unknown error */

	#define	GIM_ERROR_PRG_MESSAGE	"--->"		/*!< prefix of the messages written by the program */
	#define	GIM_ERROR_PRG_TAB		"    "		/*!< indentation of the messages written by the program */
	#define	GIM_ERROR_DEBUG		" d  "		/*!< type: debug detail, written only when the minimum type is GIM_ERROR_DEBUG */


	/*!	\enum	variable_set
		\brief	The list of commands for the menagement of the gim behavior. \n
				To know like setting up these parameters to consult the documentation on the class gim_error_obj. \n
	*/
	enum variable_set {
		GIM_ON_FATAL_EXIT = 1,	/*!<	if \a __GIM_YES when a \a FATAL error happens Gim exits */
		GIM_ON_FATAL_VERBOSE,	/*!<	if \a __GIM_YES when a \a FATAL error happens print on \a STDERR the error message */
		GIM_ALWAYSE_VERBOSE,	/*!<	if \a __GIM_YES when whichever error happens print on \a STDERR the error
										message, if \a GIM_AUTOGET is \a __GIM_YES */
		GIM_AUTOGET,			/*!<	if \a __GIM_YES when an error is setted Gim automatically call the \a get function */
		GIM_VERBOSE_ON_CRITICAL	/*!<	if \a __GIM_YES the critical errors are printed on \a STDERR */
	};

	/*! One error stored before the log file is open. */
	struct gim_error_message {
		char	type[256];		/*!< type of the error */
		char	function[256];	/*!< function that generated the error */
		char	message[1024];	/*!< text of the error */
		int		value;			/*!< value returned by the error */

		gim_error_message	* next;	/*!< next error of the list */
	};

	/*! \class		gim_error_obj
		\brief		Gim error object: sets, prints and stores the errors of the library.
	*/
	#include <mutex>
	// One lock for the error log and the memory allocator: they call each other, so one lock avoids deadlocks.
	inline std::recursive_mutex & gim_global_lock( void ) {
		static std::recursive_mutex m;
		return m;
	}

	class gim_error_obj {
		public:

			/*! \fn		void set_min_type( const char * type )
			\brief	Messages with a type lower than \a type are not written. The default is GIM_ERROR_MESSAGE.<BR>
					Order: GIM_ERROR_DEBUG < GIM_ERROR_MESSAGE < GIM_ERROR_WARNING < GIM_ERROR_CRITICAL < GIM_ERROR_FATAL.
			\param	type	the lowest type to write
			*/
			void		set_min_type( const char * type );

			/*! \fn		void get( void )
				\brief	Gim has one inner management of the errors<BR>
						With this function it is possible to interrogate, anytime, Gim on the last error setted
			*/
			void 		get( void );

			/*! \fn		_gim_flag get( _gim_int8 cmd )
				\brief	With this method you can know the behavior of Gim when an eventual error happens
				\param	cmd		One command in the enum variable_set
				\return	__GIM_YES or __GIM_NO
			*/
			_gim_flag	get( _gim_int8 cmd );

			/*! \fn		void set( char * type , char * function , char * message , int value )
				\brief	With this method you can set an error in Gim style from your program.
				\param	type	It is a short text, up to 31 characters: use the GIM_ERROR_* macros. Gim has his string \n
								but you can use yours. For a \a FATAL error you must use \a GIM_ERROR_FATAL
				\param	function	It is the name of the function generate the error.
				\param	message	The message of the error.
				\param	value	the value returned by the error. For a \a FATAL error must be \a __GIM_ERROR.
			*/
			void		set( const char * type , const char * function , const char * message , int value );

			/*! \fn		void set( char * function , char * message )
				\brief	With this method you can set an error in Gim style from your program.\n
						\a Type is by default \a GIM_ERROR_MESSAGE and value is by default \a 0
				\param	function	It is the name of the function generate the error.
				\param	message	The message of the error.
			*/
			void		set( const char * function , const char * message );

			/*! \fn		void Set( char * type , char * function , char * format , ... )
				\brief	Like set(), but the message is a printf-style format followed by its arguments. Keep the text under 500 characters: see the errors page.
				\param	type		The type of the error (see GIM_ERROR_*)
				\param	function	It is the name of the function generate the error.
				\param	format		The format of the message.
				\param	...			The arguments of the format.
			*/
			void		Set( const char * type , const char * function , const char * format , ... );
			/*!	\brief	For the programs that use Gim: writes a message of the program, not of the library.
						The function column shows the program name, e.g. "[GimUI] main()", so the lines of the
						program are easy to tell apart from the lines of Gim in the log.
				\param	type		GIM_ERROR_MESSAGE, GIM_ERROR_WARNING, GIM_ERROR_CRITICAL, ...
				\param	function	the function of the program that writes the message
				\param	format		printf-like format, with the arguments that follow
			*/
			void		app( const char * type , const char * function , const char * format , ... );
			/*! \fn		void set( char * function , int index )
				\internal	With this method Gim use set the more common errors. Use the other set function
							to report an error.
				\param	function	It is the name of the function generate the error.
				\param	index		The index of the error in gim_error_list.
			*/
			void		set( const char * function , int index );

			/*! \fn		void set( _gim_int8 cmd , _gim_flag val )
				\brief	With this method you can change the behavior of Gim when an error happens. \n
						Inside the enum variable_set there are four command you can set \a __GIM_YES or \a __GIM_NO.
				\param	cmd		One command in the enum variable_set
				\param	val		Must be \a __GIM_YES or \a __GIM_NO
			*/
			void		set( _gim_int8 cmd , _gim_flag val );

			/*! \brief	Same as set(), with the process id of the caller.
				\param	cur_pid		The process id
				\param	type		The type of the error
				\param	function	It is the name of the function generate the error.
				\param	message		The message of the error.
				\param	value		The value returned by the error.
			*/
			void		set( pid_t cur_pid , const char * type , const char * function , const char * message , int value );

			/*!	gim_error_obj constructor
			*/
			inline gim_error_obj() {
				strcpy	( type 		, GIM_ERROR_OK );
				strcpy	( function	, "__init" );
				strcpy	( message	, "Initialization ok" );
				value = 0;
				on_fatal_exit		= __GIM_YES;
				on_fatal_verbose	= __GIM_YES;
				always_verbose		= __GIM_NO;
				verbose_on_critical = __GIM_YES;
				autoget				= __GIM_YES;
				log_enable			= __GIM_YES;
				min_rank			= 1;
				first_in			= NULL;
				last_in				= NULL;
			};

		private:

	friend	class	gim_log_obj;
	friend  class   gim_home_obj;

			/*! \brief	Type of the last error. */
			char	type[32];
			/*! \brief	Function of the last error. */
			char	function[64];
			/*! \brief	Message of the last error. */
			char 	message[256];
			/*! \brief	Value of the last error. */
			int		value;

			/*! \brief	First item of the list of the errors not yet written to the log. */
			gim_error_message	* first_in;
			/*! \brief	Last item of the list of the errors not yet written to the log. */
			gim_error_message	* last_in;

			_gim_flag	on_fatal_exit;		//If ON, exit when GIM meets a fatal error - ON or OFF
			_gim_flag	on_fatal_verbose;	//If ON, write a fatal error message - ON or OFF
			_gim_flag	always_verbose;		//If ON, write any message - ON or OFF
			_gim_flag	autoget;			//If ON, call Get automatically right after Set - ON or OFF
			int		min_rank;			/*!< rank of the lowest type written to the log (see set_min_type) */
			_gim_flag   log_enable;			/*!< __GIM_YES when the errors are written to the log */
			_gim_flag   verbose_on_critical;	/*!< __GIM_YES when the critical errors are printed */

			/*! \brief	Time of the last error. */
			gim_time_obj	error_time;

			/*! \brief	Adds an error to the list of the errors not yet written to the log.
				\return	__GIM_OK on success
			*/
			_gim_flag	ladd( const char * type , const char * function , const char * message , int value );
			/*! \brief	Writes the list of the stored errors to the log and frees it. */
			void		lflush( void );
			/*! \brief	Removes a character from a string.
				\param	from		the string
				\param	to_filter	the character to remove
				\return	the filtered string
			*/
			const char *		char_filter( const char * from , char to_filter );

	};

	/*!	\struct		gim_error_list_str
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		It is used by Gim to store the error more common
	*/
	struct gim_error_list_str {
		_gim_int8	index;		/*!< index of the error */
		char		type[64];	/*!< type of the error */
		_gim_int8	value;		/*!< value returned by the error */
		char		message[256];	/*!< text of the error */
	};

	/*! The global error object of Gim. */
	extern gim_error_obj		* gim_error;
	/*! The most common errors, used by gim_error_obj::set( function , index ). */
	extern gim_error_list_str	gim_error_list[];

/*!	\page	gimdoc_error	Errors and log

	Every message of Gim goes through \ref gim_error_obj and from there to the log. Global object: \c gim_error.
	Header: gim_error.h

	\tableofcontents

	\section	gimdoc_err_what	What it is for

	A message has a type, the name of the function that reports it, a text and a value. It is written to the log, it can be
	printed on the screen, and a FATAL message ends the program (by default).

	\section	gimdoc_err_types	Types

	| Macro | In the log | Rank | Value given by Set() |
	|-------|------------|------|----------------------|
	| \c GIM_ERROR_DEBUG | " d " | 0 | __GIM_OK |
	| \c GIM_ERROR_MESSAGE | "    " | 1 | __GIM_OK |
	| \c GIM_ERROR_WARNING | " w " | 2 | __GIM_NOT_OK |
	| \c GIM_ERROR_CRITICAL | " C " | 3 | __GIM_ERROR |
	| \c GIM_ERROR_FATAL | " F " (with colour codes on a terminal) | 4 | __GIM_ERROR |
	| \c GIM_ERROR_PRG_MESSAGE | "--->" | 1 | __GIM_OK |

	\c GIM_ERROR_OK (" ok ") and \c GIM_ERROR_UNKNOWN (" UU ") exist too. Set() does not accept \c GIM_ERROR_OK: it becomes
	\c GIM_ERROR_UNKNOWN. Use set() for it.

	\section	gimdoc_err_report	How to report

	Three forms:
	- set( type , function , message , value ): the message is already a text.
	- Set( type , function , format , ... ): like printf. The value follows from the type.
	- app( type , function , format , ... ): for the programs that use Gim. The function column shows the name of the
	  program, for example "[GimUI] main()", so the lines of the program are easy to tell from those of Gim.

	The function name gets "()" added by the log.

	\code{.cpp}
	gim_error->Set( GIM_ERROR_CRITICAL , "station::load" , "file %s cannot be read" , name );
	gim_error->app( GIM_ERROR_MESSAGE , "main" , "started with %d argument(s)" , argc );
	\endcode

	\section	gimdoc_err_level	Minimum level

	Messages below the minimum level are not written, and Set() does not even format them. The minimum is read from
	\c debug::min_type in \ref gimdoc_files_conf : DEBUG, MESSAGE, WARNING, CRITICAL or FATAL. The default is MESSAGE, so DEBUG
	messages are hidden. In code:
	\code{.cpp}
	gim_error->set_min_type( GIM_ERROR_WARNING );
	\endcode

	\section	gimdoc_err_fatal	Fatal errors

	A FATAL message is written to the log. When on_fatal_verbose is on, it is printed on stderr. When on_fatal_exit is on
	(the default), the program ends with the value of the message. The program ends only when the value is not 0 (see the
	warnings), so report a FATAL error with Set() or set() and \c __GIM_ERROR.

	The behaviour is changed with set( command , __GIM_YES or __GIM_NO ) and read with get( command ):

	| Command | Default | Meaning |
	|---------|---------|---------|
	| \c GIM_ON_FATAL_EXIT | YES | end the program on a FATAL message |
	| \c GIM_ON_FATAL_VERBOSE | YES | print a FATAL message on stderr |
	| \c GIM_ALWAYSE_VERBOSE | NO | print every message on stderr |
	| \c GIM_AUTOGET | YES | call get() after every message |
	| \c GIM_VERBOSE_ON_CRITICAL | YES | print CRITICAL messages on stderr (key \c error::verbose_on_critical) |

	\section	gimdoc_err_log	The log

	The messages are written to the log file. Until the file is open they are kept in memory: the next message written
	after the file opens writes them out (see the warnings). Each line has the format of \c debug::debug_format in
	\ref gimdoc_files_conf (\ref gimdoc_conf_log describes the codes).

	\section	gimdoc_err_threads	Threads

	All the methods take the global lock of Gim, so the calls are serialized. A FATAL message from any thread ends the
	whole program. The text of the last message is kept in the object and can be read with get().

	\section	gimdoc_err_warnings	Warnings and limits

	- The formatted text of Set() goes into a buffer of 1024 characters without a check: a longer text overwrites the stack.
	  Keep messages under 500 characters.
	- A message that is not CRITICAL is copied into a buffer of 512 characters: from 512 characters on, the copy overflows
	  it. Keep messages under 500 characters.
	- char_filter() removes nothing: it returns the original text, so the newlines of a message stay in the log.
	- When the log is open and messages are waiting in memory, the waiting messages are written and the message that
	  caused the write is not: it is lost.
	- app() with \c GIM_ERROR_FATAL passes the value 0. The message is logged, but the program does not end and nothing is
	  printed on stderr. Use Set() with \c GIM_ERROR_FATAL, which passes \c __GIM_ERROR.
	- set( function , index ), the form used for the common messages, decides whether a message is FATAL by the type of the
	  message before it. The exit depends on the message before.
	- Set() accepts six types only. Any other type, for example \c GIM_ERROR_OK, becomes \c GIM_ERROR_UNKNOWN.
	- The type is compared as text: a string that is not one of the macros is never FATAL.
	- Before the log file opens, the messages are kept in memory with no limit. A program that never opens the log keeps
	  them all.
*/

#endif /* _GIM_ERROR_H_ */
