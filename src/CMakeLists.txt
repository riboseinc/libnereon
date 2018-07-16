
# libnereon library target
file (GLOB LIBNEREON_SOURCES "*.c")
add_library (nereon_single STATIC ${LIBNEREON_SOURCES})
target_include_directories (nereon_single PUBLIC ${CMAKE_BINARY_DIR}/libucl/include ${CMAKE_BINARY_DIR})

set (
	LIBNEREON_LIBRARY
	"${CMAKE_BINARY_DIR}/src/libnereon.a"
)

set (
	LIBUCL
	${CMAKE_BINARY_DIR}/libucl-inst/lib/libucl.a
)

add_custom_command (
	TARGET nereon_single
	POST_BUILD
	COMMAND mkdir -p ${CMAKE_BINARY_DIR}/src/tmp
)

add_custom_command (
	TARGET nereon_single
	POST_BUILD
	COMMAND ar -x ${CMAKE_BINARY_DIR}/src/libnereon_single.a
	COMMAND ar -x ${LIBUCL}
	COMMAND ar -rcs ${LIBNEREON_LIBRARY} *.o
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src/tmp
	COMMENT "Merging wih libucl.a library"
)

install (FILES ${LIBNEREON_LIBRARY} DESTINATION lib)
install (FILES nereon.h DESTINATION include)
