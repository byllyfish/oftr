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
# To build libofp without the JSON-RPC server:
#
#    cmake -DLIBOFP_ENABLE_JSONRPC=false ..
#
# To build without memcheck tests:
#
#    cmake -DLIBOFP_ENABLE_MEMCHECK_TESTS=false ..
#
# To build with code coverage:
#
#    cmake -DLIBOFP_ENABLE_CODE_COVERAGE=true ..
#

cmake_minimum_required(VERSION 2.8.8)
project(Libofp)

# Log version of cmake

message(STATUS "Running cmake ${CMAKE_VERSION} on ${CMAKE_SYSTEM} ${CMAKE_SYSTEM_PROCESSOR}")

# Add path for custom modules

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set the version number of the library (use semantic versioning).

set(Libofp_VERSION 0.17.0)
set(Libofp_GITHUB_URL "https://github.com/byllyfish/libofp")
set(Libofp_MAINTAINER_EMAIL "William W. Fisher <william.w.fisher@gmail.com>")

string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1" Libofp_VERSION_MAJOR ${Libofp_VERSION})
string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\2" Libofp_VERSION_MINOR ${Libofp_VERSION})
string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\3" Libofp_VERSION_PATCH ${Libofp_VERSION})

# Set version numbers for RPC API.

set(Libofp_RPC_API_MAJOR 0)
set(Libofp_RPC_API_MINOR 1)

# You can build libofp with or without the JSON-RPC server.

set(LIBOFP_ENABLE_JSONRPC TRUE CACHE BOOL "Enable the JSON-RPC server")

if(NOT LIBOFP_ENABLE_JSONRPC)
  message(STATUS "JSON-RPC server is disabled.")
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
# LIB_OFP_WARNINGS_DISABLED.

set(LIBOFP_WARNINGS_DISABLED "-Wno-unused-parameter -Wno-weak-vtables -Wno-switch-enum -Wno-unreachable-code -Wno-covered-switch-default")

# LibOFP is a C++11-only project. Disable warnings about C++98 compatibility.

set(LIBOFP_WARNINGS_CXX11 "-Wno-c++98-compat -Wno-c++98-compat-pedantic")

# Specify include directories for third party headers. We use -isystem instead
# of -I because we don't want to receive compiler warnings from these headers.

set(LIBOFP_EXTERNAL_INCLUDES "-isystem ${CMAKE_SOURCE_DIR}/external/asio/asio/include")
set(LIBOFP_EXTERNAL_INCLUDES "${LIBOFP_EXTERNAL_INCLUDES} -isystem ${CMAKE_BINARY_DIR}/external/yaml-io/include")
set(LIBOFP_EXTERNAL_INCLUDES "${LIBOFP_EXTERNAL_INCLUDES} -isystem ${CMAKE_SOURCE_DIR}/external/yaml-io/include")
if(LIBOFP_ENABLE_JSONRPC)
  set(LIBOFP_EXTERNAL_INCLUDES "${LIBOFP_EXTERNAL_INCLUDES} -isystem ${CMAKE_SOURCE_DIR}/external/boringssl/include")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # MSVC does not support -isystem; replace -isystem with -I
  string(REPLACE -isystem -I LIBOFP_EXTERNAL_INCLUDES "${LIBOFP_EXTERNAL_INCLUDES}")
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

set(LIBOFP_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing -fvisibility=hidden")
set(LIBOFP_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing -fno-rtti -fno-exceptions -std=c++11 -fvisibility=hidden")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} -pie -fPIE -rdynamic")
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -pie -fPIE -rdynamic")
endif()

if(APPLE AND UNIX)
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -stdlib=libc++")
endif()

# Add new library source files here. Add unittest source files to
# test/CMakeLists.txt.

set(LIBOFP_SOURCES
  src/ofp/actionrange.cpp
  src/ofp/actiontype.cpp
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
  src/ofp/echoreply.cpp
  src/ofp/echorequest.cpp
  src/ofp/error.cpp
  src/ofp/experimenter.cpp
  src/ofp/featuresreply.cpp
  src/ofp/flowmod.cpp
  src/ofp/flowremoved.cpp
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
  src/ofp/macaddress.cpp
  src/ofp/match.cpp
  src/ofp/matchheader.cpp
  src/ofp/matchpacket.cpp
  src/ofp/message.cpp
  src/ofp/metermod.cpp
  src/ofp/mpdesc.cpp
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
  src/ofp/originalmatch.cpp
  src/ofp/oxmfulltype.cpp
  src/ofp/oxmlist.cpp
  src/ofp/oxmtype.cpp
  src/ofp/oxmtypeset.cpp
  src/ofp/packetin.cpp
  src/ofp/packetout.cpp
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
  src/ofp/tabledesc.cpp
  src/ofp/tablefeatureproperty.cpp
  src/ofp/tablemod.cpp
  src/ofp/tablemodproperty.cpp
  src/ofp/tablestatus.cpp
  src/ofp/timestamp.cpp
  src/ofp/transmogrify.cpp
  src/ofp/types.cpp
  src/ofp/validation.cpp
  src/ofp/yaml/decoder.cpp
  src/ofp/yaml/encoder.cpp
  src/ofp/yaml/outputjson.cpp
  src/ofp/yaml/seterror.cpp
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
)

if(LIBOFP_ENABLE_JSONRPC)
  set(LIBOFP_SOURCES
    ${LIBOFP_SOURCES}
    src/ofp/defaulthandshake.cpp
    src/ofp/driver.cpp
    src/ofp/sys/asio_utils.cpp
    src/ofp/sys/connection.cpp
    src/ofp/sys/dtls_adapter.cpp
    src/ofp/sys/dtls_utils.cpp
    src/ofp/sys/engine.cpp
    src/ofp/sys/identity.cpp
    src/ofp/sys/tcp_server.cpp
    src/ofp/sys/udp_server.cpp
    src/ofp/rpc/rpcchannellistener.cpp
    src/ofp/rpc/rpcconnection.cpp
    src/ofp/rpc/rpcconnectionsession.cpp
    src/ofp/rpc/rpcconnectionstdio.cpp
    src/ofp/rpc/rpcserver.cpp
    src/ofp/rpc/rpcsession.cpp
    src/ofp/rpc/rpcencoder.cpp
    src/ofp/rpc/rpcevents.cpp
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

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(LIBOFP_WARNINGS "-Weverything ${LIBOFP_WARNINGS_CXX11} ${LIBOFP_WARNINGS_DISABLED}")
  message(STATUS "All clang warnings enabled.")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(LIBOFP_WARNINGS "-Wpedantic -Wall -Wextra -Wformat=2 -Wno-unused-parameter -Wno-missing-field-initializers")
else()
  set(LIBOFP_WARNINGS "-Wall")
endif()

# Tell linker to remove dead code.

if(APPLE)
  # Apple makes it easy.
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-dead_strip")
else()
  # Dead-strip the GCC build.
  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} -fdata-sections -ffunction-sections")
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -fdata-sections -ffunction-sections")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_BUILD_TYPE MATCHES "Debug")
  # -fsanitize=undefined doesn't seem to work with Clang 7.0.2, even with -frtti
  # added back in.
  set(LIBOFP_FSANITIZE "-fsanitize=address,alignment,bool,bounds,enum,null,object-size,shift,signed-integer-overflow -fsanitize-undefined-trap-on-error")
  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} ${LIBOFP_FSANITIZE}")
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} ${LIBOFP_FSANITIZE}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LIBOFP_FSANITIZE}")
endif()

# Compile with code coverage support.

if(LIBOFP_ENABLE_CODE_COVERAGE)
  set(LIBOFP_COVERAGE "-fprofile-arcs -ftest-coverage")
else()
  set(LIBOFP_COVERAGE "")
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

if(LIBOFP_ENABLE_JSONRPC)
  find_package(Threads)
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

# Set flags to use when compiling third party code. Basically, we just use our
# project flags with `-Wall` warnings. If we apply a stricter set, we'll clutter
# up the build log with warnings.

set(CMAKE_CXX_FLAGS "${LIBOFP_CXX_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} -Wall -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "${LIBOFP_C_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} -Wall")

# Add subdirectories of third party projects.

add_subdirectory(external/yaml-io)
if(LIBOFP_ENABLE_JSONRPC)
  add_subdirectory(external/boringssl)
endif()

# Modify flags for compiling our code. Here, we change the warning flags to
# our own super picky set (-Weverything minus a few).

set(CMAKE_CXX_FLAGS "${LIBOFP_CXX_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} ${LIBOFP_WARNINGS} ${LIBOFP_DEFINES} ${LIBOFP_COVERAGE}")
set(CMAKE_C_FLAGS "${LIBOFP_C_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} ${LIBOFP_WARNINGS} ${LIBOFP_DEFINES} ${LIBOFP_COVERAGE}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LIBOFP_COVERAGE}")

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
if(LIBOFP_ENABLE_JSONRPC)
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
endif()

#-------------------------------------------------------------------------------
# Build Tool
#-------------------------------------------------------------------------------

add_subdirectory(tools/libofp)

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
set(CPPLINT_ARGS --filter=-build/include_what_you_use,-runtime/references,-build/namespaces,-build/c++11 --root=include --linelength=200 --extensions=h,cpp,ipp)

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
