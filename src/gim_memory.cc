//
// gim_memory.cc
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

#include "../include/gim_memory.h"

gim_memory_obj	* gim_memory;

gim_memory_id memory_id_gim [] = {
	{	0xffff0000	,	"__GIM_MEM_MEMORY_UP"				} ,
	{	0xff000000	,	"__GIM_MEM_OTHER"					} ,
	{	0xff000001	,	"__GIM_MEM_OPEN_FMAP"				} ,
	{	0xff000002	,	"__GIM_MEM_READ_KEY"				} ,
	{	0xff000003	,	"__GIM_MEM_DEC_FOG_A_BUF"			} ,
	{	0xff000004	,	"__GIM_MEM_ENC_FOG_A_BUF"			} ,
	{	0xff000005	,	"__GIM_MEM_HOST_ID_GET"				} ,
	{	0xff000006	,	"__GIM_MEM_FILLBUF"					} ,
	{	0xff000007	,	"__GIM_MEM_FILLBUF_FMAP"			} ,
	{	0xff000008	,	"__GIM_MEM_FILLBUF_GCH"				} ,
	{	0xff000009	,	"__GIM_MEM_PRSP_UP"					} ,
	{	0xff000010	,	"__GIM_MEM_PRSR_ADDSECT"			} ,
	{	0xff000011	,	"__GIM_MEM_PRSR_ADDKEY"				} ,
	{	0xff000012	,	"__GIM_MEM_ENV_UP"					} ,
	{	0xff000013	,	"__GIM_MEM_NOSTD_CONV"				} ,
	{	0xff000014	,	"__GIM_MEM_START_USEC"				} ,
	{	0xff000015	,	"__GIM_MEM_FMAN_INIT"				} ,
	{	0xff000016	,	"__GIM_MEM_FMAN_NEW"				} ,
	{	0xff000017	,	"__GIM_MEM_FMAN_OPEN"				} ,
	{	0xff000018	,	"__GIM_MEM_FOBJ_LOAD"				} ,
	{	0xfff00001	,	"__GIM_W32_OPEN_FMAP"				} ,
	{	0xff000019	,	"__GIM_MEM_MTRAND_OBJ"				} ,
	{	0xff000020	,	"__GIM_MEM_FOBJ_OPEN_GCH"			} ,
	{	0xff000021	,	"__GIM_MEM_GETID_CPU"				} ,
	{	0xff000022	,	"__GIM_MEM_GETID_MEM"				} ,
	{	0xff000023	,	"__GIM_MEM_GKP_FLIST_ADD"			} ,
	{	0xff000024	,	"__GIM_MEM_FMAN_LOAD"				} ,
	{	0xff000025	,	"__GIM_HTML_PAGE"					} ,
	{	0xff000026	,	"__GIM_MEM_FILE_DATA_R"				} ,
	{	0xff000027	,	"__GIM_MEM_GKDP_TAKE"				} ,
	{	0xff000028	,	"__GIM_MEM_INTERF_DATA"				} ,
	{	0xff000029	,	"__GIM_MEM_DB_MAIN"					} ,
	{	0xff000030	,	"__GIM_MEM_DB_TABLE"				} ,
	{	0xfe000001	,	"__GGG_MEM_PRINT_SERV"				} ,
	{	0xfe000003	,	"__GGG_MEM_ADD_EVENT"				} ,
	{	0xff002000	,	"__GIM_MEM_PAGE_ADD"				} ,
	{	0xff000031	,	"__GIM_MEM_DIRECTORY_FLIST_ADD"		} ,
	{	0xff000032	,	"__GIM_MEM_DIRECTORY_ITEM"			} ,
	{	0xff000033	,	"__GIM_LIST_ITEM"					} ,
	{	0xff000034	,	"__GIM_GETOPT_ITEM"					} ,
	{	0xff000035	,	"__GIM_ASCII_BUFFER"				} ,
	{	0xff000036	,	"__GIM_GDBS_LINE"					} ,
	{	0xff000051	,	"__GIM_LIST_ITEM"					} ,
	{	0xffffffff	,	""									} ,
};

struct gim_memory_id memory_id_gkp [] = {
	{	0xff001000	,	"__GIM_MEM_GKP_FLIST_ADD"	} ,
	{	0xffffffff	,	""							} ,
};	
	
struct gim_memory_id memory_id_command [] = {
	{	0x00ff0000	,	"__ID_COMMAND_ERROR"		} ,
	{	0xffffffff	,	""							} ,
};
	

void	gim_memory_obj::up		( void ) {
	if ( startlist == NULL ) {
		gim_mem_list * L_init;
		L_init = ( gim_mem_list * ) malloc ( sizeof ( gim_mem_list ) );
		L_init->data 	= NULL;
		L_init->next	= NULL;
		L_init->caller	= 0xffff0000;
		startlist		= L_init;
		currentlist		= startlist;
		endlist			= startlist;
		allocated		= 0;
		allocated_peek	= 0;
//		gim_error->set( "gim_memory_obj::up" , 9 );
	}
//	else {
//		gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::up" , "" ,0 );
//	}
}

void	gim_memory_obj::down( void ) {
	char	message[256];
	gim_error->set( "gim_memory_obj::down" , 10 );
	if ( startlist != NULL ) 
		release_memory_list();
	sprintf( message , "Alocation peek is : %s" , metric_converter( allocated_peek ) );
	gim_error->set( "gim_memory_obj::Free", message );
	sprintf( message , "Free      [%s] index[%p] , Allocation id : %s" , metric_converter( sizeof( gim_mem_list ) ) , startlist , code_to_id( startlist->caller ) );
	gim_error->set( "gim_memory_obj::Free", message );
	free( startlist );
	startlist = NULL;
	gim_error->set( "gim_memory_obj::down" , 11 );
}



void * 	gim_memory_obj::Alloc	( size_t size , _gim_Uint32 id , _gim_flag hidden ) {
	if ( virt_mem == __GIM_OFF ) {
		if ( gim_interface != NULL )
			gim_interface->data->mem_max_allocable = max_allocation;
		if ( size > (size_t)abs( (int)max_allocation - allocated ) ) {
			if ( max_allocation > 0 ) {
				char message[256];
				sprintf( message , "The demand for memory exceeds the max_allocation [ %s / %d ]" , metric_converter( size ) , ( max_allocation - allocated ) );
				gim_error->set( GIM_ERROR_FATAL , "gim_memory_obj::alloc" , message , __GIM_ERROR );
				return NULL;
			}
		}
	}
	gim_mem_list *	tmplist = NULL;
	if ( id != __GIM_MEM_OTHER ) {
		caller = id;
	}
	tmplist = ( gim_mem_list * ) malloc ( sizeof ( gim_mem_list ) );
	if ( tmplist == NULL )
		gim_error->set( GIM_ERROR_FATAL , "gim_memory_obj::alloc", "Failed during a allocation of a new member of memory list" , __GIM_ERROR );
	tmplist->data = ( void *  ) malloc ( size );
	if ( tmplist->data == NULL ) {
		char message[256];
		sprintf( message , "Memory allocation failed on a request of %s size from %s" , metric_converter( size ) , code_to_id( caller ) );
		gim_error->set( GIM_ERROR_FATAL , "gim_memory_obj::alloc", message , __GIM_ERROR );
		return NULL;
	}
	allocated += size;
	if ( allocated > allocated_peek ) {
		allocated_peek = allocated;
	}
	if ( gim_interface != NULL ) {
		if ( gim_interface->data != NULL ) {
			gim_interface->data->mem_allocated = allocated;
			gim_interface->data->mem_ratio	  = (float)( (float)allocated / (float)max_allocation );
			gim_interface->data->mem_allocated_peek = allocated_peek;
			gim_interface->data->mem_allocated_peek_ratio = (float)( (float)allocated_peek / (float)max_allocation );
		}
	}
	memset( tmplist->data , 0 , size );
	tmplist->size = size;
	tmplist->caller = caller;
	tmplist->hide = hidden;
	tmplist->lock = this->lock;
	endlist->next = tmplist;
	endlist = tmplist;
	endlist->next = NULL;
	if ( ( hidden != __GIM_HIDE ) || ( Hide == __GIM_NO ) ) {
		char	message[256];
		sprintf( message , "Allocated [%s] index[%p] , Allocation id : %s" , metric_converter( size ) , endlist->data , code_to_id( caller ) );
		if ( tmplist->caller == id_to_code( "__ID_COMMAND_ERROR" ) ) {
			gim_error->set( GIM_ERROR_CRITICAL , "gim_memory_obj::alloc", message , __GIM_ERROR );
		}
		else {
			gim_error->set( "gim_memory_obj::alloc" , message );
		}
		if ( virt_mem == __GIM_OFF ) {
			sprintf( message , "Total memory allocated [%d] | Free memory for the application [%d]" , allocated , ( max_allocation - allocated ) );
			gim_error->set( "gim_memory_obj::alloc" , message );
		}
	}
	caller = __GIM_MEM_OTHER;
	return tmplist->data;
}


void * 	gim_memory_obj::Alloc_unlocked	( size_t size ) {
	void * tmp;
	tmp = Alloc( size );
	if ( Get_lock_status( tmp ) == __GIM_MEM_LOCK ) 
		Unlock( tmp );
	return tmp;
}


void * 	gim_memory_obj::Alloc_locked	( size_t size ) {
	void * tmp;
	tmp = Alloc( size );
	if ( Get_lock_status( tmp ) == __GIM_MEM_UNLOCK ) 
		Lock( tmp );
	return tmp;
}



void * 	gim_memory_obj::Alloc	( size_t size , _gim_Uint32 code ) {
	if ( code != __GIM_MEM_OTHER ) {
		caller = code;
	}
	return Alloc( size , code , __GIM_NOT_HIDE );
}


void * 	gim_memory_obj::Alloc	( size_t size , char * id ) {
	_gim_Uint32	code;
	code = id_to_code( id );
	if ( code != __ID_COMMAND_ERROR ) {
		caller = code;
	}
	else {
		char message[256];
		sprintf( message , "%s : code not found" , id );
		gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::Alloc" , message , __GIM_ERROR );
		caller = id_to_code( "__ID_COMMAND_ERROR" );
	}
	return Alloc( size , code , __GIM_NOT_HIDE );
}


void * 	gim_memory_obj::Alloc	( size_t size ) {
	return Alloc( size , __GIM_MEM_OTHER , __GIM_NOT_HIDE );
}


_gim_flag	gim_memory_obj::Unlock_and_free( void *  to_free ) {
	_gim_flag result = Check( to_free );
	if ( result == __GIM_NOT_EXIST ) 
		return __GIM_NOT_EXIST;
	Unlock( to_free );
	Free( to_free );
}


_gim_flag	gim_memory_obj::Free	( void *  to_free ) {
	_gim_flag	found = __GIM_NO;
	gim_mem_list *	before = startlist;
	char	message[256];
	if ( to_free == NULL ) 
		gim_error->set( GIM_ERROR_FATAL , "gim_memory_obj::Free" , "Is not possible to free a NULL pointer" , __GIM_ERROR );
	if ( startlist == NULL )
		gim_error->set( "gim_memory_obj::Free" , 14 );
	else {
		for ( currentlist = startlist ; currentlist != NULL ; currentlist = currentlist->next ) {
			if ( ( unsigned long long ) to_free == ( unsigned long long )currentlist->data ) {
				found = __GIM_YES;
				if ( currentlist->lock == __GIM_MEM_LOCK )  {
					if ( currentlist->caller == __GIM_MEM_OTHER ) {
						gim_error->set( GIM_ERROR_CRITICAL , "gim_memory_obj::Free", "CRITICAL : You are attempting to free a  LOCKED frame." , __GIM_ERROR );
						sprintf( message , "           [%s] index[%p] , Allocation id : %s." , metric_converter( currentlist->size ) , currentlist->data , code_to_id( currentlist->caller ) );
						gim_error->set( GIM_ERROR_CRITICAL , "gim_memory_obj::Free", message , __GIM_ERROR );
						gim_error->set( GIM_ERROR_CRITICAL , "gim_memory_obj::Free", "           You cannot free." , __GIM_ERROR );
//						tmplist = ( gim_mem_list * ) malloc ( sizeof ( gim_mem_list ) );
						return __GIM_NOT_OK;
					}
				}
				if ( ( currentlist->hide != __GIM_HIDE ) || ( Hide == __GIM_NO ) ) {
					sprintf( message , "Free      [%s] index[%p] , Allocation id : %s" , metric_converter( currentlist->size ) , currentlist->data , code_to_id( currentlist->caller ) );
					gim_error->set( "gim_memory_obj::Free", message );
				}
				free( currentlist->data );
				allocated -= currentlist->size;
				if ( ( currentlist->hide != __GIM_HIDE ) || ( Hide == __GIM_NO ) ) {
					if ( virt_mem == __GIM_OFF ) {
						sprintf( message , "Total memory allocated [%d] | Free memory for the application [%d]" , allocated , (_gim_Uint32)( max_allocation - allocated ) );
						gim_error->set( "gim_memory_obj::Free" , message );
					}
				}
				if ( gim_interface != NULL ) {
					if ( gim_interface->data != NULL ) {
						gim_interface->data->mem_allocated = allocated;
						gim_interface->data->mem_ratio  = (float)( (float)allocated / (float)max_allocation );
					}
				}
				before->next = currentlist->next;
				if ( currentlist->next == NULL )
					endlist = before;
				free( currentlist );
				to_free = NULL;
				return __GIM_OK;
			}
			else
				before = currentlist;
		}
		if ( found == __GIM_NO ) {
			sprintf( message , "Frame NOT found : index[%p]" , to_free );
			gim_error->set( GIM_ERROR_CRITICAL , "gim_memory_obj::Free" , message , __GIM_ERROR );
			return __GIM_NOT_OK;
		}
	}
	return __GIM_NOT_OK;
}


_gim_flag	gim_memory_obj::Check( void * to_check ) {
	for ( currentlist = startlist ; currentlist != NULL ; currentlist = currentlist->next ) {
		if ( (unsigned long long)to_check == (unsigned long long)currentlist->data )
			return __GIM_EXIST;
	}
	return __GIM_NOT_EXIST;
}

_gim_Uint32	gim_memory_obj::release_memory_list( void ) {
	gim_mem_list *	Next;
	_gim_Uint32	total = 0 , not_empty = 0 , total_size = 0 ;
	gim_error->set( "gim_memory_obj::release_memory_list" , 16 );
	for ( currentlist = startlist ; currentlist != NULL ; currentlist = Next ) {
		Next = currentlist->next;
		if ( currentlist->data != NULL ) {
			switch (currentlist->caller) {
				case __GIM_MEM_OTHER : {
					char message[256];
					sprintf( message , "Free      [%s] index[%p] __GIM_MEM_OTHER" , metric_converter( currentlist->size ) , currentlist->data );
					gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::release_memory_list", message , __GIM_ERROR );
					total_size += currentlist->size;
					not_empty++;
					free( currentlist->data );
					free( currentlist );
					currentlist = NULL;
					break;
				}
				default : {
					char message[256];
					sprintf( message , "Free      [%s] index[%p] , Allocation id : %s" , metric_converter( currentlist->size ), currentlist->data , code_to_id( currentlist->caller ) );
					gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::release_memory_list", message , __GIM_ERROR );
					total_size += currentlist->size;
					not_empty++;
					free( currentlist->data );
					free( currentlist );
					currentlist = NULL;
					break;
				}
			}
		}
		else {
			if ( currentlist->caller == id_to_code( "__GIM_MEM_MEMORY_UP" ) ) {
				total -= 1;
			}
		}
		total++;
	}
	char message[256];
	total_size += ( sizeof( gim_mem_list ) * total );
	sprintf( message , "Total element [%d] Not empty element [%d] Deallocated in MBytes [ %s - %d ]" , total , not_empty , metric_converter( total_size ) , total_size );
	gim_error->set( "gim_memory_obj::destroy_list", message );
	currentlist = NULL;
	endlist = NULL;
	return total;
}

_gim_Uint32	gim_memory_obj::id_to_code( const char * id ) {
	_gim_Uint8 c;
	for( c = 0 ; memory_id_gim[c].code != 0xffffffff ; c++ ) {
		if ( ! ( strcmp( id , memory_id_gim[c].id ) ) )
			return memory_id_gim[c].code;
	}
	for( c = 0 ; memory_id_gkp[c].code != 0xffffffff ; c++ ) {
		if ( ! ( strcmp( id , memory_id_gkp[c].id ) ) )
			return memory_id_gkp[c].code;
	}
	for( c = 0 ; memory_id_command[c].code != 0xffffffff ; c++ ) {
		if ( ! ( strcmp( id , memory_id_command[c].id ) ) )
			return memory_id_command[c].code;
	}
	return __ID_COMMAND_ERROR;
}

char *		gim_memory_obj::code_to_id( _gim_Uint32 code ) {
	_gim_Uint8 c;
	for( c = 0 ; memory_id_gim[c].code != 0xffffffff ; c++ ) {
		if ( code == memory_id_gim[c].code )
			return memory_id_gim[c].id;
	}
	for( c = 0 ; memory_id_gkp[c].code != 0xffffffff ; c++ ) {
		if ( code == memory_id_gkp[c].code )
			return memory_id_gkp[c].id;
	}
	for( c = 0 ; memory_id_command[c].code != 0xffffffff ; c++ ) {
		if ( code == memory_id_command[c].code )
			return memory_id_command[c].id;
	}
	static char message[256];
	sprintf( message , "__GIM_MEM_UNKNOWN 0x%08x" , code );
	return message;
}


void		gim_memory_obj::hide( _gim_flag flag ) {
	if ( flag == __GIM_YES ) 
		Hide = __GIM_YES;
	if ( flag == __GIM_NO ) 
		Hide = __GIM_NO;
}


_gim_Uint32	gim_memory_obj::Memory( void ) {
    FILE	* procmem;
    char	buffer[128];
	char	tmp[5][256];
	_gim_Uint32	free = 0;
	int		cached = 0;
    procmem = fopen( "/proc/meminfo" , "rb" );
    if ( procmem == NULL ) {
		return -1;
	}
    while ( fgets( buffer , 128 , procmem ) ) {
		gim_memory->caller = __GIM_MEM_READ_KEY;
		Lexical.tokenizer( buffer , ":kB\n" );
		strcpy( tmp[0] , Lexical.char_filter( Tok[0] , ' ' ) );
		strcpy( tmp[1] , Lexical.char_filter( Tok[1] , ' ' ) );
		get_int("MemFree", free );
		get_int("Cached" , cached );
	}
    fclose( procmem );
	free += cached;
	free *= __GIM_KB;
	return free;
}


void gim_memory_obj::Memory_update( void ) {
	char message[256];
	if ( Alloc_limit == __GIM_YES ) {
		gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::Memory_update", "Memory allocation limit activated" , __GIM_ERROR );
		if ( ( Limit >= 0 ) && ( Limit <= 100 ) ) 
			max_allocation = (_gim_Uint32)( ( (float)gim_identity->mi->free * (float)Limit ) / 100.00 );
			sprintf( message , "Allocation limit setted to %d%% (%s)" , Limit , metric_converter( max_allocation ) );
			gim_error->set( "gim_memory_obj::Memory_update", message );
	}
	else {
		gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::Memory_update", "Memory allocation limit NOT activated" , __GIM_ERROR );
		sprintf( message , "Allocation limit setted to 100%% (%s)" , metric_converter( gim_identity->mi->free ) );
		gim_error->set( "gim_memory_obj::Memory_update", message );
		max_allocation = gim_identity->mi->free;
	}
}


void	gim_memory_obj::mem_alloc_limit( _gim_flag flag , _gim_Uint8 mlimit ) {
	Alloc_limit = flag;
	Limit = mlimit;
	Memory_update();
}

void		gim_memory_obj::set_default_lock( _gim_flag deflock ) {
	this->lock = deflock; 
}

void	gim_memory_obj::Vmem( _gim_flag flag ) {
	virt_mem = flag;
	if ( virt_mem == __GIM_ON )
		gim_error->set( "gim_memory_obj::Vmem" , "Virtual memory enabled" );
	else if ( virt_mem == __GIM_OFF )
		gim_error->set( "gim_memory_obj::Vmem" , "Virtual memory DISABLED" );
	else
		gim_error->set( GIM_ERROR_CRITICAL , "gim_memory_obj::Vmem" , "Virtual memory : UNKNOWN VALUE" , __GIM_ERROR );
}


char *	gim_memory_obj::metric_converter( _gim_Ulong size ) {
	static char message[128];
	__GIM_CLEAR( message , 128 , char );
	if ( ( size / __GIM_MB ) >= 1 ) {
		sprintf( message , "%5ld MBytes" , ( size / __GIM_MB ) );
		return message;
	}
	//~ if ( ( ( size / __GIM_MB ) >= 1 ) && ( ( size / __GIM_GB ) < 1 ) ) {
		//~ sprintf( message , "%5ld MBytes" , ( size / __GIM_MB ) );
		//~ return message;
	//~ }
	if ( ( ( size / __GIM_KB ) >= 1 ) && ( ( size / __GIM_MB ) < 1 ) ) {
		sprintf( message , "%5ld KBytes" , ( size / __GIM_KB ) );
		return message;
	}
	if ( ( size / __GIM_KB ) < 1 ) {
		sprintf( message , "%5ld Bytes" , size );
		return message;
	}
	return NULL;
}


_gim_Uint32	gim_memory_obj::Max_allocable		( void ) {
	return max_allocation;
}


_gim_Uint32	gim_memory_obj::Allocated			( void ) {
	return allocated;
}


_gim_Uint32	gim_memory_obj::Allocated_peek		( void ) {
	return allocated_peek;
}


_gim_flag	gim_memory_obj::Virtual_memory		( void ) {
	return virt_mem;
}


_gim_flag	gim_memory_obj::Allocation_limit	( void ) {
	return Alloc_limit;
}


void		gim_memory_obj::peek_reset			( void ) {
	allocated_peek = 0;
}


_gim_flag	gim_memory_obj::Unlock( void * to_unlock ) {
	char message[256];
	for ( currentlist = startlist ; currentlist != NULL ; currentlist = currentlist->next ) {
		if ( (unsigned long long)to_unlock == (unsigned long long)currentlist->data ) {
			if ( currentlist->lock == __GIM_MEM_UNLOCK ) {
				gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::Unlock", "This frame was already Unlocked" , __GIM_OK );
				return __GIM_MEM_UNLOCK;
			}
			currentlist->lock = __GIM_MEM_UNLOCK;
			sprintf( message , "Correctly Unlocked      [%s] index[%p] , Allocation id : %s" , metric_converter( currentlist->size ) , currentlist->data , code_to_id( currentlist->caller ) );
			gim_error->set( "gim_memory_obj::Unlock", message );
			return __GIM_MEM_UNLOCK;
		}
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::Unlock", "Frame not found" , __GIM_OK );
	return __GIM_NOT_EXIST;
}


_gim_flag	gim_memory_obj::Lock( void * to_lock ) {
	char message[256];
	for ( currentlist = startlist ; currentlist != NULL ; currentlist = currentlist->next ) {
		if ( (unsigned long long)to_lock == (unsigned long long)currentlist->data ) {
			if ( currentlist->lock == __GIM_MEM_LOCK ) {
				gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::Lock", "This frame was already Locked" , __GIM_OK );
				return __GIM_MEM_LOCK;
			}
			currentlist->lock = __GIM_MEM_LOCK;
			sprintf( message , "Correctly Locked      [%s] index[%p] , Allocation id : %s" , metric_converter( currentlist->size ) , currentlist->data , code_to_id( currentlist->caller ) );
			gim_error->set( "gim_memory_obj::Lock", message );
			return __GIM_MEM_LOCK;
		}
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::Lock", "Frame not found" , __GIM_OK );
	return __GIM_NOT_EXIST;
}


_gim_flag	gim_memory_obj::Get_lock_status( void * to_check ) {
	char message[256];
	for ( currentlist = startlist ; currentlist != NULL ; currentlist = currentlist->next ) {
		if ( (unsigned long long)to_check == (unsigned long long)currentlist->data ) {
			return currentlist->lock;
		}
	}
	gim_error->set( GIM_ERROR_WARNING , "gim_memory_obj::Get_lock_status", "Frame not found" , __GIM_OK );
	return __GIM_NOT_EXIST;
}



// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------
//				MEMORY PAGE OBJ
// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------



gim_memory_page_obj::gim_memory_page_obj( size_t page_size ) {
	gim_error->set( "gim_memory_page_obj::gim_memory_page_obj", "Allocating a new Memory page" );
	page			= (_gim_buffer)gim_memory->Alloc( page_size , __GIM_MEM_PAGE_ADD , __GIM_HIDE );
	internal_page	= (_gim_buffer)gim_memory->Alloc( page_size , __GIM_MEM_PAGE_ADD , __GIM_HIDE );
	if ( ( page == NULL ) || ( internal_page == NULL ) )
		gim_error->set( GIM_ERROR_FATAL , "gim_memory_page_obj::gim_memory_page_obj", "Failed during the allocation of a new Page" , __GIM_ERROR );
	else {
		lstart = NULL;
		lcurrent = NULL;
		gim_error->set( "gim_memory_page_obj::gim_memory_page_obj", "Allocation done!!!" );
	}
}


gim_memory_page_obj::~gim_memory_page_obj() {
//	destroy_list();
	gim_memory->Free( page );
	gim_error->set( "gim_memory_page_obj::~gim_memory_page_obj", "Memory page deallocated" );
	gim_memory->Free( internal_page );
	gim_error->set( "gim_memory_page_obj::~gim_memory_page_obj", "Internal memory page deallocated" );
}
	
