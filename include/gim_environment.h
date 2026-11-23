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

	#define	GIM_ENVIRON_NAME				".gim"				/*!< name of the Gim folder in the home */
	#define	GIM_CONFIGURATION_FILE_NAME		"gim.conf"			/*!< name of the configuration file */
	#define	GIM_COMPATIBILITY_FILE_NAME		"compatibility.conf"	/*!< name of the compatibility file */
	#define	GIM_ALIAS_FILE_NAME				"alias.conf"		/*!< name of the alias file */
	#define	GIM_ENVIRON_FILE_NAME			"environment.conf"	/*!< name of the environment file */
	#define	GIM_DEBUG_FILE_NAME				"gim_main.log"		/*!< name of the debug log */
	#define	GIM_DEBUG_MEMORY_FILE_NAME		"gim_memory.log"	/*!< name of the memory log */

	#define GIM_PRIVATE_KEY_DIR_NAME		"PrivateKey"		/*!< folder of your private key */
	#define	GIM_KEY_FILE_EXTENSION			".gkk"				/*!< extension of the key files */
	#define GIM_OTHER_KEYS_DIR_NAME			"OtherKeys"			/*!< folder of the keys of other people */

	#define	GIM_PROGRAMS_DIR_NAME			"Programs"			/*!< folder of the programs that use Gim */

	#define	GIM_CONFIGURATION_TITLE			"gim_configuration"	/*!< title of the configuration file */
	#define	GIM_ALIAS_TITLE					"alias_configuration_file"	/*!< title of the alias file */
	#define	GIM_ENVIRONMENT_TITLE			"environment_configuration_file"	/*!< title of the environment file */

	/*! \class	gim_home_obj
		\brief	Manages the Gim environment: folders and configuration files in the user home.
	*/
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

			/*! \brief	Adds the default comments to the configuration file. */
			void	add_default_comment( void );

		private:
	friend	class	gim_obj;
	friend	class	gim_gkl_obj;

			/*! \brief	Creates or reads the environment: folders, environment, configuration, alias. */
			void	up( void );
			/*! \brief	Closes the environment files. */
			void	down( void );

			/*! \brief	__GIM_YES when the environment is up. */
			volatile _gim_flag	initialized;

		public:

			/*! Builds the object, with the environment not yet up. */
			inline gim_home_obj() {
				initialized = __GIM_NO;
			};
	};

	/*! \struct	gim_conf_data
		\internal
		\brief	Paths of the Gim environment (see gim_home_obj).
	*/
	struct gim_conf_data {
		char	general_home	[1024];		/*!< the Gim folder in the home */
		char	home			[1024];		/*!< the folder of the application */
		char	conf			[1024];		/*!< the configuration file */
		char	comp			[1024];		/*!< the compatibility file */
		char	alias			[1024];		/*!< the alias file */
		char	environ			[1024];		/*!< the environment file */
		char	PrivateKeyPath	[1024];		/*!< the folder of the private key */
		char	OtherKeysPath	[1024];		/*!< the folder of the keys of other people */
		char	Programs		[1024];		/*!< the folder of the programs */
	};


	/*! The global environment object of Gim. */
	extern gim_home_obj		* gim_home;
	/*! The global paths of the environment. */
	extern gim_conf_data	* env_data;
	/*! Name of the application (set with gim_set_application_name). */
	extern char				gim_application_name[256];
	extern char				gim_application_version[64];

	/*! Sets the name of the application: its environment is then kept in a folder of its own.
		\param	app_name	the name of the application
	*/
	void					gim_set_application_name( const char * app_name );
	/*!	Optional version of the program, written in the header of the log. Default: not set. */
	void					gim_set_application_version( const char * app_version );

#endif /* _GIM_ENVIRONMENT_H_ */
