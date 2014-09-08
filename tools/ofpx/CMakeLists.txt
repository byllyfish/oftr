
set(OFPX_SOURCES
  ofpx_main.cpp
  ofpx_ping.cpp
  ofpx_decode.cpp
  ofpx_encode.cpp
  ofpx_jsonrpc.cpp
)

add_executable(ofpx ${OFPX_SOURCES})
target_link_libraries(ofpx ofp ${LIBOFP_LINKED_LIBS})

add_subdirectory(test)
