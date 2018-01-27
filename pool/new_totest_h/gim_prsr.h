//
// gim_prsr.h
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

/*!	\file		gim_prsr.h
	\version	1.0-0
	\brief		Copyright (C) 2002, 2003<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v2<BR>
				[ http://www.gkript.org/gkript-gpl.html ]
*/ 		

#ifndef _GIM_PRSR_H_
#define _GIM_PRSR_H_

	/*!	\struct		prsr
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is the main node (title) of a configuration file in memory
	*/
	struct prsr {
		char		filename[64];	/*!<	The name on disk of the configuration file	*/
		char		title[64];		/*!<	The title of the configuration file	*/
		char		comment[10240];	/*!<	The comment */
		FILE		* fp;			/*!<	When opened this is the file pointer	*/
		
		struct	prsr_kvalue_section	* first_section;	/*!<	If it is not \a NULL it is the first section of the configuration file */
	};

	/*!	\struct		prsr_kvalue_section
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is the section node (section) of a configuration file in memory
	*/
	struct prsr_kvalue_section {
		char	section_name[64];	/*!<	The name of this section	*/
		char		comment[10240];		/*!<	The comment */
		_gim_flag	comment_position;	/*!<	The comment position */
		struct prsr_kvalue_field	* first_field;	/*!<	If it is not \a NULL it is the first key in section	*/
		struct prsr_kvalue_section	* next_section;	/*!<	If it is not \a NULL it is the next section	*/
	};

	/*!	\struct		prsr_kvalue_field
		\warning	\n <b>It is not possible to use directly this struct.</b>
		\internal
		\brief		This struct is tha key node (key) of a configuration file in memory
	*/
	struct prsr_kvalue_field {
		char		key[32];			/*!<	The nema of the key	*/
		_gim_flag	key_type;			/*!<	The type of the key	*/
		int			int_value;			/*!<	The int value	*/
		char		str_value[64];		/*!<	The string value */
		char		comment[10240];		/*!<	The comment */
		_gim_flag	comment_position;	/*!<	The comment position */
		struct prsr_kvalue_field	* next_field;	/*!<	If it is not \a NULL it is the next key of the section	*/
	};

#endif /* _GIM_PRSR_H_ */
