
set(OFPX_SOURCES
  ofpx_main.cpp
  ofpx_encode.cpp
  ofpx_decode.cpp
  ofpx_help.cpp
  ofpx_util.cpp
)

if(LIBOFP_ENABLE_JSONRPC)
  set(OFPX_SOURCES ${OFPX_SOURCES} ofpx_jsonrpc.cpp)
  if(APPLE)
    set(OFPX_SOURCES ${OFPX_SOURCES} ofpx_xpc.cpp)
  endif()
endif()

if(APPLE)
  # Set up Info.plist file so there will be a version number in crashreporter
  # output.
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in" "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
  set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -Wl,-sectcreate,__TEXT,__info_plist,${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
endif()

add_executable(libofp ${OFPX_SOURCES})

# Tip: Add "-framework CoreFoundation" here on Mac OS X for measuring with
# Instruments.

target_link_libraries(libofp ofp ${LIBOFP_LINKED_LIBS})

add_subdirectory(test)

# Install 'libofp'.

install(TARGETS libofp DESTINATION bin)
