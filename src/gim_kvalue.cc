//
// gim_kvalue.cc
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

#include "../include/gim_kvalue.h"
//#include "../include/gim_memory.h"
 

void gim_prsr_obj::Up( const char * filename , const char * title ) {
	gim_error->set( "gim_prsr_obj::Up" , "Starting up..." );
	prsr_obj = ( prsr * )gim_memory->Alloc( sizeof( prsr ) , __GIM_MEM_PRSP_UP , __GIM_HIDE );
	if ( filename != NULL )
		strcpy( prsr_obj->filename , filename );
	if ( title != NULL )
		strcpy( prsr_obj->title , title );
	strcpy( prsr_obj->comment , "" );
	prsr_obj->first_section = NULL;
	prsr_obj->fp = NULL;
	if ( title ) {
		char message[256];
		sprintf( message , "Conf file [%s] is UP" , prsr_obj->title );
		gim_error->set( "gim_prsr_obj::Up" , message );
	}
	else
		gim_error->set( "gim_prsr_obj::Up" , "Conf file is UP" );
}

_gim_flag gim_prsr_obj::AddSection( const char * section_name ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_section	* new_section = NULL;
	char * tmp_name;
	if ( ( ! strlen( section_name ) ) || ( strlen( section_name ) >= 64 ) ) return -1;
	tmp_name = strdup( Lexical.str_up( section_name , strlen( section_name ) ) );
	if ( ExistSection( tmp_name ) == __GIM_YES ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::AddSection" , "Section already exist" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section->next_section != NULL ; tmp_section = tmp_section->next_section ) ;
		new_section = ( prsr_kvalue_section	* )gim_memory->Alloc( sizeof( prsr_kvalue_section ) , __GIM_MEM_PRSR_ADDSECT , __GIM_HIDE );
		strcpy( new_section->section_name , tmp_name );
		new_section->next_section = NULL;
		new_section->comment_position = PRSR_NONE;
		tmp_section->next_section = new_section;
	}
	if ( prsr_obj->first_section == NULL ) {
		new_section = ( prsr_kvalue_section	* )gim_memory->Alloc( sizeof( prsr_kvalue_section ) , __GIM_MEM_PRSR_ADDSECT , __GIM_HIDE );
		strcpy( new_section->section_name , tmp_name  );
		new_section->next_section = NULL;
		new_section->comment_position = PRSR_NONE;
		prsr_obj->first_section = new_section;
	}
	return __GIM_OK;
}

_gim_flag	gim_prsr_obj::AddKey( const char * section_name , const char * key , const char * value ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field = NULL;
	prsr_kvalue_field	* new_field = NULL;
	char * tmp_section_name;
	char * tmp_key_name;
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::AddKey" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( prsr_obj->first_section == NULL )  {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::AddKey" , "Before to define a Key you must define a Section" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ( ! strlen( section_name ) ) || ( strlen( section_name ) >= 64 ) )  {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::AddKey" , "Wrong lenght of section name" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ( ! strlen( value ) ) || ( strlen( value ) >= 64 ) )  {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::AddKey" , "Wrong lenght of value" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ( ! strlen( key ) ) || ( strlen( key ) >= 64 ) )  {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::AddKey" , "Wrong lenght of Key name" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	tmp_key_name     = strdup( Lexical.str_down( key          , strlen( key          ) ) );
	if ( ExistKey( tmp_section_name , tmp_key_name ) == 1 ) return -1;
	for( ; tmp_section ; tmp_section = tmp_section->next_section ) {
		if ( ! strcmp( tmp_section_name , tmp_section->section_name ) ) break;
	}
	if ( ! tmp_section ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::AddKey" , "Section Name not found" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( tmp_section->first_field != NULL ) {
		for( tmp_field = tmp_section->first_field ; tmp_field->next_field ; tmp_field = tmp_field->next_field ) {
			if ( ! ( strcmp( tmp_field->key , tmp_key_name ) ) ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::AddKey" , "Section Name already exist" , __GIM_ERROR );
				return __GIM_NOT_OK;
			}				
		}
		new_field = ( prsr_kvalue_field * )gim_memory->Alloc( sizeof( prsr_kvalue_field ) , __GIM_MEM_PRSR_ADDKEY , __GIM_HIDE );
		strcpy( new_field->key , tmp_key_name );
		strcpy( new_field->str_value , value );
		strcpy( new_field->comment , "" );
		new_field->comment_position = PRSR_NONE;
		new_field->key_type = PRSR_GIM_INT;
		new_field->next_field = NULL;
		tmp_field->next_field = new_field;
	}
	if ( tmp_section->first_field == NULL ) {
		new_field = ( prsr_kvalue_field * )gim_memory->Alloc( sizeof( prsr_kvalue_field ) , __GIM_MEM_PRSR_ADDKEY , __GIM_HIDE );
		strcpy( new_field->key , tmp_key_name );
		strcpy( new_field->str_value , value );
		strcpy( new_field->comment , "" );
		new_field->comment_position = PRSR_NONE;
		new_field->key_type = PRSR_GIM_INT;
		new_field->next_field = NULL;
		tmp_section->first_field = new_field;
	}
	return __GIM_OK;
}


_gim_flag	gim_prsr_obj::AddKeyFlag( const char * section_name , const char * key , _gim_flag flag   ) {
	static char tmp[64];
	__GIM_CLEAR( tmp , 64 , char );
	switch (flag) {
		case __GIM_YES : {
			sprintf( tmp , "YES" );
			break;
		}
		case __GIM_NO : {
			sprintf( tmp , "NO" );
			break;
		}
		case __GIM_AUTO : {
			sprintf( tmp , "AUTO" );
			break;
		}
		case __GIM_TRUE : {
			sprintf( tmp , "TRUE" );
			break;
		}
		case __GIM_FALSE : {
			sprintf( tmp , "FALSE" );
			break;
		}
		case __LEX_A : {
			sprintf( tmp , "A" );
			break;
		}
		case __LEX_B : {
			sprintf( tmp , "B" );
			break;
		}
		case __GIM_ON : {
			sprintf( tmp , "ON" );
			break;
		}
		case __GIM_OFF : {
			sprintf( tmp , "OFF" );
			break;
		}
		case __GIM_MEM_LOCK : {
			sprintf( tmp , "LOCKED" );
			break;
		}
		case __GIM_MEM_UNLOCK : {
			sprintf( tmp , "UNLOCKED" );
			break;
		}
		case __GIM_LOG_TEXT : {
			sprintf( tmp , "HTML" );
			break;
		}
		case __GIM_LOG_HTML : {
			sprintf( tmp , "TEXT" );
			break;
		}
	}	
	return AddKey( section_name , key , tmp );
}

_gim_flag	gim_prsr_obj::AddKey( const char * section_name , const char * key , int value   ) {
	static char tmp[64];
	__GIM_CLEAR( tmp , 64 , char );
	sprintf( tmp , "%d" , value );
	return AddKey( section_name , key , tmp );
}

_gim_flag	gim_prsr_obj::AddKey( const char * section_name , const char * key , float value   ) {
	static char tmp[64];
	__GIM_CLEAR( tmp , 64 , char );
	sprintf( tmp , "%15.5f" , value );
	return AddKey( section_name , key , tmp );
}

_gim_flag	gim_prsr_obj::DelKey( const char * section_name , const char * key ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field , * before_field = NULL;
	char * tmp_section_name;
	char * tmp_key_name;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	tmp_key_name     = strdup( Lexical.str_down( key          , strlen( key          ) ) );
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::DelKey" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field ; before_field=tmp_field , tmp_field = tmp_field->next_field ) {
						if ( ! strcmp( tmp_key_name , tmp_field->key  ) ) {
							if ( ! before_field )
								tmp_section->first_field = tmp_field->next_field ;
							if ( before_field )
								before_field->next_field = tmp_field->next_field ;
							gim_error->set( "gim_prsr_obj::DelKey" , "Removing Key" );
							if ( gim_memory->Free( tmp_field ) == __GIM_NOT_OK ) {
								gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::DelKey" , "Some problems with memory" , __GIM_ERROR );
							}
							return __GIM_OK;
						}
					}
				}
			}
		}
	}
	return __GIM_NOT_OK;
}

_gim_flag	gim_prsr_obj::DelSection( const char * section_name ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_section	* before_section = NULL;

	char * tmp_section_name;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::DelSection" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( prsr_obj->first_section != NULL ) {
		char message[512];
		sprintf( message , "Removing section %s" , tmp_section_name );
		gim_error->set( "gim_prsr_obj::DelSection" , message );
		for( ; tmp_section ; before_section = tmp_section , tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				if ( ! before_section )
					prsr_obj->first_section = tmp_section->next_section ;
				if ( before_section )
					before_section->next_section = tmp_section->next_section ;
				if ( tmp_section->first_field ) {
					DelSection( tmp_section );
				}
				gim_error->set( "gim_prsr_obj::DelSection" , "Done!" );
				return __GIM_OK;
			}
		}
	}
	return __GIM_NOT_OK;
}

void	gim_prsr_obj::DelSection( prsr_kvalue_section * section ) {
	prsr_kvalue_field	* tmp_field , * tmp2_field = NULL;
	char message[512];
	sprintf( message , "Removing section %s" , section->section_name );
	gim_error->set( "gim_prsr_obj::DelSection" , message );
	for( tmp_field = section->first_field ; tmp_field ;  ) {
		char tmp[256];
		sprintf( tmp , "Removing key : %s" , tmp_field->key );
		gim_error->set( "gim_prsr_obj::DelSection" , tmp );
		tmp2_field = tmp_field->next_field;
		if ( gim_memory->Free( tmp_field ) == __GIM_NOT_OK )
			gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::DelSection" , "Some problems with memory" , __GIM_ERROR );
		tmp_field = tmp2_field;
	}
	if ( gim_memory->Free( section ) == __GIM_NOT_OK )
		gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::DelSection" , "Some problems with memory" , __GIM_ERROR );
	gim_error->set( "gim_prsr_obj::DelSection" , "Section removed" );	
}

_gim_flag	gim_prsr_obj::ChangeKey( const char * section_name , const char * key , const char * string ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field , * before_field = NULL;
	char * tmp_section_name;
	char * tmp_key_name;
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::ChangeKey" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( prsr_obj->first_section == NULL )  {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::ChangeKey" , "Before to define a Key you must define a Section" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ( ! strlen( section_name ) ) || ( strlen( section_name ) >= 64 ) )  {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::ChangeKey" , "Wrong lenght of section name" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ( ! strlen( string ) ) || ( strlen( string ) >= 64 ) )  {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::ChangeKey" , "Wrong lenght of value" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ( ! strlen( key ) ) || ( strlen( key ) >= 32 ) )  {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::ChangeKey" , "Wrong lenght of Key name" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	tmp_key_name     = strdup( Lexical.str_down( key          , strlen( key          ) ) );
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field ; before_field=tmp_field , tmp_field = tmp_field->next_field ) {
						if ( ! strcmp( tmp_key_name , tmp_field->key  ) ) {
							strcpy( tmp_field->str_value , string );
							return __GIM_OK;
						}
					}
				}
			}
		}
	}
	return __GIM_NOT_OK;
}

_gim_flag	gim_prsr_obj::ChangeKey	( const char * section_name , const char * key , int value   ) {
	char tmp[64];
	__GIM_CLEAR( tmp , 64 , char );
	sprintf( tmp , "%d" , value );
	return ChangeKey( section_name , key , tmp );
}



_gim_flag	gim_prsr_obj::ExistSection( const char * section_name ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	char * tmp_section_name;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp( tmp_section_name , tmp_section->section_name ) ) {
				return  __GIM_EXIST;
			}
		}
	}
	return __GIM_NOT_EXIST;
}


prsr_kvalue_section	* gim_prsr_obj::SearchSection( const char * section_name ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	char * tmp_section_name;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section ; tmp_section = tmp_section->next_section ) {
			if( Lexical.str_equal( tmp_section_name , tmp_section->section_name ) == __GIM_YES ) {
				return  tmp_section;
			}
		}
	}
	return NULL;
}


_gim_flag	gim_prsr_obj::ExistKey( const char * section_name , const char * key ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field;
	char * tmp_section_name;
	char * tmp_key_name;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	tmp_key_name     = strdup( Lexical.str_down( key          , strlen( key          ) ) );
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field ; tmp_field = tmp_field->next_field ) {
						if ( ! strcmp( tmp_key_name , tmp_field->key  ) ) {
							return __GIM_EXIST;
						}
					}
				}
			}
		}
	}
	return __GIM_NOT_EXIST;
}


_gim_flag	gim_prsr_obj::Down( void ) {
	prsr_kvalue_section	* tmp_section  = NULL;
	prsr_kvalue_section	* tmp2_section = NULL;
	char	tmp[256];
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::Down" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	sprintf( tmp , "%s Prsr object goes down..." , prsr_obj->title );
	gim_error->set( "gim_prsr_obj::Down" , tmp );
	if ( prsr_obj->first_section != NULL ) {
		for( tmp_section = prsr_obj->first_section ; tmp_section ; ) {
			tmp2_section = tmp_section->next_section;
			DelSection( tmp_section );
			tmp_section = tmp2_section;
		}
	}
	if ( prsr_obj->fp != NULL )
		Close( __NO_FLUSH );
	if ( gim_memory->Free( prsr_obj ) == __GIM_NOT_OK )
		gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::Down" , "Some problems with memory" , __GIM_ERROR );
	prsr_obj = NULL;
	gim_error->set( "gim_prsr_obj::Down" , "Done!" );
	return __GIM_OK;
}


_gim_flag	gim_prsr_obj::List( void ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field;
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::List" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	printf( PRSR_START_FILE , prsr_obj->title );
	puts("\n");
	if ( prsr_obj->first_section != NULL ) {
		if ( Lex == __LEX_A ) {
			for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
				printf( PRSR_START_SECTION , tmp_section->section_name );
				puts("");
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field != NULL ; tmp_field = tmp_field->next_field ) {
						printf( PRSR_A_KEY_STR , tmp_field->key , tmp_field->str_value );
						puts("");
					}
				}
				printf( "%s\n\n" , PRSR_END_SECTION);
			}
		}
		else if ( Lex == __LEX_B ) {
			for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field != NULL ; tmp_field = tmp_field->next_field ) {
						printf( PRSR_B_KEY_STR , Lexical.str_down( tmp_section->section_name, strlen( tmp_section->section_name ) ) , tmp_field->key , tmp_field->str_value );
						puts("");
					}
				}
			}
			puts("");
		}
		else if ( Lex == __LEX_C ) {
			for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field != NULL ; tmp_field = tmp_field->next_field ) {
						printf( PRSR_C_KEY_STR , Lexical.str_down( tmp_section->section_name, strlen( tmp_section->section_name ) ) , tmp_field->key , tmp_field->str_value );
						puts("");
					}
				}
			}
			puts("");
		}
	}
	puts( PRSR_END_FILE );
	return __GIM_OK;
}

_gim_flag	gim_prsr_obj::Write( void ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field;
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::Write" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( strlen( prsr_obj->filename ) == 0 ) return __GIM_NOT_OK;
	if ( ! prsr_obj->fp ) {
		if ( Open( "wb" ) == __GIM_NOT_EXIST ) return __GIM_NOT_OK;
	}
	fprintf( prsr_obj->fp , PRSR_START_FILE , prsr_obj->title );
	fprintf( prsr_obj->fp , "\n\n" );
	if ( strlen( prsr_obj->comment ) != 0 ) 
		fprintf( prsr_obj->fp , "  # %s\n" , prsr_obj->comment );
	if ( prsr_obj->first_section != NULL ) {
		if ( Lex == __LEX_A ) {
			fprintf( prsr_obj->fp , "\n" );
			for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
				if ( ( tmp_section->comment != NULL ) && ( tmp_section->comment_position == PRSR_BEFORE ) )
					fprintf( prsr_obj->fp , "\n    # %s\n" , tmp_section->comment );
				fprintf( prsr_obj->fp , PRSR_START_SECTION , tmp_section->section_name );
				if ( ( tmp_section->comment != NULL ) && ( tmp_section->comment_position == PRSR_INLINE ) )
					fprintf( prsr_obj->fp , "    # %s" , tmp_section->comment );
				fprintf( prsr_obj->fp , "\n" );
				if ( ( tmp_section->comment != NULL ) && ( tmp_section->comment_position == PRSR_AFTER ) )
					fprintf( prsr_obj->fp , "    # %s\n\n" , tmp_section->comment );
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field != NULL ; tmp_field = tmp_field->next_field ) {
						if ( ( tmp_field->comment != NULL ) && ( tmp_field->comment_position == PRSR_BEFORE ) )
							fprintf( prsr_obj->fp , "\n    # %s\n" , tmp_field->comment );
						fprintf( prsr_obj->fp , PRSR_A_KEY_STR , tmp_field->key , tmp_field->str_value );
						if ( ( tmp_field->comment != NULL ) && ( tmp_field->comment_position == PRSR_INLINE ) )
							fprintf( prsr_obj->fp , "  # %s" , tmp_field->comment );
						fprintf( prsr_obj->fp , "\n" );
						if ( ( tmp_field->comment != NULL ) && ( tmp_field->comment_position == PRSR_AFTER ) )
							fprintf( prsr_obj->fp , "    # %s\n\n" , tmp_field->comment );
					}
				}
				fprintf( prsr_obj->fp , "%s\n\n\n" , PRSR_END_SECTION);
			}
		}
		else if ( Lex == __LEX_B ) {
			for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
				if ( tmp_section->first_field ) {
					if ( ( tmp_section->comment != NULL ) && ( tmp_section->comment_position == PRSR_BEFORE ) )
						fprintf( prsr_obj->fp , "\n    # %s\n" , tmp_section->comment );
					fprintf( prsr_obj->fp , "\n  %s" , tmp_section->section_name );
					if ( ( tmp_section->comment != NULL ) && ( tmp_section->comment_position == PRSR_INLINE ) )
						fprintf( prsr_obj->fp , "        # %s" , tmp_section->comment );
					fprintf( prsr_obj->fp , "\n" );
					if ( ( tmp_section->comment != NULL ) && ( tmp_section->comment_position == PRSR_AFTER ) )
						fprintf( prsr_obj->fp , "    # %s\n\n" , tmp_section->comment );
					
					for( tmp_field = tmp_section->first_field ; tmp_field != NULL ; tmp_field = tmp_field->next_field ) {
						if ( ( tmp_field->comment != NULL ) && ( tmp_field->comment_position == PRSR_BEFORE ) )
							fprintf( prsr_obj->fp , "    # %s\n" , tmp_field->comment );
						fprintf( prsr_obj->fp , PRSR_B_KEY_STR , Lexical.str_down( tmp_section->section_name , strlen( tmp_section->section_name ) ) , tmp_field->key , tmp_field->str_value );
						if ( ( tmp_field->comment != NULL ) && ( tmp_field->comment_position == PRSR_INLINE ) )
							fprintf( prsr_obj->fp , "  # %s" , tmp_field->comment );
						fprintf( prsr_obj->fp , "\n" );
						if ( ( tmp_field->comment != NULL ) && ( tmp_field->comment_position == PRSR_AFTER ) )
							fprintf( prsr_obj->fp , "    # %s\n\n\n" , tmp_field->comment );
					}
				}
			}
		}
		else if ( Lex == __LEX_C ) {
			fprintf( prsr_obj->fp , "\n" );
			for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
				if ( ( tmp_section->comment != NULL ) && ( tmp_section->comment_position == PRSR_BEFORE ) )
					fprintf( prsr_obj->fp , "\n    # %s\n" , tmp_section->comment );
				fprintf( prsr_obj->fp , PRSR_C_START_SECTION , tmp_section->section_name );
				if ( ( tmp_section->comment != NULL ) && ( tmp_section->comment_position == PRSR_INLINE ) )
					fprintf( prsr_obj->fp , "        # %s" , tmp_section->comment );
				fprintf( prsr_obj->fp , "\n" );
				if ( ( tmp_section->comment != NULL ) && ( tmp_section->comment_position == PRSR_AFTER ) )
					fprintf( prsr_obj->fp , "    # %s\n\n" , tmp_section->comment );
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field != NULL ; tmp_field = tmp_field->next_field ) {
						if ( ( tmp_field->comment != NULL ) && ( tmp_field->comment_position == PRSR_BEFORE ) )
							fprintf( prsr_obj->fp , "\n        # %s\n" , tmp_field->comment );
						fprintf( prsr_obj->fp , PRSR_C_KEY_STR , tmp_field->key , tmp_field->str_value );
						if ( ( tmp_field->comment != NULL ) && ( tmp_field->comment_position == PRSR_INLINE ) )
							fprintf( prsr_obj->fp , "    # %s" , tmp_field->comment );
						fprintf( prsr_obj->fp , "\n" );
						if ( ( tmp_field->comment != NULL ) && ( tmp_field->comment_position == PRSR_AFTER ) )
							fprintf( prsr_obj->fp ,   "        # %s\n\n" , tmp_field->comment );
					}
				}
				fprintf( prsr_obj->fp , "%s\n\n\n" , PRSR_C_END_SECTION);
			}
		}
		
	}
	fprintf( prsr_obj->fp , "%s\n" , PRSR_END_FILE );
	Close( __NO_FLUSH );
	return __GIM_OK;
}


_gim_flag gim_prsr_obj::Read( const char * filename ) {
	_gim_int8	scan_result;
	gim_error->Set( GIM_ERROR_MESSAGE , "gim_prsr_obj::Read" , "I am trying to read: %s" , filename );
	scan_result = Lexical.scan_syntax( filename , this );
	switch ( scan_result ) {
		case __GIM_OK : {
			Up( filename , NULL );
			SetLex( Lexical.lex_type );
			if ( ! prsr_obj )
				return __GIM_NOT_OK;
			if ( Open("rb") == __GIM_NOT_EXIST ) {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::Read" , "I can't read this file" , __GIM_ERROR );
				return __GIM_NOT_EXIST;
			}
			if ( Lexical.scan( this ) == __GIM_OK ) {
				Close( __NO_FLUSH );
				gim_error->set( "gim_prsr_obj::Read" , "Succesfully read" );
				return __GIM_OK;
			}
			else {
				gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::Read" , "some problem with the file" , __GIM_ERROR );
				return __GIM_NOT_OK;
			}
		}
		case __LEX_UNKNOWN : {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::Read" , "I cannot read this file" , __GIM_ERROR );
			return __GIM_NOT_OK;
		}
		case __GIM_NOT_EXIST : {
			gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::Read" , "File not exist" , __GIM_ERROR );
			return __GIM_NOT_EXIST;
		}
		case __SYNTAX_ERROR : {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::Read" , "I cannot read this file" , __GIM_ERROR );
			return __GIM_NOT_OK;
		}
	}
	gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::Read" , "I cannot read this file" , __GIM_ERROR );
	return __GIM_NOT_OK;
}

_gim_flag	gim_prsr_obj::Open( const char * mode ) {
	prsr_obj->fp = fopen( prsr_obj->filename , mode );
	if ( ! prsr_obj->fp ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::Open" , "File not found" , __GIM_ERROR );
		return __GIM_NOT_EXIST;
	}
	else
		gim_error->set( "gim_prsr_obj::Open" , "Succesfully opened" );
	return __GIM_EXIST;
}

_gim_flag	gim_prsr_obj::Close( _gim_flag flush ) {
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::Close" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ! prsr_obj->fp ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::Close" , "No opened file." , __GIM_NOT_OK );
		return __GIM_NOT_OK;
	}
	if ( flush == __FLUSH )
		return ( Write() );
	fclose( prsr_obj->fp );
	prsr_obj->fp = NULL;
	gim_error->set( "gim_prsr_obj::Close" , "Configuration file close." );
	return __GIM_OK;
}


char *	gim_prsr_obj::GetKeySTR( const char * section_name , const char * key ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field , * before_field = NULL;
	char * tmp_section_name;
	char * tmp_key_name;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	tmp_key_name     = strdup( Lexical.str_down( key          , strlen( key          ) ) );
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetKeySTR" , "No parser object defined" , __GIM_ERROR );
		return NULL;
	}
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field ; before_field=tmp_field , tmp_field = tmp_field->next_field ) {
						if ( ! strcmp( tmp_key_name , tmp_field->key  ) ) {
							return tmp_field->str_value ;
						}
					}
				}
			}
		}
	}
	return NULL;
}


_gim_flag	gim_prsr_obj::GetKeyFLAG( const char * section_name , const char * key ) {
	char * tmp = NULL;
	tmp = strdup( GetKeySTR( section_name , key ) );
	if ( tmp != NULL ) {
		if ( Lexical.str_equal( tmp , "YES" ) == __GIM_YES ) return __GIM_YES;
		else if ( Lexical.str_equal( tmp , "NO"  ) == __GIM_YES ) return __GIM_NO;
		else if ( Lexical.str_equal( tmp , "AUTO"  ) == __GIM_YES ) return __GIM_AUTO;
		else if ( Lexical.str_equal( tmp , "TRUE"   ) == __GIM_YES ) return __GIM_TRUE;
		else if ( Lexical.str_equal( tmp , "FALSE"   ) == __GIM_YES ) return __GIM_FALSE;
		else if ( Lexical.str_equal( tmp , "A"   ) == __GIM_YES ) return __LEX_A;
		else if ( Lexical.str_equal( tmp , "B"   ) == __GIM_YES ) return __LEX_B;
		else if ( Lexical.str_equal( tmp , "ON"   ) == __GIM_YES ) return __GIM_ON;
		else if ( Lexical.str_equal( tmp , "OFF"   ) == __GIM_YES ) return __GIM_OFF;
		else if ( Lexical.str_equal( tmp , "LOCKED"   ) == __GIM_YES ) return __GIM_MEM_LOCK;
		else if ( Lexical.str_equal( tmp , "UNLOCKED"   ) == __GIM_YES ) return __GIM_MEM_UNLOCK;
		else if ( Lexical.str_equal( tmp , "TEXT"   ) == __GIM_YES ) return __GIM_LOG_TEXT;
		else if ( Lexical.str_equal( tmp , "HTML"   ) == __GIM_YES ) return __GIM_LOG_HTML;
		else 	gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetKeyFLAG" , "UNKNOWN flag value" , __GIM_ERROR );
	}
	else 
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetKeyFLAG" , "GetKeySTR return NULL." , __GIM_ERROR );
	return __GIM_ERROR;
}



int	gim_prsr_obj::GetKeyINT( const char * section_name , const char * key ) {
	char * tmp = NULL;
	tmp = strdup( GetKeySTR( section_name , key ) );
	if ( tmp != NULL )
		return atoi( tmp );
	else 
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetKeyINT" , "GetKeySTR return NULL." , __GIM_ERROR );
	return __GIM_ERROR;
}

float	gim_prsr_obj::GetKeyFLT( const char * section_name , const char * key ) {
	char * tmp = NULL;
	tmp = strdup( GetKeySTR( section_name , key ) );
	if ( tmp != NULL )
		return atof( tmp );
	else
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetKeyFLT" , "GetKeySRT return NULL." , __GIM_ERROR );
	return __GIM_ERROR;
}


_gim_int8	gim_prsr_obj::GetHowManyKey( const char * section_name ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field;
	char * tmp_section_name;
	_gim_Uint8  cont = 0;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetHowManyKey" , "No parser object defined" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field ; tmp_field = tmp_field->next_field ) {
						cont++;	
					}
				}
			}
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetHowManyKey" , "No section defined" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	return cont;
}


char *  gim_prsr_obj::GetKeyName( const char * section_name , _gim_int16 kindex ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field;
	char * tmp_section_name;
	_gim_int16  cont = 0;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetKeyName" , "No parser object defined" , __GIM_ERROR );
		return NULL;
	}
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field ; tmp_field = tmp_field->next_field , cont++ ) {
						if ( kindex == cont )
							return tmp_field->key;
					}
				}
			}
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetKeyName" , "No section defined" , __GIM_ERROR );
		return NULL;
	}
	gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetKeyName" , "Index not found" , __GIM_ERROR );
	return NULL;
}


_gim_int16	gim_prsr_obj::GetHowManySection( void ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	_gim_int16 c;

	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetHowManySection" , "No parser object defined" , __GIM_ERROR );
		return __GIM_ERROR;
	}
	if ( prsr_obj->first_section != NULL ) {
		for( c = 0 ; tmp_section ; tmp_section = tmp_section->next_section )
			c++;
		return c;
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetHowManySection" , "No section defined in this object" , __GIM_ERROR );
	}
	return __GIM_ERROR;
}

char *  gim_prsr_obj::GetSectionName( _gim_int16 secindex ) {
	
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	_gim_int16 c = 0;
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetSectionName" , "No parser object defined" , __GIM_ERROR );
		return NULL;
	}
	if ( secindex >= GetHowManySection () ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetSectionName" , "You have requested an index greater than the maximum" , __GIM_ERROR );
		return NULL;
	}
	if ( prsr_obj->first_section != NULL ) {
		for( c = 0 ; tmp_section ; tmp_section = tmp_section->next_section ) {
			if ( c == secindex ) return tmp_section->section_name;
			c++;
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetSectionName" , "No section defined in this object" , __GIM_ERROR );
		return NULL;
	}
	gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::GetSectionName" , "Section not found" , __GIM_ERROR );
	return NULL;
}

void	gim_prsr_obj::SetLex( _gim_flag Lex ) {
	if ( Lex == __LEX_A ) {
		gim_error->set( "gim_prsr_obj::SetLex" , "Lex setted to A" );
		this->Lex = __LEX_A;
	}
	if ( Lex == __LEX_B ) {
		gim_error->set( "gim_prsr_obj::SetLex" , "Lex setted to B" );
		this->Lex = __LEX_B;
	}
	if ( Lex == __LEX_C ) {
		gim_error->set( "gim_prsr_obj::SetLex" , "Lex setted to C" );
		this->Lex = __LEX_C;
	}
}

_gim_flag	gim_prsr_obj::GetLex( void ) {
	return this->Lex;
}

_gim_flag	gim_prsr_obj::RenameTitle( const char * new_title ) {
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameTitle" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ! strlen( new_title ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameTitle" , "New title is NULL" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	__GIM_CLEAR( prsr_obj->title , 64 , char );
	strcpy( prsr_obj->title , new_title );
	return __GIM_OK;
}

_gim_flag	gim_prsr_obj::RenameSection( const char * old_section , const char * new_section ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	char 	* tmp_section_name;
	char	* tmp_new_section;
	_gim_flag	found = __GIM_NO;
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameSection" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ! strlen( new_section ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameSection" , "New section is NULL" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
		if ( ! strlen( old_section ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameSection" , "Old section is NULL" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	tmp_section_name = strdup( Lexical.str_up  ( old_section , strlen( old_section ) ) );
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				found = __GIM_YES;
				__GIM_CLEAR( tmp_section->section_name , 64 , char );
				strcpy( tmp_section->section_name , new_section );
				tmp_new_section = strdup( Lexical.str_up  ( new_section , strlen( new_section ) ) );
				return __GIM_OK;
			}
		}
		if ( found == __GIM_NO ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameSection" , "Old Section not found" , __GIM_NOT_EXIST );
			return __GIM_NOT_EXIST;
		}
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameSection" , "No one section is defined in this prsp object" , __GIM_NOT_EXIST );
		return __GIM_NOT_EXIST;
	}
	return __GIM_OK;
}

_gim_flag	gim_prsr_obj::RenameKey( const char * section , const char * old_key , const char * new_key ) {
	prsr_kvalue_section	* tmp_section;
	prsr_kvalue_field	* tmp_field;
	char * tmp_section_name;
	char * tmp_old_key_name;
	char * tmp_new_key_name;
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameKey" , "No parser object defined" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	tmp_section = prsr_obj->first_section;
	if ( ! strlen( section ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameKey" , "Section name is NULL" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ! strlen( new_key ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameKey" , "New key is NULL" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( ! strlen( old_key ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::RenameKey" , "Old key is NULL" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	tmp_section_name	= strdup( Lexical.str_up  ( section	, strlen( section ) ) );
	tmp_old_key_name    = strdup( Lexical.str_down( old_key	, strlen( old_key ) ) );
	tmp_new_key_name    = strdup( Lexical.str_down( new_key	, strlen( new_key ) ) );
	if ( tmp_section != NULL ) {
		for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field ; tmp_field = tmp_field->next_field ) {
						if ( ! strcmp( tmp_old_key_name , tmp_field->key  ) ) {
							strcpy( tmp_field->key , tmp_new_key_name );
							return __GIM_OK;
						}
					}
				}
			}
		}
	}
	return __GIM_NOT_OK;
}


_gim_buffer	gim_prsr_obj::WriteOnBuffer( void ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field;
	_gim_buffer 		dest;
	
	if ( prsr_obj == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::List" , "No parser object defined" , __GIM_ERROR );
		return NULL;
	}
	dest = (_gim_buffer)gim_memory->Alloc( 10240*sizeof( char ) , __GIM_MEM_OTHER , __GIM_HIDE );
	char Tmp[512];
	sprintf( Tmp , PRSR_START_FILE , prsr_obj->title );
	sprintf( dest , "%s%s\n\n" , dest , Tmp );
	if ( prsr_obj->first_section != NULL ) {
		if ( Lex == __LEX_A ) {
			for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
				sprintf( Tmp , PRSR_START_SECTION , tmp_section->section_name );
				sprintf( dest , "%s%s\n", dest , Tmp ); 
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field != NULL ; tmp_field = tmp_field->next_field ) {
						sprintf( dest , "%s    %s = %s" , dest , tmp_field->key , tmp_field->str_value );
//						if ( tmp_field->key_type == PRSR_GIM_INT ) sprintf( dest , "%s    %s = %d" , dest , tmp_field->key , tmp_field->int_value );
						strcat( dest , "\n");
					}
				}
				sprintf( dest , "%s%s\n\n" , dest , PRSR_END_SECTION);
			}
		}
		else if ( Lex == __LEX_B ) {
			for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field != NULL ; tmp_field = tmp_field->next_field ) {
						sprintf( dest , "%s    %s::%s = %s" , dest , Lexical.str_down( tmp_section->section_name, strlen( tmp_section->section_name ) ) , tmp_field->key , tmp_field->str_value );
//						if ( tmp_field->key_type == PRSR_GIM_INT ) sprintf( dest , "%s    %s::%s = %d" , dest , Lexical.str_down( tmp_section->section_name, strlen( tmp_section->section_name ) ) , tmp_field->key , tmp_field->int_value );
						strcat( dest , "\n");
					}
				}
			}
			strcat( dest , "\n");
		}
		else if ( Lex == __LEX_C ) {
			for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
				sprintf( Tmp , PRSR_C_START_SECTION , tmp_section->section_name );
				sprintf( dest , "%s%s\n", dest , Tmp);
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field != NULL ; tmp_field = tmp_field->next_field ) {
						sprintf( dest , "%s        %-32s = %s" , dest , tmp_field->key , tmp_field->str_value );
//						if ( tmp_field->key_type == PRSR_GIM_INT ) sprintf( dest , "%s    %s = %d" , dest , tmp_field->key , tmp_field->int_value );
						strcat( dest , "\n");
					}
				}
				sprintf( dest , "%s%s\n\n" , dest , PRSR_C_END_SECTION );
			}
		}
	}
	strcat( dest , PRSR_END_FILE );
	strcat( dest , "\n");
	return dest;
}

_gim_flag	gim_prsr_obj::CopySection( gim_prsr_obj * from , const char * section_name ) {
	prsr_kvalue_section * tmp_section = from->SearchSection( section_name );
	prsr_kvalue_field	* tmp_field = NULL;
	if ( tmp_section == NULL ) return __GIM_NOT_EXIST;
	AddSection( tmp_section->section_name );
	if ( tmp_section->first_field != NULL ) {
		for( tmp_field = tmp_section->first_field ; tmp_field ; tmp_field = tmp_field->next_field ) {
			AddKey( strdup( tmp_section->section_name ) , strdup( tmp_field->key ) , strdup( tmp_field->str_value ) );
			
		}
	}
	return __GIM_OK;
}


_gim_flag	gim_prsr_obj::AddKeyComment( const char * section_name , const char * key , _gim_flag	position , const char * comment ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field;
	char * tmp_section_name;
	char * tmp_key_name;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	tmp_key_name     = strdup( Lexical.str_down( key          , strlen( key          ) ) );
	if ( comment == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::AddKeyComment" , "Comment must not be NULL. I cannot add this comment" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( strlen( comment ) > 10240 ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::AddKeyComment" , "Comment is too long. It will be skipped" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				if ( tmp_section->first_field ) {
					for( tmp_field = tmp_section->first_field ; tmp_field ; tmp_field = tmp_field->next_field ) {
						if ( ! strcmp( tmp_key_name , tmp_field->key  ) ) {
							tmp_field->comment_position = position;
							strcpy( tmp_field->comment , comment );
						}
					}
				}
			}
		}
	}
	return __GIM_NOT_EXIST;
}


_gim_flag	gim_prsr_obj::AddSectionComment( const char * section_name , _gim_flag position , char * comment ) {
	prsr_kvalue_section	* tmp_section = prsr_obj->first_section;
	prsr_kvalue_field	* tmp_field;
	char * tmp_section_name;
	char * tmp_key_name;
	tmp_section_name = strdup( Lexical.str_up  ( section_name , strlen( section_name ) ) );
	if ( comment == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_prsr_obj::AddKeyComment" , "Comment must not be NULL. I cannot add this comment" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( strlen( comment ) > 10240 ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::AddKeyComment" , "Comment is too long. It will be skipped" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( prsr_obj->first_section != NULL ) {
		for( ; tmp_section != NULL ; tmp_section = tmp_section->next_section ) {
			if( ! strcmp(  tmp_section_name , tmp_section->section_name ) ) {
				tmp_section->comment_position = position;
				strcpy( tmp_section->comment , comment );
			}
		}
	}
	return __GIM_NOT_EXIST;
}


_gim_flag	gim_prsr_obj::AddComment( const char * comment ) {
	if ( strlen( comment ) > 10240 ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::AddComment" , "Comment is too long. It will be skipped" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	strcpy( this->prsr_obj->comment , comment );
	return __GIM_OK;
}


_gim_flag   gim_prsr_obj::ParserIsUp( void ) {
	if ( prsr_obj == NULL )
		return __GIM_NO;
	return __GIM_YES;
}


_gim_flag   gim_prsr_obj::ThereIsASection( void ) {
	if ( ParserIsUp() == __GIM_YES ) {
		gim_error->set( "gim_prsr_obj::ThereIsASection" , "Yes, parser is UP." );
		if ( prsr_obj->first_section != NULL ) {
			gim_error->set( "gim_prsr_obj::ThereIsASection" , "And yes, there is at least a section." );
			return __GIM_YES;
		}
		else
			gim_error->set( "gim_prsr_obj::ThereIsASection" , "But no, there is no one section." );
	}
	else
		gim_error->set( "gim_prsr_obj::ThereIsASection" , "No, no parser in memory." );
	return __GIM_NO;
}



_gim_flag	gim_prsr_obj::ChangeKeyFlag( const char * section_name , const char * key , _gim_flag flag ) {
	static char tmp[64];
	__GIM_CLEAR( tmp , 64 , char );

	switch (flag) {
		case __GIM_YES : {
			sprintf( tmp , "YES" );
			break;
		}
		case __GIM_NO : {
			sprintf( tmp , "NO" );
			break;
		}
		case __GIM_TRUE : {
			sprintf( tmp , "TRUE" );
			break;
		}
		case __GIM_FALSE : {
			sprintf( tmp , "FALSE" );
			break;
		}
		case __LEX_A : {
			sprintf( tmp , "A" );
			break;
		}
		case __LEX_B : {
			sprintf( tmp , "B" );
			break;
		}
		case __GIM_ON : {
			sprintf( tmp , "ON" );
			break;
		}
		case __GIM_OFF : {
			sprintf( tmp , "OFF" );
			break;
		}
		case __GIM_MEM_LOCK : {
			sprintf( tmp , "LOCKED" );
			break;
		}
		case __GIM_MEM_UNLOCK : {
			sprintf( tmp , "UNLOCKED" );
			break;
		}
		case __GIM_LOG_TEXT : {
			sprintf( tmp , "HTML" );
			break;
		}
		case __GIM_LOG_HTML : {
			sprintf( tmp , "TEXT" );
			break;
		}
	}	
	return ChangeKey( section_name , key , tmp );
}



_gim_flag gim_prsr_obj::Exist( const char * filename ) {
	FILE *fp = fopen( filename , "rb" );
	if ( fp == NULL ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_prsr_obj::Exist" , "The conf file NOT exist" , __GIM_NOT_EXIST );
		return __GIM_NOT_EXIST;
	}
	gim_error->set( "gim_prsr_obj::Exist" , "The conf file exist" );
	fclose( fp );
	return __GIM_EXIST;
}
