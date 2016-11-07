#!/bin/bash
#
# Script to run clang-tidy on a source file in libofp.
# 
# WARNING: This script assumes it is running on my Mac.
# 
# Examples:
# 
#     clangtidy.sh src/ofp/actiontype.cpp
#     
#     clangtidy.sh -fix src/ofp/actiontype.cpp

set -e

CLANGTIDY="${HOME}/code/llvm-build/bin/clang-tidy"

#CHECKS="-checks=google-readability-braces-around-statements"
CHECKS="-checks=*,-cppcoreguidelines-*,-llvm-include-order,-google-build-using-namespace,-readability-implicit-bool-cast,-google-runtime-references,-google-readability-braces-around-statements,-clang-analyzer-alpha*"

# Tell clang-tidy where the C++ system headers are located.
SYSTEMCPP="-isystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1"

HEADERS="-I${HOME}/code/ofp/include -I${HOME}/code/ofp/Build+Debug/include -isystem ${HOME}/code/ofp/external/yaml-io/include -isystem ${HOME}/code/ofp/Build+Debug/external/yaml-io/include -isystem ${HOME}/code/ofp/external/asio/asio/include -isystem ${HOME}/code/ofp/external/boringssl/include"

DEFINES="-D ASIO_STANDALONE -D ASIO_NO_TYPEID -D ASIO_NO_EXCEPTIONS -D ASIO_NO_DEPRECATED -D LIBOFP_TARGET_DARWIN"

$CLANGTIDY $CHECKS "$@" -- -std=c++11 $SYSTEMCPP $HEADERS $DEFINES
