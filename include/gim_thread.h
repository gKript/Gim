//
//  gim_thread.h
//
//  Gim  -  Generic Information Manager
//  A background thread that the program starts and stops explicitly.
//  Gim never starts a thread by itself: a thread exists only when your program calls start().
//
/*#############################################################################

	This file is part of Gim library.

	Gim is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Gim is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

#############################################################################*/

#ifndef _GIM_THREAD_OBJ_H_
#define _GIM_THREAD_OBJ_H_

	#include <thread>
	#include <atomic>
	#include <functional>
	#include "gim_custom_type.h"

	/*!	\class		gim_thread_obj
		\brief		Runs a function in a background thread.

					The function receives the object itself and must check stop_requested() in its loop,
					so that request_stop() and the destructor can end it.
					An exception thrown by the function is caught and reported with gim_error.
					The destructor requests the stop and waits for the thread: no thread is left running.

					Do not call join() from inside the thread itself.
	*/
	class gim_thread_obj {
		public:
			/*!	Builds an idle thread object. No thread is started. */
			gim_thread_obj();

			/*!	Requests the stop and waits for the thread to finish. */
			~gim_thread_obj();

			/*!	Starts the thread.
				\param	body	the function run by the thread; it must return when stop_requested() is true
				\return	__GIM_OK if started, __GIM_NOT_OK if a thread is already running
			*/
			_gim_flag		start			( std::function< void( gim_thread_obj & ) > body );

			/*!	Asks the thread to stop. It does not wait: call join() to wait. */
			void			request_stop	( void );

			/*!	Tells the thread whether it must stop. Use it inside the function.
				\return	true if request_stop() was called
			*/
			bool			stop_requested	( void ) const;

			/*!	Tells whether the thread is still running.
				\return	__GIM_YES or __GIM_NO
			*/
			_gim_flag		is_running		( void ) const;

			/*!	Waits for the thread to finish, then releases it. Does nothing if no thread was started. */
			void			join			( void );

		private:
			std::thread			* th;
			std::atomic<bool>	stop_flag;
			std::atomic<bool>	running;

			gim_thread_obj( const gim_thread_obj & );
			gim_thread_obj & operator=( const gim_thread_obj & );
	};

#endif	//_GIM_THREAD_OBJ_H_
