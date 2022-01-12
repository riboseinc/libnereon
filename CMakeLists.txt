
# libnereon project settings
cmake_minimum_required (VERSION 2.8.16)
# 2.8.16 since all versions below are deprecated
project (libnereon)
set (CMAKE_BUILD_TYPE Release)

# set libnereon version
set (LIBNEREON_VERSION "0.9.5")

execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set (GIT_COMMIT_HASH ${GIT_COMMIT_HASH})

# set configuration file
configure_file (
	"${PROJECT_SOURCE_DIR}/nereon_config.h.in"
	"${PROJECT_BINARY_DIR}/nereon_config.h"
)

# check functions
include (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)

OPTION(NEREON_DEBUG
	"Build libnereon using debugging code"
	OFF)
IF(NEREON_DEBUG)
	MESSAGE("Adding DEBUG flag...")
	ADD_DEFINITIONS(-DNEREON_DEBUG)
	SET(CMAKE_BUILD_TYPE Debug)
	MESSAGE("Build type is " ${CMAKE_BUILD_TYPE})
ENDIF(NEREON_DEBUG)

# add libucl dependency
include (ExternalProject)

ExternalProject_Add (
	libucl
	URL https://github.com/vstakhov/libucl/archive/0.8.1.tar.gz
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	BINARY_DIR "${CMAKE_BINARY_DIR}/libucl"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/libucl"
	INSTALL_DIR "${CMAKE_BINARY_DIR}/libucl-inst"
	CONFIGURE_COMMAND ./configure --prefix=<INSTALL_DIR>
	BUILD_COMMAND make
	INSTALL_COMMAND make install
)

ExternalProject_Add_Step (
	libucl
	autogen
	COMMAND ./autogen.sh
	DEPENDEES download
	DEPENDERS configure
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libucl
)

# add subdirectory
add_subdirectory (src)
add_subdirectory (tools)

enable_testing ()
add_subdirectory (tests)

