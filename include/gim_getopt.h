//
//  gim_getopt.h
//
//  Gim - Generic Information Manager
//  Version 2.X-x  -  Milestone 3.0
//
//  [http://www.gkript.org]
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
// Gim is developed with Anjuta version 1.2.4  - 3.18.2
//

/*!     \file       gim_getopt.h
        \version    3.0
		\brief      Command line options parser of Gim (short options, like -a or -b value).
				    Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (asyntote)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

/*!     \class       gim_getopt_obj
        \brief       Parses the command line options and keeps the valid ones and the errors.
        \author      asyntote
        \version     3.0
        \date        2003-2017
*/

#ifndef _GIM_GETOPT_OBJ_H_
#define _GIM_GETOPT_OBJ_H_

#include    "gim_list.h"


	/*! Loops over the valid options (see gim_getopt_obj::getoption). */
	#define			GIM_GETOPT_OK( a )		for( ; (_gim_option *)a->getoption() != NULL ; a->nextoption() )
	/*! Loops over the errors (see gim_getopt_obj::geterror). */
	#define			GIM_GETOPT_ERROR( a )	for( ; (_gim_option *)a->geterror() != NULL ; a->nexterror() )


	/*! Return codes of the option scanner. */
	enum gim_option_enum {
		__GIM_OPT_OK = 0,		/*!< the option is valid */
		__GIM_OPT_NO_ARG,		/*!< the option needs an argument that is missing */
		__GIM_OPT_UNKNOWN,		/*!< the option is not known */
		__GIM_OPT_NOT_OK		/*!< generic error */
	};


	/*! One option found on the command line. */
	struct gim_option_item {
		char		option;				/*!< the option character found */
		char *		argument;			/*!< the argument found (NULL when there is none) */
		int			opterror;			/*!< the character with argument missing */
		_gim_flag   status;				/*!< the status of this option (gim_option_enum) */
	};


	/*! \class		gim_getopt_obj
		\brief		Command line parser: setopt() defines the valid options, scanopt() reads argv,
					then getoption() / geterror() list what was found.
	*/
    class gim_getopt_obj {
		public:
			/*! Sets the valid options, in the same format as getopt(3): a letter, or a letter followed
				by ':' when it needs an argument.
				\param	useropt	the valid options
			*/
			void				setopt( const char * useropt );
			/*! Scans argv with the options set by setopt().
				\param	argc	number of arguments
				\param	argv	the arguments
			*/
			void				scanopt( int argc, char * const argv[] );
			/*! Sets the valid options and scans argv.
				\param	argc		number of arguments
				\param	argv		the arguments
				\param	useropt		the valid options (see setopt())
			*/
			void				scanopt( int argc, char * const argv[] , const char * useropt );
			/*! Returns the current valid option.
				\return	the option, or NULL when there are no more
			*/
			gim_option_item *   getoption( void );
			/*! Returns the current error.
				\return	the error, or NULL when there are no more
			*/
			gim_option_item *   geterror( void );
			/*! Moves to the next valid option. */
			void				nextoption( void );
			/*! Moves to the next error. */
			void				nexterror( void );
			/*! Tells whether there were errors.
				\return	__GIM_YES or __GIM_NO
			*/
			_gim_flag			errors_present( void );
			/*! Returns the number of valid options.
				\return	the number of options
			*/
			_gim_Uint32			opt_members( void );
			/*! Returns the number of errors.
				\return	the number of errors
			*/
			_gim_Uint32			err_members( void );
			/*! Moves back to the first option and the first error. */
			void				rewind( void );
			/*! Tells whether an option was given.
				\param	opt	the option character
				\return	__GIM_YES when the option was found, __GIM_NO otherwise
			*/
			_gim_flag			search( char opt );

		private:

			/*! \brief	The valid options (see setopt()). */
			char				options[64];

			/*! \brief	The valid options found. */
			_gim_list *    		optlist;
			/*! \brief	The errors found. */
			_gim_list *			errlist;

		public:

			/*!gim_getopt_obj constructor
		    */
		    inline gim_getopt_obj() {
				gim_error->set( "gim_getopt_obj::gim_getopt_obj()" , "Getopt object allocated" );
				optlist = new gim_list_obj;
				errlist = new gim_list_obj;
				opterr = 0;
		    };


		    /*!gim_getopt_obj distructor
		    */
		    inline ~gim_getopt_obj() {
				optlist->destroy_list();
				errlist->destroy_list();
				gim_error->set( "gim_getopt_obj::gim_getopt_obj()" , "Getopt object deleted" );
		    };
    };

	/*!	\page	gimdoc_getopt	Command line options

		Reads the options of the command line (short options, like -a or -b value) and keeps the valid ones and the errors. \n
		Class: \ref gim_getopt_obj. Header: gim_getopt.h

		\tableofcontents

		\section	gimdoc_getopt_what	What it is for

		The tools of Gim read their options with this class: gkdp, gkfp, gimstat, gkmake and gimaitrain. It wraps the POSIX
		getopt() function. setopt() (or the three-argument scanopt()) defines the valid options, scanopt() reads argv, and
		every option found goes either to the list of the valid options or to the list of the errors. The caller then walks
		the two lists.

		The class reads short options only: a letter, optionally followed by a value (-o file). Long options (--name) are not
		parsed. The arguments that are not options (for example the file names) stay in argv: the tools read them from the
		position optind, which the scanner leaves after the last option.

		\section	gimdoc_getopt_before	Before you start

		- Create the ::gim_obj first: each option is allocated with the memory manager of Gim.
		- Define the valid options with setopt(), or with the three-argument scanopt(). A letter followed by ':' needs a value.
		- A leading ':' in the list asks for the detailed status: a missing value gives __GIM_OPT_NO_ARG and an unknown option
		  gives __GIM_OPT_UNKNOWN. Without the leading ':' both give __GIM_OPT_NOT_OK.
		- The lists have a current item: call rewind() before reading them.

		\section	gimdoc_getopt_example	Example

		Reading two valid options, one of them with a value, and printing the errors:
		\code{.cpp}
		gim_getopt_obj opt;
		opt.scanopt( argc , argv , ":qo:" );            // -q alone; -o needs a value; leading ':' = detailed status

		int quiet = __GIM_NO;
		const char * output = NULL;
		opt.rewind();
		for ( _gim_Uint32 i = 0 ; i < opt.opt_members() ; i++ ) {
			gim_option_item * item = opt.getoption();
			if ( item->option == 'q' )
				quiet = __GIM_YES;
			if ( item->option == 'o' )
				output = item->argument;                // a copy, valid while the object exists
			opt.nextoption();
		}

		opt.rewind();
		if ( opt.errors_present() == __GIM_YES ) {
			for ( _gim_Uint32 i = 0 ; i < opt.err_members() ; i++ ) {
				gim_option_item * err = opt.geterror();
				if ( err->status == __GIM_OPT_NO_ARG )
					printf( "-%c needs a value\n" , err->option );
				if ( err->status == __GIM_OPT_UNKNOWN )
					printf( "-%c is not valid\n" , err->option );
				opt.nexterror();
			}
		}
		\endcode

		Checking whether an option was given:
		\code{.cpp}
		if ( opt.search( 'q' ) == __GIM_YES )           // see the warnings: the position is moved
			quiet = __GIM_YES;
		\endcode

		\section	gimdoc_getopt_methods	The methods, by purpose

		| Purpose | Methods |
		|---------|---------|
		| Define the valid options | setopt(), scanopt() with three arguments |
		| Read argv | scanopt() with two arguments |
		| Read the valid options | getoption(), nextoption(), opt_members() |
		| Read the errors | geterror(), nexterror(), err_members(), errors_present() |
		| Move to the beginning | rewind() |
		| Look for one option | search() |

		The details of each one are in \ref gim_getopt_obj. The macros GIM_GETOPT_OK() and GIM_GETOPT_ERROR() loop over the
		two lists, but they do not call rewind(): call it before them. They take a pointer to the object.

		\section	gimdoc_getopt_status	Status of an option

		Each item has the fields option (the letter), argument (the value, or NULL), opterror (the character that getopt()
		reports) and status. The status is one of the values of gim_option_enum:

		| Status | Meaning | List |
		|--------|---------|------|
		| __GIM_OPT_OK | the option is valid | valid options (getoption()) |
		| __GIM_OPT_NO_ARG | the option needs a value and it is missing (needs the leading ':') | errors (geterror()) |
		| __GIM_OPT_UNKNOWN | the option is not in the list (needs the leading ':') | errors (geterror()) |
		| __GIM_OPT_NOT_OK | the option is not valid, without the leading ':' | errors (geterror()) |

		The field opterror is filled for every item, but it has a meaning only for the errors: for the valid options it is not
		defined.

		\section	gimdoc_getopt_returns	Return values

		| Method | Returns |
		|--------|---------|
		| getoption(), geterror() | the current item, or NULL when there are no more items |
		| errors_present() | __GIM_YES when there is at least one error, __GIM_NO otherwise |
		| opt_members(), err_members() | the number of items |
		| search() | __GIM_YES when the option was found, __GIM_NO otherwise |

		\section	gimdoc_getopt_ownership	Ownership of the items

		- Each item is allocated with \ref gim_memory_obj. The destructor of the object frees the items.
		- The argument of an option is a copy made with strdup(). The destructor does not free it: see the warnings.
		- After the object is destroyed, the pointers to the items and to the arguments are no longer valid. Copy the values
		  you keep before deleting the object.

		\section	gimdoc_getopt_threads	Threads

		Not thread-safe. The POSIX getopt() works on the global variables optind, optarg, optopt and opterr (on Windows,
		the Gim version works on gim_optind, gim_optarg, gim_optopt and gim_opterr). Scan the command line from one thread only,
		before the other threads start.

		\section	gimdoc_getopt_warnings	Warnings and limits

		The defects listed here were read in the source code and are not yet corrected or tested. See SYNC/TODO_GIM.md.

		- setopt() copies the list of options into a buffer of 64 characters with strcpy(): a longer list writes past the end of
		  the buffer. Keep the list under 64 characters.
		- Without setopt() and without the three-argument scanopt(), the buffer of the options is not initialized: the valid
		  options are undefined.
		- scanopt() appends to the lists: a second call on the same object does not clear the previous items. The global
		  optind is not reset by the class: a second scan of the same argv finds nothing, unless the caller sets optind to 1.
		- search() moves the current position: when the option is not found, the position is left after the last item.
		  Use rewind() and a loop with getoption() instead.
		- search() uses the single backup slot of the list: calling it while another backup is pending overwrites it.
		- The argument of an option is a copy made with strdup(), and nothing frees it: each option with a value leaks its copy.
		  The copy is also not checked for NULL.
		- The allocation of each item is not checked: when the memory is not available the item is NULL and the program stops.
		- The constructor sets the global opterr to 0: from then on getopt() does not print its own messages for the whole program.
		- The class does not parse long options (--name), and it does not check whether an option is repeated: both occurrences
		  appear in the list.
		- The macros GIM_GETOPT_OK() and GIM_GETOPT_ERROR() do not rewind the lists: a loop started from the current position
		  skips the items before it.
	*/

#endif //_GIM_GETOPT_OBJ_H_

