//
//  gim_getopt.h
//
//  Gim - Generic Information Manager
//  Version 2.X-x  -  Milestone 3.0
//
//  [http://www.gkript.org]
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
// Gim is developed with Anjuta version 1.2.4  - 3.18.2
//

/*!     \file       gim_getopt.h
        \version    3.0
		\brief      Command line options parser of Gim (short options, like -a or -b value).
				    Copyright (C) 2002, 2003<BR>
				    Danilo Zannoni (asyntote)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ http://www.gkript.org/gkript-gpl.html ]
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

#endif //_GIM_GETOPT_OBJ_H_

