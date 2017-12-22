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
		\brief      Copyright (C) 2002, 2003<BR>
				    Danilo Zannoni (asyntote)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ http://www.gkript.org/gkript-gpl.html ]
*/

/*!     \class       gim_picgim_obj
        \brief       "A Picgim infrastructure"
        \author      asyntote
        \version     3.0
        \date        2003-2017
*/

#ifndef _GIM_GETOPT_OBJ_H_
#define _GIM_GETOPT_OBJ_H_

#include    "gim_list.h"


	enum gim_option_enum {
		__GIM_OPT_OK = 0,
		__GIM_OPT_NO_ARG,
		__GIM_OPT_UNKNOWN,
		__GIM_OPT_NOT_OK
	};
		

	struct gim_option_item {
		char		option;				//  the option character found
		char *		argument;			//  the argument found
		int			opterror;			//  the character with argument missing
		_gim_flag   status;				//  the status of this option
	};

		
    class gim_getopt_obj {
		public:
			void				setopt( const char * useropt );
			void				scanopt( int argc, char * const argv[] );
			gim_option_item *   getoption( void );
			gim_option_item *   geterror( void );
			void				nextoption( void );
			void				nexterror( void );
			_gim_Uint32			opt_members( void );
			_gim_Uint32			err_members( void );
			
		private:
			char				options[64]; 
			
			_gim_list *    		optlist;
			_gim_list *			errlist;
			
		public:

			/*!gim_getopt_obj constructor
		    */
		    inline gim_getopt_obj() {
				gim_error->set( "gim_getopt_obj::gim_getopt_obj()" , "Getopt object allocated" );
				optlist = new gim_list_obj;	
				errlist = new gim_list_obj;
				optlist->size( sizeof( gim_option_item ) );
				errlist->size( sizeof( gim_option_item ) );
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

