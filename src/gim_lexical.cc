//
// gim_lexical.cc
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

#include "../include/gim_lexical.h"
#include "../include/gim_file.h"

prsr_lexical_class	Lexical;
char 				Tok[20][64];
SyntaxCoerence_obj	Syntax;

char * 	prsr_lexical_class::get_line( FILE * fp ) {
	static char line[PRSR_MAX_LINE];
	__GIM_CLEAR( line , PRSR_MAX_LINE , char );
	fgets( line , PRSR_MAX_LINE , fp );
	if ( feof(fp) ) 
		Syntax.Feof = __GIM_ON;
	return line;
}

bool	prsr_lexical_class::is_in_string ( char * s1 , const char * s2 ) {
	int flag = 0 ;
	char * start_str2 = (char *)s2;
	char * start_str1 = s1;
	if ( strlen (s1) < strlen (s2) )
		return __GIM_NO;
	for ( ; *s1 != '\0' ; s1++ )
		if ((*s1) == (*s2))
			start_str1 = s1, s2++, flag++;
	if ( flag == ( int ) strlen (start_str2))
		return __GIM_YES;
	else
		return __GIM_NO;
}

bool	prsr_lexical_class::is_in_string ( char s1 , const char * s2 ) {
	for ( ; *s2 != '\0' ; s2++ )
		if ((s1) == (*s2))
			return __GIM_YES;
	return __GIM_NO;
}

_gim_flag	prsr_lexical_class::scan_line	( char * line ) {
	if 	( lex_type == __LEX_A ) {
		if	( Syntax.Feof == __GIM_ON ) 
			return __END;
		else if	( strlen( line ) == 1 )	
			return __BLANK;
		char * tmp = strdup( char_filter( line , ' ' ) );
		if ( is_in_string ( tmp , "#" ) ) 
			return __COMMENT;
		else if	( is_in_string ( tmp , "<" ) ) 
			return __TITLE_O ;
		else if ( is_in_string ( tmp , ">" ) ) 
			return __TITLE_C ;
		else if ( is_in_string ( tmp , "[" ) ) 
			return __SECTION_O ;
		else if ( is_in_string ( tmp , "]" ) ) 
			return __SECTION_C ;
		else if ( is_in_string ( tmp , "=" ) ) 
			return __KEY ;
		else if ( ( ! is_in_string( tmp , "#=[]<>" ) ) && ( strlen( tmp ) > 0 ) )
			return __SYNTAX_ERROR;
		return __UNKNOWN;
	}
	if 	( lex_type == __LEX_B ) {
		if	( Syntax.Feof == __GIM_ON ) 
			return __END;
		else if	( strlen( line ) == 1 ) 
			return __BLANK;
		char * tmp = strdup( char_filter( line , ' ' ) );
		if	( is_in_string ( tmp , "<" ) ) 
			return __TITLE_O ;
		else if ( is_in_string ( tmp , "#" ) ) 
			return __COMMENT;
		else if ( is_in_string ( tmp , ">" ) ) 
			return __TITLE_C ;
		else if ( ( is_in_string( tmp , "::" ) ) && ( is_in_string( tmp , "=" ) ) )
			return __KEY ;
		else if ( ( ! is_in_string( tmp , "#=:<>" ) ) && ( strlen( tmp ) > 0 ) )
			return __SYNTAX_ERROR;
		return __UNKNOWN;
	}
	return __UNKNOWN;
}

_gim_flag	prsr_lexical_class::scan( gim_prsr_obj * prsr ) {
	char	* line;
	gim_error->set( "prsr_lexical_class::scan" , "start to scan" );
	Syntax.Title   = __GIM_CLOSE;
	Syntax.Section = __GIM_CLOSE;
	Syntax.Feof    = __GIM_OFF;
	if ( ! prsr->prsr_obj ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::scan" , "parser non inizializzato" , __GIM_ERROR);
		return __GIM_ERROR;
	}
	if ( ! prsr->prsr_obj->fp ) {
		gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::scan" , "Parser inizializzato ma il file non e' aperto" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	while ( ! feof( prsr->prsr_obj->fp ) ) {
		line = strdup( get_line ( prsr->prsr_obj->fp ) );
		if ( lex_type == __LEX_A ) scan_a( line , prsr );
		if ( lex_type == __LEX_B ) scan_b( line , prsr );
	}
	gim_error->set( "prsr_lexical_class::scan" , "finished" );
	
	return __GIM_OK;
}

void	prsr_lexical_class::scan_a( char * line , gim_prsr_obj * prsr ) {
	static char	last_section_open[256];
	switch ( scan_line ( line ) ) {
		case __TITLE_O : {
			char *tmp;
			tmp = strdup( line );
			tokenizer( tmp , PRSR_LEX_SEPARATORS );
			strcpy( prsr->prsr_obj->title , Tok[1] );
			Syntax.Title = __GIM_OPEN;
			char message[256];
			sprintf( message , "Creating object : %s" , Tok[1] );
			gim_error->set( "prsr_lexical_class::scan_a" , message );
			break;
		}
		case __TITLE_C : {
			Syntax.Title = __GIM_CLOSE;
			break;
		}
		case __SECTION_O : {
			char *tmp;
			tmp = strdup( line );
			tokenizer( tmp , PRSR_LEX_SEPARATORS );
			strcpy( last_section_open , Tok[1] );
			Syntax.Section = __GIM_OPEN;
			prsr->AddSection( Tok[1] );
			char message[256];
			sprintf( message , "Creating section : %s" , Tok[1] );
			gim_error->set( "prsr_lexical_class::scan_a" , message );
			break;
		}
		case __SECTION_C : {
			__GIM_CLEAR( last_section_open , 256 , char );
			Syntax.Section = __GIM_CLOSE;
			break;
		}
		case __KEY : {
			char *tmp;
			tmp = strdup( line );
			tokenizer( tmp , PRSR_LEX_SEPARATORS );
			prsr->AddKey( last_section_open , Tok[0] , Tok[1] );
			char message[256];
			sprintf( message , "Adding key %1s in section %s with value %s" , Tok[0] , last_section_open , Tok[1] );
			gim_error->set( "prsr_lexical_class::scan_a" , message );
			break;
		}
		case __END : {
			break;
		}
	}
}

void	prsr_lexical_class::scan_b( char * line , gim_prsr_obj * prsr ) {
	switch ( scan_line ( line ) ) {
		case __TITLE_O : {
			char *tmp;
			tmp = strdup( line );
			tokenizer( tmp , PRSR_LEX_SEPARATORS );
			strcpy( prsr->prsr_obj->title , Tok[1] );
			Syntax.Title = __GIM_OPEN;
			char message[256];
			sprintf( message , "Creating object : %s" , Tok[1] );
			gim_error->set( "prsr_lexical_class::scan_b" , message );
			break;
		}
		case __TITLE_C : {
			Syntax.Title = __GIM_CLOSE;
			break;
		}
		case __KEY : {
			char *tmp;
			tmp = strdup( line );
			tokenizer( tmp , PRSR_LEX_SEPARATORS );
			if ( prsr->ExistSection( Tok[0] ) == __GIM_NOT_EXIST ) {
				prsr->AddSection( Tok[0] );
				char message[256];
				sprintf( message , "    section : %s" , Tok[0] );
				gim_error->set( "prsr_lexical_class::scan_b" , message );
			}
			prsr->AddKey( Tok[0] , Tok[2] , Tok[3] );
			char message[256];
			sprintf( message , "        key %-15s : %s" , Tok[2] , Tok[3] );
			gim_error->set( "prsr_lexical_class::scan_b" , message );
			break;
		}
		case __END : {
			break;
		}
	}
}

void	prsr_lexical_class::scan_lex_type( const char * filename ) {
	char * line;
	FILE *  tmpfp;
	_gim_flag title = __GIM_NO;
	lex_type = __LEX_UNKNOWN;
	tmpfp = fopen( filename , "rb" );
	if ( tmpfp ) {
		while ( ( ! feof ( tmpfp ) ) && ( lex_type == __LEX_UNKNOWN )  ) {
			line = strdup( get_line ( tmpfp ) );
			if ( is_in_string ( line , "<" ) )
				title = __GIM_YES;
			if ( title == __GIM_YES ) {
				if ( is_in_string ( line , "[" ) )  {
					lex_type = __LEX_A;
					gim_error->set( "prsr_lexical_class::scan_lex_type" , "Lex is type A" );
				}
				if ( is_in_string ( line , "::" ) && is_in_string ( line , "=" ) ) {
					lex_type = __LEX_B;
					gim_error->set( "prsr_lexical_class::scan_lex_type" , "Lex is type B" );
				}
			}
		}
		if ( lex_type == __LEX_UNKNOWN )
			gim_error->set( "prsr_lexical_class::scan_lex_type" , "Lex is UNK__GIM_NOWN" );
		fclose(tmpfp);
	}
}

_gim_flag	prsr_lexical_class::scan_syntax( const char * filename , gim_prsr_obj * prsr ) {
	_gim_flag	error = 0;
	if ( gim_file_manager->exist( filename ) == __GIM_NOT_EXIST )
		return __GIM_NOT_EXIST;
	lex_type = __LEX_UNKNOWN; 
	scan_lex_type( filename );
	if ( lex_type == __LEX_UNKNOWN )
		return __LEX_UNKNOWN;
	switch ( lex_type ) {
		case __LEX_A : {
			prsr->Lex = __LEX_A ;
			error = syntax_a( filename );
			break;
		}
		case __LEX_B : {
			prsr->Lex = __LEX_B ;
			error = syntax_b( filename );
			break;
		}
	}
	return error;
}

_gim_flag prsr_lexical_class::syntax_a( const char * filename ) {
	unsigned int	Line_number = 0;
	char	* line , tmp_error[512];
	char	last_section_open[256];
	FILE * 	tmpfp;
	_gim_flag	error = __GIM_OK;
	tmpfp = fopen( filename , "rb" );
	if ( ! tmpfp ) return __GIM_NOT_EXIST;
	Syntax.Title   = __GIM_CLOSE;
	Syntax.Section = __GIM_CLOSE;
	Syntax.Feof    = __GIM_OFF;
	while ( ( ( Syntax.Feof == __GIM_OFF ) && ( error == 0 ) ) ) {
		line = strdup( get_line ( tmpfp ) );
		switch ( scan_line ( line ) ) {
			case __BLANK : {
				if ( Syntax.Section == __GIM_OPEN ) {
					sprintf( tmp_error , "blank line inside a section : %s::%d\n" , filename , Line_number );
					gim_error->set( "prsr_lexical_class::syntax_a" , tmp_error );
				}
				break;
			}
			case __TITLE_O : {
				if ( Syntax.Title == __GIM_OPEN ) {
					sprintf( tmp_error , "received title twice : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				Syntax.Title = __GIM_OPEN;
				break;
			}
			case __TITLE_C : {
				if ( Syntax.Section == __GIM_OPEN ) {
					sprintf( tmp_error , "received a title close with a section yet opened : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				Syntax.Title = __GIM_CLOSE;
				Syntax.Feof  = __GIM_ON;
				break;
			}
			case __SECTION_O : {
				char *tmp;
				if ( Syntax.Section == __GIM_OPEN ) {
					sprintf( tmp_error , "Syntax error : last section open : %s\n" , last_section_open );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					sprintf( tmp_error , "received open section twice : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				if ( Syntax.Title == __GIM_CLOSE ) {
					sprintf( tmp_error , "received open section before a title : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				tmp = strdup( line );
				tokenizer( tmp , PRSR_LEX_SEPARATORS );
				if ( strlen( Tok[1] ) == 0 ) {
					sprintf( tmp_error , "received a section open without a section name : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				strcpy( last_section_open , Tok[1] );
				Syntax.Section = __GIM_OPEN;
				break;
			}
			case __SECTION_C : {
				__GIM_CLEAR( last_section_open , PRSR_MAX_LINE , char );
				Syntax.Section = __GIM_CLOSE;
				break;
			}
			case __KEY : {
				char *tmp;
				if ( Syntax.Section == __GIM_CLOSE ) {
					sprintf( tmp_error , "received a key with no section open : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				tmp = strdup( line );
				tokenizer( tmp , PRSR_LEX_SEPARATORS );
				if ( strlen( Tok[2] ) || strlen( Tok[3] ) || strlen( Tok[4] ) ) {
					sprintf( tmp_error , "too much token in line : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				else {
					if ( ( ! strlen( Tok[0] ) ) || ( ! strlen( Tok[1] ) ) ) {
						if ( ! strlen( Tok[0] ) ) {
							sprintf( tmp_error , "received a value without a key : %s::%d\n" , filename , Line_number );
							gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
						}
						if ( ! strlen( Tok[1] ) ) {
							sprintf( tmp_error , "received a key without a value : %s::%d\n" , filename , Line_number );
							gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
						}
						gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , "I suggest to check the file" , __GIM_ERROR  );
						error = __SYNTAX_ERROR;
					}
				}
				break;
			}
			case __COMMENT : {
				break;
			}
			case __SYNTAX_ERROR : {
				sprintf( tmp_error , "unknow kind of line : %s::%d\n" , filename , Line_number );
				gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
				error = __SYNTAX_ERROR;
			}
			case __END : {
				if ( Syntax.Title == __GIM_CLOSE ) {
					sprintf( tmp_error , "received the end of file before a title : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				if ( strlen( line ) && ( is_in_string( ']' , line ) ) )	Syntax.Section = __GIM_CLOSE;
				if ( Syntax.Section == __GIM_OPEN ) {
					sprintf( tmp_error , "unespectly received the end of file with a section yet opened : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_a" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				gim_error->set( GIM_ERROR_WARNING , "prsr_lexical_class::syntax_a" , "Warning : maybe the file not contain the title close" , __GIM_ERROR );
				break;
			}
		}
		Line_number++;
	}
	fclose(tmpfp);
	return error;
}

_gim_flag prsr_lexical_class::syntax_b( const char * filename ) {
	unsigned int	Line_number = 0;
	char	* line , tmp_error[512];
	FILE	* tmpfp;
	_gim_flag	ever_key = __GIM_NO;
	_gim_flag	error = __GIM_OK;
	tmpfp = fopen( filename , "rb" );
	if ( ! tmpfp ) return __GIM_NOT_EXIST;
	Syntax.Title   = __GIM_CLOSE;
	Syntax.Section = __GIM_CLOSE;
	Syntax.Feof    = __GIM_OFF;
	while ( ( ( Syntax.Feof == __GIM_OFF ) && ( error == 0 ) ) ) {
		line = strdup( get_line ( tmpfp ) );
//		puts(line);
		switch ( scan_line ( line ) ) {
			case __BLANK : {
				if ( Syntax.Section == __GIM_OPEN )
					sprintf( tmp_error , "blank line inside a section : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_WARNING , "prsr_lexical_class::syntax_b" , tmp_error , __GIM_ERROR );
				break;
			}
			case __TITLE_O : {
				if ( Syntax.Title == __GIM_OPEN ) {
					sprintf( tmp_error , "received title twice : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_b" , tmp_error , __GIM_ERROR  );
					error = __SYNTAX_ERROR;
				}
				Syntax.Title = __GIM_OPEN;
				break;
			}
			case __TITLE_C : {
				if ( ever_key == __GIM_NO ) {
					sprintf( tmp_error , "no key defined in this file : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_b" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				Syntax.Title = __GIM_CLOSE;
				Syntax.Feof  = __GIM_ON;
				break;
			}
			case __KEY : {
				char *tmp;
				tmp = strdup( line );
				tokenizer( tmp , PRSR_LEX_SEPARATORS );
				if ( ( strlen( Tok[1] ) ) || ( strlen( Tok[4] ) ) ) {
					sprintf( tmp_error , "too much token in line : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_b" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				else {
					if ( ( ! strlen( Tok[0] ) ) || ( ! strlen( Tok[2] ) ) || ( ! strlen( Tok[3] ) ) ) {
						if ( ! strlen( Tok[0] ) ) {
							sprintf( tmp_error , "too much token in line : %s::%d\n" , filename , Line_number );
							gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_b" , tmp_error , __GIM_ERROR );
						}
						if ( ! strlen( Tok[2] ) ) {
							sprintf( tmp_error , "received a key withonly the Father : %s::%d\n" , filename , Line_number );
							gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_b" , tmp_error , __GIM_ERROR );
						}
						if ( ! strlen( Tok[3] ) ) {
							sprintf( tmp_error , "received a key without a value : %s::%d\n" , filename , Line_number );
							gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_b" , tmp_error , __GIM_ERROR );
						}
						error = __SYNTAX_ERROR;
					}
				}
				ever_key = __GIM_YES;
				break;
			}
			case __COMMENT : {
					sprintf( tmp_error , "Found a comment : %s::%d\n" , filename , Line_number );
					gim_error->set( "prsr_lexical_class::syntax_b" , tmp_error );
				break;
			}
			case __SYNTAX_ERROR : {
				sprintf( tmp_error , "unknow kind of line : %s::%d\n" , filename , Line_number );
				gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_b" , tmp_error , __GIM_ERROR );
				error = __SYNTAX_ERROR;
			}
			case __END : {
				if ( Syntax.Title == __GIM_CLOSE ) {
					sprintf( tmp_error , "received the end of file before a title : %s::%d\n" , filename , Line_number );
					gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::syntax_b" , tmp_error , __GIM_ERROR );
					error = __SYNTAX_ERROR;
				}
				gim_error->set( GIM_ERROR_WARNING , "prsr_lexical_class::syntax_b" , "Warning : maybe the file not contain the title close" , __GIM_ERROR  );
				break;
			}
		}
		Line_number++;
	}
	fclose( tmpfp );
	return error;
}

char *	prsr_lexical_class::char_filter( char * from , char to_filter ) {
	_gim_Uint32	a = 0 , b = 0, l =0;
	char		* tmp;
	l = strlen( from );
	tmp = strdup( from );
	for( ; a < l ; a++ ) {
		if ( tmp[a] != to_filter ) {
			from[b] = tmp[a];
			b++;
		}
	}
	from[b] = '\0';
	return from;
}

char *	prsr_lexical_class::char_subst( char * from , char to_subst , char with ) {
	_gim_Uint32	a = 0 , l =0;
	l = strlen( from );
	for( ; a < l ; a++ )
		if ( from[a] == to_subst ) {
			from[a] = with;
		}
	return from;
}

_gim_int8 prsr_lexical_class::tokenizer( char * line , const char * separator ) {
	char				* tmp = NULL;
	_gim_Uint8			l = 0;
	static unsigned int	i;
	unsigned int		a , b = 0;

	i = 0;
	__GIM_CLEAR( Tok , 1 , Tok );
	if ( gim_memory->caller != __GIM_MEM_READ_KEY )
		tmp = strdup( char_filter( line , ' ' ) );
	else {
		tmp = strdup( line );
		gim_memory->caller = __GIM_MEM_OTHER ;
	}
	l = strlen( tmp ) - 1;
	for( a = 0 ; a < l ; a++ ) {
		if ( i > 19 ) {
			gim_error->set( GIM_ERROR_CRITICAL , "prsr_lexical_class::tokenizer" , "too much token in line" , __GIM_ERROR  );
			return __SYNTAX_ERROR;
		}
		if ( is_in_string( tmp[a] , separator ) ) {
			if ( tmp[a] == '#' ) {
				a = l;
			}
			else {
				Tok[i][b] = '\0' ;
				i++;
				b=0;
			}
		}
		else {
			Tok[i][b] = tmp[a];
			b++;
		}
	}
	return i;
}


char * prsr_lexical_class::str_up( const char * line , int len ) {
	static char	* tmp = NULL;
	_gim_Uint8	a = 0;
	char	c = 0;
	tmp = strdup( line );
	for ( a = 0 ; a < len ; a++ ) {
		c = tmp[a];
		tmp[a] = toupper(c);
	}
	return tmp;
}

char * prsr_lexical_class::str_down ( const char * line , int len ) {
	static char	* tmp = NULL;
	_gim_Uint8	a = 0;
	char	c = 0;
	tmp = strdup( line );
	for ( a = 0 ; a < len ; a++ ) {
		c = tmp[a];
		tmp[a] = tolower(c);
	}
	return tmp;
}


char * prsr_lexical_class::str_offset_pre( char * line , _gim_Uint16 offset ) {

	if ( strlen( line ) > offset ) {
		return &line[offset];
	}
	else 
		return NULL;
}



void	prsr_lexical_class::strend( char * str , char chr ) {
    if ( ! str )
        return;
    char * p;
    if ( ( p = strchr( str , chr ) ) )
        *p = 0;
}

void 	prsr_lexical_class::remove_quotes( char * str ) {
    if ( ! str )
        return;
    while ( *str == '"' )
        *( str++ ) = ' ';
    
    strend( str , '"' );
}

void	prsr_lexical_class::remove_linefeed( char * str ) {
    strend( str , '\n' );
}


_gim_flag	prsr_lexical_class::str_equal( const char * str , const char * str1 ) {
	if ( str == NULL ) 
		return __GIM_ERROR;
	if ( str1 == NULL ) 
		return __GIM_ERROR;
	if ( ! ( strcmp( str , str1 ) ) )
		return __GIM_YES;
	else
		return __GIM_NO;
}



char *		prsr_lexical_class::string_trunc( const char * string , _gim_int16 trunc ) {
	_gim_Uint8	len = 0 , offset = 0; 
	int	c;
	static char	tmp[4096];
	static char	* newstr;
	__GIM_VCLEAR( tmp , sizeof( tmp ) , char , '\0' );
	strcpy( tmp , string );
	len = strlen( string );
	if ( len > trunc ) {
		offset = len - trunc;
		newstr = &tmp[offset];
		for ( c=0 ; c<3 ; c++ )
			newstr[c] = '.' ;
	}
	else {
		
		for( c = len ; c < trunc ; c++ )
			strcat(tmp," ");
		newstr = tmp ;
	}	
	return newstr ;
}


char *		prsr_lexical_class::str_nostd_conv( const char * string , char * result ) {
	static char	res[4096];
	char	tmp[1024] , tmp2[16];
	_gim_Uint16	slen , c;
	if (!result)
		result = ( char * )gim_memory->Alloc( ( sizeof( char ) * 4096 ) , __GIM_MEM_NOSTD_CONV , __GIM_HIDE );
	strcpy( result , "");
	strcpy( tmp    , "");
	slen = strlen( string );
	for ( c = 0 ; c < slen ; c++ ) {
		if ( isprint( string[c] ) ) {
			sprintf( result , "%s%c" ,tmp , ( string[c] ) );
			strcpy( tmp , result );
		}
		else {
			sprintf( tmp2  , "[%x]" , string[c] & 0xff );
			sprintf( result , "%s%s" ,tmp , tmp2);
			strcpy( tmp , result );
		}
	}
	strcpy( res , result );
	gim_memory->Free( result );
	return res;
}
