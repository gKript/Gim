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
// Gim is developed with Anjuta version 1.2.4
//

/*!	\file		gim_gkp_cryptography.h
	\version	1.0-0
	\brief		Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/ 		

#ifndef _GIM_GKP_CRYPTOGRAPHY_H
#define _GIM_GKP_CRYPTOGRAPHY_H

	/*! \class 		gim_cryptography_obj
		\brief		A Gim class to encrypt and decrypt whichever things
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	1.0-0
		\date		2003-2007
	*/
		
	/*! Progress hook of the parallel key stream: called by the thread that started it, with the bytes done out of the total (all the passes). */
	typedef void (*gim_crypt_progress_fn)( void * ctx , size_t done , size_t total );

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

			/*!	advanced_password with a nonce (the chunk id of the file): every file gets its own key stream */
			_gim_buffer	advanced_password_nonce( _gim_buffer buffer , _gim_Ulong size , const char * nonce );

			/*!	Derives the 64-byte key from the password. It reads the password and changes nothing else: safe in threads. */
			void		derive_key( unsigned char key[ 64 ] ) const;

			/*!	Number of iterations of the key stream (from [crypt] iterations in the configuration). */
			unsigned int	iterations( void ) const;

			/*!	Applies the key stream to a buffer, in place. It uses only its arguments: no shared state, no log, no progress,
				so it can run in several threads on different buffers. Same result as advanced_password_nonce.
				\param	key		the key from derive_key()
				\param	iter_n	the iterations from iterations()
				\param	nonce	the chunk id of the file
			*/
			static void	apply_stream( const unsigned char key[ 64 ] , unsigned int iter_n , const char * nonce , unsigned char * buffer , size_t size );

			/*!	Same result as apply_stream, computed by \a threads threads: the buffer is cut into ranges of blocks, each range is
				done by one thread (all the passes of its blocks). The key stream of a block does not depend on the data, and the
				XOR of the passes commutes, so the ranges are independent and the output is identical. Small buffers run in one thread.
			*/
			static void	apply_stream_parallel( const unsigned char key[ 64 ] , unsigned int iter_n , const char * nonce , unsigned char * buffer , size_t size , int threads , gim_crypt_progress_fn progress = NULL , void * ctx = NULL );

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
