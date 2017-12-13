/***************************************************************************
 *            gim_base_identity.c
 *
 *  Tue Nov 21 16:09:26 2006
 *  Copyright  2006  AsYntote
 *  asyntote@gkript.org
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include "../include/gim_identity.h"


gim_identity_obj	* gim_identity;

void	gim_identity_obj::init( void ) {
	__GIM_CLEAR( Login			, 256 , char );
	__GIM_CLEAR( Real_name		, 256 , char );
	__GIM_CLEAR( System_name	, 256 , char );
	__GIM_CLEAR( Node_name		, 256 , char );
	__GIM_CLEAR( Release		, 256 , char );
	__GIM_CLEAR( Version		, 256 , char );
	__GIM_CLEAR( Arch			, 256 , char );
	__GIM_CLEAR( Home			, 256  ,char );
	filled	= __GIM_NO;
	User_id	= 0;
}

void 	gim_identity_obj::release( void ) {
	gim_error->set( "gim_identity_obj::release" , "Identity releasing" );
	if ( processor != NULL ) {
		if ( processor->next != NULL ) {
			gim_error->set( "gim_identity_obj::release" , "Multiprocessor system releasing" );
			_gim_processor * tmp;
			char message[256];
			do {
				sprintf( message , "Processor #%d succesfully released" , processor->proc_number );
				tmp = processor->next;
				gim_memory->Free( processor );
				processor = tmp;
				gim_error->set( "gim_identity_obj::release" , message );
			} while ( processor != NULL );	
		}
		else {
			gim_memory->Free( processor );
			gim_error->set( "gim_identity_obj::release" , "Processor succesfully released" );
		}
	}
	if ( mi != NULL )
		gim_memory->Free( mi );
	gim_error->set( "gim_identity_obj::release" , "Identity succesfully released" );
}

void	gim_identity_obj::get( void ) {
	if ( filled == __GIM_NO ) {
		uid_t			me;
		struct passwd	* user_id;
		struct utsname	my_system;
		char tot[128];
		char disp[128];
		char message[256];
		_gim_processor * tmp_prc;

		gim_error->set( "gim_identity_obj::get" , "Getting identity..." );
		me = getuid ();
		user_id = getpwuid( me );
		uname ( & my_system );
		User_id = user_id->pw_uid;
		strcpy( Login		, user_id->pw_name );
		strcpy( Home		, user_id->pw_dir );
		strcpy( Real_name	, Lexical.char_filter( user_id->pw_gecos , ',' ) );
		strcpy( System_name	, my_system.sysname );
		strcpy( Node_name	, my_system.nodename );
		strcpy( Release		, my_system.release );
		strcpy( Version		, my_system.version );
		strcpy( Arch		, my_system.machine );
		filled = __GIM_YES;
		processor = cpu();
		memory();
		strcpy( tot  , gim_memory->metric_converter( mi->total ) );
		strcpy( disp , gim_memory->metric_converter( mi->free )  );
		sprintf( message , "User detected  : %s@%s on %s" , gim_identity->Login , gim_identity->Node_name , gim_identity->Release );
		gim_error->set( "gim_identity_obj::get" , message );
		for( tmp_prc = processor ; tmp_prc != NULL ; tmp_prc = tmp_prc->next ) {
			if ( tmp_prc->bogomips != 0 )
				sprintf( message , "Cpu  #%-2d  : %s @ %6.2fMHz (%6.3f) - Arch %s" , tmp_prc->proc_number , tmp_prc->model_name , tmp_prc->cpu_mhz , tmp_prc->bogomips , my_system.machine );
			else
				sprintf( message , "Cpu  #%-2d  : %s @ %6.2fMHz - Arch %s" , tmp_prc->proc_number , tmp_prc->model_name , tmp_prc->cpu_mhz ,my_system.machine );
			gim_error->set( "gim_identity_obj::get" , message );
			n_processors = tmp_prc->proc_number + 1;
			tot_bogomips += tmp_prc->bogomips;
		}
		if ( n_processors > 1 ) {
			sprintf( message , "Cpu       : Total Bogomips [ %6.3f ]" , tot_bogomips );
			gim_error->set( "gim_identity_obj::get" , message );
		}
		sprintf( message , "Mem  detected  : Total :%s | Free :%s | Ratio %2.2f%%" , tot , disp , mi->ratio );
		gim_error->set( "gim_identity_obj::get" , message );
		get_fs_stat();
		sprintf( message , "Processor hostid : %04X" , host_id );		
		gim_error->set( "gim_identity_obj::get" , message );
		gim_error->set( "gim_identity_obj::get" , "Getting identity...Done" );
	}
	else
		gim_error->set( GIM_ERROR_WARNING , "gim_identity_obj::get" , "Identity already filled" , __GIM_ERROR );		
}


_gim_processor *		gim_identity_obj::cpu( void ) {
    FILE	* cpuinfo;
    char 	buffer[128];
	char 	*tmp[5];
	int		prc_n , cur_prc;	// temp var for smp system
	_gim_processor	* proc;
	_gim_processor	* tmp_proc;
	_gim_processor	* start_proc;
	
    cpuinfo = fopen( "/proc/cpuinfo" , "rb" );
    if ( cpuinfo == NULL )
		return NULL;
	gim_error->set( "gim_identity_obj::cpu" , "Getting processor details..." );
    proc = (_gim_processor *)gim_memory->Alloc( sizeof( _gim_processor ) , __GIM_MEM_GETID_CPU , __GIM_HIDE);
	start_proc = proc;
	start_proc->next = NULL;
	prc_n = cur_prc = 0;
	proc->proc_number = 0;
	strcpy( proc->model_name , "" );
	while ( fgets( buffer , 128 , cpuinfo ) ) {
		if ( strlen( buffer ) ) {
			gim_memory->caller = __GIM_MEM_READ_KEY;
			Lexical.tokenizer( buffer , ":\n" );
			if ( strlen( Tok[1] ) ) {
				tmp[0] = strdup( Tok[0] );
				Lexical.strend( tmp[0] , '\t' );
			}
			if ( strlen( Tok[1] ) )
				tmp[1] = strdup( Lexical.str_offset_pre( Tok[1] , 1 ) ); 
			get_int( "processor" , prc_n );
			if ( prc_n != cur_prc ) {
				cur_prc = prc_n;
				proc->next = (_gim_processor *)gim_memory->Alloc( sizeof( _gim_processor ) , __GIM_MEM_GETID_CPU , __GIM_HIDE);
				proc = proc->next;
				proc->proc_number = cur_prc;
				n_processors = cur_prc + 1;
				proc->next = NULL;
			}
			get_str(	"model name"	, proc->model_name );
			get_str(	"Processor"		, proc->model_name );
			get_str(	"vendor_id"		, proc->vendor_id);
			get_int(	"cache size"	, proc->cache_size);
			get_float(	"cpu MHz"		, proc->cpu_mhz);
			get_float(	"bogomips"		, proc->bogomips);
			get_float(	"BogoMIPS"		, proc->bogomips);
			get_int(	"model"			, proc->model);
			get_int(	"cpu family"	, proc->family);
			get_int(	"stepping"		, proc->stepping);
		}
		if (  proc->cpu_mhz == 0 ) {
			if ( Lexical.is_in_string( proc->model_name , "arm" ) ) {
				proc->cpu_mhz = proc->bogomips;
				proc->bogomips = 0.00;
			}
			else {
				proc->est_cpu_mhz = proc->bogomips / 2.081472727 ;
				proc->cpu_mhz = proc->est_cpu_mhz;
			}
		}
		cpu_strfamily( proc );
    }
    fclose(cpuinfo);
    return start_proc;
}



void	gim_identity_obj::cpu_strfamily ( _gim_processor * processor ) {
    int family = processor->family;
    int model = processor->model;

    if ( ! ( strcmp( processor->vendor_id , "GenuineIntel") ) ) {
        if (family == 4) {
            strcpy( processor->strmodel , "i486 series" );
        } else if (family == 5) {
            if (model < 4) {
                strcpy( processor->strmodel , "Pentium Classic" );
            } else {
                strcpy( processor->strmodel , "Pentium MMX" );
            }
        } else if (family == 6) {
            if (model <= 1) {
                strcpy( processor->strmodel , "Pentium Pro" );
            } else if (model < 7) {
                strcpy( processor->strmodel , "Pentium II/Pentium II Xeon/Celeron" );
            } else if (model == 9) {
                strcpy( processor->strmodel , "Pentium M" );
            } else {
                strcpy( processor->strmodel , "Pentium III/Pentium III Xeon/Celeron" );
            }
        } else if (family > 6) {
            strcpy( processor->strmodel , "Pentium 4" );
        } else {
            strcpy( processor->strmodel , "i386 class" );
        }
    } else if ( ! ( strcmp( processor->vendor_id , "AuthenticAMD" ) ) ) {
        if (family == 4) {
            if (model <= 9) {
                strcpy( processor->strmodel , "AMD i80486 series" );
            } else {
                strcpy( processor->strmodel , "AMD 5x86" );
            }
        } else if (family == 5) {
            if (model <= 3) {
                strcpy( processor->strmodel , "AMD K5" );
            } else if (model <= 7) {
                strcpy( processor->strmodel , "AMD K6" );
            } else if (model == 8) {
                strcpy( processor->strmodel , "AMD K6-2" );
            } else if (model == 9) {
                strcpy( processor->strmodel , "AMD K6-III" );
            } else {
                strcpy( processor->strmodel , "AMD K6-2+/III+" );
            }
        } else if (family == 6) {
            if (model == 1) {
                strcpy( processor->strmodel , "AMD Athlon (K7" );
            } else if (model == 2) {
                strcpy( processor->strmodel , "AMD Athlon (K75)" );
            } else if (model == 3) {
                strcpy( processor->strmodel , "AMD Duron (Spitfire)" );
            } else if (model == 4) {
                strcpy( processor->strmodel , "AMD Athlon (Thunderbird)" );
            } else if (model == 6) {
                strcpy( processor->strmodel , "AMD Athlon XP/MP/4 (Palomino)" );
            } else if (model == 7) {
                strcpy( processor->strmodel , "AMD Duron (Morgan)" );
            } else if (model == 8) {
                strcpy( processor->strmodel , "AMD Athlon XP/MP (Thoroughbred)" );
            } else if (model == 10) {
                strcpy( processor->strmodel , "AMD Athlon XP/MP (Barton)" );
            } else {
                strcpy( processor->strmodel , "AMD Athlon (unknown)" );
            }
        } else if (family > 6) {
            strcpy( processor->strmodel , "AMD Opteron/Athlon64/FX" );
        } else {
            strcpy( processor->strmodel , "AMD i386 class" );
        }
    } else if ( ! ( strcmp( processor->vendor_id, "CyrixInstead" ) ) ) {
        if (family == 4) {
            strcpy( processor->strmodel , "Cyrix 5x86" );
        } else if (family == 5) {
            strcpy( processor->strmodel , "Cyrix M1 (6x86)" );
        } else if (family == 6) {
            if (model == 0) {
                strcpy( processor->strmodel , "Cyrix M2 (6x86MX)" );
            } else if (model <= 5) {
                strcpy( processor->strmodel , "VIA Cyrix III (M2 core)" );
            } else if (model == 6) {
                strcpy( processor->strmodel , "VIA Cyrix III (WinChip C5A)" );
            } else if (model == 7) {
                strcpy( processor->strmodel , "VIA Cyrix III (WinChip C5B/C)" );
            } else {
                strcpy( processor->strmodel , "VIA Cyrix III (WinChip C5C-T)" );
            }
        } else {
            strcpy( processor->strmodel , "Cyrix i386 class" );
        }
    } else if ( ! ( strcmp( processor->vendor_id , "CentaurHauls") ) ) {
        if (family == 5) {
            if (model <= 4) {
                strcpy( processor->strmodel , "Centaur WinChip C6" );
            } else if (model <= 8) {
                strcpy( processor->strmodel , "Centaur WinChip 2" );
            } else {
                strcpy( processor->strmodel , "Centaur WinChip 2A" );
            }
        } else {
            strcpy( processor->strmodel , "Centaur i386 class" );
        }
    } else if ( ! ( strcmp( processor->vendor_id , "GenuineTMx86" ) ) ) {
        strcpy( processor->strmodel , "Transmeta Crusoe TM3x00/5x00" );
    } else {
        strcpy( processor->strmodel , "Unknown" );
    }
}


_gim_memory *	gim_identity_obj::memory(void) {
    FILE	* procmem;
    char	buffer[128];
	char 	tmp[5][256];

    procmem = fopen( "/proc/meminfo" , "rb" );
    if ( procmem == NULL )
		return NULL;
	gim_error->set( "gim_identity_obj::memory" , "Getting memory details..." );
    mi = (_gim_memory *)gim_memory->Alloc( sizeof( _gim_memory ) , __GIM_MEM_GETID_MEM , __GIM_HIDE );
    while ( fgets( buffer , 128 , procmem ) ) {
		gim_memory->caller = __GIM_MEM_READ_KEY;
		Lexical.tokenizer( buffer , ":kB\n" );
		strcpy( tmp[0] , Lexical.char_filter( Tok[0] , ' ' ) );
		strcpy( tmp[1] , Lexical.char_filter( Tok[1] , ' ' ) );
		get_int("MemTotal", mi->total);
		get_int("MemFree", mi->free);
		get_int("Cached", mi->cached);
	}
    fclose(procmem);

	mi->free += mi->cached;    
    mi->used = 	mi->total - mi->free ;

    mi->total  *= 1024;
    mi->cached *= 1024;
    mi->used   *= 1024;
    mi->free   *= 1024;
	
    mi->ratio	= 100.0 * ( 1.0 - (float)((float)mi->used / (float)mi->total) );

    return mi;
}

struct statvfs vfsbuf;

void 	gim_identity_obj::get_fs_stat( void ) {
	char message[256];
	statvfs( "." , &vfsbuf );
	namemax = vfsbuf.f_namemax;
	sprintf ( message , "Max filename lenght is - %d" , namemax );
	gim_error->set( "gim_identity_obj::get_fs_stat" , message );

}


char *	gim_identity_obj::login( void ) {
	return Login;
}

char *	gim_identity_obj::node( void ) {
	return Node_name;
}


char *	gim_identity_obj::version( void ) {
	return Version;
}

char *	gim_identity_obj::home( void ) {
	return Home;
}

char *	gim_identity_obj::arch( void ) {
	return Arch;
}

int		gim_identity_obj::n_proc( void ) {
	return n_processors;
}

long	gim_identity_obj::hostid( void ) {
	return host_id;
}
