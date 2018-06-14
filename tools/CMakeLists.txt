
# nos_to_cc executable target

set (LIBNEREON_LIBRARY nereon)

add_executable (nos_to_cc nos_to_cc.c)
include_directories (../src)
link_libraries ("--static")
target_link_libraries (nos_to_cc ${LIBNEREON_LIBRARY})

install (TARGETS nos_to_cc DESTINATION bin)
