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

//#include <gim/gim.h>
#include "bmake_main.h"

_gim_flag	    do_system = __GIM_YES;

gkmake_st		gkmake;
old_gkmake_st	gkmake_old;
_gim_Uint32		running_time;

gim_ascii_file_obj *	header;


int main(int argc, char *argv[]) {

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
	_gim_flag install = __GIM_NO;
	_gim_flag autotools = __GIM_NO;

//	_gim_handler *	gkmake_fd;

//	memset( cur_path			, 0, BUFF_DIM );
	memset( cur_gkmake_file		, 0, BUFF_DIM );
	memset( command				, 0, BUFF_DIM );
	memset( command_t1			, 0, BUFF_DIM );
//	memset( command_t2			, 0, BUFF_DIM );
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
	gkopt->scanopt( argc , argv , ":RN:A:P:I:S:D:F:X:M:H:B:t:T:C:coshidan" );

	if ( ( ! gkmake_exist( "./Makefile" ) ) && ( gkopt->search( 'a' ) == __GIM_NO ) ) {
		if ( ( gkmake_exist( "./autogen.sh" ) ) || ( gkmake_exist( "./configure" ) ) || ( gkmake_exist( "./Makefile.in" ) ) ) {
			puts( "  No Makefile found, but...\n" );
			if ( gkmake_exist( "./autogen.sh" ) )
			    puts( "  \"autogen.sh\" found" );
			if ( gkmake_exist( "./configure" ) )
			    puts( "  \"configure\" found" );
			if ( gkmake_exist( "./Makefile.in" ) )
			    puts( "  \"Makefile.in\" found" );
			puts( "\n  Try to run gkmake with \'-a\' option\n" );
		}
		else
		      puts( "  No Makefile found.\n  This one not seems a project directory\n" );
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
	
	_gim_prsr * gkconf = new _gim_prsr;
	header = new gim_ascii_file_obj;
	header->set_dimension( 20 * __GIM_KB ); 
	
	if ( gkconf->Read( cur_gkmake_file ) != __GIM_OK ) {
		gkconf->Up( cur_gkmake_file , "gKmake configuration file" );
		gkconf->SetLex( __LEX_A );
	}
	else {
		strcpy( gkmake.prj_name , gkconf->GetSectionName( 0 ) );
		_gim_Uint8 c = 0, count = gkconf->GetHowManyKey( gkmake.prj_name );
		for ( c = 0 ; c < count ; c++ ) {
			//STRINGS
			if ( lex->str_equal( "prefix" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				strcpy( gkmake.prefix , gkconf->GetKeySTR( gkmake.prj_name , "prefix" ) ); 
			if ( lex->str_equal( "author" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				strcpy( gkmake.author , gkconf->GetKeySTR( gkmake.prj_name , "author" ) ); 
			if ( lex->str_equal( "include_path" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				strcpy( gkmake.include_path , gkconf->GetKeySTR( gkmake.prj_name , "include_path" ) ); 
			if ( lex->str_equal( "documentation_path" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				strcpy( gkmake.documentation , gkconf->GetKeySTR( gkmake.prj_name , "documentation_path" ) ); 
			if ( lex->str_equal( "documentation_command" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				strcpy( gkmake.doc_command , gkconf->GetKeySTR( gkmake.prj_name , "documentation_command" ) ); 
			//FLAGS
			if ( lex->str_equal( "clean" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				gkmake.clean = gkconf->GetKeyFLAG( gkmake.prj_name , "clean" ); 
			if ( lex->str_equal( "use_sudo" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				gkmake.sudo = gkconf->GetKeyFLAG( gkmake.prj_name , "use_sudo" ); 
			if ( lex->str_equal( "project_header_generation" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				gkmake.header_file = gkconf->GetKeyFLAG( gkmake.prj_name , "project_header_generation" ); 
			if ( lex->str_equal( "header_file_name" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				strcpy( gkmake.header_file_name , gkconf->GetKeySTR( gkmake.prj_name , "header_file_name" ) );
			if ( lex->str_equal( "multi_threads_enable" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) { 
				gkmake.parallel = gkconf->GetKeyFLAG( gkmake.prj_name , "multi_threads_enable" ); 
				if ( gkmake.parallel == __GIM_YES ) {
					gkmake.num_core = gkconf->GetKeyINT( gkmake.prj_name , "number_of_threads" );
				}
			}
			//  INT VALUES
			if ( lex->str_equal( "total_build_number" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				gkmake.tot_build = gkconf->GetKeyINT( gkmake.prj_name , "total_build_number" ); 
			if ( lex->str_equal( "succesful_build_number" , gkconf->GetKeyName( gkmake.prj_name , c ) ) == __GIM_YES ) 
				gkmake.ok_build = gkconf->GetKeyINT( gkmake.prj_name , "succesful_build_number" ); 
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
					delete runT;
					delete gkopt;
					delete gim;
					exit( __GIM_ERROR );
				}
				strcpy( gkmake.prj_name , gkopt->getoption()->argument );
				gkconf->AddSection( gkmake.prj_name );
//				gkconf->AddSectionComment( gkmake.prj_name , PRSR_INLINE , "This is the Project name" );
				break;
			}
			case 'A' : {
				if ( !gkopt->search( 'N' ) || !gkopt->search( 'P' ) ) {
					puts( "  Errors on option:" );
					puts( "   If you want to set a new project you have to set at least also the Project name and the Prefix" );
					print_help();
					delete runT;
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
					delete runT;
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
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "use_sudo" )
						gkconf->AddKeyFlag( gkmake.prj_name , "use_sudo" , gkmake.sudo );
				}
				break;
			}
			case 'C' : {
				if ( lex->str_equal( "YES" , gkopt->getoption()->argument ) == __GIM_YES )
					gkmake.clean = __GIM_YES;
				else 
					gkmake.clean = __GIM_NO;
				
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "clean" ) 
						gkconf->AddKeyFlag( gkmake.prj_name , "clean" , gkmake.clean );
					IF_EXIST_KEY( gkconf , gkmake.prj_name , "clean" )
						gkconf->ChangeKeyFlag( gkmake.prj_name , "clean" , gkmake.clean );
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
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "multi_threads_enable" ) {
						gkconf->AddKeyFlag( gkmake.prj_name , "multi_threads_enable" , gkmake.parallel );
					}
				}
				gkmake.num_core = atoi( gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "number_of_threads" ) {
						gkconf->AddKey( gkmake.prj_name , "number_of_threads" , gkmake.num_core );
					}
				}
				break;
			}
			case 'M' : {
				gkmake.tot_build = atoi( gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "total_build_number" ) 
						gkconf->AddKey( gkmake.prj_name , "total_build_number" , gkmake.tot_build );
				}
				break;
			}
			case 'B' : {
				gkmake.ok_build = atoi( gkopt->getoption()->argument );
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
					IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "succesful_build_number" ) 
						gkconf->AddKey( gkmake.prj_name , "succesful_build_number" , gkmake.ok_build );
				}
				break;
			}
			case 't' : {
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
				break;
			}
			case 'd' : {
				makedoc = __GIM_YES;
				break;
			}
			case 'h' : {
				print_help();
				break;
			}
			case 'a' : {
				puts( "  SERCHING FOR AUTOTOOLS...\n");
				if ( gkmake_exist( "./autogen.sh" ) )
					puts( "  \"autogen.sh\" found" );
				if ( gkmake_exist( "./configure" ) )
					puts( "  \"configure\" found" );
				if ( gkmake_exist( "./Makefile.in" ) )
					puts( "  \"Makefile.in\" found" );

				if ( ( gkmake_exist( "./autogen.sh" ) ) || ( gkmake_exist( "./configure" ) ) || ( gkmake_exist( "./Makefile.in" ) ) ) {
					puts( "  \nOK, I TRY TO CONFIGURE THIS PROJECT...\n" );
					system( "./autogen.sh" );
				}
				else 
					puts( "  \nINCOMPLETE AUTOTOOLS" );
					gkconf->Down();
					delete header;
					delete runT;
					delete gkopt;
					delete gkconf;
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
		 ( gkopt->search( 'T' ) == __GIM_YES ) || \
	     ( gkopt->search( 'a' ) == __GIM_YES ) ) ) {

		do_system = __GIM_NO;

/*		if ( ( ( gkopt->search( 'N' ) == __GIM_YES ) || ( gkopt->search( 'A' ) == __GIM_YES ) || ( gkopt->search( 'P' ) == __GIM_YES ) ) && \ 
			 ( ( !gkopt->search( 'M' ) || !gkopt->search( 'B' ) ) ) ) {
			if ( !gkopt->search( 'M' ) ) { 
				puts( "  You haven't specified information about the total build number. Set it to 0 per default" );
				gkmake.tot_build = 0;
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
						IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "total_build_number" ) 
							gkconf->AddKey( gkmake.prj_name , "total_build_number" , gkmake.tot_build );
				}
		    }
			if ( !gkopt->search( 'B' ) ) { 
				puts( "  You haven't specified information about the succesful build number. Set it to 0 per default" );
				gkmake.ok_build = 0;
				IF_EXIST_SECTION( gkconf , gkmake.prj_name ) {
						IF_NOT_EXIST_KEY( gkconf , gkmake.prj_name , "succesful_build_number" ) 
							gkconf->AddKey( gkmake.prj_name , "succesful_build_number" , gkmake.ok_build );
				}
			}
			puts("");
		}*/
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
		IF_EXIST_KEY( gkconf , gkmake.prj_name , "succesful_build_number" )
			printf( "  BUILDING (#%d) THE PROJECT ( %s )...\n\n" , ++gkmake.tot_build , command );
		else
			printf( "  BUILDING THE PROJECT ( %s )...\n\n" , command );
		if ( ! system( command ) ) {
			IF_EXIST_KEY( gkconf , gkmake.prj_name , "succesful_build_number" ) 
				gkconf->ChangeKey( gkmake.prj_name , "succesful_build_number" , ++gkmake.ok_build );
			printf( "\n  BUILT SUCCESFULLY ( %d )...\n" , gkmake.ok_build );
		}
		else
			do_system = __GIM_NO;
	}
	IF_EXIST_KEY( gkconf , gkmake.prj_name , "total_build_number" )
		gkconf->ChangeKey( gkmake.prj_name , "total_build_number" , gkmake.tot_build );
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
	delete runT;
	delete gkopt;
	delete gkconf;
	delete gim;

	exit( feedback );

}



















//}

	

	
/*	if ( gkmake_exist ( cur_old_bmake_file ) ) {
		gim->error->set( BMAKE_MESSAGE , "main()" , "An old counter file exist... updating" , __GIM_OK );
		convert_from_old( cur_old_bmake_file );
	}
*/	
/*	if ( bmake_exist ( bmake_makefile ) ) {
		bmake_fd = fopen ( cur_bmake_file, "rb" );
		if ( ! bmake_fd ) {
			gim->error->set( BMAKE_MESSAGE , "main()" , "A new project will be created" , __GIM_OK );
			new_prj( cur_bmake_file );
			time( &tp );
			strftime( timestamp , 256 , "%A %d-%m-%Y %H:%M:%S" , localtime ( &tp ) );
		}
		else {
			fread ( &bmake , sizeof ( bmake_st ), 1, bmake_fd );
			fclose ( bmake_fd );
		}
		time( &tp );
		strftime( timestamp , 256 , "%A %d-%m-%Y %H:%M:%S" , localtime ( &tp ) );
		sprintf ( cur_bmake_file, "%s/%s", cur_path, BMAKE_INCLUDE );
		sprintf ( include_bmake_file, "%s/%s%s", cur_path , str_down( bmake.prefix , strlen( bmake.prefix) ) , BMAKE_INCLUDE );
		char tmp2[256];
		sprintf( tmp2 , "%s%s" , str_down( bmake.prefix , strlen( bmake.prefix) ) , BMAKE_INCLUDE );
		bmake_fd = fopen ( include_bmake_file, "wb" );
		if ( bmake_fd ) {
			make_header( bmake_fd , tmp2 );
			fclose  ( bmake_fd );
		}
		else {
			puts("bMake : I cannot write the bmake.h");
			gim->error->set( GIM_ERROR_CRITICAL , "main" , "I cannot write the bmake.h" , __GIM_NOT_OK );
		}
		if ( strcmp( "none" , bmake.include_path ) ) {
			memset ( include_bmake_file, 0, BUFF_DIM );
			sprintf( include_bmake_file, "%s%s%s", bmake.include_path , str_down( bmake.prefix , strlen( bmake.prefix) ) , BMAKE_INCLUDE );
			char tmp2[256];
			sprintf( tmp2 , "%s%s" , str_down( bmake.prefix , strlen( bmake.prefix) ) , BMAKE_INCLUDE );
			bmake_fd = fopen ( include_bmake_file , "wb" );
			if ( bmake_fd ) {
				make_header( bmake_fd , tmp2 );
				fclose  ( bmake_fd );
			}
			else {
				puts("bMake : I cannot write the bmake.h");
				gim->error->set( GIM_ERROR_CRITICAL , "main" , "I cannot write the bmake.h" , __GIM_NOT_OK );
			}
		}
		strcpy ( command, BMAKE_COMMAND );
		if ( gim->identity->n_proc() > 1 ) {
			char tmp1[256];
			sprintf( tmp1 , " -j%d" , gim->identity->n_proc() );
			strcat( command , tmp1 );
			sprintf( tmp1 , "#%d -Multi process make activated" , gim->identity->n_proc() );
			gim->error->set( BMAKE_MESSAGE , "main" , tmp1 , __GIM_OK );
		}
		else {
			char tmp1[256];
			sprintf( tmp1 , "#%d - No SMP system detected" , gim->identity->n_proc() );
			gim->error->set( BMAKE_MESSAGE , "main" , tmp1 , __GIM_OK );
		}
		if ( argc > 1) {
			for ( ; ciclo <= argc-1 ; ciclo ++ ) {
				sprintf ( command_t1, "%s %s", command, argv[ciclo] );
				strcpy ( command, command_t1 );
			}
		}
		puts("gKmake v0.3 made by gKript.org");
		if ( gim->identity->n_proc() > 1 ) 
			printf("Compiling in parallel : %d processes\n\n" , gim->identity->n_proc() );
		feedback = system ( command );
		if ( ! feedback ) {
			bmake.make++;
			bmake.make_ok++;
		}
		else {
			bmake.make++;
		}
		sprintf ( cur_bmake_file, "%s/%s", cur_path, BMAKE_FILE_COUNTER );
		bmake_fd = fopen ( cur_bmake_file, "wb" );
		if ( bmake_fd ) {
			fwrite ( &bmake, sizeof ( bmake_st ), 1, bmake_fd );
			fclose ( bmake_fd );
		}
		else {
			puts("bMake : I cannot update the version bumber");
			gim->error->set( GIM_ERROR_CRITICAL , "main" , "I cannot update the version bumber" , __GIM_NOT_OK );
		}
	}
	else {
		feedback = system ( BMAKE_COMMAND );
		puts("bMake : I cannot update the version bumber");
		gim->error->set( GIM_ERROR_CRITICAL , "main" , "I cannot update the version bumber" , __GIM_NOT_OK );
	}
*/
