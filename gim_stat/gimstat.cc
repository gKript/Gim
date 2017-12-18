//
// gimstat.cc
//
// Gim  -  Generic Information Manager static library
// Version 2.5-x
// AsYntote
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%201.0?by=version]
//
/*#############################################################################

	Copyright (C) 2002, 2003 
	Danilo Zannoni (AsYntote)

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

#include <gim/gim.h>

#define MIN_SIZE	256

int main ( int argc , char *argv[] ) {
	if ( argc < 2 ) {
		puts( "gimstat V0.1 usage :" );
		puts( "gimstat filename \n" );
		puts( "gimstat coded by asyntote" );
		puts( "asyntote@gkript.org" );
		exit( -1 );
	}

	gim_set_application_name( "Gimstat" );

	gim_obj		* gim = new gim_obj;

	
	gim_utils_obj   *util = new gim_utils_obj;
	char	*ch;
	unsigned char nch=0;
	_gim_handler	* source;

	unsigned int gstat[256];
	unsigned int rsize=0;
	int gvarianza[256];

	float gqcaos_extension=0.0;
	float gqcaos_distribuition=0.0;
	float gqcaos_levelling=0.0;
	float gqcaos=0.0;
	
	int qgvarianza[4];
	int qgmedie[4];
	int gcaos[4];

	_gim_Uint32 c = 0;
		
	unsigned int gmin=0,gmax=0,gused=0,gdelta=0;
	char chmax , chmin;
	unsigned int cntrl=0,graph=0,space=0,others=0;

	_gim_flag   multisin=0;

	if( gim->file_manager->exist( argv[1]) == __GIM_NOT_EXIST ) {
		puts("File not exists\n");
		delete gim;
		exit(1);
	}

	if ( argc > 2 ) {
		multisin = 1;

		for ( _gim_Uint32 e = 1 ; e < argc ; e++ ) {

			if( util->isregular( argv[e] ) ) {
			
				memset( gstat , 0 , 256*sizeof(unsigned int) );
				memset( gvarianza , 0 , 256*sizeof(int) );
				memset( qgvarianza , 0 , 4*sizeof( int) );
				memset( qgmedie , 0 , 4*sizeof( int) );
				memset( gcaos , 0 , 4*sizeof( int) );

				cntrl=0;
				graph=0;
				space=0;
				others=0;

				gused = 0;
				gmax = 0;
				gmin = 0;
	
				source = gim->file_manager->open( argv[e] , __GIM_FILE_MAPPING , __GIM_READ );

				printf("%60s" , Lexical.string_trunc( argv[e] , 60 ));
				fflush(stdout);

				ch = source->fm;	

				for ( c = 0 ; c < source->size ; c++) {
					nch = *ch++;
					gstat[ nch ]++;
				} 

				for ( c = 0 ; c < 256 ;c++) {
					rsize += gstat[c];  
				}

				for ( c = 0 ; c < 256 ;c++) {
					gvarianza[ c ] = (source->size / 256) - gstat[c];
					if ( iscntrl(c) ) cntrl += gstat[c];
					if ( isgraph(c) ) graph += gstat[c];
					if ( c == ' ' ) space += gstat[c];
					if ( ( ! iscntrl(c) ) && ( ! isgraph(c) ) && ( c != ' ' ) ) others += gstat[c];
				}

				for ( c = 0 ; c < 256 ;c++) {
					  if ( gstat[c]!=0 )
						gused++;
				}
	
				for ( c = 0 ; c < 256 ; c++) {
					 if ( gstat[c] > gmax ) {
						gmax = gstat[c];
					 }
				}

				gmin = gmax;
	
				for ( c = 0 ; c < 256 ; c++) {
					if (( gstat[c] > 0 ) && (gstat[c] < gmin)) {
						gmin = gstat[c];
					}
				}

				gdelta = gmax - gmin;

				for ( c = 0 ; c < 64 ; c++ ) {
					qgvarianza[0] += gvarianza[c];
					qgvarianza[1] += gvarianza[c+64];
					qgvarianza[2] += gvarianza[c+128];
					qgvarianza[3] += gvarianza[c+192];
					qgmedie[0] += gstat[c];
					qgmedie[1] += gstat[c+64];
					qgmedie[2] += gstat[c+128];
					qgmedie[3] += gstat[c+192];
				}

				qgvarianza[0] = abs((qgvarianza[0]/64) - ((source->size/4) / gused));
				qgvarianza[1] = abs((qgvarianza[1]/64) - ((source->size/4) / gused));
				qgvarianza[2] = abs((qgvarianza[2]/64) - ((source->size/4) / gused));
				qgvarianza[3] = abs((qgvarianza[3]/64) - ((source->size/4) / gused));

				qgmedie[0] /= 64;
				qgmedie[1] /= 64;
				qgmedie[2] /= 64;
				qgmedie[3] /= 64;
	
/*				gcaos[0] = abs((qgmedie[0]*100)/abs((source->size) / gused));
				gcaos[1] = abs((qgmedie[1]*100)/abs((source->size) / gused));
				gcaos[2] = abs((qgmedie[2]*100)/abs((source->size) / gused));
				gcaos[3] = abs((qgmedie[3]*100)/abs((source->size) / gused));
*/		 
				gcaos[0] = abs( 100 - abs((qgmedie[0]*100)/abs((source->size) / gused)));
				gcaos[1] = abs( 100 - abs((qgmedie[1]*100)/abs((source->size) / gused)));
				gcaos[2] = abs( 100 - abs((qgmedie[2]*100)/abs((source->size) / gused)));
				gcaos[3] = abs( 100 - abs((qgmedie[3]*100)/abs((source->size) / gused)));

				gqcaos_extension = (float)((float)gused*100.00)/256.00;
				gqcaos_distribuition = (float)(100.00 - (float)((float)( (float)gdelta / (float)gmax ) * 100.0));
				gqcaos_levelling = (float)abs( 100.00 - (float)( (float)( (float)gcaos[0] + (float)gcaos[1] + (float)gcaos[2] + (float)gcaos[3]) / 4.00) );
				gqcaos = (float)((float)((float)gqcaos_extension + (float)gqcaos_distribuition + (float)gqcaos_levelling) / 3.00);

				printf("  -  gKaos : %3.1f%%\n" , gqcaos);

				if( source->size < MIN_SIZE) {
					puts("    WARNING:The file size is too small. The result for this file is not reliable");
				}

				gim->file_manager->close (source);
			}
		}
	}
	else { 
		multisin = -1;

		printf("analyzing ststistics for %s...\n\n" , argv[1]);
		fflush (stdout);

		memset( gstat , 0 , 256*sizeof(unsigned int) );
		memset( gvarianza , 0 , 256*sizeof(int) );
		memset( qgvarianza , 0 , 4*sizeof( int) );
		memset( qgmedie , 0 , 4*sizeof( int) );
		memset( gcaos , 0 , 4*sizeof( int) );
	
		source = gim->file_manager->open( argv[1] , __GIM_FILE_MAPPING , __GIM_READ );

		if( source->size < MIN_SIZE) {
			puts("WARNING:");
			puts("The file size is too small");
			puts("The results are not reliable\n");
		}

		printf("File size     : %d\n\n" , source->size );
		fflush (stdout);

	//	gim->file_manager->load ( source );
	
	//	ch = source->loadp;
		ch = source->fm;	

		for ( c = 0 ; c < source->size ; c++) {
			nch = *ch++;
			gstat[ nch ]++;
		} 

		for ( c = 0 ; c < 256 ;c++) {
			rsize += gstat[c];  
		}

		for ( c = 0 ; c < 256 ;c++) {
			gvarianza[ c ] = (source->size / 256) - gstat[c];
			if ( iscntrl(c) ) cntrl += gstat[c];
			if ( isgraph(c) ) graph += gstat[c];
			if ( c == ' ' ) space += gstat[c];
			if ( ( ! iscntrl(c) ) && ( ! isgraph(c) ) && ( c != ' ' ) ) others += gstat[c];
		}

	/*	for ( c = 0 ; c < 256 ;c++) {
			if ( iscntrl(c) ) cntrl += gstat[c];  
		}

		for ( c = 0 ; c < 256 ;c++) {
			if ( isgraph(c) ) graph += gstat[c];
		}

		for ( c = 0 ; c < 256 ;c++) {
			if ( c == ' ' ) space += gstat[0];
		}

		for ( c = 0 ; c < 256 ;c++) {
			if ( ( ! iscntrl(c) ) && ( ! isgraph(c) ) && ( c != ' ' ) ) others += gstat[c];
		}
	*/
		for ( c = 0 ; c < 256 ;c++) {
			  if ( gstat[c]!=0 )
				gused++;
		}
	
		for ( c = 0 ; c < 256 ; c++) {
			if ( gstat[c] > gmax ) {
				gmax = gstat[c];
				chmax = c;
	//			printf("M %u\n",gmax);
			 }
		}

		gmin = gmax;
	
		for ( c = 0 ; c < 256 ; c++) {
			if (( gstat[c] > 0 ) && (gstat[c] < gmin)) {
				gmin = gstat[c];
				chmin = c;
	//			printf("m %u\n",gmin);
			}
		}

		gdelta = gmax - gmin;

	//	unsigned int	cntrl=0 , graph=0 , space=0 , others=0;
		puts("Character statistics");
		printf("Control       : (%3.1f%%) %d\n",   (float)((float)cntrl/(float)source->size)*100.00 , cntrl );
		printf("Printable     : (%3.1f%%) %d\n",   (float)((float)graph/(float)source->size)*100.00 ,graph );
		printf("Spaces        : (%3.1f%%) %d\n",   (float)((float)space/(float)source->size)*100.00 ,space );
		printf("Others        : (%3.1f%%) %d\n\n", (float)((float)others/(float)source->size)*100.00 , others);

		puts("Extension and Distribuition statistics");
		printf("Used          : %10u\nMin times     : %10u - %3d\nMax times     : %10u - %3d\nDelta         : %10u\n\n"  , gused, gmin , (unsigned char)chmin , gmax , (unsigned char)chmax , gdelta);

		fflush (stdout);

		for ( c = 0 ; c < 64 ; c++ ) {
			qgvarianza[0] += gvarianza[c];
			qgvarianza[1] += gvarianza[c+64];
			qgvarianza[2] += gvarianza[c+128];
			qgvarianza[3] += gvarianza[c+192];
			qgmedie[0] += gstat[c];
			qgmedie[1] += gstat[c+64];
			qgmedie[2] += gstat[c+128];
			qgmedie[3] += gstat[c+192];
		}

		qgvarianza[0] = abs((qgvarianza[0]/64) - ((source->size/4) / gused));
		qgvarianza[1] = abs((qgvarianza[1]/64) - ((source->size/4) / gused));
		qgvarianza[2] = abs((qgvarianza[2]/64) - ((source->size/4) / gused));
		qgvarianza[3] = abs((qgvarianza[3]/64) - ((source->size/4) / gused));

		qgmedie[0] /= 64;
		qgmedie[1] /= 64;
		qgmedie[2] /= 64;
		qgmedie[3] /= 64;
	
		gcaos[0] = abs((qgmedie[0]*100)/abs((source->size) / gused));
		gcaos[1] = abs((qgmedie[1]*100)/abs((source->size) / gused));
		gcaos[2] = abs((qgmedie[2]*100)/abs((source->size) / gused));
		gcaos[3] = abs((qgmedie[3]*100)/abs((source->size) / gused));

	//	CH used : 256 - CH min : 125286 - CH max : 126860 - CH delta : 1574
	//	Blk 1   : 099%- Blk 2  99%   - Blk 3 100%   - Blk 4  99%
	//  Blk 4    :
	//	Mean value :
		puts("Leveling statistics");
		printf("Average value : %u\n" , abs((source->size) / gused));
		printf("Blk 1         : %3.2f%%\n", (float)((float)qgmedie[0]*100.00)/(float)abs((source->size) / (float)gused) );
		printf("Blk 2         : %3.2f%%\n", (float)((float)qgmedie[1]*100.00)/(float)abs((source->size) / (float)gused) );
		printf("Blk 3         : %3.2f%%\n", (float)((float)qgmedie[2]*100.00)/(float)abs((source->size) / (float)gused) );
		printf("Blk 4         : %3.2f%%\n\n", (float)((float)qgmedie[3]*100.00)/(float)abs((source->size) / (float)gused) );
		fflush (stdout);
	 
		gcaos[0] = abs( 100 - abs((qgmedie[0]*100)/abs((source->size) / gused)));
		gcaos[1] = abs( 100 - abs((qgmedie[1]*100)/abs((source->size) / gused)));
		gcaos[2] = abs( 100 - abs((qgmedie[2]*100)/abs((source->size) / gused)));
		gcaos[3] = abs( 100 - abs((qgmedie[3]*100)/abs((source->size) / gused)));

		gqcaos_extension = (float)((float)gused*100.00)/256.00;
		gqcaos_distribuition = (float)(100.00 - (float)((float)( (float)gdelta / (float)gmax ) * 100.0));
		gqcaos_levelling = (float)abs( 100.00 - (float)( (float)( (float)gcaos[0] + (float)gcaos[1] + (float)gcaos[2] + (float)gcaos[3]) / 4.00) );
		gqcaos = (float)((float)((float)gqcaos_extension + (float)gqcaos_distribuition + (float)gqcaos_levelling) / 3.00);

	//	        Average value :
		puts("Chaos parameters");
		printf("Extension     : %3.1f%%\n" , gqcaos_extension);
		printf("Distribuition : %3.1f%%\n" , gqcaos_distribuition);
		printf("Leveling      : %3.1f%%\n\n" , gqcaos_levelling);
		printf("gKaos result  : %3.1f%%\n\n" , gqcaos);
		//printf("gKaos result  : %d%%\n\n" , (int)gqcaos);

		gim->file_manager->close (source);
		
	}
	
	delete gim;
	
	exit( (int)gqcaos);
}
