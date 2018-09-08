#=====- CMakeLists.txt ---------------------------------------------------=====#
#
# This is the top-level CMakeLists.txt file for the LibOFP project.
#
#=====--------------------------------------------------------------------=====#
#
# If the build directory is named `Build+Debug`, cmake will build a debug
# version. Alternatively, you can build a debug version explicitly:
#
#    cmake -DCMAKE_BUILD_TYPE=debug
#
# To build without the JSON-RPC server:
#
#    cmake -DLIBOFP_ENABLE_JSONRPC=false ..
#
# To build without libpcap:
#
#    cmake -DLIBOFP_ENABLE_LIBPCAP=false ..
#
# To build without openssl:
#
#    cmake -DLIBOFP_ENABLE_OPENSSL=false ..
#
# To build without memcheck tests:
#
#    cmake -DLIBOFP_ENABLE_MEMCHECK_TESTS=false ..
#
# To build with code coverage:
#
#    cmake -DLIBOFP_ENABLE_CODE_COVERAGE=true ..
#

# N.B. Trusty supports cmake 2.8.12 (unless you use cmake3 pkg).
cmake_minimum_required(VERSION 2.8.11)

# Required for boringssl. (See external/boringssl/CMakeLists.txt)
if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

# Include file check macros honor CMAKE_REQUIRED_LIBRARIES (cmake 3.12)
if(POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

project(Libofp)

# Log version of cmake

message(STATUS "Running cmake ${CMAKE_VERSION} on ${CMAKE_SYSTEM} ${CMAKE_SYSTEM_PROCESSOR}")

# Add path for custom modules

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set the version number of the library (use semantic versioning).

set(Libofp_VERSION 0.51.0)
set(Libofp_GITHUB_URL "https://github.com/byllyfish/oftr")
set(Libofp_MAINTAINER_EMAIL "William W. Fisher <william.w.fisher@gmail.com>")
set(Libofp_COMPILER_SPEC "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1" Libofp_VERSION_MAJOR ${Libofp_VERSION})
string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\2" Libofp_VERSION_MINOR ${Libofp_VERSION})
string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\3" Libofp_VERSION_PATCH ${Libofp_VERSION})

# Set version numbers for RPC API.

set(Libofp_RPC_API_VERSION "0.9")

# You can build oftr with or without the JSON-RPC server.

set(LIBOFP_ENABLE_JSONRPC TRUE CACHE BOOL "Enable the JSON-RPC server")

if(NOT LIBOFP_ENABLE_JSONRPC)
  message(STATUS "JSON-RPC server is disabled.")
endif()

# You can build oftr without libpcap.

set(LIBOFP_ENABLE_LIBPCAP TRUE CACHE BOOL "Enable use of libpcap")

if(NOT LIBOFP_ENABLE_LIBPCAP)
  message(STATUS "libpcap is disabled.")
endif()

# You can build oftr without openssl.

set(LIBOFP_ENABLE_OPENSSL TRUE CACHE BOOL "Enable use of openssl")

if(NOT LIBOFP_ENABLE_OPENSSL)
  message(STATUS "openssl is disabled.")
endif()

# You can build with the system provided version of openssl.

set(LIBOFP_USE_BORINGSSL TRUE CACHE BOOL "Use BoringSSL (default)")

if(NOT LIBOFP_USE_BORINGSSL)
  message(STATUS "Using system-provided openssl instead of BoringSSL.")
endif()

# You can disable the valgrind memcheck tests.

set(LIBOFP_ENABLE_MEMCHECK_TESTS TRUE CACHE BOOL "Enable additional valgrind memcheck tests")
if(NOT LIBOFP_ENABLE_MEMCHECK_TESTS)
  message(STATUS "Memcheck tests are disabled.")
endif()

# You can enable code-coverage collection.

set(LIBOFP_ENABLE_CODE_COVERAGE FALSE CACHE BOOL "Enable code coverage")
if(LIBOFP_ENABLE_CODE_COVERAGE)
  message(STATUS "Code coverage is enabled.")
endif()

# Require out of source builds.

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed.
Please create a build sub-directory, cd into that directory then type 'cmake ..'
This process created the file 'CMakeCache.txt' and the directory 'CMakeFiles'.
Please delete them.")
endif()

#-------------------------------------------------------------------------------
# Settings
#-------------------------------------------------------------------------------

# Specify the set of warnings to enable. The philosophy of this project is to
# compile with every clang warning enabled, then disable a few that don't add
# anything except to clutter up the build logs. Put disabled warnings into
# LIBOFP_WARNINGS_DISABLED.

set(LIBOFP_WARNINGS_DISABLED "-Wno-unused-parameter -Wno-weak-vtables -Wno-switch-enum -Wno-unreachable-code -Wno-covered-switch-default")

# LibOFP is a C++11-only project. Disable warnings about C++98 compatibility.

set(LIBOFP_WARNINGS_CXX11 "-Wno-c++98-compat -Wno-c++98-compat-pedantic")

# Specify include directories for third party headers. We use -isystem instead
# of -I because we don't want to receive compiler warnings from these headers.

set(LIBOFP_SYSTEM_INCLUDES
  "${CMAKE_SOURCE_DIR}/external/asio/asio/include"
  "${CMAKE_BINARY_DIR}/external/yaml-io/include"
  "${CMAKE_SOURCE_DIR}/external/yaml-io/include"
)

if(LIBOFP_ENABLE_OPENSSL AND LIBOFP_USE_BORINGSSL)
  set(LIBOFP_SYSTEM_INCLUDES 
    ${LIBOFP_SYSTEM_INCLUDES}
    "${CMAKE_SOURCE_DIR}/external/boringssl/include")
endif()

# Specify additional definitions.

set(LIBOFP_DEFINES "-D ASIO_STANDALONE -D ASIO_NO_TYPEID -D ASIO_NO_EXCEPTIONS -D ASIO_NO_DEPRECATED")
#set(LIBOFP_DEFINES "${LIBOFP_DEFINES} -D ASIO_ENABLE_HANDLER_TRACKING")

if(APPLE AND UNIX)
  set(LIBOFP_DEFINES "${LIBOFP_DEFINES} -D LIBOFP_TARGET_DARWIN")
endif()

# Specify the default set of compiler flags for all sources, including third
# party libraries. Place flags that don't affect the preprocessor or warnings
# here. This set of flags controls compiler output. It is important that all
# source code be compiled with the same flags.

if(MSVC)
  set(LIBOFP_C_FLAGS "${CMAKE_C_FLAGS}")
  set(LIBOFP_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
else()
  set(LIBOFP_CPP_STD "-std=c++14")
  set(LIBOFP_HARDENING "-fstack-protector-strong -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -Wformat -Werror=format-security")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
      set(LIBOFP_HARDENING "-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -Wformat -Werror=format-security")
    endif()
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.3)
      set(LIBOFP_CPP_STD "-std=c++1y")
    endif()
  endif()
  set(LIBOFP_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing ${LIBOFP_HARDENING} -fvisibility=hidden")
  set(LIBOFP_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing ${LIBOFP_HARDENING} -fno-rtti -fno-exceptions ${LIBOFP_CPP_STD} -fvisibility=hidden")
  set(LIBOFP_LD_FLAGS "-Wl,-z,relro -Wl,-z,now")   # hardening linker flags
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} -pie -fPIE -rdynamic")
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -pie -fPIE -rdynamic")
  # Also, disable the -Wpsabi warning on ARM/GCC 7.0. (https://gcc.gnu.org/gcc-7/changes.html)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.3)
    set(LIBOFP_WARNINGS_DISABLED "${LIBOFP_WARNINGS_DISABLED} -Wno-psabi")
  endif()
endif()

if(APPLE AND UNIX)
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -stdlib=libc++")
endif()

# Add new library source files here. Add unittest source files to
# test/CMakeLists.txt.

set(LIBOFP_SOURCES
  src/ofp/actionrange.cpp
  src/ofp/actiontype.cpp
  src/ofp/actionfulltype.cpp
  src/ofp/mpaggregatestatsreply.cpp
  src/ofp/bucket.cpp
  src/ofp/bucketlist.cpp
  src/ofp/bundleaddmessage.cpp
  src/ofp/bundlecontrol.cpp
  src/ofp/bytelist.cpp
  src/ofp/byterange.cpp
  src/ofp/channellistener.cpp
  src/ofp/constants.cpp
  src/ofp/constants_error.cpp
  src/ofp/datapathid.cpp
  src/ofp/durationsec.cpp
  src/ofp/echoreply.cpp
  src/ofp/echorequest.cpp
  src/ofp/error.cpp
  src/ofp/experimenter.cpp
  src/ofp/featuresreply.cpp
  src/ofp/flowmod.cpp
  src/ofp/flowremoved.cpp
  src/ofp/flowremovedv6.cpp
  src/ofp/mpflowstatsreply.cpp
  src/ofp/mpflowstatsrequest.cpp
  src/ofp/mpflowmonitorrequest.cpp
  src/ofp/mpflowmonitorreply.cpp
  src/ofp/getasyncreply.cpp
  src/ofp/getconfigreply.cpp
  src/ofp/groupmod.cpp
  src/ofp/header.cpp
  src/ofp/hello.cpp
  src/ofp/instructionrange.cpp
  src/ofp/instructiontype.cpp
  src/ofp/ipv4address.cpp
  src/ofp/ipv6address.cpp
  src/ofp/ipv6endpoint.cpp
  src/ofp/lldpvalue.cpp
  src/ofp/log.cpp
  src/ofp/logger.cpp
  src/ofp/macaddress.cpp
  src/ofp/match.cpp
  src/ofp/matchheader.cpp
  src/ofp/matchpacket.cpp
  src/ofp/matchpacketbuilder.cpp
  src/ofp/message.cpp
  src/ofp/metermod.cpp
  src/ofp/mpdesc.cpp
  src/ofp/mpaggregatestatsreplyv6.cpp
  src/ofp/mpexperimenter.cpp
  src/ofp/mpgroupdesc.cpp
  src/ofp/mpgroupfeatures.cpp
  src/ofp/mpgroupstats.cpp
  src/ofp/mpgroupstatsrequest.cpp
  src/ofp/mpmeterconfig.cpp
  src/ofp/mpmeterconfigrequest.cpp
  src/ofp/mpmeterfeatures.cpp
  src/ofp/mpmeterstats.cpp
  src/ofp/mpportstats.cpp
  src/ofp/mpportstatsrequest.cpp
  src/ofp/mpqueuedescrequest.cpp
  src/ofp/mpqueuestats.cpp
  src/ofp/mpqueuestatsrequest.cpp
  src/ofp/mptablefeatures.cpp
  src/ofp/mptablestats.cpp
  src/ofp/multipartreply.cpp
  src/ofp/multipartrequest.cpp
  src/ofp/normalize.cpp
  src/ofp/originalmatch.cpp
  src/ofp/oxmfulltype.cpp
  src/ofp/oxmlist.cpp
  src/ofp/oxmregister.cpp
  src/ofp/oxmtype.cpp
  src/ofp/oxmtypeset.cpp
  src/ofp/packetin.cpp
  src/ofp/packetout.cpp
  src/ofp/packetoutv6.cpp
  src/ofp/pkt.cpp
  src/ofp/port.cpp
  src/ofp/portmod.cpp
  src/ofp/portrange.cpp
  src/ofp/portstatus.cpp
  src/ofp/prerequisites.cpp
  src/ofp/protocoliterator.cpp
  src/ofp/protocolversions.cpp
  src/ofp/queuegetconfigreply.cpp
  src/ofp/queue.cpp
  src/ofp/queuedesc.cpp
  src/ofp/queuegetconfigrequest.cpp
  src/ofp/queueproperty.cpp
  src/ofp/queuerange.cpp
  src/ofp/rawmessage.cpp
  src/ofp/requestforward.cpp
  src/ofp/rolerequest.cpp
  src/ofp/rolereply.cpp
  src/ofp/rolestatus.cpp
  src/ofp/rolestatusproperty.cpp
  src/ofp/setasync.cpp
  src/ofp/setconfig.cpp
  src/ofp/smallbuffer.cpp
  src/ofp/smallcstring.cpp
  src/ofp/standardmatch.cpp
  src/ofp/statheader.cpp
  src/ofp/tabledesc.cpp
  src/ofp/tablefeatureproperty.cpp
  src/ofp/tablemod.cpp
  src/ofp/tablemodproperty.cpp
  src/ofp/tablestatus.cpp
  src/ofp/timestamp.cpp
  src/ofp/types.cpp
  src/ofp/validation.cpp
  src/ofp/vlannumber.cpp
  src/ofp/yaml/decoder.cpp
  src/ofp/yaml/encoder.cpp
  src/ofp/yaml/getjson.cpp
  src/ofp/yaml/outputjson.cpp
  src/ofp/yaml/seterror.cpp
  src/ofp/yaml/ybytelist.cpp
  src/ofp/yaml/yconstants.cpp
  src/ofp/yaml/ycontext.cpp
  src/ofp/yaml/ygroupnumber.cpp
  src/ofp/yaml/ymeternumber.cpp
  src/ofp/yaml/yportnumber.cpp
  src/ofp/yaml/yqueuenumber.cpp
  src/ofp/yaml/ybuffernumber.cpp
  src/ofp/yaml/ycontrollermaxlen.cpp
  src/ofp/yaml/ytablenumber.cpp
  src/ofp/yaml/yschema.cpp
  src/ofp/demux/segmentcache.cpp
  src/ofp/demux/flowcache.cpp
  src/ofp/demux/flowstate.cpp
)

if(LIBOFP_ENABLE_JSONRPC)
  set(LIBOFP_SOURCES
    ${LIBOFP_SOURCES}
    src/ofp/driver.cpp
    src/ofp/sys/asio_utils.cpp
    src/ofp/sys/connection.cpp
    src/ofp/sys/defaulthandshake.cpp
    src/ofp/sys/engine.cpp
    src/ofp/sys/tcp_server.cpp
    src/ofp/sys/udp_server.cpp
    src/ofp/rpc/rpcchannellistener.cpp
    src/ofp/rpc/rpcconnection.cpp
    src/ofp/rpc/rpcconnectionstdio.cpp
    src/ofp/rpc/rpcconnectionunix.cpp
    src/ofp/rpc/rpcserver.cpp
    src/ofp/rpc/rpcencoder.cpp
    src/ofp/rpc/rpcevents.cpp
    src/ofp/rpc/filtertable.cpp
    src/ofp/rpc/filtertableentry.cpp
    src/ofp/rpc/filteractiongenericreply.cpp
    src/ofp/rpc/ratelimiter.cpp
  )
  if(LIBOFP_ENABLE_OPENSSL)
    set(LIBOFP_SOURCES
      ${LIBOFP_SOURCES}
      src/ofp/sys/dtls_adapter.cpp
      src/ofp/sys/dtls_utils.cpp
      src/ofp/sys/identity.cpp
      src/ofp/sys/securitycheck.cpp
    )
  endif()
endif()

if(LIBOFP_ENABLE_LIBPCAP)
  set(LIBOFP_SOURCES
    ${LIBOFP_SOURCES}
    src/ofp/demux/messagesource.cpp
    src/ofp/demux/pktsink.cpp
    src/ofp/demux/pktsource.cpp
    src/ofp/demux/pktfilter.cpp
    src/ofp/demux/messageconvert.cpp
  )
endif()

# Specify the config.h.in file.

set(LIBOFP_CONFIG_IN ${CMAKE_SOURCE_DIR}/include/ofp/config.h.in)
set(LIBOFP_CONFIG ${CMAKE_BINARY_DIR}/include/ofp/config.h)

# These source files are generated.

set(LIBOFP_SOURCES_GENERATED
  ${CMAKE_BINARY_DIR}/oxm/oxmfields.cpp
  ${CMAKE_BINARY_DIR}/oxm/oxmfieldsdata.cpp
)

# Determine flags from basename of CMAKE_BINARY_DIR. If directory name contains
# '+debug', set the build type to 'debug'.

string(TOLOWER "${CMAKE_BINARY_DIR}" binary_dir)
if(binary_dir MATCHES "/[^/]*\\+debug[^/]*$")
  set(CMAKE_BUILD_TYPE Debug)
elseif(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type is ${CMAKE_BUILD_TYPE}.")

# Specify our final warning set. Clang supports -Weverything, but GCC does not.

message(STATUS "Compiler is ${Libofp_COMPILER_SPEC}")
set(LIBOFP_SYSTEM_WARNINGS "-Wall")
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(LIBOFP_WARNINGS "-Weverything ${LIBOFP_WARNINGS_CXX11} ${LIBOFP_WARNINGS_DISABLED}")
  message(STATUS "All clang warnings enabled.")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(LIBOFP_WARNINGS "-Wpedantic -Wall -Wextra -Wformat=2 -Wstrict-aliasing=3 -Wno-unused-parameter -Wno-missing-field-initializers")
else()
  # Turn off Warnings for MSVC for now (FIXME).
  set(LIBOFP_WARNINGS "")
  set(LIBOFP_SYSTEM_WARNINGS "")
endif()

# Tell linker to remove dead code.

if(APPLE)
  # Apple makes it easy.
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-dead_strip")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Dead-strip the GCC build.
  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} -fdata-sections -ffunction-sections")
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -fdata-sections -ffunction-sections")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_BUILD_TYPE MATCHES "Debug")
  #set(LIBOFP_FSANITIZE "-fsanitize=address,undefined -fsanitize-undefined-trap-on-error")
  # [FIXME] Clang 9.0.0.9000037 in latest XCode is erroring in asyncConnect() when using
  # undefined sanitizer, apparently inside asio callback (via lambda function).
  set(LIBOFP_FSANITIZE "-fsanitize=address")
else()
  set(LIBOFP_FSANITIZE "")
endif()

# Enable Link Time Optimization in Release builds. After this step, these
# external symbols must still be left in oftr executable (MacOS X):
#
#   $ nm -gU oftr
#   0000000100000000 T __mh_execute_header
#   0000000100020510 T _libofp_buffer_free
#   00000001000201c0 T _libofp_decode
#   000000010001ff60 T _libofp_encode
#   000000010001fc80 T _libofp_version
#
# GCC also requires more changes to use gcc-ar, gcc-ranlib and possibly
# -pthread, -Wno-error=odr options. (CURRENTLY DISABLE -FLTO)
#
#if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
#  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} -pthread -flto")
#  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -pthread -flto -Wno-error=odr")
#  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -flto")
#  set(CMAKE_AR "gcc-ar")
#  set(CMAKE_NM "gcc-nm")
#  set(CMAKE_RANLIB "gcc-ranlib")
#  set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
#  set(CMAKE_C_ARCHIVE_FINISH true)
#endif()

# Compile with code coverage support.

if(LIBOFP_ENABLE_CODE_COVERAGE)
  set(LIBOFP_COVERAGE "-fprofile-arcs -ftest-coverage")
  # But, disable ctest coverage support. Coverage will run separately.
  set(COVERAGE_COMMAND "")
else()
  set(LIBOFP_COVERAGE "")
endif()

#-------------------------------------------------------------------------------
# Check system header files and symbols
#-------------------------------------------------------------------------------

include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckLibraryExists)

check_include_file(endian.h HAVE_ENDIAN_H)
check_include_file(machine/endian.h HAVE_MACHINE_ENDIAN_H)

if(LIBOFP_ENABLE_LIBPCAP)
  # "oftr decode" optionally depends on libpcap for live/offline decoding of packet captures.
  check_library_exists(pcap pcap_create "" HAVE_LIBPCAP)
  if(HAVE_LIBPCAP)
    set(CMAKE_REQUIRED_LIBRARIES pcap)
    set(LIBOFP_LINKED_LIBPCAP -lpcap)
    check_symbol_exists(pcap_open_offline_with_tstamp_precision pcap/pcap.h HAVE_PCAP_OPEN_OFFLINE_WITH_TSTAMP_PRECISION)
    check_symbol_exists(pcap_open_dead_with_tstamp_precision pcap/pcap.h HAVE_PCAP_OPEN_DEAD_WITH_TSTAMP_PRECISION)
  else()
    message(FATAL_ERROR "libpcap is not available.")
  endif()
endif()

if(LIBOFP_ENABLE_OPENSSL)
  if(LIBOFP_USE_BORINGSSL)
    set(HAVE_SSL_CTX_SET_KEYLOG_CALLBACK 1)
  else()
    set(CMAKE_REQUIRED_LIBRARIES ssl)
    check_symbol_exists(SSL_CTX_set_keylog_callback openssl/ssl.h HAVE_SSL_CTX_SET_KEYLOG_CALLBACK)
  endif()
endif()

#-------------------------------------------------------------------------------
# Build Library
#-------------------------------------------------------------------------------

# Prepare for testing. Subdirectories can add tests.

set(MEMORYCHECK_COMMAND_OPTIONS "-q --leak-check=no --error-exitcode=99")
include(CTest)

if(MEMORYCHECK_COMMAND AND LIBOFP_ENABLE_MEMCHECK_TESTS)
  set(LIBOFP_MEMCHECK "${MEMORYCHECK_COMMAND} ${MEMORYCHECK_COMMAND_OPTIONS}")
  message(STATUS "Memcheck command: ${LIBOFP_MEMCHECK}")
endif()

# Use pthreads.

find_package(Threads)
if(LIBOFP_ENABLE_JSONRPC)
  set(LIBOFP_LINKED_LIBS ${CMAKE_THREAD_LIBS_INIT})
endif()

# Call custom function to check for Python YAML module (optional for testing).

find_package(PythonInterp REQUIRED)
find_package(PythonModule REQUIRED)
find_python_module(yaml)

# Look up the specific git commits for libofp, asio, and boringssl. If we are
# building from a tarball instead of a git repo, the git commits will be empty,
# and they need to be loaded from the GIT-VERSION.cmake file.

include(GIT-VERSION.cmake OPTIONAL)
if(NOT Libofp_GIT_COMMIT_LIBOFP)  
  execute_process(COMMAND git rev-parse --verify HEAD OUTPUT_VARIABLE Libofp_GIT_COMMIT_LIBOFP)
  string(STRIP "${Libofp_GIT_COMMIT_LIBOFP}" Libofp_GIT_COMMIT_LIBOFP)

  execute_process(COMMAND git submodule status ${CMAKE_SOURCE_DIR}/external/asio OUTPUT_VARIABLE Libofp_GIT_COMMIT_ASIO)
  if(NOT "${Libofp_GIT_COMMIT_ASIO}" STREQUAL "")
    string(SUBSTRING "${Libofp_GIT_COMMIT_ASIO}" 1 40 Libofp_GIT_COMMIT_ASIO)
  endif()

  execute_process(COMMAND git submodule status ${CMAKE_SOURCE_DIR}/external/boringssl OUTPUT_VARIABLE Libofp_GIT_COMMIT_BORINGSSL)
  if(NOT "${Libofp_GIT_COMMIT_BORINGSSL}" STREQUAL "")
    string(SUBSTRING "${Libofp_GIT_COMMIT_BORINGSSL}" 1 40 Libofp_GIT_COMMIT_BORINGSSL)
  endif()
endif()

message(STATUS "Libofp commit: ${Libofp_GIT_COMMIT_LIBOFP}")
message(STATUS "Asio commit: ${Libofp_GIT_COMMIT_ASIO}")
message(STATUS "BoringSSL commit: ${Libofp_GIT_COMMIT_BORINGSSL}")

# Specify auto-generated config.h file.

configure_file("${LIBOFP_CONFIG_IN}" "${LIBOFP_CONFIG}")

# Copy over the CTestCustom.cmake file into the build tree.

configure_file("${CMAKE_SOURCE_DIR}/CTestCustom.cmake" "${CMAKE_BINARY_DIR}/CTestCustom.cmake")

# DataTypes.h in yaml-io depends on having these set. These macros have no effect
# in C++11.

add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

# Add system includes.

include_directories(SYSTEM ${LIBOFP_SYSTEM_INCLUDES})

# Set flags to use when compiling third party code. Basically, we just use our
# project flags with `-Wall` warnings. If we apply a stricter set, we'll clutter
# up the build log with warnings.

set(CMAKE_CXX_FLAGS "${LIBOFP_CXX_FLAGS} ${LIBOFP_SYSTEM_WARNINGS}")
set(CMAKE_C_FLAGS "${LIBOFP_C_FLAGS} ${LIBOFP_SYSTEM_WARNINGS}")

# Add subdirectories of third party projects.

add_subdirectory(external/yaml-io)
if(LIBOFP_ENABLE_OPENSSL AND LIBOFP_USE_BORINGSSL)
  # Boringssl uses the __FILE__ predefined macro in "openssl/err.h". Redefine
  # __FILE__ to __func__ just for boringssl. (TODO)
  # Some tests use alignas(64) which elicits a warning on GCC 7.0+/ARM.
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=attributes")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=attributes")
  endif()
  add_subdirectory(external/boringssl)
endif()

# Modify flags for compiling our code. Here, we change the warning flags to
# our own super picky set (-Weverything minus a few).

set(CMAKE_CXX_FLAGS "${LIBOFP_CXX_FLAGS} ${LIBOFP_WARNINGS} ${LIBOFP_DEFINES} ${LIBOFP_COVERAGE} ${LIBOFP_FSANITIZE}")
set(CMAKE_C_FLAGS "${LIBOFP_C_FLAGS} ${LIBOFP_WARNINGS} ${LIBOFP_DEFINES} ${LIBOFP_COVERAGE} ${LIBOFP_FSANITIZE}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LIBOFP_COVERAGE} ${LIBOFP_FSANITIZE} ${LIBOFP_LDFLAGS}")

message(STATUS "LIBOFP_C_FLAGS: ${LIBOFP_C_FLAGS}")
message(STATUS "LIBOFP_CXX_FLAGS: ${LIBOFP_CXX_FLAGS}")

# Set up the source include directory. N.B. Includes for third-party sources are
# baked into CMAKE_CXX_FLAGS using -isystem.

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include)

# Before we can build libofp.a, we need to build oxmfields.h, oxmfields.cpp,
# and oxmfieldsdata.cpp. Generated source files are built into directory 'oxm'
# in the cmake binary directory.

add_subdirectory(oxm)

# Tell CMake the oxm source files won't be available until build time.

set_source_files_properties(${LIBOFP_SOURCES_GENERATED} PROPERTIES GENERATED 1)

# Build libofp.a.

add_library(ofp ${LIBOFP_SOURCES} ${LIBOFP_SOURCES_GENERATED})
if(LIBOFP_ENABLE_OPENSSL)
  target_link_libraries(ofp yamlio ssl crypto)
else()
  target_link_libraries(ofp yamlio)
endif()

# Make sure the source is generated before the executable builds.

add_dependencies(ofp Libofp_oxmfields_h Libofp_oxmfields_cpp Libofp_oxmfieldsdata_cpp)

#-------------------------------------------------------------------------------
# Build Examples
#-------------------------------------------------------------------------------

if(LIBOFP_ENABLE_JSONRPC)
  add_subdirectory(example/nulldemo)
  add_subdirectory(example/testagent)
  add_subdirectory(example/controller)
  add_subdirectory(example/python)
  add_subdirectory(example/flowbench)
  add_subdirectory(example/ctypes)
  add_subdirectory(example/cbenchreply)
endif()

#-------------------------------------------------------------------------------
# Build Tool
#-------------------------------------------------------------------------------

add_subdirectory(tools/oftr)

#-------------------------------------------------------------------------------
# Test
#-------------------------------------------------------------------------------

add_subdirectory(test)

#-------------------------------------------------------------------------------
# Documentation
#-------------------------------------------------------------------------------

add_subdirectory(docs)

#-------------------------------------------------------------------------------
# CPack Installer Support
#-------------------------------------------------------------------------------

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${Libofp_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Libofp_VERSION_MINOR}")
set(CPACK_PACKAGE_CONTACT "${Libofp_MAINTAINER_EMAIL}")
set(CPACK_STRIP_FILES TRUE)
include(CPack)

#-------------------------------------------------------------------------------
# Support for Running Cpplint
#-------------------------------------------------------------------------------

set(CPPLINT_PATH external/cpplint/cpplint.py)
set(CPPLINT_ARGS --filter=-build/include,-runtime/references,-build/namespaces,-build/c++11 --root=include --linelength=200 --extensions=h,cpp,ipp)

add_custom_target(cpplint
  COMMAND find include src tools example oxm test \( -name "*.h" -o -name "*.cpp" -o -name "*.ipp" \) -exec ${PYTHON_EXECUTABLE} ${CPPLINT_PATH} ${CPPLINT_ARGS} {} +
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Checking all libofp code with cpplint" VERBATIM
)

#-------------------------------------------------------------------------------
# Support for Running Clang-format
#-------------------------------------------------------------------------------

add_custom_target(format-all
  COMMAND find include src tools example oxm test \( -name "*.h" -o -name "*.cpp" -o -name "*.ipp" \) -exec clang-format -i {} +
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Formatting all libofp code with clang-format" VERBATIM
)

#-------------------------------------------------------------------------------
# Support for Running Clang-tidy
#-------------------------------------------------------------------------------

set(CLANGTIDY_PATH external/cpplint/clangtidy.sh)

add_custom_target(clang-tidy
  COMMAND find src -name "*.cpp" -print -exec ${CLANGTIDY_PATH} {} \;
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Checking all libofp code with clang-tidy" VERBATIM
)


# Run unit tests for boringssl.

if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
  add_test(
    NAME boringssl-all-tests 
    COMMAND go run util/all_tests.go -build-dir "${CMAKE_BINARY_DIR}/external/boringssl/"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/external/boringssl"
  )
  set_tests_properties(boringssl-all-tests PROPERTIES TIMEOUT 300)
endif()
