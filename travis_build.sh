#!/bin/bash

set -e

# Log current travis settings...
echo "TRAVIS_EVENT_TYPE: ${TRAVIS_EVENT_TYPE}"
echo "TRAVIS_BRANCH:     ${TRAVIS_BRANCH}"
echo "TRAVIS_TAG:        ${TRAVIS_TAG}"
echo "TRAVIS_COMMIT:     ${TRAVIS_COMMIT}"
echo "TRAVIS_OS_NAME:    ${TRAVIS_OS_NAME}"       # "linux" or "osx"
echo "PYENV_ROOT:        ${PYENV_ROOT}"

# Build release version.

mkdir -p Build
cd Build
cmake ..
MAKEFLAGS=-j4 ctest -j4 --output-on-failure -D Experimental
cd ..

# Test code coverage with debug build (linux only). Include zof integration tests
# as part of code coverage.

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    mkdir -p Build+Debug
    cd Build+Debug
    cmake -DLIBOFP_ENABLE_CODE_COVERAGE=true -DLIBOFP_ENABLE_MEMCHECK_TESTS=false ..
    MAKEFLAGS=-j4 ctest -j4 -D Experimental

	# Run zof integration tests too.
	export ZOF_OFTR_PATH="$(pwd)/tools/oftr/oftr"
	$ZOF_OFTR_PATH version
	git clone --depth=1 "https://github.com/byllyfish/zof.git"
	cd zof
	eval "$(pyenv init -)"
	pyenv install 3.5.2
	pyenv shell 3.5.2
	python -m venv venv3
	source venv3/bin/activate
	pip install -r requirements.txt
	PYTHONPATH="$(pwd)" ./test/integration_tests.sh || echo "ZOF Integration tests failed."
	deactivate
	cd ..

    echo "Submit to codecov..."
    bash <(curl -s https://codecov.io/bash)
    echo "Done submitting to codecov."
fi
