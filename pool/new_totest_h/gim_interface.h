//
// gim_interface.h
//
// Gim  -  Generic Information Manager static library
// Version 2.0-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.0?by=version]
//
/*#############################################################################

	Copyright (C) 2002, 2007 
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

/*!	\file		gim_interface.h
	\version	2.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://gkript.org/gk_gpl_v2.php ]
*/

#ifndef _GIM_INTERFACE_OBJ_H_
#define _GIM_INTERFACE_OBJ_H_

	#define	 GET_LOAD_AVG   true

	#include "gim_base_header.h"

	/*!	\struct		gim_interface_data
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is used by Gim to manage the interface data.
	*/
	struct gim_interface_data {
		// data
		char		item_sou_name[2048];		/*!<	This field contains the name, if any, of a buffer or source file is processed. */
		char		item_dst_name[2048];		/*!<	This field contains the name, if any, of a buffer or destination file is processed. */
		char		item_dna_name[2048];		/*!<	This field contains the name, if any, of a buffer or dna file is processed. */
		_gim_Uint32	sou_size;					/*!<	This field contains the size, if any, of a buffer or source file is processed. */
		_gim_Uint32	sou_position;				/*!<	This field contains the current position, if any, of a buffer or destination file is processed. */
		float		sou_ratio;					/*!<	This field contains the ratio between size and position, if any, of a buffer or destination file is processed. */
		_gim_Uint8	sou_step;					/*!<	This field contains a processing step on a buffer or file source that is processed. */
		_gim_Uint32	dst_size;					/*!<	This field contains the size, if any, of a buffer or destination file is processed. */
		_gim_Uint32	dst_position;				/*!<	This field contains the current position, if any, of a buffer or destination file is processed. */
		float		dst_ratio;					/*!<	This field contains the ratio between size and position, if any, of a buffer or destination file is processed. */
		_gim_Uint8	dst_step;					/*!<	This field contains a processing step on a buffer or file destination that is processed. */
		_gim_Uint32	dna_size;					/*!<	This field contains the size, if any, of a buffer or dna file is processed. */
		_gim_Uint32	dna_position;				/*!<	This field contains the current position, if any, of a buffer or dna file is processed. */
		float		dna_ratio;					/*!<	This field contains the ratio between size and position, if any, of a buffer or dna file is processed. */
		_gim_Uint8	dna_step;					/*!<	This field contains a processing step on a buffer or file dna that is processed. */
		// memory
		_gim_Uint32	mem_allocated;				/*!<	This field contains the amount of allocated memory */
		_gim_Uint32	mem_allocated_peek;			/*!<	This field contains the maximum peek of allocated memory */
		float		mem_allocated_peek_ratio;	/*!<	This field contains the ratio between peek and maximum allocable memory */
		_gim_Uint32	mem_max_allocable;			/*!<	This field contains the maximum allocable memory */
		float		mem_ratio;					/*!<	This field contains the ratio between free and allocated memory */
		// cpu load avarage
		//   om  = one minute
		//   fm  = five minutes
		//   ftm = fiveteen minutes
		double		load_avg_om;				/*!<	This field shows the system load in the last minute */
		double		load_avg_fm;				/*!<	This field shows the system load in the last five minute */
		double		load_avg_ftm;				/*!<	This field shows the system load in the last fiveteen minute */
		double		cpu_user;					/*!<	This field contain the user load in this moment */
		double		cpu_nice;					/*!<	This field contain the nice load in this moment */
		double		cpu_system;					/*!<	This field contain the system load in this moment */
		double		cpu_idle;					/*!<	This field contain the idle load in this moment */
		double		cpu_total;
		double		old_cpu_user;				/*!<	This field contain the lsat readed user load */
		double		old_cpu_nice;				/*!<	This field contain the lsat readed user load */
		double		old_cpu_system;				/*!<	This field contain the lsat readed user load */
		double		old_cpu_idle;				/*!<	This field contain the lsat readed user load */
		double		old_cpu_total;
	};
	
	/*! \class 		gim_interface_obj
		\brief		With this class it is possible to know the state of Gim during some operations.
		\author		Danilo Zannoni  -  asyntote@gkript.org
		\version	2.0-0
		\date		2003-2008
	*/
	class gim_interface_obj {
		public:
			/*! \fn		_gim_flag check ( void )
				\brief	With this method you can if you need to update the interface. This metho campare the two internal interfaces \a data and \a backup.
				\return	\a __GIM_YES if the interface is changed, \a __GIM_NO if interface has not changed.
			*/
			_gim_flag			check			( void );

			/*! \fn		_gim_flag check ( gim_interface_data * to_check )
				\brief	With this method you can if you need to update the interface. This metho campare the internal interface \a data with a your personal interface.
				\param	to_check	Yor personal interface.	  
				\return	\a __GIM_YES if the interface is changed, \a __GIM_NO if interface has not changed.
			*/
			_gim_flag			check			( gim_interface_data * to_check );

			/*! \fn		gim_interface_data * get( void )
				\brief	With this method you can obtain a pointer to the internal interface \a data.
				\return	The pointer to the internal interface \a data. This method cannot return \a NULL.
			*/
			gim_interface_data	* get			( void );

			/*! \fn		void clear ( void )
				\brief	With this method you can clear the two internal interfaces \a data and \a backup.
			*/
			void				clear			( void );

			/*! \fn		void clear ( gim_interface_data * to_clear )
				\brief	With this method you can clear yor personal interface.
				\param	to_clear	Yor personal interface to clear.	  
			*/
			void				clear			( gim_interface_data * to_clear );

			/*! \fn			void update_load_avg ( void )
				\brief		With this method you can update the value of the system load of the internal interface.
				\warning 	<b>The system load is NOT automatically updated.</b>
			*/
			void				update_load_avg	( void );

			/*! \fn		double get_lavg_om ( void )
				\brief	With this method you can obtain the system load of the last minute.
				\return	The system load of the last minute.
			*/
			double				get_lavg_om		( void );

			/*! \fn		double get_lavg_fm ( void )
				\brief	With this method you can obtain the system load of the last five minutes.
				\return	The system load of the last five minutes.
			*/
			double				get_lavg_fm		( void );

			/*! \fn		double get_lavg_ftm ( void )
				\brief	With this method you can obtain the system load of the last fiveteen minutes.
				\return	The system load of the last fiveteen minutes.
			*/
			double				get_lavg_ftm	( void );

			void				init_perc_cpu_load( void );
			
		private:
	friend	class	gim_memory_obj;
	friend	class	gim_cryptography_obj;
	friend	class	gim_gkdp_obj;
	friend	class	gim_gkfp_obj;
		
			gim_interface_data	* data;
			gim_interface_data	* backup;
		
			void				align		( void );
	
		public:
			/*!	gim_interface_obj constructor
			*/
			gim_interface_obj();
				
			/*!	~gim_interface_obj destructor
			*/
			 ~gim_interface_obj();
	};
	
	extern gim_interface_obj	* gim_interface;

#endif	//_GIM_INTERFACE_OBJ_H_
