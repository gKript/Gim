//
// gim_gkp_cryptography.h
//
// Gim  -  Generic Information Manager static library
// Version 1.0-0
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
//
// Gim is developed with Anjuta version 1.2.4
//

/*!	\file		gim_gkp_cryptography.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_GKP_CRYPTOGRAPHY_H
#define _GIM_GKP_CRYPTOGRAPHY_H

	/*! \class 		gim_cryptography_obj
		\brief		A Gim class to encrypt and decrypt whichever things
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	1.0-0
		\date		2003-2007
	*/
		class gim_cryptography_obj : public  gim_password_obj { 
		public:
		
			//~ void		init_password( gim_password_obj * password );

			/*!	\fn		_gim_buffer password( _gim_buffer buffer , _gim_Ulong size )
				\brief	A simple but efficient method to encrypt or decrypt a buffer \n
						Performance = very fast \n
						Safety = not excessive good \n
						Password = needed \n
				\param	buffer	The buffer you want to crypt/decrypt
				\param	size	The size of the buffer
				\return a buffer crypted/decrypted
			*/
			_gim_buffer	simple_password( _gim_buffer buffer , _gim_Ulong size );

			/*!	\fn		_gim_buffer advanced_password( _gim_buffer buffer , _gim_Ulong size )
				\brief	An efficient method to encrypt or decrypt a buffer \n
						Performance = not excessive good \n
						Safety = very good \n
						Password = needed \n
				\param	buffer	The buffer you want to crypt/decrypt
				\param	size	The size of the buffer
				\return a buffer crypted/decrypted
			*/
			_gim_buffer	advanced_password( _gim_buffer buffer , _gim_Ulong size );

			/*!	\fn		_gim_buffer	dna( _gim_buffer buffer , _gim_Ulong size , char * dna_file_name )
				\brief	An very good method to encrypt or decrypt a buffer \n
						It use an external file as key, so an other file is needed.
						Performance = good \n
						Safety = optimal \n
						Password = not needed \n
				\param	buffer			The buffer you want to crypt/decrypt
				\param	size			The size of the buffer
				\param	dna_file_name	The name of the file used as key
				\return a buffer crypted/decrypted
			*/
			_gim_buffer	dna( _gim_buffer buffer , _gim_Ulong size , char * dna_file_name );

			/*!	\fn		_gim_buffer advanced_dna( _gim_buffer buffer , _gim_Ulong size , char * dna_file_name )
				\brief	An very good method to encrypt or decrypt a buffer \n
						It use an external file as key, so an other file is needed.
						Performance = not excessive good \n
						Safety = maximum \n
						Password = not needed \n	
				\param	buffer			The buffer you want to crypt/decrypt
				\param	size			The size of the buffer
				\param	dna_file_name	The name of the file used as key
				\return a buffer crypted/decrypted
			*/
			_gim_buffer advanced_dna( _gim_buffer buffer , _gim_Ulong size , char * dna_file_name );
		
		private:
			_gim_Uint8	iter;
		
		public:
			/*! gim_cryptography_obj constructor
			*/
			gim_cryptography_obj();

	};
		
	extern gim_cryptography_obj * gim_crypt; 


#endif /* _GIM_GKP_CRYPTOGRAPHY_H */
