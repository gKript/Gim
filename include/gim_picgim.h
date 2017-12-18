//
//  gim_picgim.h
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

/*!     \file       gim_picgim.h
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

#ifndef _GIM_PICGIM_OBJ_H_
#define _GIM_PICGIM_OBJ_H_


		
    class gim_picgim_obj {
		public:
			
		private:

		public:

			/*!gim_picgim_obj constructor
		    */
		    inline gim_picgim_obj() {
				gim_error->set( "gim_picgim_obj::gim_picgim_obj()" , "A new Picgim infrastructure allocated" );
		    };


		    /*!gim_picgim_obj distructor
		    */
		    inline ~gim_picgim_obj() {

		    };
    };

#endif // _GIM_PICGIM_OBJ_H_

