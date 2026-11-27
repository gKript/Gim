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

	Copyright (C) 2002-2026 
	Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)
	and
	Adriano Zannoni (Tatti)

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

/*
	This file only serve for the automated documentation from doxygen 
*/

/*!	\file		gim_doxygen.h
	\version	2.5-0
	\brief		This file only serve for the automated documentation from doxygen <BR>
				Copyright (C) 2002-2026<BR>
				Danilo Zannoni (AsYntote) - Corrado Tumiati (SkyMatrix)<BR>
				Released under the terms of the GNU General Public License v3<BR>
				[ https://www.gnu.org/licenses/gpl-3.0.html ]
*/

/*!	\mainpage	Gim - Generic Information Manager

	\section	gimdoc_home_intro	What is Gim

	Gim is a C++ library that makes the development of any kind of program easier: memory, files, configuration,
	logs, databases, cryptography and more, behind one object. A set of command line tools is built on top of it.

	Copyright (C) 2002-2026 - 2026 Danilo Zannoni (AsYntote) and Corrado Tumiati (SkyMatrix). \n
	Released under the terms of the \ref gimdoc_license.

	\section	gimdoc_home_start	Start here

	- \subpage	gimdoc_start
	- \subpage	gimdoc_language

	\section	gimdoc_home_classes	The library

	- \subpage	gimdoc_classes

	\section	gimdoc_home_tools	Tools and files

	- \subpage	gimdoc_tools
	- \subpage	gimdoc_files

	\section	gimdoc_home_project	The project

	- \subpage	gimdoc_project

	The points of this documentation that still have to be written are collected in the \ref todo.
*/


/*!	\page	gimdoc_start	Getting started

	\tableofcontents

	\section	gimdoc_start_build	Building Gim with CMake

	The CMake project builds the static library and, unless GIM_BUILD_TOOLS is OFF, the command line tools.
	It needs CMake 3.16 or later and a C++14 compiler.

	\code{.sh}
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build
	cmake --install build
	\endcode

	The headers are installed in the folder \c gim of the include path, so a program includes \c <gim/gim.h>.

	\todo	Getting started: describe the autotools build on Linux and the MinGW and MSVC builds on Windows.

	\section	gimdoc_start_first	A first program

	Include \c gim.h, create one ::gim_obj and call the methods of its members.

	\code{.cpp}
	#include <stdio.h>
	#include <stdlib.h>
	#include <gim/gim.h>

	int main( int argc , char ** argv ) {
		if ( gim_check_version( 2 , 9 ) != __GIM_OK ) {
			printf( "%s\n\n" , gim_version() );
			printf( "For %s is necessary Gim >= 2.9\n" , argv[0] );
			exit( -1 );
		}
		gim_set_application_name( "test" );
		gim_obj * gim = new gim_obj;
		printf( "I am %s on %s\n" , gim->identity->login() , gim->identity->node() );
		delete gim;
		return 0;
	}
	\endcode

	\section	gimdoc_start_link	Compiling and linking

	\code{.sh}
	g++ -o test main.cc -lgim
	\endcode

	The library uses threads: where the compiler does not link them by itself, add \c -pthread.

	\section	gimdoc_start_next	Where to go next

	- \ref gimdoc_language
	- \ref gimdoc_classes
	- \ref gimdoc_files_conf
*/


/*!	\page	gimdoc_language	The language of Gim

	\tableofcontents

	\section	gimdoc_language_return	Return values

	Most methods return a ::_gim_flag that holds one of the constants of ::_gim_return_value.

	\warning	Always compare with the named constant. \c __GIM_NO is 0 but \c __GIM_OK is not, and \c __GIM_ERROR is -1:
				tests like <tt>if ( f() )</tt> or <tt>if ( !f() )</tt> do not tell success from failure.

	\code{.cpp}
	if ( gim_check_version( 2 , 9 ) == __GIM_OK ) {
		// right
	}
	\endcode

	\section	gimdoc_language_types	Types

	| Type | C type |
	|------|--------|
	| \c _gim_flag | char |
	| \c _gim_Uchar, \c _gim_Uint8 | unsigned char |
	| \c _gim_int8 | char |
	| \c _gim_int16 / \c _gim_Uint16 | short / unsigned short |
	| \c _gim_int32 / \c _gim_Uint32 | int / unsigned int |
	| \c _gim_long / \c _gim_Ulong | int / unsigned int |
	| \c _gim_Llong / \c _gim_ULlong | long long / unsigned long long |
	| \c _gim_address | unsigned long long |
	| \c _gim_buffer / \c _gim_Ubuffer | char * / unsigned char * |

	\section	gimdoc_language_alias	Short names of the classes

	| Short name | Class |
	|------------|-------|
	| \c _gim_kvalue, \c _gim_prsr | \ref gim_prsr_obj |
	| \c _gim_string | \ref gim_string_obj |
	| \c _gim_list | \ref gim_list_obj |
	| \c _gim_file | \ref gim_file_obj |
	| \c _gim_time | \ref gim_time_obj |
	| \c _gim_html | \ref gim_html_obj |
	| \c _gim_checksum | \ref gim_checksum_obj |
	| \c _gim_rand | \ref MTRand |
	| \c _gim_gkfp | \ref gim_gkfp_obj |

	\section	gimdoc_language_objects	The members of gim_obj

	\c error, \c memory, \c conf, \c compatibility, \c alias, \c environ, \c identity, \c interface, \c home,
	\c file_manager, \c key, \c checksum, \c crypt. See \ref gim_obj.

	\todo	Language: naming rules of classes and methods, and the rules about who frees what.
*/


/*!	\page	gimdoc_classes	The classes

	The classes are grouped by what they are for.

	- \subpage	gimdoc_classes_core
	- \subpage	gimdoc_classes_data
	- \subpage	gimdoc_classes_system
	- \subpage	gimdoc_classes_security
	- \subpage	gimdoc_classes_picgim
	- \subpage	gimdoc_classes_other

	\section	gimdoc_classes_threads	Using the classes with threads

	What has been checked by reading the source. A class that is not in this table has not been checked:
	read its source before using it from more than one thread.

	| Class | Thread-safe |
	|-------|-------------|
	| \ref gim_memory_obj | yes |
	| \ref gim_error_obj | yes |
	| \ref gim_file_manager_obj | yes |
	| \ref gim_file_obj | yes, for separate instances |
	| \ref gim_thread_obj | one object, one thread |
	| \c gim_md5_obj | yes, when every thread uses its own context |
	| \ref gim_prsr_obj | \b no: see \ref gimdoc_kv_threads |
	| \ref gim_db_obj | \b no: shared buffers, see \ref gimdoc_db_threads |
	| \ref prsr_lexical_class | \b no: shared global buffers |
	| \ref gim_time_obj | separate instances only; get_usec() and process() return a shared static buffer: use get_usec_buf(), process_buf() and date_buf() (thread-safe, not yet tested) |
	| \ref gim_checksum_obj | \b no: md5() returns a static buffer |
	| \ref gim_cryptography_obj | derive_key(), apply_stream() and apply_stream_parallel(): yes; advanced_password_nonce(): \b no |
*/


/*!	\page	gimdoc_classes_core	Core classes

	\tableofcontents

	\section	gimdoc_core_obj	gim_obj
	The main object: creates and owns the other core objects. Reference: \ref gim_obj

	\section	gimdoc_core_memory	Memory
	Reference: \ref gim_memory_obj
	- \subpage	gimdoc_memory

	\section	gimdoc_core_error	Errors and log
	Reference: \ref gim_error_obj, \ref gim_log_obj
	- \subpage	gimdoc_error

	\section	gimdoc_core_interface	Interface
	Data about the running system, such as CPU, memory and load. Reference: \ref gim_interface_obj
	\todo	Interface: page with purpose, example and sampling.

	\section	gimdoc_core_thread	Threads
	Runs a function in a background thread. The permission of each class is in the \c thread section of \ref gimdoc_files_conf.
	Reference: \ref gim_thread_obj
	\todo	Threads: page with an example.
*/


/*!	\page	gimdoc_classes_data	Data classes

	\tableofcontents

	\section	gimdoc_data_string	Strings
	Reference: \ref gim_string_obj
	- \subpage	gimdoc_string

	\section	gimdoc_data_list	Lists
	A list of generic pointers; items are numbered from 1. Reference: \ref gim_list_obj
	- \subpage	gimdoc_list

	\section	gimdoc_data_kvalue	Key/value files
	Reads, creates, changes and writes configuration files. Reference: \ref gim_prsr_obj
	- \subpage	gimdoc_kvalue

	\section	gimdoc_data_lexical	Lexical analysis
	The analyzer used by the key/value and database classes. Reference: \ref prsr_lexical_class, \ref SyntaxCoerence_obj
	\todo	Lexical analysis: page with the tokens.

	\section	gimdoc_data_db	Database
	A light database of tables, fields and records. Scripts are described in \ref gimdoc_files_gdbs, the
	interactive use in \ref gimdoc_tool_gimshell. Reference: \ref gim_db_obj
	- \subpage	gimdoc_db

	\section	gimdoc_data_ascii	ASCII buffers
	Reference: \ref gim_ascii_file_obj
	\todo	ASCII buffers: page with purpose and example.
*/


/*!	\page	gimdoc_classes_system	System classes

	\tableofcontents

	\section	gimdoc_system_file	Files
	Reference: \ref gim_file_obj, \ref gim_file_manager_obj

	\warning	load() allocates a buffer that close() already frees: calling unload() after close() is a double free.

	- \subpage	gimdoc_file

	\section	gimdoc_system_dir	Directories
	Work with the directories of a file system. Reference: \ref gim_directory_obj
	\todo	Directories: page with an example.

	\section	gimdoc_system_env	Environment
	Manages the Gim environment (home folder and configuration) automatically. Reference: \ref gim_home_obj
	\todo	Environment: page with the folders created on Linux and Windows.

	\section	gimdoc_system_identity	Identity
	Information about the user and the host. Reference: \ref gim_identity_obj
	\todo	Identity: page with the list of the available data.

	\section	gimdoc_system_time	Time
	A timer with a resolution up to the microsecond. Reference: \ref gim_time_obj
	\todo	Time: page with an example.

	\section	gimdoc_system_getopt	Command line options
	Parses the command line options and keeps the valid ones and the errors. Reference: \ref gim_getopt_obj
	Page with an example, the status values and the warnings: \ref gimdoc_getopt

	\section	gimdoc_system_serial	Serial port
	8 data bits, no parity, 1 stop bit, no flow control, raw mode. Reference: \ref gim_serial_obj
	\todo	Serial port: page with an example and the background reading.

	\section	gimdoc_system_gpio	Raspberry GPIO
	Exports a pin as input or output, then reads or writes it. Reference: \ref gim_rpi_gpio_obj
	\todo	Raspberry GPIO: page with an example.
*/


/*!	\page	gimdoc_classes_security	Security classes

	\tableofcontents

	\section	gimdoc_security_checksum	Checksums
	Digests of everything. Reference: \ref gim_checksum_obj
	\todo	Checksums: page with the algorithms and an example.

	\section	gimdoc_security_crypt	Cryptography
	Encrypts and decrypts. Reference: \ref gim_cryptography_obj
	\todo	Cryptography: page with the method, its limits and an example.

	\section	gimdoc_security_password	Passwords
	Reference: \ref gim_password_obj
	\todo	Passwords: page with purpose and example.

	\section	gimdoc_security_key	Private keys
	Reference: \ref gim_private_key_obj
	\todo	Private keys: page with the key file and how to create one.

	\section	gimdoc_security_gkp	GKP packages
	The packages made by \ref gimdoc_tool_gkfp and \ref gimdoc_tool_gkdp.
	Reference: \ref gim_gkfp_obj, \ref gim_gkdp_obj
	\todo	GKP packages: page with the format.

	\section	gimdoc_security_random	Random numbers
	Mersenne Twister generator. Reference: \ref MTRand
	\todo	Random numbers: page with an example.
*/


/*!	\page	gimdoc_classes_picgim	AI and PicGIM classes

	\tableofcontents

	\section	gimdoc_picgim_train	Trainer of the integer model
	Used by \ref gimdoc_tool_gimaitrain. Reference: \ref gim_ai_train.h
	\todo	Trainer: page with the dataset and the generated header.

	\section	gimdoc_picgim_obj	PicGIM
	Reference: \ref gim_picgim_obj, \ref gim_picgim_gkh32_obj
	\todo	PicGIM: page with purpose and example.
*/


/*!	\page	gimdoc_classes_other	Other classes

	\tableofcontents

	\section	gimdoc_other_html	HTML pages
	Reference: \ref gim_html_obj
	\todo	HTML pages: page with an example.

	\section	gimdoc_other_notyet	Not yet part of Gim
	The GKL and CNC classes are documented only for completeness: no Gim header includes them yet.
	Reference: \ref gim_gkl_obj, \ref gim_cnc_obj
*/


/*!	\page	gimdoc_tools	The tools

	Gim comes with command line tools built on the library. They are built with CMake (option GIM_BUILD_TOOLS) and
	installed in the bin folder. Every tool prints its usage with an unknown option or without its arguments.

	| Tool | What it does |
	|------|--------------|
	| \ref gimdoc_tool_gkfp | creates and extracts GKP packages of a folder |
	| \ref gimdoc_tool_gkdp | creates a GKP package with a folder inside, and checks it with gimstat |
	| \ref gimdoc_tool_gimstat | statistical tests on a file, and its GKaos score |
	| \ref gimdoc_tool_gkmake | builds and installs a project, with the configuration saved per project |
	| \ref gimdoc_tool_gimshell | interactive shell for the Gim databases |
	| \ref gimdoc_tool_gimaitrain | trains the integer model used by PicGIM |

	- \subpage	gimdoc_tool_gkfp
	- \subpage	gimdoc_tool_gkdp
	- \subpage	gimdoc_tool_gimstat
	- \subpage	gimdoc_tool_gkmake
	- \subpage	gimdoc_tool_gimshell
	- \subpage	gimdoc_tool_gimaitrain
*/


/*!	\page	gimdoc_tool_gkfp	gkfp

	Creates a GKP package from a folder, or extracts a package. Source: gim_gkfp/gkfp.cc.

	\section	gimdoc_gkfp_purpose	What it is for

	\b gkfp packs a folder tree into one file, the GKP package, and unpacks it again. A GKP file is a container: a header
	(its first five bytes say whether it is a GKFP or a GKDP package, the program that wrote it, the user name, the number
	of members and two flags for compression and encryption), followed by the members, each one preceded by a small record
	that checks the name and the size of the file it describes.

	Use \b gkfp when you need to move or store a set of files as one object, with the option to protect it with a password.
	Packing with a password encrypts every member with the cipher of \ref gim_cryptography_obj, so the package can be
	stored or sent without its content being readable. Without a password the package is only a container: anyone can
	extract it.

	The packing and the unpacking go through \ref gim_gkfp_obj. The member files are read and written with
	\ref gim_file_manager_obj, which keeps every file open through one lock, and the progress of a large package is
	shown on the screen as a bar with the estimated time, unless -q is used.

	Typical cases: archiving a project before a change, moving a configuration folder to another machine, delivering
	the files of a product in a form that can be checked later. Extracting the package in an empty folder reproduces the
	folder tree of the source.

	Limits: the package is not compressed in the documented flags, so its size is close to the size of the content. For a
	package that must be proved strong, see \ref gimdoc_tool_gkdp.

	\section	gimdoc_gkfp_usage	Usage
	\verbatim
	gkfp [-q] -c out.gkp path [password]
	gkfp [-q] -x in.gkp [password]
	gkfp [-q] file.gkp [password]
	\endverbatim

	\section	gimdoc_gkfp_options	Options
	| Option | Meaning |
	|--------|---------|
	| -c | create the package from \c path |
	| -x | extract the package into the current folder |
	| -q | quiet: no screen output; the log stays complete |

	\c -c and \c -x together are not allowed. Without them, an existing package is extracted; otherwise a new one is created.
	With a \c password the package is encrypted. \c \b gkfp exits with -1 when the arguments are wrong.

	\section	gimdoc_gkfp_conf	Configuration
	Section \c gkfp of \ref gimdoc_files_conf : \c skip_gimstat (YES skips the check of the package) and \c quiet.

	The source is an \ref gkfp.cc "example" of a program made with Gim.

	\section	gimdoc_gkfp_source	Source: gim_gkfp/gkfp.cc
	\htmlonly <details><summary>show the source code of gim_gkfp/gkfp.cc</summary> \endhtmlonly
	\include gim_gkfp/gkfp.cc
	\htmlonly </details> \endhtmlonly
*/


/*!	\page	gimdoc_tool_gkdp	gkdp

	Creates a GKP package with a folder inside, and checks the result with \ref gimdoc_tool_gimstat. Source: gim_gkdp/gkdp.cc.

	\section	gimdoc_gkdp_purpose	What it is for

	gkdp creates a GKDP package from a folder, the data variant of the GKP container described in \ref gimdoc_tool_gkfp.
	Its difference from gkfp is the check at the end: when the package is encrypted, gkdp runs \ref gimdoc_tool_gimstat
	on it and reads the GKaos score, a number from 0 to 100 that tells how much the content looks like random data.

	Use \b gkdp when the package is meant to hide its content and the password must be strong enough to do so. An
	encrypted package whose content still has a recognizable structure (repeated blocks, text, unused areas) gets a low
	GKaos score: \b gkdp then warns that the content could be violated and suggests to repeat the command with a password,
	because the encryption is not hiding that structure enough.

	The threshold is 55. Below it the warning is printed on the screen and written to the log as a warning. Between 55
	and 100 the package is accepted without comments. A very small package is not evaluated: the statistical tests need
	a minimum amount of data, and the program says so instead of giving a number.

	The check runs \b gimstat through the PATH, so \b gimstat must be installed where \b gkdp can find it. It is skipped with
	\c skip_gimstat = YES in the configuration of \b gkdp, and it never runs on a plain package, which has no content to hide.

	Typical cases: packaging the data of a device or of a test before sending it, and checking that the package is not
	trivially readable. The check does not prove the encryption is strong: it only verifies that the result looks random
	to the tests of gimstat. For the cipher itself see \ref gim_cryptography_obj.

	\section	gimdoc_gkdp_usage	Usage
	\verbatim
	gkdp [-q] GKP_file_name path [password]
	\endverbatim

	\section	gimdoc_gkdp_flow	What it does
	- Creates the package from \c path. With a \c password the content is encrypted; without it, the package is plain.
	- Only an encrypted package is checked: it runs \c \b gimstat on the package, which must be in the PATH. A plain package is not checked.
	- With a GKaos score below 55 it prints a warning and suggests to repeat the command with a password.
	- When the file is too small for the analysis, the GKaos is not evaluated.

	\section	gimdoc_gkdp_options	Options
	| Option | Meaning |
	|--------|---------|
	| -q | quiet: no banner and no status on screen |

	\section	gimdoc_gkdp_conf	Configuration
	Section \c gkdp of \ref gimdoc_files_conf : \c skip_gimstat (YES skips the check) and \c quiet.

	\section	gimdoc_gkdp_exit	Exit codes
	0 at the end; -1 when the arguments are wrong or Gim is too old.

	\section	gimdoc_gkdp_source	Source: gim_gkdp/gkdp.cc
	\htmlonly <details><summary>show the source code of gim_gkdp/gkdp.cc</summary> \endhtmlonly
	\include gim_gkdp/gkdp.cc
	\htmlonly </details> \endhtmlonly
*/


/*!	\page	gimdoc_tool_gimstat	gimstat

	Statistical tests on a file, with a score called GKaos (0 to 100). Source: gim_stat/gimstat.cc.

	\section	gimdoc_gimstat_purpose	What it is for

	\b gimstat measures how random a file looks. It runs a series of statistical tests on the bytes of the file and combines
	their results into a single score, the GKaos, from 0 to 100. A high score means the bytes have no visible structure:
	the distribution of the values is even, the bits are balanced, the neighbouring bytes are not correlated, and the
	binary patterns look like those of a random source. A low score means the file has structure: text, repeated blocks,
	long runs of zeros, or a sequence generated by a simple formula.

	Use \b gimstat to check the output of an encryption or of a random generator, to find out whether a file contains
	unencrypted parts, or to compare two versions of a file. It is also the check that \ref gimdoc_tool_gkdp runs on every
	encrypted package.

	The tests are the ones in the source: a chi-square test on the 256 byte values, a monobit test on each bit position,
	the serial correlation at several lags, the rank of binary matrices built from 32-bit words, and others. Each test
	prints its p-value and PASS or FAIL; the tests with many sub-tests use a Bonferroni threshold, so a single FAIL in a
	large family is not a false alarm.

	For big files (more than 16 MB) the program also divides the file into blocks and reports the GKaos of the blocks: the
	whole-file score is then only informative, because one structured area can be hidden inside a random file. The
	block map shows where the structure is, one character for each block of a 10 x 10 grid.

	The score is a heuristic: a high score does not prove security, and a low score does not prove the content is
	secret. Use it as a first check, and read the individual tests when the score is in doubt.

	\section	gimdoc_gimstat_usage	Usage
	\verbatim
	gimstat filename
	gimstat -q filename
	\endverbatim

	\section	gimdoc_gimstat_out	Output
	The program prints the chaos parameters (extension, distribution, levelling), the results of the tests and the GKaos score.
	With \c -q only the GKaos line is printed, on one line. The log is always complete.

	For files bigger than 16 MB the verdict is the GKaos of the blocks: the score of the whole file is only informative.

	\section	gimdoc_gimstat_map	The map of the blocks
	The file is shown as a 10 x 10 map, in file order. The character of a block is its score:
	\verbatim
	' ' 90-100   '.' 80-90   ':' 70-80   '-' 60-70   '=' 50-60
	'+' 40-50    '*' 30-40   '#' 20-30   '%' 10-20   '@' 0-10
	\endverbatim

	\section	gimdoc_gimstat_exit	Exit code
	The exit code is the GKaos score. \ref gimdoc_tool_gkdp uses it: a score below 55 is a warning.

	\todo	gimstat: the list of the tests and how each one contributes to the score.

	\section	gimdoc_gimstat_source	Source: gim_stat/gimstat.cc
	\htmlonly <details><summary>show the source code of gim_stat/gimstat.cc</summary> \endhtmlonly
	\include gim_stat/gimstat.cc
	\htmlonly </details> \endhtmlonly
*/


/*!	\page	gimdoc_tool_gkmake	gkmake

	Builds, cleans and installs a project. The options of the project are saved in the configuration of \b gkmake
	(\c .gkmake_conf in the project folder, and the build folders in \c ~/.gim/gKmake/build.conf).

	\tableofcontents

	Source: gkmake/main.cc.

	\section	gimdoc_gkmake_purpose	What it is for

	\b gkmake builds a project from its source folder, without writing a Makefile by hand. It remembers the options of the
	project in a configuration file, so the same command can be repeated on every build, and it keeps a count of the
	builds and of the successful ones. It can build on several cores, clean, install with or without sudo, make a tarball
	of the sources, build the documentation, and launch the autotools when the project uses them.

	Use \b gkmake for the Gim projects and the programs made with them: the same commands work for every project, and the
	configuration stays in the project folder. The options marked as saved in the table below are written
	to the configuration file, so they do not need to be typed again.

	The project name, the author and the prefix are used in the generated header file, when the project asks for one.
	The build folder of each project is recorded in the user configuration, so a build can be cleaned or reinstalled
	without remembering where it was made.

	Typical cases: rebuilding a library after every change, installing the same program on several machines, producing the
	tarball of a release, and generating the documentation with the same command every time. Options that change the
	installation (-i, -s) act on the system, so read them before using them.

	Limits: \b gkmake drives the native build tools. It does not replace them: when a project needs an unusual compiler
	option, the option belongs to the project configuration of the build tool, not to gkmake.

	\section	gimdoc_gkmake_usage	Usage
	\verbatim
	gkmake [options]
	\endverbatim
	Run it in the folder of the project. The options marked "saved" are kept in the project configuration.

	\section	gimdoc_gkmake_project	Project description
	| Option | Meaning |
	|--------|---------|
	| -R | delete the configuration file |
	| -N name | project name |
	| -A name | author's name |
	| -P prefix | prefix of the project |
	| -I path | include path |
	| -H YES/NO | generate the project header file |
	| -F name | header file name |
	| -E command | editor command |
	| -D path | documentation path |
	| -X command | documentation command |
	| -S YES/NO | always use sudo to install (saved) |
	| -C YES/NO | always clean before make (saved) |
	| -T 1-n/NO | number of cores (saved) |
	| -M number | total build number |
	| -B number | successful build number |
	| -K folder | build folder of this project (saved in build.conf) |

	\section	gimdoc_gkmake_build	Build
	| Option | Meaning |
	|--------|---------|
	| -t 1-n | number of cores for this build |
	| -c | clean before make |
	| -n | distclean |
	| -z | generate the tarball |
	| -d | build the documentation |
	| -i | install |
	| -s | use sudo to install |
	| -a | launch autotools |

	\section	gimdoc_gkmake_details	Details
	| Option | Meaning |
	|--------|---------|
	| -o | print the configuration file of this project |
	| -e | edit the configuration file of this project |
	| -h | print the help |

	Example: creating a project called \b gkmake, by ASYNTOTE, with the prefix GKM and 4 cores:
	\verbatim
	gkmake -Ngkmake -Aasyntote -PGKM -T4
	\endverbatim

	\todo	gkmake: the CMake mode, and an example with a complete project.

	\section	gimdoc_gkmake_source	Source: gkmake/main.cc
	\htmlonly <details><summary>show the source code of gkmake/main.cc</summary> \endhtmlonly
	\include gkmake/main.cc
	\htmlonly </details> \endhtmlonly
*/


/*!	\page	gimdoc_tool_gimshell	gimshell

	Interactive shell for the Gim databases. It starts without arguments and reads commands until \c exit. Source: gim_shell/main.cc.

	\section	gimdoc_gimshell_purpose	What it is for

	\b gimshell is a command line interface to the Gim databases. It lets you create a database from a GDBS script, look at
	its tables, insert, select, update and delete records, and move a table to and from a CSV file, without writing a
	program. Every command works on the database chosen with \c db use, so a session can work on several databases in turn.

	Use \b gimshell to inspect the data of an application, to correct a few records by hand, to test a GDBS script before
	using it in a program, or to prepare a dataset. It is also the easiest way to check that the library works as expected:
	the operations of the shell are the same methods of \ref gim_db_obj that a program calls.

	The select command filters with WHERE, sorts with ORDER BY and limits the result with LIMIT, or with LAST for the
	records at the end of the table. The operators are equality, inequality and the four order comparisons; numbers are
	compared as numbers, the other types as text.

	Typical cases: checking the records written by a PicGIM project, exporting a table with table export to train the
	integer model with \b gimaitrain, and importing a corrected CSV file with table import, which checks the whole file
	before it changes the table.

	Limits: the values are single words, without spaces or quotes, and each command is one line. The shell is not meant for
	large amounts of data: use a program with the API for those.

	\section	gimdoc_gimshell_commands	Commands
	\verbatim
	help                                    list the available commands
	version                                 show the Gim version
	exit | quit                             leave the shell
	db create <name> [script.gdbs]          create a database
	db list | dblist                        list the databases
	db use <name>                           choose the database
	table list                              list the tables
	table show <name>                       show a table
	table drop <name>                       remove a table and its records (PERMANENT: deletes its files)
	table export <name> <file.csv>          write the records to a CSV file
	table import <name> <file.csv>          add the records of a CSV file
	insert <table> <field> <value> ...      add a record
	select <table> [where ...] [order by <f> [desc]] [limit n | last n]
	update <table> set <f> <v> [...] where <f> <op> <v> [and ...]
	delete <table> where <f> <op> <v> [and ...]
	\endverbatim

	The operators are  =  !=  <  <=  >  >= ; see \ref gimdoc_db_operators. \c delete and \c update need a \c where with at
	least one condition. The help of the \c table command still lists only list and show.

	\section	gimdoc_gimshell_db	The databases
	A database created with \c db create is current until another one is chosen with \c db use. The script of \c db create
	is a \ref gimdoc_files_gdbs "GDBS script".

	\todo	gimshell: a complete session, with a permanent database.

	\section	gimdoc_gimshell_source	Source: gim_shell/main.cc
	\htmlonly <details><summary>show the source code of gim_shell/main.cc</summary> \endhtmlonly
	\include gim_shell/main.cc
	\htmlonly </details> \endhtmlonly
*/


/*!	\page	gimdoc_tool_gimaitrain	gimaitrain

	Trains the integer model that PicGIM runs on the chip, and writes it as a C header. Source: gim_ai_train/main.cc.
	The same options and exit codes as the Python script \c scripts/gim_ai_train.py.

	\section	gimdoc_gimaitrain_purpose	What it is for

	\b gimaitrain trains the small neural network that PicGIM runs on the chip, and writes it as a C header that the firmware
	includes. The training happens on the PC with the floating point numbers; the model that goes to the chip works with
	integers only, because the PIC has no floating point unit. The tool therefore does three things in sequence: it trains
	the network, it quantizes the weights to 8 or 16 bits, and it runs the integer model on the same test samples to
	compare its accuracy with the float one.

	The header is written only when the integer model is accepted: its accuracy must not be lower than the float one by
	more than the tolerance (0.02 by default). A rejected model produces no header, but the report, when requested, still
	explains why, so the training can be adjusted and repeated.

	Use \b gimaitrain when the sensor data of a PicGIM project changes, or when the network has to be retrained for a new
	chip. The training uses a fixed seed (--seed), so a run can be repeated. The synthetic-out option creates a labelled
	dataset, useful to test the whole chain before real data is available.

	The dataset is a CSV file with the label and the raw samples of each record. The number of hidden layers and their
	size are set with --hidden: one layer of 8 neurons by default, two layers of 8 (--hidden 8,8) for the 8-8-3 network
	of the current PicGIM project.

	Typical cases: retraining the model of a project after collecting new data, checking that a quantization to 8 bits is
	good enough before choosing 16 bits, and producing the test vectors that the firmware checks on the chip.

	Limits: the model is small by design. Its quality depends on the dataset: a dataset with few samples or unbalanced
	labels gives an accepted model that is not useful in practice. The accuracy check compares the integer model with the
	float one, not with the real task.

	\section	gimdoc_gimaitrain_usage	Usage
	\verbatim
	gimaitrain --dataset data.csv --out pgim_ai_model.h [--report r.txt]
	           [--out-bits 8|16] [--hidden 8,8] [--epochs 200] [--lr 0.05]
	           [--test-frac 0.25] [--seed 1] [--tol 0.02] [--test-vectors 16]
	gimaitrain --synthetic-out synth.csv [--synthetic-n 600] [--seed 1]
	\endverbatim

	\section	gimdoc_gimaitrain_opts	Options
	| Option | Default | Meaning |
	|--------|---------|---------|
	| --dataset | - | CSV dataset to train on (required with --out) |
	| --out | - | C header to write, only when the model is accepted (required) |
	| --report | - | text report, written also when the model is rejected |
	| --out-bits | 8 | bits of the quantized weights: 8 or 16 |
	| --hidden | 8 | sizes of the hidden layers, comma separated (8,8 for two layers) |
	| --epochs | 200 | training epochs |
	| --lr | 0.05 | learning rate |
	| --test-frac | 0.25 | fraction of the samples used for the test |
	| --seed | 1 | seed of the random generator |
	| --tol | 0.02 | largest accuracy loss of the integer model compared to the float one |
	| --test-vectors | 16 | test vectors written in the header |
	| --synthetic-out | - | writes a synthetic dataset and stops |
	| --synthetic-n | 600 | samples of the synthetic dataset |
	| -h, --help | - | prints the usage |

	\section	gimdoc_gimaitrain_exit	Exit codes
	| Code | Meaning |
	|------|---------|
	| 0 | the model is accepted and the header is written (or the synthetic dataset) |
	| 1 | bad input: an unknown option, a missing value, a missing --dataset or --out, a file that cannot be read or written |
	| 2 | the model is rejected: the integer accuracy is below the float one minus the tolerance, or a check of the model failed |

	The report, when requested, is written before the decision, so it explains also the rejected models.

	\todo	gimaitrain: the format of the dataset, with an example.

	\section	gimdoc_gimaitrain_source	Source: gim_ai_train/main.cc
	\htmlonly <details><summary>show the source code of gim_ai_train/main.cc</summary> \endhtmlonly
	\include gim_ai_train/main.cc
	\htmlonly </details> \endhtmlonly
*/


/*!	\page	gimdoc_files	The files

	- \subpage	gimdoc_files_conf
	- \subpage	gimdoc_files_lex
	- \subpage	gimdoc_files_gdbs
	- \subpage	gimdoc_files_gkp
*/


/*!	\page	gimdoc_files_conf	gim.conf

	The configuration of Gim. It is written by the programs through \ref gim_prsr_obj : it is not meant to be edited by hand.
	A complete file is in the examples: \ref gim.conf "gim.conf"

		\section	gimdoc_conf_sections	Sections
	| Section | Content |
	|---------|---------|
	| \c version | version of Gim that wrote the file |
	| \c system | \c sig_trap: handling of the signals |
	| \c debug | log: \c debug_format (layout of a line); \c debug (YES forces the visual debug: fatal messages on screen, always verbose, automatic get); \c min_type (lowest level written: DEBUG, MESSAGE, WARNING, CRITICAL or FATAL, default MESSAGE, see \ref gimdoc_err_level); \c f_debug and \c f_debug_file (a copy of the log in a file) |
	| \c error | \c verbose_on_critical: print the CRITICAL messages on screen (see \ref gimdoc_error) |
	| \c memory | \c hide_in_log, \c virtual_memory, \c alloc_limit, \c limit, \c default_lock (see \ref gimdoc_mem_config); \c log_in_file and \c log_file_name |
	| \c crypt | \c iterations |
	| \c alias, \c compatibility | enabled or not, and syntax of the file |
	| \c key | folders of the keys |
	| \c programs | folders of the programs |
	| \c mersenne, \c prime, \c password | parameters of those classes |
	| \c thread | permission of each class to run its own thread |
	| \c me | \c nick: the nickname of the user |

\section	gimdoc_conf_log	Log format
	| Code | Meaning |
	|------|---------|
	| \c \$t | precision timer |
	| \c \$p | process ID |
	| \c \$k | message kind |
	| \c \$f | calling function |
	| \c \$m | message |
	| \c \$v | returned value |
	| \c \$T | thread flag |
	| \c \$\$ | the dollar character |
	| \c _ | a space |

	\todo	gim.conf: every key with its values and default.
*/


/*!	\page	gimdoc_files_lex	Syntax of the configuration files

	\ref gimdoc_kvalue "The key/value class" reads three syntaxes. A file begins with the title and ends with a line that contains >.
	The spaces of the titles and of the names are removed.

	\warning	A line that contains \# is a comment, all of it: do not put a comment after a key.

	\section	gimdoc_lex_a	Lex A
	\verbatim
	< title

	    # comment
	    [ SECTION
	        key = value
	    ]

	>
	\endverbatim
	The values cannot contain: \# < > [ ]

	\section	gimdoc_lex_b	Lex B
	\verbatim
	< title

	  SECTION
	    # comment
	        section::key = value

	>
	\endverbatim
	The section of a key is the name before the colons; the line with the name alone is only for the reader. \n
	The values cannot contain: \# < >

	\section	gimdoc_lex_c	Lex C
	\verbatim
	< title

	    # comment
	    ( SECTION ) {
	        key                              = value
	    }

	>
	\endverbatim
	The bracket that opens the section is on the same line as its name. \n
	The values cannot contain: \# < > { }

	\section	gimdoc_lex_which	How the syntax is recognized

	After the title, the first line that matches decides: \c [ is Lex A, two colons and \c = are Lex B, an open round
	bracket followed by a space is Lex C. Comments count too: avoid these characters in the comments before the first section.

	\section	gimdoc_lex_lines	Lines

	- Keep the lines shorter than 256 characters.
	- End the last line with a newline: a last line without it is ignored.
	- Use Unix line endings (LF): with CRLF the names of the sections in Lex A get a hidden character at the end.
*/

/*!	\page	gimdoc_files_gdbs	GDBS scripts

	A GDBS script (Gim DataBase Script) creates a database.

	\section	gimdoc_gdbs_rules	Rules
	- every command ends with a semicolon
	- \# begins a comment
	- tokens are separated by spaces, = or ;
	- the whole script is read before it runs: a syntax error stops all of it and nothing is created
	- ADD FIELD and SET FIELD always come after PIN TABLE
	- the ids of the fields start from 1: a field numbered 0 is lost when the database is read again
	- the file must exist: a missing file stops the program
	- a second script needs a new database object

		\section	gimdoc_gdbs_commands	Commands
	\verbatim
	CREATE DB <name>                                    create the database (the name only)
	SET DB <property>                                   VOLATILE, PERMANENT, SAVE_MEMORY, PERFORMANCE or BALANCED
	CREATE TABLE <name>                                 create a table
	PIN TABLE <name>                                    select the current table
	SET TABLE <property>                                VOLATILE, PERMANENT or VIRTUAL (the table pinned)
	ADD FIELD <id> <TYPE> <name>                        add a field to the pinned table
	SET FIELD <id> <PROPERTY>                           set a property of the field
	UNPIN TABLE                                         deselect the current table
	INSERT <table> <field> <value> [<field> <value> ...]
	UPDATE <table> SET <field> <value> [<field> <value> ...] WHERE <field> <value> [AND <field> <value> ...]
	DELETE <table> WHERE <field> <value> [AND <field> <value> ...]
	DROP TABLE <table>
	\endverbatim

	Types: INT, FLOAT, PERCENTAGE, BOOL, FLAG, STRING, DATE, TIME, TIMESTAMP. \n
	Properties: KEY, NOT_KEY, IS_INDEX, IS_NOT_INDEX, AUTOINCREMENTAL, IS_UNIQUE. \n
	\ref gimdoc_db_types explains what each one does.

	In UPDATE and DELETE the conditions are equalities only, and WHERE needs at least one condition. The values cannot
	contain spaces, and a line has at most 20 tokens. The operators  != < <= > >=  are available in the shell and in the API: \ref gimdoc_db_operators.

\section	gimdoc_gdbs_insert	The INSERT command

	The field and the value are separated by a space or by =: \c INSERT \c clienti \c codice=1 \c nome=Rossi;

	\warning	INSERT takes the values by \b position, in the order of the fields of the table, and ignores the names.
				Write them in that order, all of them. The type is not checked, the KEY is not checked. A FLOAT value is not stored.
				BOOL wants true or false, FLAG wants on, off, yes or no. The code can add records with checks:
				\ref gim_db_obj::insert_record "insert_record()".
	\section	gimdoc_gdbs_example	Example
	\verbatim
	CREATE DB anagrafica;
	SET DB VOLATILE;
	CREATE TABLE clienti;
	PIN TABLE clienti;
	ADD FIELD 1 INT codice;
	SET FIELD 1 KEY;
	ADD FIELD 2 STRING nome;
	UNPIN TABLE;
	\endverbatim
*/


/*!	\page	gimdoc_files_gkp	GKP packages

	\todo	GKP packages: format of the file, versions and compatibility.
*/


/*!	\page	gimdoc_project	The project

	- \subpage	gimdoc_license
	- \subpage	gimdoc_changelog

	\section	gimdoc_project_links	Links
	- Source code and tickets: https://github.com/gKript/Gim
	- gKript: http://www.gkript.org

	\section	gimdoc_project_doc	This documentation
	Made with Doxygen. The structure of these pages is in gim_doxygen.h, the text of each class is in its own header.
	If you find an error or a point that needs to be clearer, write to documentation@gkript.org.
*/


/*!	\page	gimdoc_license	GNU General Public License

	Gim is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

	Gim is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*/


/*!	\page	gimdoc_changelog	ChangeLog

	\todo	ChangeLog: to be written from the history of the repository.
*/


/*!	\example	alias.conf
	\brief		A __LEX_B Gim Key/value file example
*/

/*!	\example	gim.conf
	\brief		A __LEX_B Gim Key/value file example
*/

/*!	\example	asyntote.gkk
	\brief		This is an example of a personal key generated by Gim
*/

/*!	\example	gkfp.cc
	\brief		An example of an application made using Gim: the code of the gkfp utility.
*/

/*!	\example	environment.conf
	\brief		A __LEX_B Gim Key/value file example
*/
