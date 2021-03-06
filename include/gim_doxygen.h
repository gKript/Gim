//
// gim_doxygen.h
//
// Gim  -  Generic Information Manager static library
// Version 2.5-0
// AsYntote - SkyMatrix
//
//  [http://www.gkript.org]
//  [http://phorus.nibbles.it/trac/gkript/milestone/Gim%202.0]
//
/*#############################################################################

	Copyright (C) 2002, 2003 
	Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)
	and
	Adriano Zannoni (Tatti)

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

/*
	This file only serve for the automated documentation from doxygen 
*/

/*!	\file		gim_doxygen.h
	\version	2.5-0
	\brief		This file only serve for the automated documentation from doxygen <BR>
				Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		


/*! \mainpage	
	\brief		<BR><BR><IMG src="Logo_gKript_documentazione.png" width="300" height="75" align="left" border="0"><BR><BR> 
				<center><h1>Gim - Generic Information Manager Documentation</h1><h3 align="center">Version 3.0-x</h3><br></center>
	\section	intro_sec Introduction
		
				Copyright (C) 2002, 2020 
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix) 
				Project under the terms of the GNU General Public License v2 
				[ http://www.gkript.org/gkript-gpl.html ] 
				  
				Target of Gim library is to make easy the development of whichever program. 
				We're developing it during our free time just to have fun and to increase our experience. 
				It's written in C++ language and is released under GPL (GNU GENERAL PUBLIC LICENSE) 
				We really hope that this project, like other similar in this area, could be adopded as a good alternative to the complex and well known algorithms available on the market. 
				  
				Lo scopo della libreria Gim e' rendere facile lo sviluppo di qualsiasi programma. 
				Viene sviluppata nel tempo libero per divertimento e per nostra esperienza. 
				E' scritto in linguaggio C++ e viene rilasciato sotto Licenza GPL (GNU GENERAL PUBLIC LICENSE). 
				Speriamo che questo progetto, non unico nel suo campo, possa essere visto come una possibile e simpatica alternativa ai complessi e conosciutissimi algoritmi dalla disponibilita' e portata di chiunque.  
				
	\section	how_to_use How to use Gim
	
				To use the Gim is simple. 
				It is enough to include gim.h and to declare a gim_obj,  dynamically or statically. 
				To this point it will be sufficient to call the methods of the class. 
				As far as the compilation of your project it is necessary to include the gim library to the linker library list as \a -lgim  
				You can find in \a http://gkript.org some projects uses Gim   
	
	\section	documentation Documentation
				This documentation is made with Doxygen [ http://www.stack.nl/~dimitri/doxygen/index.html ]. 
				The version of this documentation is 3.0-x. If you find error or you think there is need more precision in some point, 
				please write me here [ documentation@gkript.org ].
				
	\section	Example A simple example use Gim
				
	\code
	
#include <stdio.h>
#include <unistd.h>				
#include <gim/gim.h>
int main( int argc , char **argv ) {
	if ( gim_check_version(2,1,0) != __GIM_OK ) {
		printf( "%s\n\n" , gim_version() );
		printf( "Gim is not updated at the required version.\n" );
		printf( "For %s is necessary Gim >= 2.1-0\n" , argv[0] );
		exit( -1 );
	}
	gim_set_application_name( "test" );
	gim_obj * gim = new gim_obj;
	printf( "I am %s on %s\n" , gim->identity->login() , gim->identity->node() );
	delete gim;
	return 0;
}

	\endcode

				Save this source in a file called main.cc and compile with this command :  
				g++ -o test main.cc -lgim  
*/

/*! 
	\example	alias.conf
	\brief		This is a __LEX_A Gim Key/value file example
*/

/*! \example	gim.conf
	\brief		This is a __LEX_B Gim Key/value file example
*/

/*! \example	asyntote.gkk
	\brief		This is an example of a personal key generated by Gim
*/

/*! \example	gkfp.cc
	\brief		This is an example of an application made using Gim. It is the code of the Gkdp utility.
*/

/*! \example	environment.conf
	\brief		This is a __LEX_B Gim Key/value file example
*/
