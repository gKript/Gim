//
// gim_log.h
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

/*!	\file		gim_log.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_LOG_H_
#define _GIM_LOG_H_

	#include "gim_base_header.h"
	#include "gim_kvalue.h"
	#include "gim_environment.h"
	#include "gim_version.h"

	
	#ifdef	__GIM_EXTERNAL_DEBUG
		#define		__GIM_LOG_FORCE		__GIM_YES
	#else
		#ifdef	DEBUG_LEVEL_0
			#define		__GIM_LOG_FORCE		__GIM_YES
		#else
			#define		__GIM_LOG_FORCE		__GIM_NO
		#endif
	#endif
	
	#define		__GIM_LOG_FILENAME		GIM_DEBUG_FILE_NAME

//	--- KIND OF MESSAGES
	#define	__GIM_LOG_FATAL		"-F- "
//  ---

	#define DEBUG_FORMAT_DEFAULT	"$t_$p_$T_$k_$f_|_$m"
	#define DEBUG_FORMAT_COMMENT 	"This is the legend for the log format : \n    #        $t  :  Precision timer\n    #        $p  :  Process ID\n    #        $k  :  Message kind\n    #        $f  :  Function caller\n    #        $m  :  Message\n    #        $v  :  Returned value\n    #        $T  :  Thread flag\n    #        $$  :  The '$' char\n    #         _  :  This char replace the space in this Format."


	#ifndef __GIM_LOG_TYPE 

		enum log_file_type {
			__GIM_LOG_TEXT = 41,
			__GIM_LOG_HTML,
		};
		#define __GIM_LOG_TYPE

	#endif
	
	//~ class gim_personal_log_obj {
		//~ public :
			
		//~ private :
			//~ char	progname[64];
			//~ char	filename[64];
			//~ char	filepath[2048];
			
		//~ public :
	//~ };

	//~ struct	gim_log_list {
		//~ _gim_log		* log;
		//~ gim_log_list	* next;
	//~ };

	//~ struct	gim_log_event {
		//~ pid_t			cur_pid;
		//~ const	char	* t;
		//~ const	char	* func;
		//~ const	char	* mess;
		//~ int				val;
		
		//~ gim_log_event	* next;
	//~ };
	
	/*! \class 		gim_log_obj
		\warning	\n <b>It is not possible to use directly this class.</b>
		\internal
		\brief		Class used by Gim to analyze configuration file
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	1.0-0
		\date		2003-2007
	*/
	class gim_log_obj {
		public :
			/*!	\fn		void start( void )
				\brief	This method is necessary to Gim to start the log system
			*/
			void	start	( void );

			/*!	\fn		void stop( void )
				\brief	This method is used by Gim to stop the log system
			*/
			void	stop	( void );

			/*!	\fn		void addline( const char * , const char * , const char * , int )
				\brief	This method is used by Gim to add a line to the log.
				\param	t		It is the type of a message
				\param	func	It is the name of coller function
				\param	mess 	The message to add
				\param	val		The returned value for this message
			*/
			void		addline ( const char * t , const char * func , const char * mess , int val );
			void		addline	( pid_t cur_pid , const char *t , const char *func , const char *mess , int val );
		
			void		addline_format( const char *t , const char *func , const char *mess , int val );
			void		addline_format( pid_t cur_pid , const char *t , const char *func , const char *mess , int val );
		
			_gim_log	* add_log_system	( const char * program_name , const char * log_file_name );
			_gim_log	* add_log_system	( const char * program_name , const char * log_file_name , const char * log_path_name );
		
			void		add_event ( const char *t , const char *func , const char *mess , int val );
			void		add_event ( pid_t cur_pid , const char *t , const char *func , const char *mess , int val );
		
			_gim_flag	is_open	( void );
			
		private:
			_gim_flag	enabled;
			_gim_flag	initialized;
			char		format[1024];
			FILE		* log_fd;
		
			//~ gim_log_list	* startlist_system;
			//~ gim_log_list	* currentlist_system;

			//~ gim_log_event	* startlist_event;
			//~ gim_log_event	* currentlist_event;
		
		public:

			/*! The constructor
			*/
			inline gim_log_obj() {
				initialized = __GIM_NO;
				enabled		= __GIM_NO;
				log_fd = NULL;
			};

			/*! The destructor
			*/
			inline ~gim_log_obj() {
				if ( initialized == __GIM_YES ) {
					if ( gim_error ) 
						gim_error->set( "gim_log_obj::~gim_log_obj" , "Closing log on destructor" );
					stop();
				}
			};

	};
	
	extern class gim_log_obj	* gim_log;

#endif /* _GIM_LOG_H_ */
