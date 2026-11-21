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

/*!	\file		gim_error.h
	\version	2.0-0
	\brief		Error management of Gim: messages, levels and the behavior on fatal errors.
				Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
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
	class gim_error_obj {
		public:

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
				\param	type	It is a string of 3 bytes and it describe the error type. Gim has his string \n
								but you can use yours. For a \a FATAL error you must use \a GIM_ERROR_FATAL
				\param	function	It is the name of the function generate the error.
				\param	message	The message of the error.
				\param	value	the value returned by the error. For a \a FATAL error must be \a __GIM_ERROR.
			*/
			void		set( const char * type , const char * function , const char * message , int value );

			/*! \fn		void set( char * function , char * message )
				\brief	With this method you can set an error in Gim style from your program.\n
						\a Type is per dafault \a GIM_ERROR_MESSAGE and value is per default \a 0
				\param	function	It is the name of the function generate the error.
				\param	message	The message of the error.
			*/
			void		set( const char * function , const char * message );

			/*! \fn		void Set( char * type , char * function , char * format , ... )
				\brief	Like set(), but the message is a printf-style format followed by its arguments.
				\param	type		The type of the error (see GIM_ERROR_*)
				\param	function	It is the name of the function generate the error.
				\param	format		The format of the message.
				\param	...			The arguments of the format.
			*/
			void		Set( const char * type , const char * function , const char * format , ... );

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

			_gim_flag	on_fatal_exit;		//Se ON esce se GIM incontra un errore fatale - ON o OFF
			_gim_flag	on_fatal_verbose;	//Se ON scrive un messaggio di errore - ON o OFF
			_gim_flag	always_verbose;		//Se ON scrive qualsiasi messaggio - ON o OFF
			_gim_flag	autoget;			//Se ON fa la Get subito dopo la Set in maniera automatica - ON o OFF
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

#endif /* _GIM_ERROR_H_ */
