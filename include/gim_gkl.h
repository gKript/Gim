//
// gim_gkl.h
//
// Gim  -  Generic Information Manager static library
// Version 2.0-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.0?by=version]
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

/*!	\file		gim_gkl.h
	\version	2.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://gkript.org/gk_gpl_v2.php ]
	\warning	This file is not yet included by any Gim header. It is documented only for completeness.
				You cannot use the code inside this header.
	\todo		The GKript Language is planned on the Gim 3.0 Milestone on Trac (Ticket#35). You can visit this link if you want more datils
				[ http://phorus.nibbles.it/trac/gkript/query?status=new&status=assigned&status=reopened&milestone=Gim+3.0 ]
*/

#ifndef _GIM_GKL_H
#define _GIM_GKL_H

	#include	"gim_base_header.h"
	#include	"gim_gkl_custom_types.h"
	#include	"gim_gkl_commands.h"
	#include	"gim_gkl_variable.h"

	/*! \class 		gim_gkl_obj
		\brief		Generic Information Manager Gkl object\n\n
					<b>GKL</b> - GKript Language\n
					With this class you can execute script written in <b>GKL</b> - GKprit Language\n
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008
		\warning	This class is not yet included by any Gim header. It is documented only for completeness.
					You cannot use the code of this class.
		\todo		The GKript Language is planned on the Gim 3.0 Milestone on Trac (Ticket#35). You can visit this link if you want more datils
					[ http://phorus.nibbles.it/trac/gkript/query?status=new&status=assigned&status=reopened&milestone=Gim+3.0 ]
	*/
	class gim_gkl_obj {
		public:

		private:
			
		public:
			/*!
				gim_gkp_obj constructor	
			*/
			inline gim_gkl_obj() {
			};
						
	
			/*!
				gim_gkp_obj destructor	
			*/
			inline virtual ~gim_gkl_obj() {
			};
	};

#endif /* _GIM_GKL_H */
