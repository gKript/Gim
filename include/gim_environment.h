//
// gim_environment.h
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

/*!	\file		gim_environment.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003 \n 
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix) \n
				Released under the terms of the GNU General Public License v2 \n
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

/*!	\class	gim_home_obj
	\brief		gim_home_obj manage automatically the Gim's environment. \n
				There are some methods which allow you to have important information \n
				as the environment for your program or the path for the personal key \n
				of your friends
	\author		Danilo Zannoni  -  asyntote@gkript.org
	\version	1.0-0
	\date		2003-2007
*/

/*! \struct		gim_conf_data
	\warning	\n <b>It is not possible to use directly this struct.</b>
	\internal
	\brief		It contain all path to manage the environment
*/

/*!	\def	GIM_ENVIRON_NAME
			The name of Gim's environment maked in the user home directory
	\def	GIM_CONFIGURATION_FILE_NAME
			The name of Gim's configuration file
	\def	GIM_COMPATIBILITY_FILE_NAME
			The name of Gim's compatibility file
	\def	GIM_ALIAS_FILE_NAME
			The name of Gim's alias file
	\def	GIM_DEBUG_FILE_NAME
			The name of Gim's alias file
	\def	GIM_PRIVATE_KEY_DIR_NAME
			The path where Gim store your private key
	\def	GIM_KEY_FILE_EXTENSION
			The extension of filename of the key when you or other people grant the personal key
	\def	GIM_OTHER_KEYS_DIR_NAME
			The path where Gim store the personal key of your friends
	\def	GIM_PROGRAM_DIR_NAME
			The path where you can create an environment for your program that use gim
	\def	GIM_CONFIGURATION_TITLE
			The title for the configuration kvalue file
	\def	GIM_ALIAS_TITLE
			The title for the alias kvalue file
*/

#ifndef _GIM_ENVIRONMENT_H_
#define _GIM_ENVIRONMENT_H_

	#include <sys/types.h>
	#include <sys/stat.h>

	#define	GIM_ENVIRON_NAME				".gim"
	#define	GIM_CONFIGURATION_FILE_NAME		"gim.conf"
	#define	GIM_COMPATIBILITY_FILE_NAME		"compatibility.conf"
	#define	GIM_ALIAS_FILE_NAME				"alias.conf"
	#define	GIM_ENVIRON_FILE_NAME			"environment.conf"
	#define	GIM_DEBUG_FILE_NAME				"gim_main.log"
	#define	GIM_DEBUG_MEMORY_FILE_NAME		"gim_memory.log"

	#define GIM_PRIVATE_KEY_DIR_NAME		"PrivateKey"
	#define	GIM_KEY_FILE_EXTENSION			".gkk"
	#define GIM_OTHER_KEYS_DIR_NAME			"OtherKeys"

	#define	GIM_PROGRAMS_DIR_NAME			"Programs"

	#define	GIM_CONFIGURATION_TITLE			"gim_configuration"
	#define	GIM_ALIAS_TITLE					"alias_configuration_file"
	#define	GIM_ENVIRONMENT_TITLE			"environment_configuration_file"

	class gim_home_obj {
		public:
			/*!	\fn		char *	get_generic_programs_env( void )
   				\brief	To get a generic environment for saving your file and share with other programs use Gim
				\return The path of the environment for program
 			*/
			char *	get_generic_programs_env( void );

			/*!	\fn		char *	get_programs_env( void )
   				\brief	To get the environment for your program
				\return The path of the environment for program
 			*/
			char *	get_programs_env( void );
		
			/*!	\fn		char *	get_other_keys_path( void )
   				\brief	To get the path where Gim store the personal key of your friends
				\return The others personal keys path
 			*/
			char *	get_other_keys_path( void );
		
			void	add_default_comment( void );
			
		private:
	friend	class	gim_obj;
	friend	class	gim_gkl_obj;

			void	up( void );
			void	down( void );

			volatile _gim_flag	initialized;

		public:
		
			inline gim_home_obj() {
				initialized = __GIM_NO;
			};
	};

	struct gim_conf_data {
		char	general_home	[1024];
		char	home			[1024];
		char	conf			[1024];
		char	comp			[1024];
		char	alias			[1024];
		char	environ			[1024];
		char	PrivateKeyPath	[1024];
		char	OtherKeysPath	[1024];
		char	Programs		[1024];
	};


	extern gim_home_obj		* gim_home;
	extern gim_conf_data	* env_data;
	extern char				gim_application_name[256];
	
	void					gim_set_application_name( const char * app_name );
	
#endif /* _GIM_ENVIRONMENT_H_ */
