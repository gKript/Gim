/*!     \file       gim_cnc.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

//
//  gim_cnc.cc
//
//  Gim - Generic Information Manager
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

 #include	"../include/gim_cnc.h"

cnc_Kernel_data_struct		cnc_data;
cnc_Kernel_feature_struct	cnc_feature;
gim_cnc_alfabeto_obj		cnc_alfabeto;
gim_cnc_generic_obj			cnc_generic;
gim_password_obj			* cnc_password;

_gim_flag	gim_cnc_obj::set ( _gim_int8 feature_to_set , _gim_flag state ) {
	gim_error->set( "gim_cnc_obj::set" , "Changeged a Feature in CNC kernel" );
	if ( ( state == __GIM_ON ) || ( state == __GIM_OFF ) ) {
		switch (feature_to_set)
		{
			case NOSKEMA :	{
				cnc_feature.NoSkema = state;
				return __GIM_OK;
				break;
			}
			case VARIABLEAB : {
				cnc_feature.VariableAB = state;
				cnc_VariableAB( __GIM_YES ) {
					cnc_alfabeto.init();
					cnc_alfabeto.make();
				}
				return __GIM_OK;
				break;
			}
			default : {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_cnc_obj::set" , "Changeged a Feature in CNC kernel" , __GIM_ERROR );
				return __GIM_NOT_OK;
			}
		}
	}
	return __GIM_NOT_OK;
}

_gim_flag	gim_cnc_obj::get ( _gim_int8 feature_to_get ) {
	switch ( feature_to_get )
	{
		case NOSKEMA :	{
			return cnc_feature.NoSkema ;
		}
		case VARIABLEAB : {
			return cnc_feature.VariableAB ;
		}
	}
	return __GIM_ERROR;
}

