/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cc
 * Copyright (C) 2017 asyntote <asyntote@viprox>
 * 
 * gKmake is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gKmake is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "bmake_main.h"

_gim_flag				do_system = __GIM_YES;
_gim_flag				install = __GIM_NO;
_gim_flag				autotools = __GIM_NO;
_gim_flag				writec = __GIM_YES;

gkmake_st				gkmake;
old_gkmake_st			gkmake_old;
_gim_Uint32				running_time;

gim_ascii_file_obj *	header;

int main( int argc , char *argv[] ) {
	int ciclo = 1, feedback = 0 , makedoc = __GIM_NO;
	time_t	tp;
	char cur_path[BUFF_DIM];
	char cur_gkmake_file[BUFF_DIM];
	char gkmake_makefile[BUFF_DIM];
	char cur_old_gkmake_file[BUFF_DIM];
	char command_t1[BUFF_DIM];
	char command[BUFF_DIM];
	char include_gkmake_file[BUFF_DIM];
	char thread_command[16];

	memset( cur_gkmake_file		, 0, BUFF_DIM );
	memset( command				, 0, BUFF_DIM );
	memset( command_t1			, 0, BUFF_DIM );
	memset( gkmake_makefile		, 0, BUFF_DIM );
	memset( include_gkmake_file , 0, BUFF_DIM );

	sprintf ( cur_gkmake_file, "%s/%s", get_current_dir_name(), GKMAKE_FILE_COUNTER );
	sprintf ( gkmake_makefile, "%s/%s", get_current_dir_name(), MAKEFILE );
	sprintf ( cur_path, "%s" , get_current_dir_name() );
	
	if ( gim_check_version(2,5,0) != __GIM_OK ) {
		printf( "%s\n\n" , gim_version() );
		printf( "Gim is not updated at the required version.\n" );
		printf( "For %s is necessary Gim >= 2.5-0\n" , argv[0] );
		exit( -1 );
	}

	hand_shake();

	gim_set_application_name( "gKmake" );
	gim_obj * gim = new gim_obj;
	_gim_time * runT = new _gim_time;
	prsr_lexical_class * lex = new prsr_lexical_class;
	runT->start();
	
	gim->error->set( "main" , "gkmake start here" );

	gim_getopt_obj * gkopt = new gim_getopt_obj;
	gkopt->scanopt( argc , argv , ":RN:A:P:I:S:D:F:X:M:H:B:T:C:E:t:coeshidanz" );

	if ( ( ! gkmake_exist( "./Makefile" ) ) && ( gkopt->search( 'a' ) == __GIM_NO ) ) {
		if ( ( ( gkmake_exist( "./autogen.sh" ) ) || ( gkmake_exist( "./configure" ) ) ) && ( gkmake_exist( "./Makefile.in" ) ) ) {
			puts( "  No Makefile found, but...\n" );
			if ( gkmake_exist( "./autogen.sh" ) )
				puts( "  \"autogen.sh\" found" );
			else
				puts( "  \"autogen.sh\" NOT found" );					
			if ( gkmake_exist( "./configure" ) )
				puts( "  \"configure\" found" );
			else
				puts( "  \"configure\" NOT found" );
			if ( gkmake_exist( "./Makefile.in" ) )
				puts( "  \"Makefile.in\" found" );
			else
				puts( "  \"Makefile.in\" NOT found" );
			puts( "\n  Try to run gkmake with \'-a\' option\n" );
		}
		else
		    puts( "  No Makefile found.\n  This one not seems a project directory\n" );
		delete runT;
		delete lex;
		delete gkopt;
		delete gim;
		exit( -1 );
	}
	
	if ( gkopt->errors_present() == __GIM_YES ) {
		puts( "  Errors on option:" );
		GIM_GETOPT_ERROR( gkopt ) {
			if ( gkopt->geterror()->status == __GIM_OPT_NO_ARG )
				printf( "   A mandatory argument for otion \033[1m-%c\033[0m is missing\n" , gkopt->geterror()->option  );
			if ( gkopt->geterror()->status == __GIM_OPT_UNKNOWN )
				printf( "   Option \033[1m-%c\033[0m is unknown\n" , gkopt->geterror()->option );
			if ( gkopt->geterror()->status == __GIM_OPT_NOT_OK )
				printf( "   Some error in \033[1m-%c\033[0m option\n" , gkopt->geterror()->option );
		}
		puts("");
		print_help();
		delete runT;
		delete lex;
		delete gkopt;
		delete gim;
		exit( __GIM_ERROR );
	}

	if ( ( gkopt->opt_members() ) && \
	   ( ( gkopt->search( 'N' ) == __GIM_YES ) || \
	     ( gkopt->search( 'A' ) == __GIM_YES ) || \
	     ( gkopt->search( 'P' ) == __GIM_YES ) ) ) {
		remove( cur_gkmake_file );
	}
	else {
		if ( ! gkmake_exist( ".gkmake_conf" ) ) {
			writec = __GIM_NO;
		}
	}
	
	_gim_prsr * gkconf = new _gim_prsr;
	header = new gim_ascii_file_obj;
	header->set_dimension( 20 * __GIM_KB ); 
	
	if ( gkconf->Read( cur_gkmake_file ) != __GIM_OK ) {
//		puts(" NON Letto !");
		gkconf->Up( cur_gkmake_file , "gKmake configuration file" );
		gkconf->SetLex( __LEX_C );
		gkconf->AddSection( gim->identity->login() );
		gkconf->AddKey( gim->identity->login() , "coder" , gim->identity->login() );
		gkconf->AddKey( gim->identity->login() , "host" , gim->identity->node() );
		gkconf->AddKey( gim->identity->login() , "architecture" , gim->identity->arch() );
		gkconf->AddKey( gim->identity->login() , "gim_rel" , gim_version_mini() );
	}
	else {
//		printf( "%s\n" , gkmake.prj_name );
		for ( _gim_Uint8 s = 0 ; s < gkconf->GetHowManySection() ; s++ ) {
			char sectname[64];
			__GIM_CLEAR( sectname , 64 , char );
//			strcpy( gkmake.prj_name , gkconf->GetSectionName( s ) );
			strcpy( sectname , gkconf->GetSectionName( s ) );
//			printf( "working on section: %s\n" , sectname );

			_gim_Uint8 c = 0, count = gkconf->GetHowManyKey( sectname );
			for ( c = 0 ; c < count ; c++ ) {
				char	tmpkeyname[64];
				__GIM_CLEAR( tmpkeyname , 64 , char );
				strcpy( tmpkeyname , gkconf->GetKeyName( sectname , c ) );
				
/*STRINGS */	if ( lex->str_equal( "prefix" , tmpkeyname ) == __GIM_YES ) {
					strcpy( gkmake.prefix , gkconf->GetKeySTR( sectname , "prefix" ) );
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.prefix );
				} 
				else if ( lex->str_equal( "author" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.author , gkconf->GetKeySTR( sectname , "author" ) );
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.author );
				} 
				else if ( lex->str_equal( "include_path" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.include_path , gkconf->GetKeySTR( sectname , "include_path" ) );
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.include_path );
				} 
				else if ( lex->str_equal( "documentation_path" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.documentation , gkconf->GetKeySTR( sectname , "documentation_path" ) );
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.documentation );
				} 
				else if ( lex->str_equal( "documentation_command" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.doc_command , gkconf->GetKeySTR( sectname , "documentation_command" ) );
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.doc_command );
				} 
				else if ( lex->str_equal( "editor_command" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.editor_command , gkconf->GetKeySTR( sectname , "editor_command" ) );
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.editor_command );
				}
				else if ( lex->str_equal( "header_file_name" , tmpkeyname ) == __GIM_YES ) { 
					strcpy( gkmake.header_file_name , gkconf->GetKeySTR( sectname , "header_file_name" ) );
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %s" , sectname , tmpkeyname , gkmake.header_file_name);
				}
/*FLAGS */		else if ( lex->str_equal( "clean" , tmpkeyname ) == __GIM_YES ) {
					gkmake.clean = gkconf->GetKeyFLAG( sectname , "clean" ); 
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.clean );
				}
				else if ( lex->str_equal( "use_sudo" , tmpkeyname ) == __GIM_YES ) { 
					gkmake.sudo = gkconf->GetKeyFLAG( sectname , "use_sudo" ); 
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.sudo );
				}
				else if ( lex->str_equal( "project_header_generation" , tmpkeyname ) == __GIM_YES ) { 
					gkmake.header_file = gkconf->GetKeyFLAG( sectname , "project_header_generation" );
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.header_file );
				} 
				else if ( lex->str_equal( "multi_threads_enable" , tmpkeyname ) == __GIM_YES ) { 
					gkmake.parallel = gkconf->GetKeyFLAG( sectname , "multi_threads_enable" ); 
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.parallel );
					if ( gkmake.parallel == __GIM_YES ) {
						IF_EXIST_KEY( gkconf , sectname , "number_of_threads" ) {
							gkmake.num_core = gkconf->GetKeyINT( sectname , "number_of_threads" );
							gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key number_of_threads - Value %d" , sectname , gkmake.num_core );
							if ( gkmake.num_core == 0 )
								gkmake.num_core = gim->identity->n_proc() + 1;
						}
						else {
							gkmake.num_core = gim->identity->n_proc() + 1;
							gkconf->AddKey( gim->identity->login() , "number_of_threads" , gkmake.num_core );
						}
					}
				}
/*INT VALUES */	else if ( lex->str_equal( "total_build_number" , tmpkeyname ) == __GIM_YES ) {
					gkmake.tot_build = gkconf->GetKeyINT( sectname , "total_build_number" );
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.tot_build );
				} 
				else if ( lex->str_equal( "succesful_build_number" , tmpkeyname ) == __GIM_YES ) { 
					gkmake.ok_build = gkconf->GetKeyINT( sectname , "succesful_build_number" ); 
					gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "  Section %s - Key %s - Value %d" , sectname , tmpkeyname , gkmake.ok_build );
				}
//				printf( " Section %s - Key %s\n" , sectname , tmpkeyname );
			}
		}
	}
	gkopt->rewind();
	
	GIM_GETOPT_OK( gkopt ) {
		switch ( gkopt->getoption()->option ) {
			case 'N' : {
				if ( !gkopt->search( 'A' )  || !gkopt->search( 'P' ) ) {
					puts( "  Errors on option:" );
					puts( "   If you want to set a new project you have to set at least also the Author and the Prefix\n" );
					print_help();
					gkconf->Down();
					delete header;
					delete gkconf;
					delete runT;
					delete lex;
					delete gkopt;
					delete gim;
					exit( __GIM_ERROR );
				}
				strcpy( gkmake.prj_name , gkopt->getoption()->argument );
				gkconf->AddSection( gkmake.prj_name );
//				gkconf->AddSectionComment( gkmake.prj_name , PRSR_INLINE , "This is the Project name" );
				gkconf->AddKey( gkmake.prj_name , "project_name" , gkmake.prj_name );
				break;
			}
			case 'A' : {
				if ( !gkopt->search( 'N' ) || !gkopt->search( 'P' ) ) {
					puts( "  Errors on option:" );
					puts( "   If you want to set a new project you have to set at least also the Project name and the Prefix" );
					print_help();
					gkconf->Down();
					delete header;
					delete gkconf;
					delete runT;
					delete lex;
					delete gkopt;
					delete gim;
					exit( __GIM_ERROR );
				}
				strcpy( gkmake.author , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "author" )
						gkconf->AddKey( gkmake.prj_name , "author" , gkmake.author );
				}
				break;
			}
			case 'P' : {
				if ( !gkopt->search( 'N' ) || !gkopt->search( 'A' ) ) {
					puts( "  Errors on option:" );
					puts( "   If you want to set a new project you have to set at least also the Project name and the Author" );
					print_help();
					gkconf->Down();
					delete header;
					delete gkconf;
					delete runT;
					delete lex;
					delete gkopt;
					delete gim;
					exit( __GIM_ERROR );
				}
				strcpy( gkmake.prefix , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "prefix" ) 
						gkconf->AddKey( gkmake.prj_name , "prefix" , gkmake.prefix );
				}
				break;
			}
			case 'I' : {
				strcpy( gkmake.include_path , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "include_path" )
						gkconf->AddKey( gkmake.prj_name , "include_path" , gkmake.include_path );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "include_path" )
						gkconf->ChangeKey( gkmake.prj_name , "include_path" , gkmake.include_path );
				}
				break;
			}
			case 'D' : {
				strcpy( gkmake.documentation , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_path" )
						gkconf->AddKey( gkmake.prj_name , "documentation_path" , gkmake.documentation );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_path" )
						gkconf->ChangeKey( gkmake.prj_name , "documentation_path" , gkmake.documentation );
				}
				break;
			}
			case 'E' : {
				strcpy( gkmake.editor_command , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "editor_command" )
						gkconf->AddKey( gim->identity->login() , "editor_command" , gkmake.editor_command );
					IF_EXIST_KEY( gkconf , gim->identity->login() , "editor_command" )
						gkconf->ChangeKey( gim->identity->login() , "editor_command" , gkmake.editor_command );
				}
				break;
			}
			case 'X' : {
				strcpy( gkmake.doc_command , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_command" )
						gkconf->AddKey( gkmake.prj_name , "documentation_command" , gkmake.doc_command );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_command" )
						gkconf->ChangeKey( gkmake.prj_name , "documentation_command" , gkmake.doc_command );
				}
				break;
			}
			case 'S' : {
				if ( lex->str_equal( "YES" , gkopt->getoption()->argument ) == __GIM_YES )
					gkmake.sudo = __GIM_YES;
				else 
					gkmake.sudo = __GIM_NO;
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "use_sudo" )
						gkconf->AddKeyFlag( gim->identity->login() , "use_sudo" , gkmake.sudo );
				}
				break;
			}
			case 'C' : {
				if ( lex->str_equal( "YES" , gkopt->getoption()->argument ) == __GIM_YES )
					gkmake.clean = __GIM_YES;
				else 
					gkmake.clean = __GIM_NO;
				
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "clean" ) 
						gkconf->AddKeyFlag( gim->identity->login() , "clean" , gkmake.clean );
					IF_EXIST_KEY( gkconf , gim->identity->login() , "clean" )
						gkconf->ChangeKeyFlag( gim->identity->login() , "clean" , gkmake.clean );
				}
				break;
			}
			case 'H' : {
				if ( lex->str_equal( "YES" , gkopt->getoption()->argument ) == __GIM_YES )
					gkmake.header_file = __GIM_YES;
				else 
					gkmake.header_file = __GIM_NO;
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "project_header_generation" ) 
						gkconf->AddKeyFlag( gkmake.prj_name , "project_header_generation" , gkmake.header_file );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "project_header_generation" )
						gkconf->ChangeKeyFlag( gkmake.prj_name , "project_header_generation" , gkmake.header_file );
				}
				break; 
			}
			case 'F' : {
				strcpy( gkmake.header_file_name , gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "header_file_name" )
						gkconf->AddKey( gkmake.prj_name , "header_file_name" , gkmake.header_file_name );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "header_file_name" )
						gkconf->ChangeKey( gkmake.prj_name , "header_file_name" , gkmake.header_file_name );
				}
				break;
			}
			case 'T' : {
				gkmake.parallel = __GIM_YES;
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "multi_threads_enable" ) {
						gkconf->AddKeyFlag( gim->identity->login() , "multi_threads_enable" , gkmake.parallel );
					}
				}
				gkmake.num_core = atoi( gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "number_of_threads" ) {
						gkconf->AddKey( gim->identity->login() , "number_of_threads" , gkmake.num_core );
					}
				}
				break;
			}
			case 'M' : {
				gkmake.tot_build = atoi( gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "total_build_number" ) 
						gkconf->AddKey( gim->identity->login() , "total_build_number" , gkmake.tot_build );
				}
				break;
			}
			case 'B' : {
				gkmake.ok_build = atoi( gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gim->identity->login() ) {
					IF_NOT_EXIST_KEY( gkconf , gim->identity->login() , "succesful_build_number" ) 
						gkconf->AddKey( gim->identity->login() , "succesful_build_number" , gkmake.ok_build );
				}
				break;
			}
			case 't' : {
				if( ! strncmp( gkopt->getoption()->argument , "auto" , 4 ) )
					gkmake.num_core = gim->identity->n_proc();
				else	 
					gkmake.num_core = atoi( gkopt->getoption()->argument );
				sprintf( thread_command , " -j%d" , gkmake.num_core );
				gkmake.parallel = __GIM_YES;
				printf( "  MULTI THREADS build activated (%d threads)...\n" , gkmake.num_core );
				break;
			}
			case 'c' : {
				puts( "  CLEANING NOW...\n");
				if ( do_system == __GIM_YES )
					system( "make clean" );
				puts("");
				break;
			}
			case 'n' : {
				puts( "  DISTCLEAN FOR THE PROJECT NOW...\n");
				if ( do_system == __GIM_YES )
					system( "make distclean" );
				puts("");
				do_system = __GIM_NO;
				writec = __GIM_NO;
				break;
			}
			case 'd' : {
				makedoc = __GIM_YES;
				break;
			}
			case 'h' : {
				do_system = __GIM_NO;
				writec = __GIM_NO;
				print_help();
				break;
			}
			case 'a' : {
				writec = __GIM_NO;
				puts( "  SERCHING FOR AUTOTOOLS...\n");
				if ( gkmake_exist( "./autogen.sh" ) )
					puts( "  \"autogen.sh\" found" );
				else
					puts( "  \"autogen.sh\" NOT found" );					
				if ( gkmake_exist( "./configure" ) )
					puts( "  \"configure\" found" );
				else
					puts( "  \"configure\" NOT found" );
				if ( gkmake_exist( "./Makefile.in" ) )
					puts( "  \"Makefile.in\" found" );
				else
					puts( "  \"Makefile.in\" NOT found" );

				if ( ( ( gkmake_exist( "./autogen.sh" ) ) || ( gkmake_exist( "./configure" ) ) ) && ( gkmake_exist( "./Makefile.in" ) ) ) {
					puts( "  \nOK, I TRY TO CONFIGURE THIS PROJECT...\n" );
					if ( gkmake_exist( "./autogen.sh" ) )
						system( "./autogen.sh" );
					else if ( gkmake_exist( "./configure" ) )
						system( "./configure" );
					
				}
				else 
					puts( "\n  INCOMPLETE AUTOTOOLS" );
					gkconf->Down();
					delete header;
					delete gkconf;
					delete runT;
					delete lex;
					delete gkopt;
					delete gim;
					exit( 0 );
			}			
			case 'z' : {
				writec = __GIM_NO;
				puts( "\n  BUILD DISTRIBUITION PACKAGE..." );
				if ( ! system( "make" ) ) {
					system( "make dist" );
				}
				if ( runT->get() > 2 ) {
					printf( "\n  GKMAKE ran for %d seconds\n" , runT->get() );
					printf(   "  GKMAKE built by %s@%s on %s\n" , gim->identity->login() , gim->identity->node() , gkmake.last_build );
				}
				puts("");
				gkconf->Down();
				delete header;
				delete gkconf;
				delete runT;
				delete lex;
				delete gkopt;
				delete gim;
				exit( 0 );
			}
			case 'o' : {
				if ( gkmake_exist( ".gkmake_conf" ) ) {
					puts( "\n  PRINT CONFIGURATION FILE...\n" );
					puts( gkconf->WriteOnBuffer() );
					puts("");
				}
				else {
					puts("\n  ERROR: configuration file not found. Project not yet configured");
				}
				gkconf->Down();
				delete header;
				delete gkconf;
				delete runT;
				delete lex;
				delete gkopt;
				delete gim;
				exit( 0 );
			}
			case 'e' : {
				char mess[64];
				writec = __GIM_NO;
				do_system = __GIM_NO;
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "editor_command" ) {
						puts( "\n  EDIT CONFIGURATION FILE...\n" );
						sprintf( mess, "%s .gkmake_conf" , gkmake.editor_command ); 
						system( mess );
					}
					else
						puts("\n  ERROR: No editor command configured");
				}
				gkconf->Down();
				delete header;
				delete gkconf;
				delete runT;
				delete lex;
				delete gkopt;
				delete gim;
				exit( 0 );
			}
			case 'i' : {
				install = __GIM_YES;
				if ( gkmake.sudo == __GIM_YES ) 
					sprintf( command_t1 , "sudo make install" );
				else
					sprintf( command_t1 , "make install" );
				break;
			}
		}		
	}

	if ( ( gkopt->opt_members() ) && \
	   ( ( gkopt->search( 'N' ) == __GIM_YES ) || \
	     ( gkopt->search( 'A' ) == __GIM_YES ) || \
	     ( gkopt->search( 'P' ) == __GIM_YES ) || \
	     ( gkopt->search( 'I' ) == __GIM_YES ) || \
	     ( gkopt->search( 'D' ) == __GIM_YES ) || \
	     ( gkopt->search( 'S' ) == __GIM_YES ) || \
	     ( gkopt->search( 'C' ) == __GIM_YES ) || \
		 ( gkopt->search( 'H' ) == __GIM_YES ) || \
		 ( gkopt->search( 'F' ) == __GIM_YES ) || \
		 ( gkopt->search( 'E' ) == __GIM_YES ) || \
		 ( gkopt->search( 'T' ) == __GIM_YES ) || \
	     ( gkopt->search( 'a' ) == __GIM_YES ) ) ) {

		do_system = __GIM_NO;

	if ( writec == __GIM_YES )
		gkconf->Write();
		puts( "  New configuration file wrote\n" );
	}

	time( &tp );
	strftime( gkmake.last_build , 256 , "%A %d-%m-%Y %H:%M:%S" , localtime ( &tp ) );


	_gim_flag   stop = __GIM_YES;
	
	if ( do_system == __GIM_YES ) {
		if ( gkmake.clean == __GIM_YES ) {
			puts( "  CLEANING NOW...\n");
			system( "make clean" );
			puts("");
		}
	}
	
	sprintf( command , "%s" , GKMAKE_COMMAND );

	if ( gkmake.parallel == __GIM_YES ) {
		char tmp[16];
		sprintf( tmp , " -j%d" , gkmake.num_core );
		strcat( command , tmp );
	}

	if ( do_system == __GIM_YES ) {
		if ( writec == __GIM_YES ) {
			IF_EXIST_KEY( gkconf , gkmake.prj_name , "succesful_build_number" )
				printf( "  BUILDING (#%d) THE PROJECT ( %s )...\n\n" , ++gkmake.tot_build , command );
			else
				printf( "  BUILDING THE PROJECT ( %s )...\n\n" , command ); 
		}
		else 
			printf( "  BUILDING THE PROJECT ( %s )...\n\n" , command );
		
		if ( ! system( command ) ) {
			IF_EXIST_KEY( gkconf , gkmake.prj_name , "succesful_build_number" ) { 
				gkconf->ChangeKey( gkmake.prj_name , "succesful_build_number" , ++gkmake.ok_build );
				gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "    Upgrading Succesfull Build to %d" , gkmake.ok_build );
			}
			else 
				gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "    Succesfull Build NOT FOUND!" );
			if ( writec == __GIM_YES )
				printf( "\n  BUILT SUCCESFULLY ( %d )\n" , gkmake.ok_build );
			else
				printf( "\n  BUILT SUCCESFULLY\n" );
		}
		else
			do_system = __GIM_NO;
	}
	IF_EXIST_KEY( gkconf , gkmake.prj_name , "total_build_number" ) {
		gkconf->ChangeKey( gkmake.prj_name , "total_build_number" , gkmake.tot_build );
		gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "    Upgrading Total Build to %d" , gkmake.tot_build );
	}
	else 
		gim->error->Set( GIM_ERROR_PRG_MESSAGE , "gkmain::main.cc::main()" , "    Total Build NOT FOUND!" );
	
	if ( writec == __GIM_YES )
		gkconf->Write();
	
	if ( do_system == __GIM_YES ) {
		if ( install == __GIM_YES ) {
			printf( "\n  INSTALLING THE PROJECT  ( %s )...\n\n" , command_t1 );
		if ( system( command_t1 ) ) 
			printf( "\n  ERROR INSTALLING THE PROJECT...\n\n" );
		}
	}


	if ( do_system == __GIM_YES ) {
		if ( makedoc == __GIM_YES ) {
			IF_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_path" ) { 
				IF_EXIST_KEY( gkconf , gkmake.prj_name , "documentation_command" ) {
					puts( "\n  GENERATING DOCUMENTATION...\n\n" );
					if ( chdir( gkconf->GetKeySTR( gkmake.prj_name , "documentation_path" ) ) ) {
						puts( "\n  ERROR : Some error with the Documentation path...\n\n" );
					}
					else {
						system( gkconf->GetKeySTR( gkmake.prj_name , "documentation_command" ) );
						chdir( cur_path );
					}
				}
			}
		}
	}

	if ( ( gkmake.header_file == __GIM_YES ) && ( do_system == __GIM_YES ) ) {
		_gim_handler * headf;
		if ( chdir( gkconf->GetKeySTR( gkmake.prj_name , "include_path" ) ) ) {
			puts( "\n  ERROR : Some error with the include path...\n\n" );
		}
		else {
			printf( "\n  CREATING HEADER FILE ( %s%s )...\n\n" , gkmake.include_path , gkmake.header_file_name );
			make_header();
			headf = gim->file_manager->open( gkmake.header_file_name , __GIM_FILE_POINTER , __GIM_WRITE );
			fputs( header->get_buffer() , headf->fp );
			gim->file_manager->close( headf );
			chdir( cur_path );
		}
	}

	
	if ( runT->get() > 2 ) {
		printf( "\n  GKMAKE ran for %d seconds\n" , runT->get() );
		printf(   "  GKMAKE built by %s@%s on %s\n" , gim->identity->login() , gim->identity->node() , gkmake.last_build );
	}
	puts("");

	gkconf->Down();
	delete header;
	delete gkconf;
	delete runT;
	delete lex;
	delete gkopt;
	delete gim;

	exit( feedback );

}



