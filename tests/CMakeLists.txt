
set (
	LIBNEREON
	${CMAKE_BINARY_DIR}/src/libnereon.a
)

add_custom_command (
	OUTPUT rvc.nos.c
	COMMAND ../tools/nos2cc ${CMAKE_CURRENT_SOURCE_DIR}/rvc.nos ${CMAKE_CURRENT_BINARY_DIR}/ rvc
	DEPENDS rvc.nos nos2cc
)

add_executable (nos_test rvc.nos.c nos_test.c)
include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/tests)
target_link_libraries (nos_test ${LIBNEREON})
add_dependencies(nos_test libnereon)

add_test (NAME nos_test COMMAND nos_test)

set (
	LIBUCL
	${CMAKE_BINARY_DIR}/libucl-inst/lib/libucl.a
)

add_executable (ucl_test ucl_test.c)
target_include_directories (ucl_test PUBLIC ${CMAKE_BINARY_DIR}/libucl-inst/include)
target_link_libraries (ucl_test ${LIBUCL})
add_dependencies(ucl_test libucl)

add_test (NAME ucl_test COMMAND ucl_test ${CMAKE_CURRENT_SOURCE_DIR}/rvd.nos)

