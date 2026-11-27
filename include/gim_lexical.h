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

/*!	\file		gim_lexical.h
	\version	2.0-0
	\brief		Lexical analyzer of the configuration files (Lex A, B and C syntaxes).
				Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ http://gkript.org/gk_gpl_v2.php ]
*/

#ifndef _CNC_PRSR_LEXICAL_SCANNER_H_
#define _CNC_PRSR_LEXICAL_SCANNER_H_

	#include "gim_base_header.h"
	#include "gim_kvalue.h"
	#include "gim_prsr.h"

	/*! Maximum length of a line in a configuration file. */
	#define PRSR_MAX_LINE		512
	/*! Line termination. */
	#define PRSR_TERMINATION	"\n"

	/*! Key character that opens a title (Lex B and C). */
	#define PRSR_LEX_TITLE_O	"<"
	/*! Key character that closes a title (Lex B and C). */
	#define PRSR_LEX_TITLE_C	">"
	/*! Key character that opens a section (Lex A). */
	#define PRSR_LEX_SECTION_O	"["
	/*! Key character that closes a section (Lex A). */
	#define PRSR_LEX_SECTION_C	"]"
	/*! Key character that starts a comment. */
	#define PRSR_LEX_COMMENT	"#"

	/*! All the separator characters used by the tokenizer. */
	#define PRSR_LEX_SEPARATORS "<>[]=:%()#\n"

	/*!
		\enum	prsr_syntax
		\brief	The tokens found by the lexical analyzer in a line of a configuration file.
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
		_gim_flag	Title;		/*!< __GIM_OPEN while inside a title */
		_gim_flag	Section;	/*!< __GIM_OPEN while inside a section */
		_gim_flag	Feof;		/*!< __GIM_ON when the end of the file is reached */
	};

	/*! Global syntax state used while scanning a file. */
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
			/*! Checks the syntax of a file and scans it into a parser.
				\param filename	the configuration file
				\param prsr		the parser to fill
				\return	__GIM_OK on success, __GIM_NOT_EXIST when the file does not exist, __LEX_UNKNOWN when the lex type is not recognized
			*/
			_gim_flag	scan_syntax		( const char * filename , gim_prsr_obj * prsr );
			/*! Scans the open file of a parser, line by line.
				\param prsr	the parser (with the file open)
				\return	__GIM_OK when finished
			*/
			_gim_flag	scan			( gim_prsr_obj * prsr );
			/*! Tells which token a line contains, according to the lex type.
				\param line	the line to examine
				\return	one of the prsr_syntax values
			*/
			_gim_flag	scan_line		( char * line );
			/*! Checks the file with the Lex A rules.
				\param filename	the configuration file
				\return	__GIM_OK when the file is coherent, an error code otherwise
			*/
			_gim_flag 	syntax_a		( const char * filename );
			/*! Checks the file with the Lex B rules.
				\param filename	the configuration file
				\return	__GIM_OK when the file is coherent, an error code otherwise
			*/
			_gim_flag 	syntax_b		( const char * filename );
			/*! Checks the file with the Lex C rules.
				\param filename	the configuration file
				\return	__GIM_OK when the file is coherent, an error code otherwise
			*/
			_gim_flag 	syntax_c		( const char * filename );
			/*! Splits a line into the global array Tok, using the separators.
				After the first '=' the rest of the line is the value: only '#' ends it.
				\param line			the line (modified)
				\param separator	the separator characters
				\return	the number of tokens found
			*/
			_gim_flag	tokenizer		( char * line , const char * separator );
			/*! Compares two strings.
				\param str	first string
				\param str1	second string
				\return	__GIM_YES when equal, __GIM_NO when different, __GIM_ERROR when a string is NULL
			*/
			_gim_flag	str_equal		( const char * str , const char * str1 );
			/*! Tells whether all the characters of s2 appear in s1, in the same order.
				\param s1	the string to look into
				\param s2	the characters to look for
				\return	true when all of them are found
			*/
			bool		is_in_string	( char *s1 , const char * s2 );
			/*! Tells whether a character is in a string.
				\param s1	the character to look for
				\param s2	the string to look into
				\return	true when found
			*/
			bool		is_in_string	( char  s1 , const char * s2 );
			/*! Reads a line from a file.
				\param fp	the file
				\return	the line (static buffer); sets Syntax.Feof at the end of the file
			*/
			char *		get_line		( FILE * fp );
			/*! Removes all the occurrences of a character from a string (in place).
				\param from			the string
				\param to_filter	the character to remove
				\return	the string
			*/
			char *		char_filter		( char * from , char to_filter );
			/*! Replaces all the occurrences of a character in a string (in place).
				\param from		the string
				\param to_subst	the character to replace
				\param with		the replacement
				\return	the string
			*/
			char *		char_subst		( char * from , char to_subst , char with );
			/*! Returns an upper case copy of the first len characters of a string.
				\param line	the string
				\param len	number of characters to convert
				\return	the copy (allocated)
			*/
			char *		str_up			( const char * line , int len );
			/*! Returns a lower case copy of the first len characters of a string.
				\param line	the string
				\param len	number of characters to convert
				\return	the copy (allocated)
			*/
			char *		str_down		( const char * line , int len );
			/*! Returns the string starting at an offset.
				\param line		the string
				\param offset	the offset
				\return	pointer inside line, or NULL when the offset is past the end
			*/
			char *		str_offset_pre	( char * line , _gim_Uint16 offset );
			/*! Converts the non printable characters of a string into their hexadecimal form "[xx]".
				\param string	the string to convert
				\param result	buffer for the result, or NULL to allocate it
				\return	the converted string
			*/
			char *		str_nostd_conv	( const char * string , char * result );
			/*! Returns the last trunc characters of a string, with "..." in front when it is too long.
				\param string	the string
				\param trunc	the length of the result
				\return	the result (static buffer)
			*/
			char *		string_trunc	( const char * string , _gim_int16 trunc );
			/*! Finds out the lex type of a file (A, B or C) and stores it in lex_type.
				\param filename	the configuration file
			*/
			void		scan_lex_type	( const char * filename);
				/*! Tells if a file has no sections and no keys (only the title, comments or blank lines).
					\param filename	the file to check
					\return	__GIM_OK if the file is empty, __GIM_NOT_OK if it has a section or a key, __GIM_NOT_EXIST if the file does not exist
				*/
				_gim_flag	scan_empty		( const char * filename );
			/*! Handles one line with the Lex A rules and updates the parser.
				\param line	the line
				\param prsr	the parser
			*/
			void		scan_a			( char * line , gim_prsr_obj * prsr );
			/*! Handles one line with the Lex B rules and updates the parser.
				\param line	the line
				\param prsr	the parser
			*/
			void		scan_b			( char * line , gim_prsr_obj * prsr );
			/*! Handles one line with the Lex C rules and updates the parser.
				\param line	the line
				\param prsr	the parser
			*/
			void		scan_c			( char * line , gim_prsr_obj * prsr );
			/*! Cuts a string at the first occurrence of a character (in place).
				\param str	the string
				\param chr	the character
			*/
			void		strend			( char * str , char chr );
			/*! Cuts a string at the first occurrence of a character (in place) and returns the position.
				\param str	the string
				\param chr	the character
				\return	pointer to the cut position, or NULL
			*/
			char *		strend_s		( char * str , char chr );
			/*! Removes the double quotes of a string (in place).
				\param str	the string
			*/
			void 		remove_quotes	( char * str );
			/*! Removes the line feed at the end of a string (in place).
				\param str	the string
			*/
			void		remove_linefeed	( char * str );

		private:
	friend	class	gim_prsr_obj;

			/*! Lex type of the file being scanned (__LEX_A, __LEX_B or __LEX_C). */
			_gim_flag	lex_type;
	};

	/*! Global lexical analyzer. */
	extern prsr_lexical_class	Lexical;
	/*! Tokens of the last tokenized line: Tok[0] is the first token, and so on. */
	extern char					Tok[20][64];

#endif /* _CNC_PRSR_LEXICAL_SCANNER_H_ */
