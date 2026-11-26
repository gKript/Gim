cmake_minimum_required(VERSION 3.16)
project(gim VERSION 2.9.2 LANGUAGES C CXX)

option(GIM_BUILD_TOOLS "Build gim command line tools (gkdp, gkfp, gimshell, ...)" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# ---------------------------------------------------------------- library
set(GIM_SOURCES
	gim.cc gim_checksum.cc gim_cnc.cc gim_cnc_alfabeto.cc gim_cnc_coding.cc
	gim_cnc_generic.cc gim_db.cc gim_directory.cc gim_enhanced.cc
	gim_environment.cc gim_error.cc gim_file.cc gim_gkfp_file_data.cc
	gim_gkfp_file_list.cc gim_gkfp_header.cc gim_gkl.cc gim_gkl_commands.cc
	gim_gkl_variable.cc gim_gkp.cc gim_gkp_cryptography.cc gim_gkp_file_data.cc
	gim_gkp_file_list.cc gim_gkp_header.cc gim_gkp_password.cc gim_gkp_utils.cc
	gim_html.cc gim_identity.cc gim_interface.cc gim_kvalue.cc gim_lexical.cc
	gim_log.cc gim_md5.cc gim_memory.cc gim_private_key.cc gim_sha1.cc
	gim_sha256.cc gim_sha512.cc gim_signal.cc gim_string.cc gim_time.cc
	gim_version.cc gim_list.cc gim_picgim.cc gim_getopt.cc gim_ascii_buffer.cc
	gim_mersennetwister.cc gim_ai_train.cc gim_serial.cc gim_rpi_gpio.cc gim_thread.cc)
if(WIN32)
	list(APPEND GIM_SOURCES gim_platform_win32.cc)
endif()
list(TRANSFORM GIM_SOURCES PREPEND "src/")

add_library(gim STATIC ${GIM_SOURCES})
add_library(gim::gim ALIAS gim)

# gim_thread uses std::thread: link the thread library (pthread on Linux, winpthreads on MinGW)
find_package(Threads REQUIRED)
target_link_libraries(gim PUBLIC Threads::Threads)

# the tools include <gim/gim.h> (installed layout): stage the headers that way
file(GLOB GIM_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
# gim_gkmake.h (build counters) is rewritten by gkmake at every run and is included only by src/gim_version.cc,
# from the source folder: staging it would make every staged header a configure dependency and re-run CMake
# after each gkmake build.
list(FILTER GIM_HEADERS EXCLUDE REGEX "/gim_gkmake\\.h$")
foreach(h ${GIM_HEADERS})
	get_filename_component(hn ${h} NAME)
	configure_file(${h} "${CMAKE_BINARY_DIR}/stage/gim/${hn}" COPYONLY)
endforeach()
target_include_directories(gim PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/stage>
	$<INSTALL_INTERFACE:include>)

if(MSVC)
	target_compile_options(gim PRIVATE /W0 /utf-8)
	target_compile_definitions(gim PUBLIC _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
else()
	target_compile_options(gim PRIVATE -w)
endif()

# ------------------------------------------------------------------ tools
if(GIM_BUILD_TOOLS)
	# Windows: after each build every tool is copied into a folder of the PATH. The default is a folder in the
	# user profile (%USERPROFILE%/.gim/bin): one per PC, never on a disk shared between PCs, so two machines
	# cannot overwrite each other's executables. Set GIM_GKMAKE_COPY_DIR to another folder, or to an empty
	# value to disable the copy.
	# A running exe cannot be overwritten, so the old copy is first renamed (see cmake/gkmake_copy.cmake).
	if(WIN32)
		set(gim_default_copy_dir "")
		if(DEFINED ENV{USERPROFILE})
			file(TO_CMAKE_PATH "$ENV{USERPROFILE}/.gim/bin" gim_default_copy_dir)
		endif()
		set(GIM_GKMAKE_COPY_DIR "${gim_default_copy_dir}" CACHE PATH "folder that receives a copy of the tools (Windows only; empty = no copy)")
		# the default folder is created when it is missing; a folder chosen by the user is never created
		if(GIM_GKMAKE_COPY_DIR AND ( GIM_GKMAKE_COPY_DIR STREQUAL gim_default_copy_dir ) AND NOT EXISTS "${GIM_GKMAKE_COPY_DIR}")
			file(MAKE_DIRECTORY "${GIM_GKMAKE_COPY_DIR}")
		endif()
		# The copy folder must not be the build folder: the copy would delete the tools it just linked.
		set(gim_tool_copy ON)
		if(GIM_GKMAKE_COPY_DIR AND EXISTS "${GIM_GKMAKE_COPY_DIR}")
			file(REAL_PATH "${GIM_GKMAKE_COPY_DIR}" gim_copy_real)
			file(REAL_PATH "${CMAKE_BINARY_DIR}" gim_bin_real)
			string(TOLOWER "${gim_copy_real}" gim_copy_real)
			string(TOLOWER "${gim_bin_real}" gim_bin_real)
			if(gim_copy_real STREQUAL gim_bin_real)
				message(WARNING "GIM_GKMAKE_COPY_DIR is the build folder itself (${CMAKE_BINARY_DIR}): tool copy disabled. "
								"Build in another folder (e.g. build-<host>) or point GIM_GKMAKE_COPY_DIR elsewhere.")
				set(gim_tool_copy OFF)
			endif()
		endif()
	endif()
	function(gim_tool name)
		add_executable(${name} ${ARGN})
		target_link_libraries(${name} PRIVATE gim::gim)
		if(MSVC)
			target_compile_options(${name} PRIVATE /W0 /utf-8)
		else()
			target_compile_options(${name} PRIVATE -w)
		endif()
		if(MINGW)
			target_link_options(${name} PRIVATE -static)
		endif()
		install(TARGETS ${name} RUNTIME DESTINATION bin)
			if(WIN32 AND gim_tool_copy AND EXISTS "${GIM_GKMAKE_COPY_DIR}")
				add_custom_command(TARGET ${name} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -DSRC=$<TARGET_FILE:${name}> -DDST=${GIM_GKMAKE_COPY_DIR}/${name}.exe -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gkmake_copy.cmake
					VERBATIM)
			endif()
	endfunction()

	gim_tool(gkdp        gim_gkdp/gkdp.cc)
	gim_tool(gkfp        gim_gkfp/gkfp.cc)
	gim_tool(gimstat     gim_stat/gimstat.cc)
	gim_tool(gkmake      gkmake/main.cc gkmake/gkm_header.cc)
	gim_tool(gimaitrain gim_ai_train/main.cc)
	gim_tool(gimshell
		gim_shell/main.cc gim_shell/gim_sh_callback.cc gim_shell/gim_sh_commands.cc
		gim_shell/gim_sh_environ_obj.cc gim_shell/gim_sh_mainloop.cc gim_shell/gim_sh_db.cc gim_shell/gim_sh_table.cc gim_shell/gim_sh_record.cc)
endif()

# ---------------------------------------------------------------- install
install(TARGETS gim ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/gim FILES_MATCHING PATTERN "*.h")
