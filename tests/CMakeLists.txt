
set (
	LIBNEREON
	nereon
)

add_custom_command (
	OUTPUT rvd.nos.c
	COMMAND ../tools/nos_to_cc ${CMAKE_CURRENT_SOURCE_DIR}/rvd.nos ${CMAKE_CURRENT_BINARY_DIR}/
#	COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/rvd.noc ${CMAKE_CURRENT_BINARY_DIR}/
	DEPENDS rvd.nos
)

add_executable (noc_test rvd.nos.c noc_test.c)
include_directories (../src ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (noc_test ${LIBNEREON})

add_test (NAME noc_test COMMAND noc_test)

add_executable (nos_test rvd.nos.c nos_test.c)
include_directories (../src ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (nos_test ${LIBNEREON})

set (
	LIBUCL
	${CMAKE_BINARY_DIR}/libucl/lib/libucl.a
)

add_test (NAME nos_test COMMAND nos_test)

add_executable (ucl_test ucl_test.c)
target_include_directories (ucl_test PUBLIC ${CMAKE_BINARY_DIR}/libucl/include)
target_link_libraries (ucl_test ${LIBUCL})

add_test (NAME ucl_test COMMAND ucl_test ${CMAKE_CURRENT_SOURCE_DIR}/rvd.nos)
