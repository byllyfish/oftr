#!/bin/bash

set -e


# Log current travis settings...
echo "TRAVIS_EVENT_TYPE: ${TRAVIS_EVENT_TYPE}"
echo "TRAVIS_BRANCH:     ${TRAVIS_BRANCH}"
echo "TRAVIS_TAG:        ${TRAVIS_TAG}"
echo "TRAVIS_COMMIT:     ${TRAVIS_COMMIT}"
echo "TRAVIS_OS_NAME:    ${TRAVIS_OS_NAME}"       # "linux" or "osx"

# Build release version.

mkdir -p Build
cd Build
cmake ..
MAKEFLAGS=-j4 ctest -j4 --output-on-failure -D Experimental
cd ..

# Test code coverage with debug build (linux only).

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    mkdir -p Build+Debug
    cd Build+Debug
    cmake -DLIBOFP_ENABLE_CODE_COVERAGE=true -DLIBOFP_ENABLE_MEMCHECK_TESTS=false ..
    MAKEFLAGS=-j4 ctest -j4 -D Experimental
    echo "Submit to codecov..."
    bash <(curl -s https://codecov.io/bash)
    echo "Done submitting to codecov."
fi
