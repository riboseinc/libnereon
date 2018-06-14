
set (
	LIBNEREON
	nereon
)

add_custom_command (
	OUTPUT rvd.nos.c
	COMMAND ../tools/nos_to_cc ${CMAKE_CURRENT_SOURCE_DIR}/rvd.nos ${CMAKE_CURRENT_BINARY_DIR}/
	DEPENDS rvd.nos
)

add_executable (noc_test rvd.nos.c noc_test.c)
include_directories (../src)
target_link_libraries (noc_test ${LIBNEREON})

add_executable (nos_test rvd.nos.c nos_test.c)
include_directories (../src)
target_link_libraries (nos_test ${LIBNEREON})

set (
	LIBUCL
	${CMAKE_BINARY_DIR}/libucl/lib/libucl.a
)

add_executable (ucl_test ucl_test.c)
target_include_directories (ucl_test PUBLIC ${CMAKE_BINARY_DIR}/libucl/include)
target_link_libraries (ucl_test ${LIBUCL})
