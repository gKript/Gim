//
// gim.h
//
// Gim  -  Generic Information Manager static library
// Version 2.5-x
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

/*!	\file		gim.h
	\version	2.5-x
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://gkript.org/gk_gpl_v2.php ]
*/ 		

#ifndef	__cplusplus
	#error	"Gim nedd a C++ compiler. G++ GNU c++ compiler should be better"
#endif

#ifndef _GIM_H
#define _GIM_H

	#include <stdio.h>

	#include "gim_doxygen.h"
	#include "gim_version.h"
	#include "gim_define.h"
	#include "gim_custom_type.h"
	#include "gim_log.h"
	#include "gim_memory.h"
	#include "gim_time.h"
	#include "gim_error.h"
	#include "gim_kvalue.h"
	#include "gim_environment.h"
	#include "gim_identity.h"
	#include "gim_interface.h"
	#include "gim_signal.h"
	#include "gim_string.h"
	#include "gim_list.h"
	#include "gim_getopt.h"
	#include "gim_ascii_buffer.h"

	#include "gim_picgim.h"
	#include "gim_picgim_GKH32.h"

	#include "gim_file.h"
	#include "gim_mersennetwister.h"
	#include "gim_private_key.h"
	#include "gim_checksum.h"

	#include "gim_gkp_utils.h"
	#include "gim_gkp_header.h"
	#include "gim_gkp_file_list.h"
	#include "gim_gkp_file_data.h"
	#include "gim_gkp_password.h"
	#include "gim_gkp_cryptography.h"
	#include "gim_html.h"
	#include "gim_gkl.h"
	#include "gim_lzh.h"
	#include "gim_compression.h"
	#include "gim_cnc.h"
 	#include "gim_db.h"
	#include "gim_directory.h"
 
	#include "gim_gkp_header.h"
	#include "gim_gkp_file_list.h"
	#include "gim_gkp_file_data.h"

	/*! \class 		gim_obj
		\brief		Generic Information Manager Documentation base object
		\author		Danilo Zannoni
		\author		Corrado Tumiati
		\version	2.0-0
		\date		2003-2008
	*/
	class gim_obj {
		public:

			/*! \fn		virtual void down ( _gim_flag code )
				\brief	This method say to Gim to unload every allocated memory and classes before to delete.
				\param	code	The exit code.
			*/
	virtual void 					down				( _gim_flag code );	

			/*! \fn		void stop_operation ( void )
				\brief	This method say to Gim to stop the current operation if possible.
			*/
			void					stop_operation		( void );

			/*! \fn		_gim_th_state get_state ( void )
				\brief	This method say to Gim to stop the current operation if possible.
				\return The current Gim state. The state are defined in the \a GimThState enumerator.
			*/
			_gim_th_state			get_state			( void );

			/*! \fn		void set_state ( _gim_th_state state )
				\brief	With this method you can say to Gim to the new state.
				\param	state	The new Gim state. The state are defined in the \a GimThState enumerator.
			*/
			void					set_state			( _gim_th_state state );

			/*!
				\brief	Useful to report an error divided in 5 levels of priority\n\n
						See the methods of gim_error_obj.
			*/
			gim_error_obj			* error;

			/*!
				\brief	Using this class, insted the standard malloc end free, you are sure to hold trace of all the operations 
						in memory and being able to verify if exist any problems.\n\n
						See the methods of gim_memory_obj.
			*/
			gim_memory_obj			* memory;

			/*!
				\brief	With this class you can manage the configuration file of Gim [~/.gim/gim.conf].\n\n
						See the methods of gim_prsr_obj.
			*/
			gim_prsr_obj			* conf;

			/*!
				\brief	With this class you can manage the compatibility file of Gim [~/.gim/compatibility.conf].\n\n
						See the methods of gim_prsr_obj.
			*/
			gim_prsr_obj			* compatibility;

			/*!
				\brief	With this class you can manage the alias file of Gim [~/.gim/alias.conf].\n\n
						See the methods of gim_prsr_obj.
			*/
			gim_prsr_obj			* alias;

			/*!
				\brief	With this class you can manage the alias file of Gim [~/.gim/alias.conf].\n\n
						See the methods of gim_prsr_obj.
			*/
			gim_prsr_obj			* environ;

			/*!
				\brief	Gim knows who you are :) and with this class you can use this informations.\n\n
						See the methods of gim_identity_obj.
			*/
			gim_identity_obj		* identity;

			/*!
				\brief	
			*/
			gim_interface_obj		* interface;

			/*!
				\brief	In this struct you can find the path for your programs and to get the path of the other public keys .\n\n
						See the methods of gim_home_obj.
			*/
			gim_home_obj			* home;

			/*!
				\brief	To use this class, instead the standard open/close or fopen/fclose, is useful to hold trace 
						of operation on the files or knowing if problems exist.\n\n
						See the methods of gim_file_manager_obj.
			*/
			gim_file_manager_obj	* file_manager;

			/*!
				\brief	With this class you can manage your key\n\n
						See the methods of gim_private_key_obj.
			*/
			gim_private_key_obj		* key;

			/*!
				\brief	To make digest of any objects [files or generic buffers] with 4 different algorithms\n\n
						See the methods of gim_file_manager_obj.
			*/
			gim_checksum_obj		* checksum;

			/*!
				\brief	With this class you can crypt and decrypt a GKDP file.\n\n
						See the methods of gim_cryptography_obj.
			*/
			gim_cryptography_obj	* crypt;

			/*!
				\brief	With this class you can compress any kind of data.\n\n
						See the methods of gim_compression_obj.
			*/
			gim_compression_obj		* compression;

		private:
	friend	class	gim_home_obj;
	friend	class	gim_error_obj;

	friend	void	abort( int sig );

			void 	gim_init( void );
			void 	gim_release( void );
			void 	mt_test( void );
			void 	mt_test_check( void );

			gim_conf_data	* environment;

			_gim_flag		debug;
			_gim_th_state	gim_state;

		public:
			/*!
				gim_obj constructor	
			*/
			inline gim_obj() {
				gim_interface = NULL;
				gim_init();	
			};
			
			/*!
				gim_obj destructor	
			*/
			inline virtual ~gim_obj() {
				gim_release();
			};
	};

	typedef class gim_obj	gim_base_obj;

	extern gim_obj	* internal_gim;

	/*!	\enum	gim_gkp_mode
		\brief	This enum contain the two possible mode to treat data in Gpk
	*/
	enum gim_gkp_mode {
		__GIM_FOG = 1,	/*!<	Use this mode to crypt all data stored in a Gkp file	*/
		__GIM_SUN,		/*!<	Use this mode to store the uncrypted data in a Gkp file	*/
	};

	/*!	\struct		gim_gkdp_data_struct
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is uesd as defined type ::_gim_gkdp.
					It is used to take a file from a GKDP file. It will be used as inner file descriptor.
	*/
	struct gim_gkdp_data_struct {
		char		name[256];	/*!<	The name of the file taken from a GKDP	*/
		_gim_buffer	data;		/*!<	The buffer contain the data file	*/
		_gim_Uint32	size;		/*!<	The size of the bufer	*/
	};

	/*! \class 		gim_gkfp_obj
		\brief		Generic Information Manager Documentation gkfp object\n\n
					<b>GKP</b> - GKript Package\n
					With this class you can create, open and use <b>only GKFP file</b> and 
					you are not able to manage GKDP files\n
					<b>GKDP</b> - GKript Data Packaged\n
					<b>GKFP</b> - GKript File Packaged\n
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\author		Corrado Tumiati - skymatrix@gkript.org
		\version	2.0-0
		\date		2003-2008
	*/
	class gim_gkfp_obj {
		public:
			void		Set_prg		( const char * prg_name , _gim_Uint8 major , _gim_Uint8 minor , _gim_Uint8 subminor );
			void		New			( const char * filename , _gim_flag recursion , _gim_flag crypt , _gim_flag compress , _gim_flag hash );
			_gim_Ulong	New_path	( const char * path_in );
			void		New_root	( const char * path_in );
			_gim_flag	Write		( void );
			_gim_map	Open		( const char * filename );
			void 		Extract		( const char * filename );
		
			/*! \fn		void		set_password( char * passwd )
				\brief	With this method you can set a password in a \a GKDP file.
				\param	passwd		The \a password to set
			*/
			void		set_password( const char * passwd );
			
			/*! \fn	void write( _gim_handler * out , _gim_gkp_flist * startlist )
				\brief	Use this to write the content of the files present in the file list into a new gkp file
				\param	out			The pointer to the \a ::_gim_handler of the file you want to write
				\param	startlist	The pointer to the start of the file list
			*/
			_gim_flag	write_data( _gim_handler * out , _gim_gkp_flist * startlist );
		
			/*! \fn	void read( _gim_map map , _gim_gkp_flist * startlist )
				\brief	Use this to read the content of the files present in the file list into an existing gkp file
				\param	map		The \a ::_gim_map of the file you want to read previously opened in file mapping. 
				\param	startlist	The pointer to the start of the file list
			*/
			_gim_flag	read_data( _gim_map map , _gim_gkp_flist * startlist );
		
			_gim_flag	extract_data( _gim_gkp_flist * startlist );
			
			/*! \fn		void clear( void )
				\brief	Use this method to clear the file list
			*/
			void				lclear( void );

			/*! \fn		void write( _gim_handler * out )
				\brief	Use this method to write the file list into the Gkp file
				\param	out		Apointer to the \a ::_gim_handler of the new Gkp file
			*/
			void				lwrite( _gim_handler * out );

			/*! \fn		_gim_Uint32 make( char * dir_name , _gim_flag recursive , _gim_flag hashing )
				\brief	With this method you can create automatically the file list for a new Gkp file
				\param	dir_name	The name of the directory contain the file to put into a Gkp file
				\param	recursive	The flag to know if you want a recursion or not
				\param	hashing		A flag that describe if you want to hash the file or not
				\return The number of files stored into the file list
			*/
			_gim_Uint32			lmake( const char * dir_name , _gim_Uint16 level );

			/*! \fn		_gim_map read( _gim_map map )
				\brief	With this method you can create automatically the file list for a new Gkp file
				\param	map		The \a ::_gim_map of the file you want to read previously opened in file mapping.
				\return The _gim_map updated
			*/
			_gim_map			lread( _gim_map map );

			/*! \fn		_gim_gkp_flist * get( void )
				\brief	This method is useful to get the first member of the file list.
						With this member you can brows the list.
				\return A pointer to the first member of list or \n NULL in case of error
				\see	next( _gim_gkp_flist * tmplist )
			*/
			_gim_gkp_flist *	lget( void );

			/*! \fn		_gim_gkp_flist * next( _gim_gkp_flist * tmplist )
				\brief	This method is useful to get the next member of the file list if exist.
						With this member you can brows the list.
				\param	A pointer to a member of the file list.
				\return A pointer to the next member of list or \n NULL in case of end of list
				\see	get( void )
			*/
			_gim_gkp_flist *	lnext( _gim_gkp_flist * tmplist );

			gim_cryptography_obj	* crypt;
			
			gim_file_header_struct	header;
			gim_file_header_verify	verify;

		private:
	friend	class gim_gkfp_header_obj;
	friend	class gim_gkfp_file_list_obj;
	friend	class gim_gkfp_file_data_obj;

			char				file_name[256];
			_gim_handler *		hand;	

			void 				linit( void );
			void 				lrelease( void );
			_gim_gkp_flist *	ladd( void );
			void				ldestroy( void );
			
			char				old_root[2048];
			char				new_root[2048];
			
			_gim_gkp_flist *	startlist;
			_gim_gkp_flist *	currentlist;
			_gim_gkp_flist *	endlist;

			char *				hread	( _gim_map map );
			_gim_flag			hwrite	( _gim_handler * out );

			char				prog[32];

		public:
			
			inline gim_gkfp_obj() {
				__GIM_CLEAR( file_name , 256  , char );
				__GIM_CLEAR( &header  , 1   , gim_file_header_struct );
				header.gim_major = GIM_MAJOR;
				header.gim_minor = GIM_MINOR;
				header.gim_subminor = (_gim_Uint16)GIM_SUBMINOR;
				hand			= NULL;
				linit();
				crypt 			= new gim_cryptography_obj;
			};

			inline ~gim_gkfp_obj() {
				delete	crypt;
				lrelease();
			};
	};

	/*! \class 		gim_gkdp_obj
		\brief		Generic Information Manager Documentation gkfp object\n\n
					<b>GKP</b> - GKript Package\n
					With this class you can create, open and use <b>only GKDP file</b> and 
					you are not able to manage GKFP files\n
					<b>GKDP</b> - GKript Data Packaged\n
					<b>GKFP</b> - GKript File Packaged\n
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008	*/
	class gim_gkdp_obj {
		public:
		
			/*! \fn		void		set_password( char * passwd )
				\brief	With this method you can set a password in a \a GKDP file.
				\param	passwd		The \a password to set
			*/
			void		set_password( const char * passwd );
	
			/*! \fn		void gkp_set_prg( char * prg_name , _gim_Uint8 major , _gim_Uint8 minor , _gim_Uint8 subminor )
				\brief	With this method you can insert in a \a GKDP the application name and version. 
				\param	prg_name	The application name that create a new \a GKDP
				\param	major		The \a major number of the application version
				\param	minor		The \a minor number of the application version
				\param	subminor	The \a subminor number of the application version
			*/
			void		set_prg( const char * prg_name , _gim_Uint8 major , _gim_Uint8 minor , _gim_Uint8 subminor );

			/*! \fn		void gkdp_new( char * filename , _gim_flag crypt )
				\brief	With this method you can insert in a \a GKDP the application name and version.
				\param	filename	The file name of the new \a GKDP you want to create.
				\param	crypt		The \a major number of the application version. \n
									\a crypt must be : \n
									\a __GIM_FOG	to enable the crypting \n
									\a __GIM_SUN	to leave the data in broad daylight
			*/
			void		New( const char * filename , _gim_flag crypt );

			/*! \fn		void		gkdp_new_path( char * path_in )
				\brief	With this method you can insert a path in the \a GKDP.
				\param	path_in		The path name that you want to put in a new \a GKDP.
			*/
			void		new_path( const char * path_in );

			/*! \fn		void 		gkdp_write( void )
				\brief	With this method you can write the new \a GKDP.
			*/
			void 		write( void );

			/*! \fn		void 		gkdp_open( char * filename )
				\brief	Use this method to open an existent \a GKDP.
				\param	filename	The file name of the \a GKDP you want to open.
			*/
			void 		open( const char * filename );

			/*! \fn		void 		gkdp_close( void )
				\brief	With this method you can close an opened \a GKDP.
			*/
			void 		close( void );
			
			/*! \fn		_gim_gkdp *	gkdp_take( char * name )
				\brief	With this method you can \a take an item (file) inside a \a GKDP.
				\param	name	The \a item name you want to take from the \a GKDP file.
				\return	A \a _gim_gkdp obj or \a NULL if fail.
			*/
			_gim_gkdp *	take( const char * name );
			
			/*! \fn		void		gkdp_leave( _gim_gkdp * gkdp )
				\brief	With this method you can \a leave an item (file) taken earlier.
				\param	gkdp	The \a _gim_gkdp obj you want to leave.
			*/
			void		leave( _gim_gkdp * gkdp );
			
			/*! \fn		_gim_flag	tmp_extract( _gim_gkdp * to_extract , char * path )
				\brief	With this method you can \a extract an item (file) physically on the disk at the specified \a path.
				\param	to_extract	The \a _gim_gkdp obj you want to extract.
				\param	path		The \a path where you want to extract the \a item.
				\return	\a __GIM_OK on success or \a __GIM_NOT_OK if fail.
			*/
			_gim_flag	tmp_extract( _gim_gkdp * to_extract , const char * path );

		private:	
			
			char *		hread( _gim_map map );
			_gim_flag	hwrite( _gim_handler * out );
		
			char		prog[32];

		
			/*! \fn		void set_manual( void )
				\brief	Use this method in order to inform Gim that the file list will be created manually 
			*/
			void				set_manual( void );

			/*! \fn		void add( char * name , _gim_buffer data , int size )
				\brief	Use this method to add manually a file into the file list
				\param	name	The name of the file you want to add
				\param	data	A buffer contain the the data of file you want to add
				\param	size	The size of the buffer
			*/
			void				add( const char * name , _gim_buffer data , int size );

			/*! \fn		void clear( void )
				\brief	Use this method to clear the file list
			*/
			void				clear( void );

			/*! \fn		void lwrite( _gim_handler * out )
				\brief	Use this method to write the file list into the Gkp file
				\param	out	A pointer to the \a ::_gim_handler of the new Gkp file
			*/
			void				lwrite( _gim_handler * out );

			/*! \fn		_gim_Uint32 make_gkdp( char * dir_name )
				\brief	With this method you can create automatically the file list for a new Gkp file
				\param	dir_name	The name of the directory contain the file to put into a Gkp file
				\return The number of files stored into the file list
			*/
			_gim_Uint32			make_gkdp( const char * dir_name );
			
			/*! \fn		_gim_map lread( _gim_map map )
				\brief	With this method you can create automatically the file list for a new Gkp file
				\param	map		The \a ::_gim_map of the file you want to read previously opened in file mapping.
				\return The _gim_map updated
			*/
			_gim_map			lread( _gim_map map );

			/*! \fn		_gim_gkp_flist * lget( void )
				\brief	This method is useful to get the first member of the file list.
						With this member you can brows the list.
				\return A pointer to the first member of list or \n NULL in case of error
				\see	next( _gim_gkp_flist * tmplist )
			*/
			_gim_gkp_flist *	lget( void );

			/*! \fn		_gim_gkp_flist * next( _gim_gkp_flist * tmplist )
				\brief	This method is useful to get the next member of the file list if exist.
						With this member you can brows the list.
				\param	A pointer to a member of the file list.
				\return A pointer to the next member of list or \n NULL in case of end of list
				\see	get( void )
			*/
			_gim_gkp_flist *	next( _gim_gkp_flist * tmplist );
		
			/*! \fn	void	write_gkdp( _gim_handler * out )
				\brief	Use this to write the content of the files present in the file list into a new gkp file
				\param	out			The pointer to the \a ::_gim_handler of the file you want to write
			*/
			void	write_gkdp( _gim_handler * out );
		
			/*! \fn	void	read_gkdp( _gim_map map )
				\brief	Use this to read the content of the files present in the file list into an existing gkp file
				\param	map		The \a ::_gim_map of the file you want to read previously opened in file mapping. 
			*/
			void	read_gkdp( _gim_map map );
			
			gim_cryptography_obj	* crypt;
		
		private:
			char				file_name[256];
			_gim_handler		* hand;
			void 				init( void );
			void 				release( void );
			_gim_gkp_flist *	add( void );
			void				destroy( void );

			gim_file_header_struct	header;
			gim_file_header_verify	verify;
		
			_gim_gkp_flist 		*startlist;
			_gim_gkp_flist 		*currentlist;
			_gim_gkp_flist 		*endlist;

		public:

			/*!
				gim_gkdp_obj constructor	
			*/
			inline gim_gkdp_obj() {
				crypt		= new gim_cryptography_obj;
				__GIM_CLEAR( file_name , 256  , char );
				hand = NULL;
				__GIM_CLEAR( &header  , 1   , gim_file_header_struct );
				__GIM_CLEAR( &verify  , 1   , gim_file_header_verify );
				header.gim_major = GIM_MAJOR;
				header.gim_minor = GIM_MINOR;
				header.gim_subminor = (_gim_Uint16)GIM_SUBMINOR;
				init();
			};

			/*!
				gim_gkdp_obj destructor	
			*/
			inline virtual ~gim_gkdp_obj() {
				delete crypt;
				release();
			};
	};

#endif /* _GIM_H */
