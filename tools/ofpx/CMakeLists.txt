
set(OFPX_SOURCES
  ofpx_main.cpp
  ofpx_encode.cpp
  ofpx_decode.cpp
  ofpx_jsonrpc.cpp
  ofpx_help.cpp
)

if(APPLE AND UNIX)
  set(OFPX_SOURCES ${OFPX_SOURCES} ofpx_xpc.cpp)
endif()

add_executable(ofpx ${OFPX_SOURCES})

# Tip: Add "-framework CoreFoundation" here on Mac OS X for measuring with
# Instruments.

target_link_libraries(ofpx ofp ${LIBOFP_LINKED_LIBS})

add_subdirectory(test)

# Install ofpx.

install(TARGETS ofpx DESTINATION bin)
