//
// gim_gkfp_file_data.cc
//
// Gim  -  Generic Information Manager static library
// Version 1.0-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%201.0?by=version]
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
//
// Gim is developed with Anjuta version 1.2.4
//

/*!     \file       gim_gkfp_file_data.cc
		\brief      Copyright (C) 2002-2026<BR>
				    Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
					Released under the terms of the GNU General Public License v3<BR>
					[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <string.h>
#include "../include/gim.h"
#include "../include/gim_thread.h" 


gim_gkfp_progress_fn gim_gkfp_progress = NULL;

/* ---- parallel encryption of the GKFP chunks ----
   The workers only read a file, compute the md5 and apply the key stream: they use no shared Gim object
   (gim_checksum returns a static buffer and gim_file_manager keeps a shared list, so they are not used here).
   Everything that touches the log, the progress or the package is done by the main thread, in file order. */

static int gkfp_write_threads( void ) {
	unsigned int n = std::thread::hardware_concurrency();
	if ( n == 0 )
		n = 1;
	if ( n > 8 )
		n = 8;
	if ( gim_thread_allowed( "gim_gkfp" ) != __GIM_YES )
		n = 1;
	return (int) n;
}

struct gkfp_fog_job {
	char			path[ 1024 ];
	_gim_Uint32		size;
	char			id[ GIM_MD5_SIZE ];
	char			xhash[ GIM_MD5_SIZE ];
	unsigned char *	data;
	gim_file_obj *	file;
	bool			ok;
};

/* md5 of a buffer as hex text, with its own state: safe in threads */
static void gkfp_md5_hex( const unsigned char * buf , size_t size , char * hex ) {
	MD5_CTX			c;
	gim_md5_obj		m;
	unsigned char	digest[ 16 ];
	size_t			done = 0;
	int				i;
	m.MD5Init( &c );
	while ( done < size ) {
		size_t piece = size - done;
		if ( piece > ( (size_t) 1 << 30 ) )
			piece = (size_t) 1 << 30;
		m.MD5Update( &c , (unsigned char *) ( buf + done ) , (int) piece );
		done += piece;
	}
	m.MD5Final( digest , &c );
	for ( i = 0 ; i < 16 ; i++ )
		snprintf( hex + 2 * i , 3 , "%02x" , digest[ i ] );
	hex[ 32 ] = 0;
}

/* Sets the worker progress hook of this object (NULL = none). It is per object, not global. */
void gim_gkfp_obj::set_worker_progress( gim_gkfp_worker_progress_fn fn ) {
	worker_progress = fn;
}

/* Context of the progress of the key stream of one file: it is called by the thread that runs the file's worker. */
struct gkfp_crypt_ctx {
	gim_gkfp_worker_progress_fn	report;
	int							worker;
	const char					* name;
	long long					size;
	std::atomic<size_t>			* progress;
};

static void gkfp_crypt_tick( void * ctx_ptr , size_t done , size_t total ) {
	gkfp_crypt_ctx * c = (gkfp_crypt_ctx *) ctx_ptr;
	size_t scaled = ( total > 0 ) ? (size_t) ( (unsigned long long) done * (unsigned long long) c->size / (unsigned long long) total ) : 0;
	if ( c->progress != NULL )
		c->progress->store( scaled );
	if ( c->report != NULL )
		c->report( c->worker , (long long) scaled , c->size , c->name , "Crypting" );
}

/* One file of an encrypted package, run by the worker `worker` (its index, 0 .. threads - 1).
   The file is read with gim_file_obj: its buffer is the one of Gim (gim_memory) and it stays in the job until the writer
   has flushed it (see gkfp_fog_release). `report` is the worker progress hook (may be NULL): it gets this file only.
   `progress` receives the bytes of the key stream done, so the writer can show them. */
static void gkfp_fog_work( gkfp_fog_job * job , const unsigned char * key , unsigned int iter , int inner , int worker , gim_gkfp_worker_progress_fn report , std::atomic<size_t> * progress ) {
	const char		* slash = strrchr( job->path , '/' );
	const char		* name = ( slash != NULL ) ? slash + 1 : job->path;
	const long long	size = (long long) job->size;
	gim_error->app( GIM_ERROR_MESSAGE , "gkfp_worker" , "start: %s (%u bytes), %d inner thread(s)" , job->path , (unsigned) job->size , inner );
	job->ok = false;
	job->file = new gim_file_obj( job->path , __GIM_FILE_POINTER , __GIM_READ );
	if ( job->file->check() != __GIM_EXIST || job->file->size() != job->size )
		return;
	if ( report != NULL )
		report( worker , 0 , size , name , "Reading" );
	job->data = (unsigned char *) job->file->load();
	if ( job->data == NULL )
		return;
	if ( report != NULL )
		report( worker , size , size , name , "Reading" );
	if ( report != NULL )
		report( worker , 0 , size , name , "Hashing" );
	gkfp_md5_hex( job->data , job->size , job->id );
	if ( report != NULL )
		report( worker , size , size , name , "Hashing" );
	gkfp_crypt_ctx crypt_ctx = { report , worker , name , size , progress };
	gim_cryptography_obj::apply_stream_parallel( key , iter , job->id , job->data , job->size , inner , gkfp_crypt_tick , &crypt_ctx );
	if ( report != NULL )
		report( worker , size , size , name , "Crypting" );
	if ( report != NULL )
		report( worker , 0 , size , name , "Hashing" );
	gkfp_md5_hex( job->data , job->size , job->xhash );
	if ( report != NULL )
		report( worker , size , size , name , "Hashing" );
	job->ok = true;
	gim_error->app( GIM_ERROR_MESSAGE , "gkfp_worker" , "end: %s , id %s" , job->path , job->id );
}

/* Gives back the buffer of a job: the file is closed (only if it was opened; close() frees the buffer), then the object is deleted. */
static void gkfp_fog_release( gkfp_fog_job & job ) {
	if ( job.file != NULL ) {
		if ( job.file->check() == __GIM_EXIST ) {
			job.file->close();   /* close() frees the buffer loaded by load() too */
		}
		delete job.file;
		job.file = NULL;
	}
	job.data = NULL;
}

_gim_flag	gim_gkfp_obj::write_data( _gim_handler * out , _gim_gkp_flist * startlist ) {
	if ( header.crypt == __GIM_FOG )
		return write_fog_batches( out , startlist , gkfp_write_threads() );
	/* encrypted packages: the files are encrypted in parallel batches (see write_fog_batches); plain packages below */
	char			path[1024];
	char			message[256];
	_gim_gkp_flist	* currentlist = NULL;
	_gim_handler	* in = NULL;
	char	 		* file = NULL , tname[1024];
	_gim_gkp_chunk	chunk;
	int total = 0 , done = 0;
	for ( currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link )
		if ( currentlist->type == REGFILE )
			total++;
	//gim_interface->clear();
	for( currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link ) {
		if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
		if ( currentlist->type == REGFILE ) {
			done++;
			if ( gim_gkfp_progress != NULL )
				gim_gkfp_progress( done , total , currentlist->name );
			gim_interface->data->dst_ratio = (float)( (float)currentlist->id / (float)endlist->id );
			sprintf( message , "Cur id : %d  |  end id : %d  |  dst_ratio %f" , currentlist->id , endlist->id , gim_interface->data->dst_ratio );
			gim_error->set( "gim_gkfp_file_data_obj::write" , message );
			sprintf( tname , "%s%s" , currentlist->path , currentlist->name ) ; if ( worker_progress != NULL ) worker_progress( 0 , 0 , (long long) currentlist->size , currentlist->name , "Reading" );
			__GIM_CLEAR( &chunk , 1 ,  _gim_gkp_chunk );
			if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			if ( header.crypt == __GIM_FOG )
				in = gim_file_manager->open( tname , __GIM_FILE_POINTER , __GIM_READ );
			if ( header.crypt == __GIM_SUN )
				in = gim_file_manager->open( tname , __GIM_FILE_MAPPING , __GIM_READ );
			if ( in == NULL ) {
				sprintf( message , "I cannot open file %s" , currentlist->name );
				gim_error->set( GIM_ERROR_FATAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
			}
			if ( header.crypt == __GIM_FOG ) {
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Loading" );
				file = gim_file_manager->load( in );
				if ( file == NULL ) 
					gim_error->set( GIM_ERROR_FATAL , "gim_file_data_obj::data" , "I cannot load file" , __GIM_NOT_OK );
				gim_checksum->md5_buf( chunk.id , sizeof( chunk.id ) , (char *) file , in->size ) ;
				crypt->set_secondary( currentlist->name );
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Crypting" );
				crypt->advanced_password_nonce( file , in->size , chunk.id );
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Hashing" );
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
				gim_checksum->md5_buf( chunk.xhash , sizeof( chunk.xhash ) , (char *) file , in->size );
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			}
			else {
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Hashing" );
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
				gim_checksum->md5_buf( chunk.id , sizeof( chunk.id ) , (char *) in->fm , in->size ) ;
				if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			}
			char message[1024];
			sprintf( message , "writing chunk id - %d" , currentlist->id );
			gim_error->set( "gim_file_data_obj::data" , message );
			if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			gim_file_manager->flush( out , (char *)&chunk , sizeof( chunk ) );
			__GIM_CLEAR( message , 1024 , char );
			sprintf( message , "writing file - %s" , currentlist->name );
			gim_error->set( "gim_file_data_obj::data" , message );
			if ( header.crypt == __GIM_FOG ) {
				sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
				strcpy( gim_interface->data->item_dna_name , "Flushing" );
				gim_file_manager->flush( out , file , in->size );
			}
			if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			if ( header.crypt == __GIM_SUN )
				{ if ( worker_progress != NULL ) worker_progress( 0 , 0 , (long long) currentlist->size , currentlist->name , "Flushing" ); gim_file_manager->flush( out , in->fm , in->size ); if ( worker_progress != NULL ) { worker_progress( 0 , (long long) currentlist->size , (long long) currentlist->size , currentlist->name , "Flushing" ); worker_progress( 0 , 0 , 0 , "" , "" ); } }
			gim_file_manager->close( in );
		}
	}
	return __GIM_OK;
}
	

_gim_flag	gim_gkfp_obj::read_data( _gim_map map , _gim_gkp_flist * startlist ) {
	_gim_gkp_flist 	* currentlist = NULL;
	_gim_gkp_chunk	chunk;
	_gim_buffer		data = NULL;
	int 			c;
	for( c = 0 , currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link , c++ ) {
		if ( currentlist->type == REGFILE ) {
			char message[256];
			gim_interface->data->sou_size = gim_interface->data->sou_position = c + 1;
			sprintf( message , "Reading GKP::%s " , currentlist->name );
			gim_error->set( "gim_file_data_obj::data" , message );
			__GIM_CLEAR( &chunk , 1 , chunk );
			memcpy( &chunk , map , sizeof( chunk ) );
			map += sizeof( chunk );
			currentlist->handle = map;
			strcpy( currentlist->nonce , chunk.id );
			data = (_gim_buffer)gim_memory->Alloc( currentlist->size , __GIM_MEM_FILE_DATA_R , __GIM_HIDE );
			memcpy( data , map , currentlist->size );
			map += currentlist->size;
			sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
			sprintf( gim_interface->data->item_dna_name , "Hashing" );
			if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
			if ( header.crypt == __GIM_SUN ) {
				char digest[ GIM_MD5_SIZE ]; gim_checksum->md5_buf( digest , sizeof( digest ) , (char *) data , currentlist->size ); if ( Lexical.str_equal( chunk.id , digest ) == __GIM_NO ) {
					char message[256];
					sprintf( message , "GKP::%s checksum error" , currentlist->name );
					gim_error->set( GIM_ERROR_CRITICAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
				}
			}
			if ( header.crypt == __GIM_FOG ) {
				char digest[ GIM_MD5_SIZE ]; gim_checksum->md5_buf( digest , sizeof( digest ) , (char *) data , currentlist->size ); if ( Lexical.str_equal( chunk.xhash , digest ) == __GIM_NO ) {
					char message[256];
					sprintf( message , "GKP::%s checksum error" , currentlist->name );
					gim_error->set( GIM_ERROR_CRITICAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
				}
			}
			gim_memory->Free( data );
		}
	}
	return __GIM_OK;
}


_gim_flag	gim_gkfp_obj::extract_data( _gim_gkp_flist * startlist ) {
	_gim_gkp_flist 	* currentlist = NULL;
	_gim_gkp_chunk	chunk;
	_gim_buffer		data = NULL;
	char 			message[256] , tname[1024];
	char			TmpFileName[2048];
	gim_utils_obj	* util = new gim_utils_obj;
	char 			* path_tmp;
	int  			len;
	for( currentlist = startlist ; currentlist != NULL ; currentlist = currentlist->link ) {
		switch ( currentlist->type ) {
			case ROOT : {
				if ( strlen( new_root ) != 0 ) {
					//printf("Root - %s..." , new_root );
					mkdir( new_root , currentlist->stat.st_mode );
					//puts( "Created" );
				}
				else {
					//printf("Root - %s..." , currentlist->name );                           
					mkdir( currentlist->name , currentlist->stat.st_mode );
					//puts( "Created" );
				}
				break;
			}
			case REGFILE	:
			case EMPTY		: {
				_gim_handler * file;
				gim_interface->data->dst_ratio = (float)( (float)currentlist->id / (float)endlist->id );
				sprintf( message , "Cur id : %d  |  end id : %d  |  dst_ratio %f" , currentlist->id , endlist->id , gim_interface->data->dst_ratio );
				gim_error->set( "gim_gkfp_obj::extract_data" , message );
				sprintf( tname , "%s%s" , currentlist->path , currentlist->name ) ;
				if ( internal_gim->get_state() == __GIM_TH_STOP ) 
					return __GIM_NOT_OK;
				sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				if ( strlen( new_root ) != 0 ) {
					char m[2048];
					//printf( " tmpfilename before : %s\n after : %s\n" , TmpFileName , util->str_subtraction( TmpFileName , old_root ) );
					sprintf( m , "%s%s" , new_root , util->str_subtraction( TmpFileName , old_root ) );
					strcpy( TmpFileName , m );
//					printf( " tmpfilename : %s\n" , TmpFileName );
				}
				file = gim_file_manager->open( TmpFileName , __GIM_FILE_POINTER , __GIM_WRITE );
				if ( file == NULL ) {
					sprintf( message , "Cannot open %s for writing" , TmpFileName );
					gim_error->set( GIM_ERROR_FATAL , "gim_gkfp_obj::extract_data" , message , __GIM_ERROR );
				}
				if ( header.crypt == __GIM_SUN ) {
					gim_file_manager->flush( file , currentlist->handle , currentlist->size );
				}
				if ( header.crypt == __GIM_FOG ) {
					sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
					sprintf( gim_interface->data->item_dna_name , "Loading" );
					currentlist->buffer = (_gim_buffer)gim_memory->Alloc( currentlist->size * sizeof( char ) );
					memcpy( currentlist->buffer , currentlist->handle , currentlist->size );
					crypt->set_secondary( currentlist->name );
					sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
					sprintf( gim_interface->data->item_dna_name , "Crypting" );
					crypt->advanced_password_nonce( currentlist->buffer , currentlist->size , currentlist->nonce );
					if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
					sprintf( gim_interface->data->item_dst_name , "%s" , currentlist->name );
					sprintf( gim_interface->data->item_dna_name , "Flushing" );
					if ( internal_gim->get_state() == __GIM_TH_STOP ) return __GIM_NOT_OK;
					gim_file_manager->flush( file , currentlist->buffer , currentlist->size );
					gim_memory->Unlock_and_free( currentlist->buffer );
					currentlist->buffer = NULL;	/* already freed: ldestroy() must not free it again */
				}
				gim_file_manager->close( file );
				chmod( TmpFileName , currentlist->stat.st_mode );
				break;
			}
			case REGNODE	: {
				//~ sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				//~ printf("Node  - CREATE  -  %s\n" , TmpFileName );
				sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				if ( strlen( new_root ) != 0 ) {
					char m[2048];
					//printf( " tmpfilename before : %s\n after : %s\n" , TmpFileName , util->str_subtraction( TmpFileName , old_root ) );
					sprintf( m , "%s%s/" , new_root , util->str_subtraction( TmpFileName , old_root ) );
					strcpy( TmpFileName , m );
					//printf( " tmpfilename : %s\n" , TmpFileName );
				}
//				sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				mkdir( TmpFileName , currentlist->stat.st_mode );
				break;
			}
			case LINK : {
				//~ sprintf( TmpFileName , "%s%s" , currentlist->path , currentlist->name );
				//~ printf("Link  - SKIPPED - %s\n" , TmpFileName );
				break;
			}
			case BLK : {
				break;
			}
			case FIFO : {
				break;
			}
			case CHR : {
				break;
			}
			case SOCK : {
				break;
			}
			case UNKNOWN : {
				break;
			}
			default: {
			}
		}
	}
	return __GIM_OK;
}
		


/* Encrypted packages: `threads` workers encrypt the files; the main thread writes them in file order.
   A worker that ends takes the next file at once, and the writer waits only for the next file of the order.
   Memory is bounded: at most 2 * threads files and BATCH_BYTES of data are encrypted and not yet written.
   A big file (4 MB or more) runs alone and uses all the threads inside (the key stream is split in ranges of blocks).
   The workers are gim_thread_obj objects: they run only if [thread] gim_gkfp allows it; otherwise the files are done
   one at a time by the main thread. Chunk id, chunk hash and data are the same as the sequential writer. */
_gim_flag gim_gkfp_obj::write_fog_batches( _gim_handler * out , _gim_gkp_flist * startlist , int threads ) {
	const size_t					BATCH_BYTES = (size_t) 256 << 20;
	const size_t					BIG_BYTES = (size_t) 4 << 20;
	std::vector< _gim_gkp_flist * >	items;
	_gim_gkp_flist					* currentlist = NULL;
	unsigned char					key[ 64 ];
	unsigned int					iter;
	int								total = 0 , done = 0;
	char							message[ 1024 ];
	gim_gkfp_worker_progress_fn		report = worker_progress;
	if ( threads < 1 )
		threads = 1;
	for ( currentlist = startlist->link ; currentlist != NULL ; currentlist = currentlist->link )
		if ( currentlist->type == REGFILE ) {
			items.push_back( currentlist );
			total++;
		}
	crypt->derive_key( key );
	iter = crypt->iterations();

	const size_t					n = items.size();
	const size_t					window = (size_t) threads * 2;
	const bool						threaded = ( threads > 1 ) && ( gim_thread_allowed( "gim_gkfp" ) == __GIM_YES );
	std::vector< gkfp_fog_job >		jobs( n );
	std::vector< char >				ready( n , 0 );
	std::vector< int >				owner( n , 0 );
	std::vector< std::atomic<size_t> >	progress( n );
	for ( size_t i = 0 ; i < n ; i++ ) {
		sprintf( jobs[ i ].path , "%s%s" , items[ i ]->path , items[ i ]->name );
		jobs[ i ].size = items[ i ]->size;
		jobs[ i ].ok = false;
		jobs[ i ].file = NULL;
		jobs[ i ].data = NULL;
		progress[ i ].store( 0 );
	}
	std::mutex						lock;
	std::condition_variable			cv;
	size_t							next_start = 0 , next_write = 0 , inflight = 0;
	int								running = 0;
	bool							big_running = false;
	bool							stop = false;
	gim_time_obj					t_run;
	t_run.start_usec();
	gim_error->app( GIM_ERROR_MESSAGE , "gkfp_pipeline" , "start: %u file(s), %d worker(s)%s, window %u file(s)" , (unsigned) n , threaded ? threads : 1 , threaded ? "" : " (not threaded)" , (unsigned) window );

	std::vector< gim_thread_obj * > pool;
	if ( threaded )
		for ( int w = 0 ; w < threads ; w++ ) {
			gim_thread_obj * t = new gim_thread_obj;
			t->start( [ & , w ]( gim_thread_obj & ) {
				for ( ; ; ) {
					size_t	i;
					bool	big;
					int		inner;
					{
						std::unique_lock< std::mutex > guard( lock );
						cv.wait( guard , [ & ]() {
							if ( stop || next_start >= n )
								return true;
							bool next_big = jobs[ next_start ].size >= BIG_BYTES;
							return next_start < next_write + window &&
								( inflight == 0 || inflight + jobs[ next_start ].size <= BATCH_BYTES ) &&
								( next_big ? running == 0 : ! big_running );
						} );
						if ( stop || next_start >= n )
							return;
						i = next_start++;
						big = jobs[ i ].size >= BIG_BYTES;
						inflight += jobs[ i ].size;
						running++;
						if ( big )
							big_running = true;
						inner = big ? threads : 1;
					}
					gkfp_fog_work( &jobs[ i ] , key , iter , inner , w , report , &progress[ i ] );
					{
						std::lock_guard< std::mutex > guard( lock );
						ready[ i ] = 1;
						owner[ i ] = w;
						running--;
						if ( big )
							big_running = false;
					}
					cv.notify_all();
				}
			} );
			pool.push_back( t );
		}

	_gim_flag	result = __GIM_OK;
	for ( size_t pos = 0 ; pos < n ; pos++ ) {
		_gim_gkp_flist		* it = items[ pos ];
		gkfp_fog_job		& job = jobs[ pos ];
		_gim_gkp_chunk		chunk;
		if ( threaded ) {
			std::unique_lock< std::mutex > guard( lock );
			while ( ready[ pos ] == 0 ) {
				cv.wait_for( guard , std::chrono::milliseconds( 50 ) );
					if ( ready[ pos ] == 0 ) {
						if ( gim_interface != NULL && gim_interface->data != NULL ) {
							sprintf( gim_interface->data->item_dst_name , "%s" , it->name );
							strcpy( gim_interface->data->item_dna_name , "Crypting" );
							gim_interface->data->sou_size = (_gim_Uint32) job.size;
							gim_interface->data->sou_position = (_gim_Uint32) progress[ pos ].load();
							gim_interface->data->sou_ratio = ( job.size > 0 ) ? (float)( (double) progress[ pos ].load() / (double) job.size ) : 0.0f;
						}
					}
			}
		}
		else {
			gkfp_fog_work( &job , key , iter , 1 , 0 , report , &progress[ pos ] );
			owner[ pos ] = 0;
		}
		if ( internal_gim->get_state() == __GIM_TH_STOP ) {
			result = __GIM_NOT_OK;
			break;
		}
		done++;
		if ( gim_gkfp_progress != NULL )
			gim_gkfp_progress( done , total , it->name );
		gim_interface->data->dst_ratio = (float)( (float)it->id / (float)endlist->id );
		sprintf( message , "Cur id : %d | end id : %d | dst_ratio %f" , it->id , endlist->id , gim_interface->data->dst_ratio );
		gim_error->set( "gim_gkfp_file_data_obj::write" , message );
		if ( ! job.ok ) {
			sprintf( message , "I cannot open file %s" , it->name );
			gim_error->set( GIM_ERROR_FATAL , "gim_file_data_obj::data" , message , __GIM_ERROR );
			result = __GIM_NOT_OK;
			break;
		}
		__GIM_CLEAR( &chunk , 1 , _gim_gkp_chunk );
		strcpy( chunk.id , job.id );
		strcpy( chunk.xhash , job.xhash );
		sprintf( gim_interface->data->item_dst_name , "%s" , it->name );
		strcpy( gim_interface->data->item_dna_name , "Flushing" );
		sprintf( message , "writing chunk id - %d" , it->id );
		gim_error->set( "gim_file_data_obj::data" , message );
		gim_file_manager->flush( out , (char *) &chunk , sizeof( chunk ) );
		sprintf( message , "writing file - %s" , it->name );
		gim_error->set( "gim_file_data_obj::data" , message );
		if ( internal_gim->get_state() == __GIM_TH_STOP ) {
			result = __GIM_NOT_OK;
			break;
		}
		if ( report != NULL )
			report( owner[ pos ] , 0 , (long long) job.size , it->name , "Flushing" );
		gim_file_manager->flush( out , (char *) job.data , job.size );
		if ( report != NULL ) {
			report( owner[ pos ] , (long long) job.size , (long long) job.size , it->name , "Flushing" );
			report( owner[ pos ] , 0 , 0 , "" , "" );
		}
		gkfp_fog_release( job );
		{
			std::lock_guard< std::mutex > guard( lock );
			inflight -= job.size;
			next_write = pos + 1;
		}
		cv.notify_all();
	}

	{
		std::lock_guard< std::mutex > guard( lock );
		stop = true;
	}
	cv.notify_all();
	for ( size_t w = 0 ; w < pool.size() ; w++ )
		delete pool[ w ];
	for ( size_t k = 0 ; k < n ; k++ )
		gkfp_fog_release( jobs[ k ] );
	if ( report != NULL )
		for ( int w = 0 ; w < threads ; w++ )
			report( w , 0 , 0 , "" , "" );
	gim_error->app( GIM_ERROR_MESSAGE , "gkfp_pipeline" , "done in %s s" , t_run.get_usec() );
	return result;
}

