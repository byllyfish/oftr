
add_executable(libofpexec libofpexec_main.cpp)
target_link_libraries(libofpexec ofp ${LIBOFP_LINKED_LIBS})
install(TARGETS libofpexec DESTINATION bin)

add_subdirectory(test)
