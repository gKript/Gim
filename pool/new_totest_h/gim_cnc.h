//
// gim_cnc.h
//
// Gim  -  Generic Information Manager static library
// Version 2.5-x
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.0]
//
/*#############################################################################

	Copyright (C) 2002, 2003 
	Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)
	and
	Adriano Zannoni (Tatti)

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
		
/*!	\file		gim_cnc.h
	\version	2.5-x
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
	//~\warning	This file is not yet included by any Gim header. It is documented only for completeness.
				//~ You cannot use the code inside this header.
	//~\todo		The compression is planned on the Gim 2.5 Milestone on Trac. You can visit this link if you want more datils
				//~ [ http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.5 ]
*/ 

#ifndef CNC_C
	#define CNC_C
		
	#include "gim_base_header.h"
	#include "gim_mersennetwister.h"
	#include "gim_gkp_password.h"
	#include "gim_cnc_coding.h"
			
	enum cnc_kernel_features_enum {
		NOSKEMA		= 2,	/*!< \define	define to use with cnc_class::set and must be \a ON or \a OFF */
		VARIABLEAB,			/*!< \define	define to use with cnc_class::set and must be \a ON or \a OFF */
		VARIABLEBASE,		/*!< \todo */
		ROTATIONS,			/*!< \todo */
		KEYFILE,			/*!< \todo */
	};
		
	#define	cnc_Encoding(a)				if ( cnc_feature.Encoding			==  a )
	#define	cnc_VariableAB(a)			if ( cnc_feature.VariableAB			==  a )
	#define	cnc_NoSkema(a)				if ( cnc_feature.NoSkema			==	a )
	#define	cnc_Debug(a)				if ( cnc_feature.Debug				==	a )

	#define NL					'\n'
	#define SPACE				' '
	
	/*! \class 		cnc_Kernel_data_struct
		\brief		CnC internal struct where all data are stored
	*/
	struct cnc_Kernel_data_struct {
		
		_gim_Uint32			coding_base;			//Parametro fondamentale per Encoding/Decoding
		_gim_Uint32			coding_digit;			//Parametro fondamentale per Encoding/Decoding
		
		gim_password_obj	password;

		/*!<	\var	cnc_char	key_name[256];
				\brief	Key filename 
				\todo	*/
		char				key_name[256];
	};
	
	/*! \class 		cnc_Kernel_feature_struct
		\brief		CnC internal struct where all CnC Features are enabled or disabled
	*/
	struct cnc_Kernel_feature_struct {
		_gim_flag Encoding;
		_gim_flag Additive;				/*!< \todo */
		_gim_flag Multiplier;			/*!< \todo */
		_gim_flag NoSkema;
		_gim_flag VariableAB;
		_gim_flag VariableBase;			/*!< \todo */
		_gim_flag Rotations;			/*!< \todo */
		_gim_flag KeyFile;				/*!< \todo */
	};

	#define MIN_CH				'!'
	#define MAX_CH				'~'
	#define MAX_BASE			( MAX_CH - MIN_CH )
	#define MAX_DIGIT			64
	#define MIN_BASE			2
	#define MIN_DIGIT			2
	
	class gim_cnc_alfabeto_obj {
		public:
			char 		alfabeto[MAX_BASE];
			_gim_int8	rotazione[MAX_BASE];
			_gim_int8	posizione[MAX_BASE];
		
			void		init	( void );
			_gim_flag	control	( int );
			void		make	( void );
	};
	
	class  gim_cnc_generic_obj {
		public:
			char *		del_path			( char * str );
			char		num2char			( int resto );
			void 		del_ext				( char * str1 , char * str2 );
			_gim_int8	char2num			( char dato );
			int			elev				( int num );
			_gim_flag	instr				( char * str1 , char * str2 );
			_gim_flag	isPrime				( _gim_Ulong num );
	};
	
	extern	cnc_Kernel_data_struct		cnc_data;
	extern	cnc_Kernel_feature_struct	cnc_feature;
	extern	gim_cnc_alfabeto_obj		cnc_alfabeto;
	extern	gim_cnc_generic_obj			cnc_generic;
	extern	gim_password_obj			* cnc_password;

	/*! \class 		gim_cnc_obj
		\brief		Code 'N' Crypt Documentation
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\author		Corrado Tumiati - skymatrix@gkript.org
		\version	0.3-5
		\date		2003-2006
	*/
	class  gim_cnc_obj {
		public:
			gim_cnc_encoding_obj		* encoding;
			gim_cnc_decoding_obj		* decoding;

			/*!
				\brief	Get the state ( \a ON or \a OFF ) of a feature of CnC ; e.g. <tt>get( ADVANCEDPASSWORD )</tt>
				\param	feature	One of the possible macro defined before in this file<BR>
						( search // BEGIN OF FEATURES )<BR>
						( ( \a feature == \a NOSKEMA ) ||<BR>
						  ( \a feature == \a ENCODING ) ||<BR>
						  ( \a feature == \a VARIABLEAB ) ||<BR>
				\return	\a ON if the feature is setted , \a OFF if not. \a ERROR if \a feature does not exists
			*/
			_gim_flag	get				( _gim_int8 feature );
			
			/*!
				\brief	Set a CnC feature \a ON or \a Off; e.g. <tt>set( ADVANCEDPASSWORD , ON )</tt>
				\param	feature	One of the possible macro defined before in this file<BR>
							( search // BEGIN OF FEATURES )<BR>
							( ( \a feature == \a NOSKEMA ) ||<BR>
							  ( \a feature == \a ENCODING ) ||<BR>
							  ( \a feature == \a VARIABLEAB ) ||<BR>
				\param	state	the value of specified \a feature, \a ON or \a OFF<BR>
						( ( \a state == \a ON ) || ( \a state == \a OFF) )
				\return \a OK if the \a feature is setted , \a NOT_OK if not
			*/
			_gim_flag	set				( _gim_int8 feature , _gim_flag state );

			gim_cnc_obj() {
				gim_error->set( "gim_cnc_obj::gim_cnc_obj" , "Start CNC kernel initialize" );
				encoding		= new gim_cnc_encoding_obj;
				decoding		= new gim_cnc_decoding_obj;
				cnc_password	= new gim_password_obj;
				
				__GIM_CLEAR( &cnc_data , 1 , cnc_Kernel_data_struct );
				__GIM_CLEAR( &cnc_feature , 1 , cnc_Kernel_feature_struct );
			
				gim_error->set( "gim_cnc_obj::gim_cnc_obj" , "CNC kernel succesfully initialized" );
			}

			~gim_cnc_obj() {
				gim_error->set( "gim_cnc_obj::~gim_cnc_obj" , "Start CNC kernel shutdown" );
				delete encoding;
				delete decoding;
				delete cnc_password;
				gim_error->set( "gim_cnc_obj::~gim_cnc_obj" , "CNC now is down" );
			}
	};
	
#endif
