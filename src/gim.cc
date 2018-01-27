//
// gim.cc
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

#include "../include/gim.h"

gim_obj	* internal_gim;

gim_prsr_obj	* gim_conf;
gim_prsr_obj	* gim_compatibility;
gim_prsr_obj	* gim_alias;
gim_prsr_obj	* gim_environ;

/* ----------------------/
 *  gim_base_obj : Begin
 * ---------------------*/
 
void gim_obj::gim_init( void ) {
	set_state( __GIM_TH_INIT );

	error			= new gim_error_obj;
	gim_error		= error;

	memory			= new gim_memory_obj;
	gim_memory		= memory;

	gim_log			= new gim_log_obj;

	conf			= new gim_prsr_obj;
	gim_conf		= conf;

	compatibility		= new gim_prsr_obj;
	gim_compatibility	= compatibility;

	alias			= new gim_prsr_obj;
	gim_alias		= alias;

	environ			= new gim_prsr_obj;
	gim_environ		= environ;
	
	identity		= new gim_identity_obj;
	gim_identity	= identity;

	environment		= new gim_conf_data;
	env_data		= environment;

	home			= new gim_home_obj;
	gim_home		= home;
	
	internal_gim	= this;

	memory->up();
	gim_log->start();
	home->up();

	interface		= new gim_interface_obj;
	gim_interface	= interface;
	
	file_manager		= new gim_file_manager_obj;
	gim_file_manager	= file_manager;
	
	key					= new gim_private_key_obj;
	gim_key				= key;
	
	checksum			= new gim_checksum_obj;
	gim_checksum		= checksum;	
	
	mt_test_check();
	
	crypt				= new gim_cryptography_obj;
	gim_crypt			= crypt;
	
/*	compression			= new gim_compression_obj;
	gim_compression		= compression;
*/	
	error->set( "gim_base_obj::init" , 0 );
	set_state( __GIM_TH_IDLE );
}


void gim_obj::gim_release( void ) {
	set_state( __GIM_TH_EXIT );
	error->set( "gim_base_obj::release" , 18 );

	delete crypt;
	crypt= NULL;

/*	delete compression;
	compression = NULL;
*/
	delete  checksum;
	checksum = NULL;

	delete	key;
	key = NULL;

	delete	file_manager;
	file_manager = NULL;
	
	delete 	identity;
	identity		= NULL;
	
	delete	interface;
	interface		= NULL;
	gim_interface	= NULL;

	home->down();
	gim_memory->down();

	delete	memory;
	memory			= NULL;
	delete	compatibility;
	compatibility	= NULL;
	delete	alias;
	alias			= NULL;	
	delete	environ;
	environ			= NULL;
	delete	conf;
	conf			= NULL;
	delete	environment;
	environment		= NULL;
	delete	home;
	home			= NULL;
	
/*	gim_compression		= NULL;*/
	gim_file_manager	= NULL;
	gim_key				= NULL;
	gim_checksum		= NULL;
	gim_time			= NULL;
	gim_memory			= NULL;
	gim_conf			= NULL;
	gim_compatibility	= NULL;
	gim_alias			= NULL;
	gim_environ			= NULL;
	env_data			= NULL;
	gim_identity		= NULL;
	gim_interface		= NULL;
	gim_home			= NULL;
	gim_crypt			= NULL;

	error->set( "gim_base_obj::release" , 19 );
	
	delete gim_log;
	delete error;
	error			= NULL;
	
	gim_log			= NULL;
	gim_error		= NULL;
	internal_gim	= NULL;
}


void gim_obj::down( _gim_flag code ) {
	set_state( __GIM_TH_EXIT );
	error->set( GIM_ERROR_CRITICAL , "gim_base_obj::down" , "Gim shutdown. Serious!!!" , __GIM_ERROR );
	error->set( "gim_base_obj::down" , 18 );
	gim_file_manager->release();
	gim_identity->release();
	home->down();
	gim_memory->down();
	exit( code );
}


void			gim_obj::set_state( _gim_th_state state ) {
	gim_state = state;
}


void			gim_obj::stop_operation( void ) {
	set_state( __GIM_TH_STOP );
}


_gim_th_state	gim_obj::get_state			( void ) {
	return gim_state;
}


/* --------------------/
 *  gim_base_obj : End
 * -------------------*/


void gim_obj::mt_test( void ) {
	gim_error->set( "gim_enhanced_base_obj::mt_test" , "MersenneTwister random number generation test..." );
	gim_time_obj t;
	_gim_rand	mt;
	int iters = gim_conf->GetKeyINT( "mersenne" , "iterations" );
	int test[iters];
	int c = 0;
	char res[128];
	time_t	sys = ::time( NULL );
	mt.seed( sys );
	t.start_usec();
	for( c = 0 ; c < iters ; c++ ) {
		test[c] = mt.randInt( 50 );
	}
	strcpy( res , t.get_usec() );
	char message[256];
	__GIM_CLEAR( message , 256 , char );
	for( c = 0 ; c < 20 ; c++ ) 
		sprintf( message , "%s%2d " ,message , test[c] );
	strcat( message , "..." );
	gim_error->set( "gim_enhanced_base_obj::mt_test" , message );
	sprintf( message , "%d number generated in sec [%s]. Test done" , iters , res );
	gim_error->set( "gim_enhanced_base_obj::mt_test" , message );
}


void gim_obj::mt_test_check( void ) {
	volatile int changed = __GIM_NO;
	if ( gim_conf->ExistSection( "mersenne" ) == __GIM_EXIST ) {
		if ( gim_conf->ExistKey( "mersenne" , "test" ) == __GIM_EXIST ) {
			if ( gim_conf->ExistKey( "mersenne" , "iterations" ) == __GIM_EXIST ) {
				if ( gim_conf->GetKeyFLAG( "mersenne" , "test" ) == __GIM_YES ) {
					mt_test();
				}
			}
		}
		else {
			gim_conf->AddKeyFlag( "mersenne" , "test" , __GIM_NO );
			gim_error->set( GIM_ERROR_WARNING , "gim_enhanced_base_obj::mt_test_check" , "Mersenne::test key not found. Test setted NO per default" , __GIM_ERROR );
			changed = __GIM_YES;
		}
		if ( gim_conf->ExistKey( "mersenne" , "iterations" ) == __GIM_NOT_EXIST ) {
			gim_conf->AddKey( "mersenne" , "iterations" , 200 );
			gim_error->set( GIM_ERROR_WARNING , "gim_enhanced_base_obj::mt_test_check" , "Mersenne::iterations key not found. Iterations setted 200 per default" , __GIM_ERROR );
			changed = __GIM_YES;
		}
	}
	else {
		gim_conf->AddSection( "mersenne" );
		gim_conf->AddKeyFlag( "mersenne" , "test" , __GIM_NO );
		gim_error->set( GIM_ERROR_WARNING , "gim_enhanced_base_obj::mt_test_check" , "Mersenne Section not found. Test setted NO per default" , __GIM_ERROR );
		changed = __GIM_YES;
	}
	if ( changed == __GIM_YES ) {
		gim_home->add_default_comment();
		gim_conf->Write();
	}
}



// **********************
//
//	GKDP code start here
//
// **********************




void	gim_gkdp_obj::set_prg( const char * prg_name , _gim_Uint8 major , _gim_Uint8 minor , _gim_Uint8 subminor ) {
	strcpy( header.program , prg_name );
	header.major = major;
	header.minor = minor;
	header.built = subminor;
}




void	gim_gkdp_obj::New( const char * filename , _gim_flag crypt ) {
	strcpy( header.title , __GKP_TITLE_DATA );
	strcpy( file_name , filename );
	strcpy( header.user , gim_identity->login() );
	strcpy( header.hostname , gim_identity->node() );
	header.recursive = __GIM_NO;
	header.crypt = crypt;
	header.compression = __GIM_NO;
	header.hashed_file = __GIM_NO;
	header.xhk = __GIM_NO;
}


void	gim_gkdp_obj::new_path( const char * path_in ) {
	header.members = make_gkdp( path_in );
}


void 	gim_gkdp_obj::write( void ) {
	if ( ! strlen( file_name ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_gkp_obj::write" , "New file not yet setted. I don't write anything" , __GIM_NOT_OK );
	}
	else {
		_gim_handler * out = gim_file_manager->open( file_name , __GIM_FILE_POINTER , __GIM_WRITE );
		if ( out == NULL ) {
			gim_error->set( GIM_ERROR_FATAL , "gim_gkp_obj::write" , "Is not possible to write on disk" , __GIM_ERROR );
		}
		hwrite( out );
		lwrite( out );
		write_gkdp( out );
		gim_file_manager->close( out );
		gim_error->set( "gim_gkp_obj::write" , "New GKP file wrote" );
	}
}


void 	gim_gkdp_obj::open( const char * filename ) {
	char message[1024];
	strcpy( file_name , filename );
	hand = gim_file_manager->open( filename , __GIM_FILE_MAPPING , __GIM_READ );
	if ( hand == NULL ) {
		gim_error->set( GIM_ERROR_FATAL , "gim_gkp_obj::open" , "GKP data file not found" , __GIM_ERROR );
	}
	_gim_map map = hand->fm;
	map = hread( map );
	sprintf( message , "Calculated : %s" , gim_checksum->md5( (char *)&(header) , sizeof( gim_file_header_struct) ) );
	gim_error->set( "gim_gkp_obj::open" , message );
	sprintf( message , "Readed     : %s" , verify.hash );
	gim_error->set( "gim_gkp_obj::open" , message );
	if ( Lexical.str_equal( verify.hash , gim_checksum->md5( (char *)&(header) , sizeof( gim_file_header_struct) ) ) != __GIM_YES ) {
		gim_file_manager->close( hand );
		gim_error->set( GIM_ERROR_FATAL , "gim_gkp_obj::open" , "Gkp file header corrupted." , __GIM_ERROR );
	}
	map = lread( map );
	read_gkdp( map );
}


void	gim_gkdp_obj::close( void ) {
	char message[256];
	sprintf( message , "Closing GKDP file [%s]" , hand->name );
	gim_error->set( "gim_gkp_obj::gkdp_close" , message );
	gim_file_manager->close( hand );
	__GIM_CLEAR( &(header) , 1 , gim_file_header_struct );
	destroy();
	gim_error->set( "gim_gkp_obj::close" , "Closed" );
}


_gim_gkdp *	gim_gkdp_obj::take( const char * name ) {
	char message[256];
	if ( name == NULL ) 
		gim_error->set( GIM_ERROR_CRITICAL , "gim_gkp_obj::gkdp_take" , "You cannot take a NULL name" , __GIM_ERROR );
	for( currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link ) {
		if ( Lexical.str_equal( name , currentlist->name ) == __GIM_YES ) {
			_gim_gkdp * gkdp = (_gim_gkdp *)gim_memory->Alloc( sizeof( _gim_gkdp ) , __GIM_MEM_GKDP_TAKE , __GIM_HIDE );
			strcpy( gkdp->name , currentlist->name );
			currentlist->buffer = (_gim_buffer)gim_memory->Alloc( currentlist->size , __GIM_MEM_GKDP_TAKE , __GIM_HIDE );
			memcpy( currentlist->buffer , currentlist->handle , currentlist->size );
			gkdp->data = currentlist->buffer;
			gkdp->size = currentlist->size;
			if ( header.crypt == __GIM_FOG ) {
				crypt->set_secondary( currentlist->name );
				crypt->advanced_password( gkdp->data , gkdp->size );
			}
			gkdp->size--;
			sprintf( message , "File GKP::%s taken" , gkdp->name );
			gim_error->set( "gim_gkp_obj::gkdp_take" , message );
			return gkdp;
		}
	}
	sprintf( message , "File GKP::%s not found" , name );
	gim_error->set( GIM_ERROR_CRITICAL , "gim_gkp_obj::gkdp_take" , message , __GIM_ERROR );
	return NULL;
}


void		gim_gkdp_obj::leave( _gim_gkdp * gkdp ) {
	char message[256];
	_gim_gkp_flist * currentlist = lget();
	for( currentlist = next( currentlist ) ; currentlist != NULL ; currentlist = next( currentlist ) ) {
		if ( Lexical.str_equal( gkdp->name , currentlist->name ) == __GIM_YES ) {
			if ( currentlist->buffer != NULL ) {
				gim_memory->Free( currentlist->buffer );
				currentlist->buffer = NULL;
				gim_memory->Free( gkdp );
				sprintf( message , "File GKP::%s left" , currentlist->name );
				gim_error->set( "gim_gkp_obj::leave" , message );
			}
		}
	}
}

void		gim_gkdp_obj::set_password( const char * passwd ) {
	if ( passwd != NULL ) {
		crypt->set( passwd );
		gim_error->set( "gim_gkdp_obj::set_password" , "A new password succesfully setted" );
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_gkdp_obj::set_password" , "You cannot pass NULL to this function" , __GIM_ERROR );
	}	
}


_gim_flag		gim_gkdp_obj::tmp_extract( _gim_gkdp * to_extract , const char * path ) {
	char			message[256] , AbsoluteFileName[2048];
	_gim_handler	*hand;
	if ( to_extract == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_gkdp_obj::tmp_extract" , "You cannot extract a NULL item" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	if ( path == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_gkdp_obj::tmp_extract" , "Path cannot be NULL" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	sprintf( AbsoluteFileName , "%s%s" , path , to_extract->name );
	sprintf( message , "Item GKP::%s is ready to extract in %s" , to_extract->name , path );
	gim_error->set( "gim_gkdp_obj::tmp_extract::gkdp_take" , message );
	hand = gim_file_manager->open( AbsoluteFileName , __GIM_FILE_POINTER , __GIM_WRITE );
	if ( hand == NULL ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_gkdp_obj::tmp_extract" , "Error in opening file for writing" , __GIM_ERROR );
		return __GIM_NOT_OK;
	}
	gim_file_manager->flush( hand , to_extract->data , to_extract->size );
	gim_file_manager->close( hand );
	sprintf( message , "Item GKP::%s succesfully extracted" , to_extract->name );
	gim_error->set( "gim_gkdp_obj::tmp_extract::gkdp_take" , message );
	return __GIM_OK;
}

// **********************
//
//	GKFP code start here
//
// **********************


void	gim_gkfp_obj::New( const char * filename , _gim_flag recursion , _gim_flag crypt , _gim_flag compress , _gim_flag hash ) {
	strcpy( header.title , __GKP_TITLE_FILE );
	strcpy( file_name , filename );
	strcpy( header.user , gim_identity->login() );
	strcpy( header.hostname , gim_identity->node() );
	header.recursive = recursion;
	header.crypt = crypt;
	header.compression = compress;
	header.hashed_file = hash;
	header.xhk = __GIM_NO;
}


_gim_Ulong	gim_gkfp_obj::New_path( const char * path_in ) {
	header.members = lmake( path_in , __GIM_ROOT );
	return header.members;
}

void	gim_gkfp_obj::New_root	( const char * path_in ) {
	char m[256];
	strcpy( new_root , path_in );
	sprintf( m , "New root changed in %s" , new_root );
	gim_error->set( "gim_gkfp_obj::New_root" , m );
}


_gim_flag 	gim_gkfp_obj::Write( void ) {
	if ( ! strlen( file_name ) ) {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_gkfp_obj::Write" , "New file not yet setted. I don't write anything" , __GIM_ERROR );
		return __GIM_NOT_EXIST;
	}
	else {
		_gim_handler * out = gim_file_manager->open( file_name , __GIM_FILE_POINTER , __GIM_WRITE );
		if ( out == NULL ) {
			gim_error->set( GIM_ERROR_FATAL , "gim_gkfp_obj::Write" , "Is not possible to write on disk" , __GIM_ERROR );
		}
		if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
		hwrite( out );
		if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
		lwrite( out );
		if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
		if ( write_data( out , lget() ) == __GIM_NOT_OK ) 
			return __GIM_NOT_OK;
		gim_file_manager->close( out );
		gim_error->set( "gim_gkfp_obj::Write" , "New GKP file wrote" );
	}
	return __GIM_OK;
}

void	gim_gkfp_obj::Set_prg( const char * prg_name , _gim_Uint8 major , _gim_Uint8 minor , _gim_Uint8 subminor ) {
	strcpy( header.program , prg_name );
	header.major = major;
	header.minor = minor;
	header.built = subminor;
}


_gim_map 	gim_gkfp_obj::Open( const char * filename ) {
	strcpy( file_name , filename );
	hand = gim_file_manager->open( filename , __GIM_FILE_MAPPING , __GIM_READ );
	if ( hand == NULL ) {
		char m[256];
		sprintf( m , "GKFP file not found : %s" , filename );
		gim_error->set( GIM_ERROR_FATAL , "gim_gkfp_obj::open" , m , __GIM_ERROR );
	}
	_gim_map map = hand->fm;
	map = hread( map );
	if ( Lexical.str_equal( verify.hash , gim_checksum->md5( (char *)&(header) , sizeof( gim_file_header_struct) ) ) != __GIM_YES ) {
		gim_file_manager->close( hand );
		gim_error->set( GIM_ERROR_FATAL , "gim_gkfp_obj::open" , "GKFP file header corrupted." , __GIM_ERROR );
	}
	map = lread( map );
	read_data( map , lget() );
	return map;
}



void 	gim_gkfp_obj::Extract( const char * filename ) {
	strcpy( file_name , filename );
	hand = gim_file_manager->open( filename , __GIM_FILE_MAPPING , __GIM_READ );
	if ( hand == NULL ) {
		gim_error->set( GIM_ERROR_FATAL , "gim_gkfp_obj::open" , "GKFP file not found" , __GIM_ERROR );
	}
	_gim_map map = hand->fm;
	map = hread( map );
	if ( Lexical.str_equal( verify.hash , gim_checksum->md5( (char *)&(header) , sizeof( gim_file_header_struct) ) ) != __GIM_YES ) {
		gim_file_manager->close( hand );
		gim_error->set( GIM_ERROR_FATAL , "gim_gkfp_obj::open" , "GKFP file header corrupted." , __GIM_ERROR );
	}
	map = lread( map );
	read_data( map , lget() );
	extract_data( lget() );
}

void		gim_gkfp_obj::set_password( const char * passwd ) {
	if ( passwd != NULL ) {
		crypt->set( passwd );
		gim_error->set( "gim_gkdp_obj::set_password" , "A new password succesfully setted" );
	}
	else {
		gim_error->set( GIM_ERROR_CRITICAL , "gim_gkdp_obj::set_password" , "You cannot pass NULL to this function" , __GIM_ERROR );
	}	
}


