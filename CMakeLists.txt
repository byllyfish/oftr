#=====- CMakeLists.txt ---------------------------------------------------=====#
#
# This is the top-level CMakeLists.txt file for the LibOFP project. This file 
# relies on these sub-directory files:
# 
#    - external/yaml-io/CMakeLists.txt
#    - external/boost_1_54_0_asio/CMakeLists.txt
#    - external/googletest/CMakeLists.txt
#    - test/CMakeLists.txt
#
#=====--------------------------------------------------------------------=====#

cmake_minimum_required (VERSION 2.6)
project(libofp)

#-------------------------------------------------------------------------------
# Settings
#-------------------------------------------------------------------------------

# Specify the set of warnings to enable. The philosophy of this project is to
# compile with every clang warning enabled, then disable a few that don't add
# anything except to clutter up the build logs. Put disabled warnings into 
# LIB_OFP_WARNINGS_DISABLED.

set(LIBOFP_WARNINGS_DISABLED "-Wno-unused-parameter -Wno-documentation -Wno-weak-vtables")

# LibOFP is a C++11-only project. Disable warnings about C++98 compatibility.

set(LIBOFP_WARNINGS_CXX11 "-Wno-c++98-compat -Wno-c++98-compat-pedantic")

# Specify our final warning set.

set(LIBOFP_WARNINGS "-Weverything ${LIBOFP_WARNINGS_CXX11} ${LIBOFP_WARNINGS_DISABLED}")

# Specify the include directory for the library. Don't include the directories
# for third party headers. (See next setting)

set(LIBOFP_INCLUDES "include")

# Specify include directories for third party headers. We use -isystem instead
# of -I because we don't want to receive compiler warnings from these headers.

set(LIBOFP_EXTERNAL_INCLUDES "-isystem ${CMAKE_SOURCE_DIR}/external/boost_1_54_0_asio -isystem ${CMAKE_SOURCE_DIR}/external/yaml-io/include")

# Specify the default set of compiler flags for all sources, including third
# party libraries. Place flags that don't affect the preprocessor or warnings
# here. This set of flags controls compiler output. It is important that all
# source code be compiled with the same flags.

set(LIBOFP_CXX_FLAGS "-std=c++11 -stdlib=libc++ -fstrict-aliasing")

# Add new library source files here. Add unittest source files to 
# test/CMakeLists.txt.

set(LIBOFP_SOURCES
	src/ofp/actionrange.cpp
	src/ofp/channel.cpp
	src/ofp/channellistener.cpp
	src/ofp/connectiontimer.cpp
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
	src/ofp/flowstatsreply.cpp
	src/ofp/flowstatsrequest.cpp
	src/ofp/getconfigreply.cpp
	src/ofp/header.cpp
	src/ofp/hello.cpp
	src/ofp/instructionlist.cpp
	src/ofp/ipv4address.cpp
	src/ofp/ipv6address.cpp
	src/ofp/log.cpp
	src/ofp/message.cpp
	src/ofp/multipartreply.cpp
	src/ofp/multipartrequest.cpp
	src/ofp/ofp.cpp
	src/ofp/originalmatch.cpp
	src/ofp/oxmfields.cpp
	src/ofp/oxmfieldsdata.cpp
	src/ofp/oxmlist.cpp
	src/ofp/oxmtype.cpp
	src/ofp/oxmtypeset.cpp
	src/ofp/packetin.cpp
	src/ofp/packetout.cpp
	src/ofp/port.cpp
	src/ofp/portstatus.cpp
	src/ofp/prerequisites.cpp
	src/ofp/protocolversions.cpp
	src/ofp/setconfig.cpp
	src/ofp/standardmatch.cpp
	src/ofp/sys/boost_asio.cpp
	src/ofp/sys/connection.cpp
	src/ofp/sys/engine.cpp
	src/ofp/sys/tcp_connection.cpp
	src/ofp/sys/tcp_server.cpp
	src/ofp/sys/udp_connection.cpp
	src/ofp/sys/udp_server.cpp
	src/ofp/transmogrify.cpp
	src/ofp/types.cpp
	src/ofp/version.cpp
	src/ofp/writable.cpp
	src/ofp/yaml/apichannellistener.cpp
	src/ofp/yaml/apiconnection.cpp
	src/ofp/yaml/apiencoder.cpp
	src/ofp/yaml/apievents.cpp
	src/ofp/yaml/apiserver.cpp
	src/ofp/yaml/decoder.cpp
	src/ofp/yaml/encoder.cpp
	src/ofp/yaml/yflowmod.cpp
	src/ofp/yaml/ysetconfig.cpp
	src/ofp-c/ofpprotocol.cpp
	src/ofp-c/ofpstring.cpp
)

# Specify the libofpexec main source file.

set(LIBOFPEXEC_SOURCES src/ofp/libofpexec_main.cpp)

#-------------------------------------------------------------------------------
# Build
#-------------------------------------------------------------------------------

# Prepare for testing. Subdirectories can add tests.

enable_testing(true)

# Set flags to use when compiling third party code. Basically, we just use our
# project flags with `-Wall` warnings. If we apply a stricter set, we'll clutter
# up the build log with warnings.

set(CMAKE_CXX_FLAGS "${LIBOFP_CXX_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} -Wall")

# Add subdirectories of third party projects.

add_subdirectory(external/yaml-io)
add_subdirectory(external/boost_1_54_0_asio)
add_subdirectory(external/googletest)

# Modify flags for compiling our code. Here, we change the warning flags to
# our own super picky set (-Weverything minus a few).

set(CMAKE_CXX_FLAGS "${LIBOFP_CXX_FLAGS} ${LIBOFP_EXTERNAL_INCLUDES} ${LIBOFP_WARNINGS}")

# Set up the source include directory. N.B. Includes for third-party sources are
# baked into CMAKE_CXX_FLAGS using -isystem.

include_directories(${LIBOFP_INCLUDES})

# Build libofp.a and libofpexec.

add_library(ofp ${LIBOFP_SOURCES})
add_executable(libofpexec ${LIBOFPEXEC_SOURCES})
target_link_libraries(libofpexec ofp yamlio boostasio)

#-------------------------------------------------------------------------------
# Test
#-------------------------------------------------------------------------------

add_subdirectory(test)



