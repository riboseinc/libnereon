
# libnereon project settings
cmake_minimum_required (VERSION 2.8.11)
project (libnereon)
set (CMAKE_BUILD_TYPE Release)

# check functions
include (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)

check_function_exists (strlcpy HAVE_STRLCPY)
check_function_exists (strlcat HAVE_STRLCAT)
check_function_exists (getprogname HAVE_GETPROGNAME)

if (HAVE_STRLCPY)
	add_definitions(-DHAVE_STRLCPY)
endif()

if (HAVE_STRLCAT)
	add_definitions(-DHAVE_STRLCAT)
endif()

if (HAVE_GETPROGNAME)
	add_definitions(-DHAVE_GETPROGNAME)
endif()

# add libucl dependency
include(ExternalProject)

ExternalProject_Add (
	libucl
	URL https://github.com/vstakhov/libucl/archive/0.8.1.tar.gz
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	BINARY_DIR "${CMAKE_BINARY_DIR}/libucl"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/libucl"
	INSTALL_DIR "${CMAKE_BINARY_DIR}/libucl"
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

