

#include <gim/gim.h>


int main( int argc, char *argv[] ) {
	int c=0 , l=0;
	
	gim_set_application_name( "GDBStest" );
	gim_obj		* gim = new gim_obj;
	gim_db_obj  * db = new gim_db_obj;
	
	if ( db->read( "valori_temperatura" ) == __GIM_OK ) { 
		puts( "OK letto" );
	}
	else 
		db->init_from_gdbs( argv[1] );
	
	delete db;
	delete gim;
	exit(0);
}
