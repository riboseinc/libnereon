
# libnereon library target
file (GLOB LIBNEREON_SOURCES "*.c")
add_library (nereon STATIC ${LIBNEREON_SOURCES})
target_include_directories (nereon PUBLIC ${CMAKE_BINARY_DIR}/libucl/include)

set (
	LIBNEREON_LIBRARY
	"${CMAKE_BINARY_DIR}/src/libnereon.a"
)

set (
	LIBUCL
	${CMAKE_BINARY_DIR}/libucl/lib/libucl.a
)

add_custom_command (
	TARGET nereon
	POST_BUILD
	COMMAND libtool -static ${CMAKE_BINARY_DIR}/src/libnereon.a ${LIBUCL} -o ${LIBNEREON_LIBRARY}
	COMMENT "Merging wih libucl.a library"
)

install (TARGETS nereon DESTINATION lib)
install (FILES nereon.h DESTINATION include)
