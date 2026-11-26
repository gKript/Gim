# Copies a tool (SRC, in the current build folder) over DST, a copy that may be in use (a running gkmake).
# Usage: cmake -DSRC=<file> -DDST=<file> -P gkmake_copy.cmake
# Failures are not errors: the build must not stop because the copy in the PATH is busy.

# SRC and DST must be different files: otherwise the rename below would move the freshly linked
# tool away and the final cleanup would delete it (copy folder == build folder).
file(REAL_PATH "${SRC}" src_real)
file(REAL_PATH "${DST}" dst_real)
string(TOLOWER "${src_real}" src_real)		# Windows paths are case-insensitive
string(TOLOWER "${dst_real}" dst_real)
if(src_real STREQUAL dst_real)
	message(STATUS "gkmake: copy to ${DST} skipped (source and destination are the same file)")
	return()
endif()

if(EXISTS "${DST}")
	# renaming a running .exe is allowed on Windows: the old copy goes away at the next run
	execute_process(COMMAND ${CMAKE_COMMAND} -E rename "${DST}" "${DST}.old" RESULT_VARIABLE rename_rc OUTPUT_QUIET ERROR_QUIET)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${SRC}" "${DST}" RESULT_VARIABLE copy_rc OUTPUT_QUIET ERROR_QUIET)
if(NOT copy_rc EQUAL 0)
	message(STATUS "gkmake: copy to ${DST} skipped (file busy)")
endif()

if(EXISTS "${DST}.old")
	execute_process(COMMAND ${CMAKE_COMMAND} -E remove "${DST}.old" OUTPUT_QUIET ERROR_QUIET)
endif()
