//
// gim_identity.h
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

/*!	\file		gim_identity.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_BASE_IDENTITY_H
#define _GIM_BASE_IDENTITY_H

	#include "gim_base_header.h"
	#include "gim_lexical.h"
	#include "gim_version.h"

	/*! \struct		gim_processor_struct
		\warning	\n <b>It is not possible to use directly this class.</b>
		\internal
		\brief		This struct contain all informations collected about the host's processor.  Defined as type ::_gim_processor
	*/
	struct gim_processor_struct {
		int 	proc_number;
		char	model_name[256];	/*!<	The model name of the processor	*/
		char	processor[256];		/*!<	The alternative model name of the processor	*/
		char	vendor_id[256];		/*!<	The identity of the processor vendor	*/
		int		cache_size;			/*!<	The size of the processor cache	*/
		float	bogomips;			/*!<	The bogomips of this processor	*/
		float	cpu_mhz;			/*!<	The clock of this processor	*/
		float	est_cpu_mhz;		/*!<	If is not avaible the cpu MHz Gim try to estimate from BogoMips	*/
		int		model;				/*!<	The model of this processor	*/
		int		family;				/*!<	The family of this processor	*/
		int		stepping;			/*!<	The stepping of this processor	*/
		char	strmodel[256];		/*!<	A more datailed string about this processor	*/
		
		gim_processor_struct	* next;	/*!<	pointer to an other eventual processor	*/
	};
	
	/*! \struct		gim_memory_struct
		\warning	\n <b>It is not possible to use directly this class.</b>
		\internal
		\brief		This struct contain all informations collected about the host's memory. Defined as type ::_gim_memory
	*/
	struct gim_memory_struct {
		_gim_Uint32	total;		/*!<	The total mamory installed on the host	*/
		_gim_Uint32	used;		/*!<	The memory used in this moment	*/
		_gim_Uint32	free;		/*!<	It is the memory available for Gim	*/
		_gim_Uint32	cached;		/*!<	The memory cached	*/
		float		ratio;		/*!<	used/free expressed	*/
	};	
	
	
	/*! \def		get_str(field_name,ptr)
		\brief		It is used to get a string parsed from a file
	*/
	#define get_str(field_name,ptr)               \
	  if ( Lexical.str_equal( tmp[0] , field_name ) == __GIM_YES ) { \
		strcpy( ptr , tmp[1] );                   \
		continue;                                 \
	  }
	  
	/*! \def		get_int(field_name,ptr)
		\brief		It is used to get an int parsed from a file
	*/
	#define get_int(field_name,ptr)               \
	  if ( Lexical.str_equal( tmp[0] , field_name ) == __GIM_YES ) { \
/*		printf("dal file [%s , %s]\n" , tmp[0] , tmp[1]); */ \
		ptr = (_gim_Uint32)atoi(tmp[1]);             \
/*		printf("come intero [%s , %d]\n" , tmp[0] , ptr ); */  \
		continue;                                 \
	  }
	  
	/*! \def		get_float(field_name,ptr)
		\brief		It is used to get a float parsed from a file
	*/
	#define get_float(field_name,ptr)             \
	  if ( Lexical.str_equal( tmp[0] , field_name ) == __GIM_YES ) { \
		ptr = atof(tmp[1]);                  \
		continue;                                 \
	  }

	/*! \class 		gim_identity_obj
		\brief		It is a clas with all the informations about user and host 
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	1.0-0
		\date		2003-2007
	*/
	class gim_identity_obj {
		public:		
			_gim_processor			* processor;	/*!<	A pointer to the informations about processor	*/
			_gim_memory				* mi;			/*!<	A pointer to the informations about memory	*/
		
			/*!	\fn		void get( void )
				\brief	Method useful to fill all inner struct with the relative information
			*/
			void	get		( void );

			/*!	\fn		char * login( void )
				\brief	Method useful to get the login name
				\return	the login name
			*/
			char *	login	( void );

			/*!	\fn		char * node( void )
				\brief	Use it to get the host name
				\return	the host name
			*/
			char *	node	( void );

			/*!	\fn		char * version( void )
				\brief	Method useful to get the running kernel version
				\return	the running kernel version
			*/
			char *	version	( void );

			/*!	\fn		char * home( void )
				\brief	Method useful to get the home path
				\return	the home path
			*/
			char *	home	( void );

			/*!	\fn		char *	arch	( void )
				\brief	Method useful to get the architecture of the host
				\return	the architecture of the host
			*/
			char *	arch	( void );
			
			/*!	\fn		int	n_proc( void )
				\brief	Method useful to know how many processor are in the system
				\return	the architecture of the host
			*/
			int		n_proc	( void );

			/*!	\fn		long hostid( void )
				\brief	Method useful to know the processor hostid
				\return	the hostid of the processor
			*/
			long	hostid	( void );

		private:
	friend	class gim_home_obj;
	friend	class gim_private_key_obj;
	friend	class gim_memory_obj;
	friend	class gim_obj;
	friend	class gim_gkl_obj;

			int		User_id;
			char	Login[256];
			char	Real_name[256];
			char	System_name[256];
			char	Node_name[256];
			char	Release[256];
			char	Version[256];
			char	Arch[256];
			char	Home[256];
			int		namemax;
			long	host_id;
			int		n_processors;
			float	tot_bogomips;
			volatile _gim_flag	filled;
			
			void				init( void );
			void 				release( void );
			_gim_memory *		memory(void);
			_gim_processor *	cpu( void );
			void				cpu_strfamily( _gim_processor * processor );
			void 				get_fs_stat( void );
	
		public:
			/* the constructor
			*/
			inline gim_identity_obj() {
				host_id = gethostid();
				init();
			};
			
			/* the destructor
			*/
			inline ~gim_identity_obj() {
				release();
			};
	};
	
	extern gim_identity_obj	* gim_identity;
	extern struct statvfs	vfsbuf;

#endif /* _GIM_BASE_IDENTITY_H */
