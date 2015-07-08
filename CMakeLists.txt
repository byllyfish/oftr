#=====- CMakeLists.txt ---------------------------------------------------=====#
#
# This is the top-level CMakeLists.txt file for the LibOFP project.
#
#=====--------------------------------------------------------------------=====#
#
# The CMakeLists.txt file supports building debug, 32 bit, or 64 bit builds. To
# set the build options, create a Build directory with your options in the 
# directory name. The following options are supported:
#
#    +debug    Build a debug version.
#    +32bit    Build a 32-bit version.
#    +64bit    Build a 64-bit version.
#
# For example, if the build directory is named `Build+Debug`, cmake will build
# debug version.
#

cmake_minimum_required(VERSION 2.8.8)
project(Libofp)

# Add path for custom modules

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

# Set the version number of the library (use semantic versioning).

set(Libofp_VERSION_MAJOR 0)
set(Libofp_VERSION_MINOR 5)
set(Libofp_VERSION_PATCH 0)
set(Libofp_DOWNLOAD_URL "https://github.com/byllyfish/libofp")
set(Libofp_MAINTAINER_EMAIL "Bill Fisher <william.w.fisher@gmail.com>")

# You can build libofp with or without the JSON-RPC server.

set(LIBOFP_ENABLE_JSONRPC TRUE CACHE BOOL "Enable the JSON-RPC server")

if(NOT LIBOFP_ENABLE_JSONRPC)
  message(STATUS "JSON-RPC server is disabled.")
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

set(LIBOFP_EXTERNAL_INCLUDES "-isystem ${CMAKE_SOURCE_DIR}/external/asio/asio/include -isystem ${CMAKE_BINARY_DIR}/external/yaml-io/include -isystem ${CMAKE_SOURCE_DIR}/external/yaml-io/include -isystem ${CMAKE_SOURCE_DIR}/external/boringssl/include")

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
  # MSVC does not support -isystem; replace -isystem with -I
  string(REPLACE -isystem -I LIBOFP_EXTERNAL_INCLUDES "${LIBOFP_EXTERNAL_INCLUDES}")
endif()

# Specify additional definitions.

set(LIBOFP_DEFINES "-D ASIO_STANDALONE -D ASIO_NO_TYPEID -D ASIO_NO_EXCEPTIONS")
#set(LIBOFP_DEFINES "${LIBOFP_DEFINES} -D ASIO_ENABLE_HANDLER_TRACKING")
# -D ASIO_NO_DEPRECATED -D ASIO_NO_IOSTREAM 

if(APPLE AND UNIX)
  set(LIBOFP_DEFINES "${LIBOFP_DEFINES} -D LIBOFP_TARGET_DARWIN")
endif()

# Specify the default set of compiler flags for all sources, including third
# party libraries. Place flags that don't affect the preprocessor or warnings
# here. This set of flags controls compiler output. It is important that all
# source code be compiled with the same flags.

set(LIBOFP_C_FLAGS "-fstrict-aliasing")
set(LIBOFP_CXX_FLAGS "-fstrict-aliasing -fno-rtti -fno-exceptions -std=c++11")

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
  src/ofp/defaulthandshake.cpp
  src/ofp/driver.cpp
  src/ofp/echoreply.cpp
  src/ofp/echorequest.cpp
  src/ofp/enetaddress.cpp
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
  src/ofp/log.cpp
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
  src/ofp/mpqueuestats.cpp
  src/ofp/mpqueuestatsrequest.cpp
  src/ofp/mptablefeatures.cpp
  src/ofp/mptablestats.cpp
  src/ofp/multipartreply.cpp
  src/ofp/multipartrequest.cpp
  src/ofp/originalmatch.cpp
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
  src/ofp/queuegetconfigrequest.cpp
  src/ofp/queueproperty.cpp
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
  src/ofp/tablefeatureproperty.cpp
  src/ofp/tablemod.cpp
  src/ofp/timestamp.cpp
  src/ofp/transmogrify.cpp
  src/ofp/types.cpp
  src/ofp/validation.cpp
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
  src/ofp/yaml/decoder.cpp
  src/ofp/yaml/encoder.cpp
  src/ofp/yaml/outputjson.cpp
  src/ofp/yaml/seterror.cpp
  src/ofp/yaml/yconstants.cpp
  src/ofp/yaml/ycontext.cpp
  src/ofp/yaml/ygroupnumber.cpp
  src/ofp/yaml/yportnumber.cpp
  src/ofp/yaml/ybuffernumber.cpp
  src/ofp/yaml/ycontrollermaxlen.cpp
  src/ofp/yaml/ytablenumber.cpp
  src/ofp/yaml/yschema.cpp
)

# Specify the config.h.in file.

set(LIBOFP_CONFIG_IN ${CMAKE_SOURCE_DIR}/include/ofp/config.h.in)
set(LIBOFP_CONFIG ${CMAKE_BINARY_DIR}/include/ofp/config.h)

# These source files are generated.

set(LIBOFP_SOURCES_GENERATED
  ${CMAKE_BINARY_DIR}/oxm/oxmfields.cpp
  ${CMAKE_BINARY_DIR}/oxm/oxmfieldsdata.cpp
)

# Determine flags from basename of CMAKE_BINARY_DIR. If directory name contains
# '+debug', set the build type to 'debug', otherwise set to release build. The
# comparison test for '+debug' is case-insensitive.

string(TOLOWER "${CMAKE_BINARY_DIR}" binary_dir)
if("${binary_dir}" MATCHES "/[^/]*\\+debug[^/]*$")
  message(STATUS "Setting build type to DEBUG.")
  set(CMAKE_BUILD_TYPE debug)
else()
  message(STATUS "Setting build type to RELEASE.")
  set(CMAKE_BUILD_TYPE release)
endif()

# If binary_dir contains +32bit or +64bit set the -m32 or -m64 flag respectively.

if("${binary_dir}" MATCHES "/[^/]*\\+64bit[^/]*$")
  message(STATUS "Adding -m64 build option.")
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -m64")
  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} -m64")
elseif("${binary_dir}" MATCHES "/[^/]*\\+32bit[^/]*$")
  message(STATUS "Adding -m32 build option.")
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -m32")
  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} -m32")
endif()

# Specify our final warning set. Clang supports -Weverything, but GCC does not.

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(LIBOFP_WARNINGS "-Weverything ${LIBOFP_WARNINGS_CXX11} ${LIBOFP_WARNINGS_DISABLED}")
  message(STATUS "All clang warnings enabled.")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(LIBOFP_WARNINGS "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
else()
  set(LIBOFP_WARNINGS "-Wall")
endif()

# Tell linker to remove dead code on OSX.

if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,-dead_strip")
endif()

#-------------------------------------------------------------------------------
# Build Library
#-------------------------------------------------------------------------------

# Prepare for testing. Subdirectories can add tests.
# N.B. The suppressions file must be defined before `include(CTest)`; otherwise
# the correct value will not appear in the generated DartConfiguration.tcl.

set(MEMORYCHECK_COMMAND_OPTIONS "-v --trace-children=yes --trace-children-skip=/bin/sleep,/usr/bin/diff,/usr/bin/basename,/usr/bin/dirname --leak-check=full --error-exitcode=99")
set(MEMORYCHECK_SUPPRESSIONS_FILE "${CMAKE_SOURCE_DIR}/test/valgrind.supp")
include(CTest)

# Use pthreads.

find_package(Threads)
set(LIBOFP_LINKED_LIBS ${CMAKE_THREAD_LIBS_INIT})

# Call custom function to check for Python YAML module (optional for testing).

find_package(PythonInterp REQUIRED)
find_package(PythonModule REQUIRED)
find_python_module(yaml)

# Look up the specific git commits for libofp, asio, and boringssl.

execute_process(COMMAND git rev-parse --verify HEAD OUTPUT_VARIABLE Libofp_GIT_COMMIT_LIBOFP)
string(STRIP "${Libofp_GIT_COMMIT_LIBOFP}" Libofp_GIT_COMMIT_LIBOFP)
message(STATUS "Libofp commit: ${Libofp_GIT_COMMIT_LIBOFP}")

execute_process(COMMAND git submodule status ${CMAKE_SOURCE_DIR}/external/asio OUTPUT_VARIABLE Libofp_GIT_COMMIT_ASIO)
string(SUBSTRING "${Libofp_GIT_COMMIT_ASIO}" 1 40 Libofp_GIT_COMMIT_ASIO)
message(STATUS "Asio commit: ${Libofp_GIT_COMMIT_ASIO}")

execute_process(COMMAND git submodule status ${CMAKE_SOURCE_DIR}/external/boringssl OUTPUT_VARIABLE Libofp_GIT_COMMIT_BORINGSSL)
string(SUBSTRING "${Libofp_GIT_COMMIT_BORINGSSL}" 1 40 Libofp_GIT_COMMIT_BORINGSSL)
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
add_subdirectory(external/googletest)
add_subdirectory(external/boringssl)

# Modify flags for compiling our code. Here, we change the warning flags to
# our own super picky set (-Weverything minus a few).

set(CMAKE_CXX_FLAGS "${LIBOFP_CXX_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} ${LIBOFP_WARNINGS} ${LIBOFP_DEFINES}")
set(CMAKE_C_FLAGS "${LIBOFP_C_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} ${LIBOFP_WARNINGS} ${LIBOFP_DEFINES}")

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
target_link_libraries(ofp yamlio ssl crypto)

# Make sure the source is generated before the executable builds.

add_dependencies(ofp Libofp_oxmfields_h Libofp_oxmfields_cpp Libofp_oxmfieldsdata_cpp)

#-------------------------------------------------------------------------------
# Build Examples
#-------------------------------------------------------------------------------

add_subdirectory(example/nulldemo)
add_subdirectory(example/testagent)
add_subdirectory(example/controller)
add_subdirectory(example/python)
add_subdirectory(example/flowbench)

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
# Install Library
#-------------------------------------------------------------------------------

# Set install prefix to local if CMAKE_BUILD_TYPE is debug.

if(CMAKE_BUILD_TYPE STREQUAL "debug")
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/local)
endif()

#-------------------------------------------------------------------------------
# CPack Installer Support
#-------------------------------------------------------------------------------

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${Libofp_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Libofp_VERSION_MINOR}")
set(CPACK_PACKAGE_CONTACT "${Libofp_MAINTAINER_EMAIL}")
set(CPACK_STRIP_FILES "bin/libofp")
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
