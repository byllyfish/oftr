
set(OFTR_SOURCES
  oftr_main.cpp
  oftr_encode.cpp
  oftr_decode.cpp
  oftr_help.cpp
  oftr_util.cpp
)

if(LIBOFP_ENABLE_JSONRPC)
  set(OFTR_SOURCES ${OFTR_SOURCES} oftr_jsonrpc.cpp)
endif()

if(APPLE)
  # Set up Info.plist file so there will be a version number in crashreporter
  # output.
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in" "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
  set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -Wl,-sectcreate,__TEXT,__info_plist,${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
endif()

add_executable(oftr ${OFTR_SOURCES})

# Tip: Add "-framework CoreFoundation" here on Mac OS X for measuring with
# Instruments.

target_link_libraries(oftr ofp ${LIBOFP_LINKED_LIBS} ${LIBOFP_LINKED_LIBPCAP})

add_subdirectory(test)

# Install 'oftr'.

install(TARGETS oftr DESTINATION bin)
