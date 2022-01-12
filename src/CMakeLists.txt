# libnereon library target

set (
	LIBNEREON_SOURCES
	nereon.c
	ctx.c
	nos.c
	cli.c
	util.c
	err.c
)

add_library (nereon_single STATIC ${LIBNEREON_SOURCES})
target_include_directories (nereon_single PUBLIC ${CMAKE_BINARY_DIR}/libucl/include ${CMAKE_BINARY_DIR})
add_dependencies(nereon_single libucl)

set (
	LIBNEREON_LIBRARY
	"${CMAKE_BINARY_DIR}/src/libnereon.a"
)

set (
	LIBUCL
	${CMAKE_BINARY_DIR}/libucl-inst/lib/libucl.a
)

add_custom_target(tmpdir
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/tmp
    COMMENT "Creating working directory at ${CMAKE_BINARY_DIR}/src/tmp"
)

add_custom_target(libnereon
    COMMAND ar -x ${CMAKE_BINARY_DIR}/src/libnereon_single.a
    COMMAND ar -x ${LIBUCL}
    COMMAND ar -rcs ${LIBNEREON_LIBRARY} *.o*
    COMMENT "Merging wih libucl.a library"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src/tmp
    DEPENDS nereon_single tmpdir
    BYPRODUCTS ${LIBNEREON_LIBRARY}
)

install (FILES ${LIBNEREON_LIBRARY} DESTINATION lib)
install (FILES nereon.h noc.h nos.h common.h cli.h DESTINATION include/nereon)
