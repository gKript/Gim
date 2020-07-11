//
// gim_kvalue.h
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

/*!	\file		gim_kvalue.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_PRSR_KVALUE_H_
#define _GIM_PRSR_KVALUE_H_

	#include "gim_base_header.h"
	#include "gim_prsr.h"
	#include "gim_lexical.h"

	
	#ifndef __GIM_LOG_TYPE 
		enum log_file_type {
			__GIM_LOG_TEXT = 41,
			__GIM_LOG_HTML,
		};
		#define __GIM_LOG_TYPE
	#endif

	
	#define	PRSR_GIM_INT			0	
	#define PRSR_GIM_STR			1

	#define PRSR_START_FILE			"< %s"
	#define PRSR_END_FILE			">"

	#define PRSR_START_SECTION		"    [ %s"
	#define PRSR_END_SECTION		"    ]"

	#define PRSR_A_KEY_INT			"        %s = %d"
	#define PRSR_A_KEY_STR			"        %s = %s"

	#define PRSR_B_START_SECTION	"    %s"
	#define PRSR_B_KEY_INT			"        %s::%s = %d"
	#define PRSR_B_KEY_STR			"        %s::%s = %s"
//	#define PRSR_B_SEPARATOR		"  >\n\n"

	#define PRSR_C_START_SECTION	"    ( %s ) { "
	#define PRSR_C_END_SECTION		"    } " //\n    # ------------------------------------"
	#define PRSR_C_KEY_INT			"        %-32s = %d"
	#define PRSR_C_KEY_STR			"        %-32s = %s"

	
	enum	gim_prsr_comment_position {
		PRSR_NONE	= -1,
		PRSR_BEFORE	= 1,
		PRSR_INLINE,
		PRSR_AFTER
	};

	#define	__NO_FLUSH			0
	#define	__FLUSH				1

	/*!	\def	IF_EXIST_SECTION_IN_CONF( section )
		\brief	Use this macro to verify the existence of a section in gim_conf prsr obj
	*/
	#define IF_EXIST_SECTION_IN_CONF( section )		if ( gim_conf->ExistSection( section ) == __GIM_EXIST ) 

	/*!	\def	IF_EXIST_KEY_IN_CONF( section , key )
		\brief	Use this macro to verify the existence of a key in a section in gim_conf prsr obj
	*/
	#define IF_EXIST_KEY_IN_CONF( section , key )	if ( gim_conf->ExistSection( section ) == __GIM_EXIST ) if ( gim_conf->ExistKey( section , key ) == __GIM_EXIST )

	/*!	\def	IF_EXIST_SECTION( section )
		\brief	Use this macro to verify the existence of a section in gim_conf prsr obj
	*/
	#define IF_EXIST_SECTION( obj_prsr , section )		if ( obj_prsr->ExistSection( section ) == __GIM_EXIST ) 

	/*!	\def	IF_EXIST_KEY_IN_CONF( section , key )
		\brief	Use this macro to verify the existence of a key in a section in gim_conf prsr obj
	*/
	#define IF_EXIST_KEY( obj_prsr , section , key )	if ( obj_prsr->ExistSection( section ) == __GIM_EXIST ) if ( obj_prsr->ExistKey( section , key ) == __GIM_EXIST )

	/*!	\def	IF_NOT_EXIST_SECTION( section )
		\brief	Use this macro to verify the not existence of a section in gim_conf prsr obj
	*/
	#define IF_NOT_EXIST_SECTION( obj_prsr , section )		if ( obj_prsr->ExistSection( section ) == __GIM_NOT_EXIST ) 

	/*!	\def	IF_NOT_EXIST_KEY( section , key )
		\brief	Use this macro to verify the not existence of a key in a section in gim_conf prsr obj
	*/
	#define IF_NOT_EXIST_KEY( obj_prsr , section , key )	if ( obj_prsr->ExistSection( section ) == __GIM_EXIST ) if ( obj_prsr->ExistKey( section , key ) == __GIM_NOT_EXIST )

	/*! \class 		gim_prsr_obj
		\brief		With this class you ara able to manage the configuration file key/value.
					There are two example in the \a example section abaout a config file in __LEX_A and in __LEX_B.
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	1.0-0
		\date		2003-2007
	*/
	class gim_prsr_obj {
		public:

			/*!	\fn		void 	Up( const char * filename , const char * title )
				\brief	This function is used in order to generate a configuration file from code
				\param	filename	The name of the file you want to generate\n
									\a filename != NULL
				\param	title		The title of the ini\n
									\a title > 0
			*/
			void 	Up( const char * filename , const char * title );
		
			/*!	\fn		_gim_flag	AddComment( const char * comment )
				\brief	This function is used to put a comment on the file
				\param	comment		The comment to add
									\a comment != NULL
				\return	\a __OK if the object is release \n
						\a __NOT_OK if not
			*/
			_gim_flag	AddComment( const char * comment );

			/*!	\fn		_gim_flag	Down( void )
				\brief	This function is used in order to destroy the object and release all the allocated memmory
				\return	\a __OK if the object is release \n
						\a __NOT_OK if not
			*/
			_gim_flag	Down( void );
		
			/*!	\fn		_gim_flag	Read( const char * filename )S
				\brief	This function is used in order to read an existing configuration file
				\param	filename	The name of the file you want to read\n
									\a filename != NULL
				\return \a __OK if everything it's ok\n
						\a __NOT_OK if the file exist but some error occurred\n
						\a __NOT_EXIST if file not found
			*/
			_gim_flag	Read( const char * filename );
			
			/*!	\fn		_gim_flag	Write( void )
				\brief	This function is used in order to write a configuration file to disk
				\return \a __OK if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	Write( void );
			
			/*!	\fn		_gim_buffer	WriteOnBuffer( void )
				\brief	This function is used in order to get a buffer filled with a configuration file
				\return \a __OK if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_buffer	WriteOnBuffer( void );
			
			/*!	\fn		_gim_flag	List( void )
				\brief	This function is used in order to print on \a stdout a configuration file
				\return \a __OK if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	List( void );
			
			/*!	\fn		void	SetLex( _gim_flag Lex )
				\brief	This function is used in order to write a configuration file
				\param	Lex	You can choose two kind of lexical for your ini file.\n
							Default is \a __LEX_UNKNOW \n 
							( ( \a Lex == \a __LEX_A ) || ( \a Lex == \a __LEX_B ) )
			*/
			void	SetLex( _gim_flag Lex );
			
			/*!	\fn		_gim_flag	GetLex( void )
				\brief	Use this function if you want to get the lexical used in a configuration file
				\return	\a __LEX_UNKNOW	if the lexical is not yet defined \n
						\a __LEX_A 		if the lexical if ( strlen( filenameis defined as \a __LEX_A \n 
						\a __LEX_B		if the lexical is defined as \a __LEX_B
			*/
			_gim_flag	GetLex( void );
			
			/*!	\fn		_gim_flag	AddSection( const char * section_name )
				\brief	To add a new section to the ini file
				\param	section_name	The name of the new section you want to add\n
										\a section_name != NULL
				\return \a __OK if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	AddSection( const char * section_name );
		
			_gim_flag	AddSectionComment( const char * section_name , _gim_flag position , char * comment );
			
			/*!	\fn		_gim_flag	DelSection( const char * section_name )
				\brief	To delete an existing section from a ini file
				\param	section_name	The name of the section you want to delete\n
										\a section_name != NULL
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	DelSection( const char * section_name );
			
			/*!	\fn		_gim_flag	CopySection( gim_prsr_obj * from , const char * section_name )
				\brief	To copy an existing section from a configuration file to another one
				\paran	from			A pointer to the class of the source configuration file
				\param	section_name	The name of the section you want to delete\n
										\a section_name != NULL
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	CopySection( gim_prsr_obj * from , const char * section_name );
			
			/*!	\fn		_gim_int16	GetHowManySection( void )
				\brief	To know how many section are present in a ini file
				\return \a The number of section present\n
						\a __ERROR if some error occurred
			*/
			_gim_int16	GetHowManySection( void );
			
			/*! \fn		char *  GetSectionName( _gim_int16 secindex )
				\brief	To get the name of a specific section number
				\param	secindex	The section number you want to knwo the name\n
									\a secindex >= 0
				\return \a The name of section\n
						\a __ERROR if an error occurred
			*/
			char *  GetSectionName( _gim_int16 secindex );
			
			/*!	\fn		_gim_int16 GetHowManyKey( const char * section_name )
				\brief	To know how many section are present in a ini file
				\return \a The number of Keys present inside a section \n
						\a __ERROR if some error occurred
			 	\todo	Near in the future ( inside 2.5 milestone )
			*/
			_gim_int8	GetHowManyKey( const char * section_name );
			
			/*! \fn		char * GetKeyName( const char * section_name , _gim_int16 kindex )
				\brief	To get the name of a specific section number
				\param	section_name	The name of section wich you want to get haow many Key are inside \n
									\a section_name != NULL
				\param	kindex			The index of the key you want to know the name \n
									\a kindex > 0
				\return \a The name of the Key\n
						\a __ERROR if an error occurred
			 	\todo	Near in the future ( inside 2.5 milestone )
			*/
			char *  GetKeyName( const char * section_name , _gim_int16 kindex );

			/*!	\fn		_gim_flag	AddKey( const char * section_name , const char * key , int value )
				\brief	To add a new key in a existing section to a ini file ( \a value must be an \a int )
				\param	section_name	The name of the section you want to add a new key\n
										\a section_name != NULL
				\param	key				The name of the new key you want to add\n
										\a key != NULL
				\param	value			The value of the new key 
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	AddKey( const char * section_name , const char * key , int value );

			/*! \fn		_gim_flag	AddKey( const char * section_name , const char * key , const char * value )
				\brief	To add a new key in a existing section to a ini file ( \a value must be a \a char* )
				\param	section_name	The name of the section you want to add a new key\n
										\a section_name != NULL
				\param	key				The name of the new key you want to add\n
										\a key != NULL
				\param	value			The value of the new key \n
										\a value != NULL
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	AddKey( const char * section_name , const char * key , const char * value );

			/*! \fn		_gim_flag	AddKey( const char * section_name , const char * key , float value )
				\brief	To add a new key in a existing section to a ini file ( \a value must be a \a float )
				\param	section_name	The name of the section you want to add a new key\n
										\a section_name != NULL
				\param	key				The name of the new key you want to add\n
										\a key != NULL
				\param	value			The value of the new key 
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	AddKey( const char * section_name , const char * key , float value );
			
			/*!	\fn		_gim_flag	AddKeyFlag( const char * section_name , const char * key , _gim_flag flag )
				\brief	To add a new key in a existing section to a ini file ( \a value must be a \a flag)
				\param	section_name	The name of the section you want to add a new key\n
										\a section_name != NULL
				\param	key				The name of the new key you want to add\n
										\a key != NULL
				\param	flag			The value of the new key ( __GIM_YES or __GIM_NO )
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	AddKeyFlag( const char * section_name , const char * key , _gim_flag flag );
		
		
			_gim_flag	AddKeyComment( const char * section_name , const char * key , _gim_flag	position , const char * comment );

			/*! \fn		_gim_flag	ChangeKey( const char * section_name , const char * key , const char * string )
				\brief	To change the value to an existing key
				\param	section_name	The name of the section you want to add a new key\n
										\a section_name != NULL
				\param	key				The name of the new key you want to add\n
										\a key != NULL
				\param	string			The value of the new key \n
										\a string != NULL
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	ChangeKey( const char * section_name , const char * key , const char * string );

			/*!	\fn		_gim_flag	ChangeKey( const char * section_name , const char * key , int value )
				\brief	To change the value to an existing key
				\param	section_name	The name of the section you want to add a new key\n
										\a section_name != NULL
				\param	key				The name of the new key you want to add\n
										\a key != NULL
				\param	value			The value of the new key
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	ChangeKey( const char * section_name , const char * key , int value );

			_gim_flag	ChangeKeyFlag( const char * section_name , const char * key , _gim_flag flag );

			/*!	\fn		_gim_flag	DelKey( const char * section_name , const char * key )
				\brief	To delete an existing key
				\param	section_name	The name of the section from which you want to delete a key\n
										\a section_name != NULL
				\param	key				The name of the new key you want to delete\n
										\a key != NULL
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	DelKey( const char * section_name , const char * key );
			
			/*!	\fn		_gim_flag	RenameTitle( const char * new_title )S
				\brief	To rename the title of a ini file
				\param	new_title	The new title\n
									\a new_title != NULL
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	RenameTitle( const char * new_title );

			/*!	\fn		_gim_flag	RenameSection( const char * old_section , const char * new_section )
				\brief	To rename an existing section of a ini file
				\param	old_section	The section you want to rename\n
									\a old_section != NULL
				\param	new_section	The new name for the section\n
									\a new_section != NULL
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	RenameSection( const char * old_section , const char * new_section );

			/*!	\fn		_gim_flag	RenameKey( const char * section , const char * old_key , const char * new_key )
				\brief	To rename an existing section of a ini file
				\param	section		The section that contains the key \n
									\a section != NULL
				\param	old_key		The key you want to rename\n
									\a old_key != NULL
				\param	new_key		The new name for the key\n
									\a new_key != NULL
				\return \a __OK		if everything it's ok\n
						\a __NOT_OK if some error occurred
			*/
			_gim_flag	RenameKey( const char * section , const char * old_key , const char * new_key );

			/*!	\fn		char *	GetKeySTR( const char * section_name , const char * key )
				\brief	In order to obtain the value of one key ( \a char* )
				\param	section_name	The section that contains the key \n
										\a section_name != NULL
				\param	key				The name of the key\n
										\a key != NULL
				\return \a The value \n
						\a NULL if some error occurred
			*/
			char *	GetKeySTR( const char * section_name , const char * key );

			/*!	\fn		int	GetKeyINT( const char * section_name , const char * key )
				\brief	In order to obtain the value of one key ( \a int )
				\param	section_name	The section that contains the key \n
										\a section_name != NULL
				\param	key				The name of the key\n
										\a key != NULL
				\return \a The value \n
						\a if will be __ERROR it's better check the error with the \p get() from cnc_error_class 
			*/
			int	GetKeyINT( const char * section_name , const char * key );

			/*!	\fn		float	GetKeyFLT( const char * section_name , const char * key )
				\brief	In order to obtain the value of one key ( \a float )
				\param	section_name	The section that contains the key \n
										\a section_name != NULL
				\param	key				The name of the key\n
										\a key != NULL
				\return \a The value \n
						\a if will be __ERROR it's better check the error with the \p get() from cnc_error_class 
			*/
			float	GetKeyFLT( const char * section_name , const char * key );
			
			/*!	\fn		_gim_flag	GetKeyFLAG( const char * section_name , const char * key )
				\brief	In order to obtain the value of one key ( \a flag )
				\param	section_name	The section that contains the key \n
										\a section_name != NULL
				\param	key				The name of the key\n
										\a key != NULL
				\return \a The flag \n
						\a if will be __ERROR it's better check the error with the \p get() from cnc_error_class 
			*/
			_gim_flag	GetKeyFLAG( const char * section_name , const char * key );

			/*!	\fn		_gim_flag	ExistSection( const char * section_name )
				\brief	In order to check if a section exitst or not
				\param	section_name	The section to check\n
										\a section_name != NULL
				\return \a __EXIST		if the section exist \n
						\a __NOT_EXIST	if don't
			*/
			_gim_flag	ExistSection( const char * section_name );

			/*!	\fn		_gim_flag	ExistKey( const char * section_name , const char * key )
				\brief	In order to check if a key exitst or not inside a specific section
				\param	section_name	The section contain the key\n
										\a section_name != NULL
				\param	key				The name of the key to check\n
										\a key != NULL
				\return \a __EXIST		if the section exist \n
						\a __NOT_EXIST	if don't
			*/
			_gim_flag	ExistKey( const char * section_name , const char * key );

			/*!	\fn		_gim_flag	Exist( const char * filename )S
				\brief	This function is used in order to read an existing configuration file
				\param	filename	The name of the file you want to read\n
									\a filename != NULL
				\return \a __NOT_OK if the file exist but some error occurred\n
						\a __NOT_EXIST if file not found
			*/
			_gim_flag	Exist( const char * filename );
			

			_gim_flag   ParserIsUp( void );

			_gim_flag   ThereIsASection( void );
	
			/*!
				\brief	The constructor
			*/
			inline gim_prsr_obj() {
				prsr_obj = NULL;
				Lex = __LEX_UNKNOWN;
			}

		private:
			friend		class prsr_lexical_class;
			friend		class gim_home_obj;
			friend		class gim_log_obj;
				
			prsr		* prsr_obj;
			
			_gim_flag	Lex;

			void					DelSection		( prsr_kvalue_section * section );			
			_gim_flag				Open			( const char * mode );
			_gim_flag				Close			(  _gim_flag flush );
			prsr_kvalue_section	*	SearchSection	( const char * section_name );
	};
	
	
	extern gim_prsr_obj	* gim_conf;
	extern gim_prsr_obj	* gim_compatibility;
	extern gim_prsr_obj	* gim_alias;
	extern gim_prsr_obj	* gim_environ;
	
#endif /* _GIM_PRSR_KVALUE_H_ */
