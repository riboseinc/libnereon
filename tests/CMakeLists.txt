
set (
	LIBNEREON
	${CMAKE_BINARY_DIR}/src/libnereon.a
)

add_custom_command (
	OUTPUT rvd.nos.c
	COMMAND ../tools/nos2cc ${CMAKE_CURRENT_SOURCE_DIR}/rvd.nos ${CMAKE_CURRENT_BINARY_DIR}/ rvd
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/rvd.noc ${CMAKE_CURRENT_BINARY_DIR}/ 2>/dev/null || :
	DEPENDS rvd.nos
)

add_custom_command (
	OUTPUT rvc.nos.c
	COMMAND ../tools/nos2cc ${CMAKE_CURRENT_SOURCE_DIR}/rvc.nos ${CMAKE_CURRENT_BINARY_DIR}/ rvc
	DEPENDS rvc.nos
)

add_executable (noc_test rvd.nos.c noc_test.c)
include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_BINRARY_DIR}/tests)
target_link_libraries (noc_test ${LIBNEREON})

add_test (NAME noc_test COMMAND noc_test)

add_executable (nos_test rvd.nos.c nos_test.c)
include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/tests)
target_link_libraries (nos_test ${LIBNEREON})

add_test (NAME nos_test COMMAND nos_test)

add_executable (nos_test_multi rvc.nos.c nos_test_multi.c)
include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/tests)
target_link_libraries (nos_test_multi ${LIBNEREON})

add_test (NAME nos_test_multi COMMAND nos_test_multi)

add_executable (rtr2_conf_test rtr2_conf_test.c)
include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/tests)
target_link_libraries (rtr2_conf_test ${LIBNEREON})

add_test (NAME rtr2_conf_test COMMAND rtr2_conf_test ${CMAKE_CURRENT_SOURCE_DIR}/rtr2.conf)

set (
	LIBUCL
	${CMAKE_BINARY_DIR}/libucl-inst/lib/libucl.a
)

add_executable (ucl_test ucl_test.c)
target_include_directories (ucl_test PUBLIC ${CMAKE_BINARY_DIR}/libucl-inst/include)
target_link_libraries (ucl_test ${LIBUCL})

add_test (NAME ucl_test COMMAND ucl_test ${CMAKE_CURRENT_SOURCE_DIR}/rvd.nos)
