// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA

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

