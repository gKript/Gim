//
// gim_lexical.h
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

/*!	\file		gim_lexical.h
	\version	2.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://gkript.org/gk_gpl_v2.php ]
*/ 		

#ifndef _CNC_PRSR_LEXICAL_SCANNER_H_
#define _CNC_PRSR_LEXICAL_SCANNER_H_
	
	#include "gim_base_header.h"
	#include "gim_kvalue.h"
	#include "gim_prsr.h"
	
	#define PRSR_MAX_LINE		512		/*!<	The max lenght of a line in a configuration file	*/
	#define PRSR_TERMINATION	"\n"	/*!<	The line termination	*/
	
	#define PRSR_LEX_TITLE_O	"<"		/*!<	This is a key character for a title open TOK	*/
	#define PRSR_LEX_TITLE_C	">"		/*!<	This is a key character for a title close TOK	*/
	#define PRSR_LEX_SECTION_O	"["		/*!<	This is a key character for a section open TOK	*/
	#define PRSR_LEX_SECTION_C	"]"		/*!<	This is a key character for a section close TOK	*/
	#define PRSR_LEX_COMMENT	"#"		/*!<	This is a key character for a comment TOK		*/
	
	#define PRSR_LEX_SEPARATORS "<>[]=:%()#\n"	/*!<	This is the string of all separator characters	*/
	
	/*!
		\enum	file_type
		\brief	The list of file type you can find on a filesystem. \n
	*/	
	enum prsr_syntax {
		__SYNTAX_ERROR = -2,	/*!<	Some error processing the configuration file */
		__TITLE_O = 1,			/*!<	found a title open TOK */
		__TITLE_C,				/*!<	found a title close TOK */
		__SECTION_O,			/*!<	found a section open TOK */
		__SECTION_C,			/*!<	found a section close TOK */
		__KEY,					/*!<	found a key TOK */
		__COMMENT,				/*!<	found a comment TOK */
		__BLANK,				/*!<	found a blank row */
		__END,					/*!<	found a end TOK */
		__UNKNOWN,				/*!<	found something of disowned  */
	};

	/*!	\struct		SyntaxCoerence_obj
		\warning	\n <b>It is not possible to use directly this class.</b>
		\internal
		\brief		It is used by Gim to check the coerence of the syntax of a configuration file
	*/
	struct	SyntaxCoerence_obj {
		_gim_flag	Title;
		_gim_flag	Section;
		_gim_flag	Feof;
	};
	
	extern	SyntaxCoerence_obj	Syntax;
	
	/*! \class 		prsr_lexical_class
		\warning	\n <b>It is not possible to use directly this class.</b>
		\internal
		\brief		Class used by Gim to analyse configuration file
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008
	*/
	class	prsr_lexical_class {
		public:
			_gim_flag	scan_syntax		( const char * filename , gim_prsr_obj * prsr );
			_gim_flag	scan			( gim_prsr_obj * prsr );
			_gim_flag	scan_line		( char * line );
			_gim_flag 	syntax_a		( const char * filename );
			_gim_flag 	syntax_b		( const char * filename );
			_gim_flag 	syntax_c		( const char * filename );
			_gim_flag	tokenizer		( char * line , const char * separator );
			_gim_flag	str_equal		( const char * str , const char * str1 );
			bool		is_in_string	( char *s1 , const char * s2 );
			bool		is_in_string	( char  s1 , const char * s2 );
			char *		get_line		( FILE * fp );
			char *		char_filter		( char * from , char to_filter );
			char *		char_subst		( char * from , char to_subst , char with );
			char *		str_up			( const char * line , int len );
			char *		str_down		( const char * line , int len );
			char *		str_offset_pre	( char * line , _gim_Uint16 offset );
			char *		str_nostd_conv	( const char * string , char * result );
			char *		string_trunc	( const char * string , _gim_int16 trunc );
			void		scan_lex_type	( const char * filename);
			void		scan_a			( char * line , gim_prsr_obj * prsr );
			void		scan_b			( char * line , gim_prsr_obj * prsr );
			void		scan_c			( char * line , gim_prsr_obj * prsr );
			void		strend			( char * str , char chr );
			char *		strend_s		( char * str , char chr );
			void 		remove_quotes	( char * str );
			void		remove_linefeed	( char * str );
			
		private:
	friend	class	gim_prsr_obj;
		
			_gim_flag	lex_type;
	};
	
	extern prsr_lexical_class	Lexical;
	extern char					Tok[20][64];

#endif /* _CNC_PRSR_LEXICAL_SCANNER_H_ */
