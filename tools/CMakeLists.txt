
# nos2cc executable target

set (LIBNEREON_LIBRARY ${CMAKE_BINARY_DIR}/src/libnereon.a)
set (LIBUCL_LIBRARY ${CMAKE_BINARY_DIR}/libucl-inst/lib/libucl.a)

add_executable (nos2cc nos2cc.c)
target_include_directories (nos2cc PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries (nos2cc ${LIBNEREON_LIBRARY})

install (TARGETS nos2cc DESTINATION bin)

add_executable (json2noc json2noc.c)
target_include_directories (json2noc PUBLIC ${CMAKE_BINARY_DIR}/libucl-inst/include)
target_link_libraries (json2noc ${LIBUCL_LIBRARY})

install (TARGETS json2noc DESTINATION bin)
