#=====- CMakeLists.txt ---------------------------------------------------=====#
#
# This is the top-level CMakeLists.txt file for the LibOFP project.
#
#=====--------------------------------------------------------------------=====#

cmake_minimum_required(VERSION 2.8)
project(libofp)

# Add path for custom modules

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

# Set the version number of the library.

set(libofp_VERSION_MAJOR 0)
set(libofp_VERSION_MINOR 2)

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

set(LIBOFP_WARNINGS_DISABLED "-Wno-unused-parameter -Wno-documentation -Wno-weak-vtables -Wno-switch-enum -Wno-unreachable-code")

# LibOFP is a C++11-only project. Disable warnings about C++98 compatibility.

set(LIBOFP_WARNINGS_CXX11 "-Wno-c++98-compat -Wno-c++98-compat-pedantic")

# Specify include directories for third party headers. We use -isystem instead
# of -I because we don't want to receive compiler warnings from these headers.

set(LIBOFP_EXTERNAL_INCLUDES "-isystem ${CMAKE_SOURCE_DIR}/external/asio/asio/include -isystem ${CMAKE_SOURCE_DIR}/external/yaml-io/include")

# Specify additional definitions.

set(LIBOFP_DEFINES "-D ASIO_STANDALONE -D ASIO_NO_TYPEID -D ASIO_NO_EXCEPTIONS")

# Specify the default set of compiler flags for all sources, including third
# party libraries. Place flags that don't affect the preprocessor or warnings
# here. This set of flags controls compiler output. It is important that all
# source code be compiled with the same flags.

set(LIBOFP_C_FLAGS "-fstrict-aliasing")
set(LIBOFP_CXX_FLAGS "-fstrict-aliasing -fno-rtti -fno-exceptions -std=c++11")

if (APPLE AND UNIX)
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -stdlib=libc++")
endif()

# Add new library source files here. Add unittest source files to
# test/CMakeLists.txt.

set(LIBOFP_SOURCES
	src/ofp/actionrange.cpp
  src/ofp/mpaggregatestatsreply.cpp
  src/ofp/bucket.cpp
  src/ofp/bucketlist.cpp
  src/ofp/bytelist.cpp
  src/ofp/byterange.cpp
	src/ofp/channellistener.cpp
	src/ofp/connectiontimer.cpp
  src/ofp/constants.cpp
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
  src/ofp/getasyncreply.cpp
	src/ofp/getconfigreply.cpp
  src/ofp/groupmod.cpp
	src/ofp/header.cpp
	src/ofp/hello.cpp
  src/ofp/instructionrange.cpp
	src/ofp/ipv4address.cpp
	src/ofp/ipv6address.cpp
  src/ofp/ipv6endpoint.cpp
	src/ofp/log.cpp
	src/ofp/message.cpp
  src/ofp/mpportstats.cpp
  src/ofp/mpportstatsrequest.cpp
  src/ofp/mpqueuestats.cpp
  src/ofp/mpqueuestatsrequest.cpp
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
  src/ofp/rolerequest.cpp
  src/ofp/rolereply.cpp
  src/ofp/setasync.cpp
	src/ofp/setconfig.cpp
  src/ofp/smallbuffer.cpp
	src/ofp/standardmatch.cpp
	src/ofp/sys/asio_utils.cpp
	src/ofp/sys/connection.cpp
	src/ofp/sys/engine.cpp
	src/ofp/sys/tcp_server.cpp
	src/ofp/sys/udp_connection.cpp
	src/ofp/sys/udp_server.cpp
  src/ofp/tablemod.cpp
	src/ofp/transmogrify.cpp
	src/ofp/types.cpp
	src/ofp/api/apichannellistener.cpp
	src/ofp/api/apiconnection.cpp
  src/ofp/api/apiconnectionsession.cpp
  src/ofp/api/apiconnectionstdio.cpp
	src/ofp/api/apiencoder.cpp
	src/ofp/api/apievents.cpp
	src/ofp/api/apiserver.cpp
  src/ofp/api/apisession.cpp
	src/ofp/yaml/decoder.cpp
	src/ofp/yaml/encoder.cpp
  src/ofp/yaml/outputjson.cpp
  src/ofp/yaml/yconstants.cpp
)

# Specify the config.h.in file.

set(LIBOFP_CONFIG_IN ${CMAKE_SOURCE_DIR}/include/ofp/config.h.in)
set(LIBOFP_CONFIG ${CMAKE_BINARY_DIR}/include/ofp/config.h)

# These source files are generated.

set(LIBOFP_SOURCES_GENERATED
  ${CMAKE_BINARY_DIR}/oxm/oxmfields.cpp
  ${CMAKE_BINARY_DIR}/oxm/oxmfieldsdata.cpp
)

set(LIBOFP_HEADERS_GENERATED
  ${CMAKE_BINARY_DIR}/include/ofp/config.h
  ${CMAKE_BINARY_DIR}/include/ofp/oxmfields.h
)

# Specify list of libofp's header files. This list is used during installs.
# Note: Two header files are generated during the build -- they are not included.
# Generate list using `find . -name '*.h' -print
# TODO: Not all of these headers are required by library clients. We could just
# require files included from ofp.h.

set(LIBOFP_HEADER_FILES
  include/ofp/actioniterator.h
  include/ofp/actionlist.h
  include/ofp/actionrange.h
  include/ofp/actions.h
  include/ofp/actiontype.h
  include/ofp/mpaggregatestatsreply.h
  include/ofp/mpaggregatestatsrequest.h
  include/ofp/array.h
  include/ofp/bucket.h
  include/ofp/bucketlist.h
  include/ofp/bucketrange.h
  include/ofp/bytelist.h
  include/ofp/byteorder.h
  include/ofp/byterange.h
  include/ofp/channel.h
  include/ofp/channellistener.h
  include/ofp/constants.h
  include/ofp/constants_deprecated.h
  include/ofp/constants_error.h
  include/ofp/datapathid.h
  include/ofp/defaultauxiliarylistener.h
  include/ofp/defaulthandshake.h
  include/ofp/deferred.h
  include/ofp/driver.h
  include/ofp/echoreply.h
  include/ofp/echorequest.h
  include/ofp/enetaddress.h
  include/ofp/error.h
  include/ofp/experimenter.h
  include/ofp/featuresreply.h
  include/ofp/flowmod.h
  include/ofp/flowremoved.h
  include/ofp/mpflowstatsreply.h
  include/ofp/mpflowstatsrequest.h
  include/ofp/getasyncreply.h
  include/ofp/getconfigreply.h
  include/ofp/groupmod.h
  include/ofp/header.h
  include/ofp/headeronly.h
  include/ofp/hello.h
  include/ofp/instructioniterator.h
  include/ofp/instructionlist.h
  include/ofp/instructionrange.h
  include/ofp/instructions.h
  include/ofp/instructiontype.h
  include/ofp/ipv4address.h
  include/ofp/ipv6address.h
  include/ofp/ipv6endpoint.h
  include/ofp/log.h
  include/ofp/match.h
  include/ofp/matchbuilder.h
  include/ofp/memorychannel.h
  include/ofp/message.h
  include/ofp/mpdesc.h
  include/ofp/mpportstats.h
  include/ofp/mpportstatsrequest.h
  include/ofp/mpqueuestats.h
  include/ofp/mpqueuestatsrequest.h
  include/ofp/mptablestats.h
  include/ofp/multipartreply.h
  include/ofp/multipartrequest.h
  include/ofp/originalmatch.h
  include/ofp/oxmiterator.h
  include/ofp/oxmlist.h
  include/ofp/oxmrange.h
  include/ofp/oxmtype.h
  include/ofp/oxmtypeset.h
  include/ofp/oxmvalue.h
  include/ofp/packetin.h
  include/ofp/packetout.h
  include/ofp/padding.h
  include/ofp/port.h
  include/ofp/portlist.h
  include/ofp/portmod.h
  include/ofp/portstatus.h
  include/ofp/prerequisites.h
  include/ofp/protocolrange.h
  include/ofp/protocoliterator.h
  include/ofp/protocolmsg.h
  include/ofp/protocolversions.h
  include/ofp/queuegetconfigreply.h
  include/ofp/queuegetconfigrequest.h
  include/ofp/rolereply.h
  include/ofp/rolerequest.h
  include/ofp/setasync.h
  include/ofp/setconfig.h
  include/ofp/smallcstring.h
  include/ofp/standardmatch.h
  include/ofp/sys/asio_utils.h
  include/ofp/sys/buffered.h
  include/ofp/sys/connection.h
  include/ofp/sys/connectiontimer.h
  include/ofp/sys/engine.h
  include/ofp/sys/plaintext.h
  include/ofp/sys/server.h
  include/ofp/sys/tcp_connection.h
  include/ofp/sys/tcp_server.h
  include/ofp/sys/udp_connection.h
  include/ofp/sys/udp_server.h
  include/ofp/tablemod.h
  include/ofp/transmogrify.h
  include/ofp/types.h
  include/ofp/unittest.h
  include/ofp/writable.h
  include/ofp/api/apichannellistener.h
  include/ofp/api/apiconnection.h
  include/ofp/api/apiconnectionsession.h
  include/ofp/api/apiconnectionstdio.h
  include/ofp/api/apiencoder.h
  include/ofp/api/apievents.h
  include/ofp/api/apiserver.h
  include/ofp/api/apisession.h
  include/ofp/yaml/decoder.h
  include/ofp/yaml/encoder.h
  include/ofp/yaml/outputjson.h
  include/ofp/yaml/yactions.h
  include/ofp/yaml/yaddress.h
  include/ofp/yaml/ympaggregatestatsreply.h
  include/ofp/yaml/ybucket.h
  include/ofp/yaml/ybytelist.h
  include/ofp/yaml/ybyteorder.h
  include/ofp/yaml/yconstants.h
  include/ofp/yaml/ydatapathid.h
  include/ofp/yaml/yecho.h
  include/ofp/yaml/yerror.h
  include/ofp/yaml/yexperimenter.h
  include/ofp/yaml/yfeaturesreply.h
  include/ofp/yaml/yflowmod.h
  include/ofp/yaml/yflowremoved.h
  include/ofp/yaml/ympflowstatsreply.h
  include/ofp/yaml/ympflowstatsrequest.h
  include/ofp/yaml/ygetasyncreply.h
  include/ofp/yaml/ygetconfigreply.h
  include/ofp/yaml/ygroupmod.h
  include/ofp/yaml/yheaderonly.h
  include/ofp/yaml/yhello.h
  include/ofp/yaml/yinstructions.h
  include/ofp/yaml/yllvm.h
  include/ofp/yaml/ymatch.h
  include/ofp/yaml/ympdesc.h
  include/ofp/yaml/ympportstats.h
  include/ofp/yaml/ympportstatsrequest.h
  include/ofp/yaml/ympqueuestats.h
  include/ofp/yaml/ympqueuestatsrequest.h
  include/ofp/yaml/ymptablestats.h
  include/ofp/yaml/ymultipartreply.h
  include/ofp/yaml/ymultipartrequest.h
  include/ofp/yaml/ypacketin.h
  include/ofp/yaml/ypacketout.h
  include/ofp/yaml/yport.h
  include/ofp/yaml/yportmod.h
  include/ofp/yaml/yportstatus.h
  include/ofp/yaml/yqueuegetconfigrequest.h
  include/ofp/yaml/yrolereply.h
  include/ofp/yaml/yrolerequest.h
  include/ofp/yaml/ysetasync.h
  include/ofp/yaml/ysetconfig.h
  include/ofp/yaml/ysmallcstring.h
  include/ofp/yaml/ytablemod.h
  include/ofp/ofp.h
)

# Determine flags from basename of CMAKE_BINARY_DIR. If directory name contains
# '+debug', set the build type to 'debug', otherwise set to release build. The
# comparison test for '+debug' is case-insensitive.

string(TOLOWER "${CMAKE_BINARY_DIR}" binary_dir)
if ("${binary_dir}" MATCHES "/[^/]*\\+debug[^/]*$")
  message(STATUS "Setting build type to DEBUG.")
  set(CMAKE_BUILD_TYPE debug)
else()
  message(STATUS "Setting build type to RELEASE.")
  set(CMAKE_BUILD_TYPE release)
endif()

# If binary_dir contains +32bit or +64bit set the -m32 or -m64 flag respectively.

if ("${binary_dir}" MATCHES "/[^/]*\\+64bit[^/]*$")
  message(STATUS "Adding -m64 build option.")
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -m64")
  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} -m64")
elseif("${binary_dir}" MATCHES "/[^/]*\\+32bit[^/]*$")
  message(STATUS "Adding -m32 build option.")
  set(LIBOFP_CXX_FLAGS "${LIBOFP_CXX_FLAGS} -m32")
  set(LIBOFP_C_FLAGS "${LIBOFP_C_FLAGS} -m32")
endif()

# Specify our final warning set. Clang supports -Weverything, but GCC does not.

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(LIBOFP_WARNINGS "-Weverything ${LIBOFP_WARNINGS_CXX11} ${LIBOFP_WARNINGS_DISABLED}")
else()
  set(LIBOFP_WARNINGS "-Wall")
endif()

#-------------------------------------------------------------------------------
# Build Library
#-------------------------------------------------------------------------------

# Prepare for testing. Subdirectories can add tests.
# N.B. The suppressions file must be defined before `include(CTest)`; otherwise
# the correct value will not appear in the generated DartConfiguration.tcl.

set(MEMORYCHECK_COMMAND_OPTIONS "-v --trace-children=yes --leak-check=full")
set(MEMORYCHECK_SUPPRESSIONS_FILE "${CMAKE_SOURCE_DIR}/test/valgrind.supp")
include(CTest)

# Boost may pull in pthreads. We need OpenSSL for TLS support.

find_package(Threads)
find_package(OpenSSL)

set(LIBOFP_LINKED_LIBS ${CMAKE_THREAD_LIBS_INIT} ssl crypto)

# Call custom function to check for Python YAML module (optional for testing).

find_package(PythonInterp REQUIRED)
find_package(PythonModule REQUIRED)
find_python_module(yaml)

# Specify auto-generated config.h file.

configure_file("${LIBOFP_CONFIG_IN}" "${LIBOFP_CONFIG}")

# Copy over the CTestCustom.cmake file into the build tree.

configure_file("${CMAKE_SOURCE_DIR}/CTestCustom.cmake" "${CMAKE_BINARY_DIR}/CTestCustom.cmake")

# Set flags to use when compiling third party code. Basically, we just use our
# project flags with `-Wall` warnings. If we apply a stricter set, we'll clutter
# up the build log with warnings.

set(CMAKE_CXX_FLAGS "${LIBOFP_CXX_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} -Wall -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "${LIBOFP_C_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} -Wall")

# Add subdirectories of third party projects.

add_subdirectory(external/yaml-io)
add_subdirectory(external/googletest)

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
target_link_libraries(ofp yamlio)

# Make sure the source is generated before the executable builds.

add_dependencies(ofp libofp_oxmfields_h libofp_oxmfields_cpp libofp_oxmfieldsdata_cpp)

#-------------------------------------------------------------------------------
# Build Examples
#-------------------------------------------------------------------------------

add_subdirectory(example/nulldemo)
add_subdirectory(example/testagent)
add_subdirectory(example/controller)
add_subdirectory(example/libofpexec)
add_subdirectory(example/python)
add_subdirectory(example/bridgeofp)
add_subdirectory(example/flowbench)

#-------------------------------------------------------------------------------
# Build Tool
#-------------------------------------------------------------------------------

add_subdirectory(tools/ofpx)

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

# Install libofp.a.

install(TARGETS ofp DESTINATION lib)

# Preserve directory structure of header files when installing.

foreach(HEADER ${LIBOFP_HEADER_FILES})
  string(REGEX MATCH "(.*)/" DIR ${HEADER})
  install(FILES ${HEADER} DESTINATION ${DIR})
endforeach(HEADER)

# Install generated headers also.

install(FILES ${LIBOFP_HEADERS_GENERATED} DESTINATION include/ofp)

#-------------------------------------------------------------------------------
# CPack Installer Support
#-------------------------------------------------------------------------------

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${libofp_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${libofp_VERSION_MINOR}")
include(CPack)

#-------------------------------------------------------------------------------
# Support for Running Cpplint
#-------------------------------------------------------------------------------

set(CPPLINT_PATH external/cpplint/cpplint.py)
set(CPPLINT_ARGS --filter=-build/include_what_you_use,-runtime/references,-whitespace,-build/namespaces --root=include)

add_custom_target(cpplint
  COMMAND ${PYTHON_EXECUTABLE} ${CPPLINT_PATH} ${CPPLINT_ARGS} ${LIBOFP_HEADER_FILES} ${LIBOFP_HEADERS_GENERATED} ${LIBOFP_SOURCES}
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
