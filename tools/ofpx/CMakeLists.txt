
add_executable(ofpx ofpx_main.cpp ofpx_ping.cpp ofpx_decode.cpp)
target_link_libraries(ofpx ofp ${LIBOFP_LINKED_LIBS})

add_subdirectory(test)
