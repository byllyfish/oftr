#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`

# Make sure that we only install the files we expect.

cd ..
make install DESTDIR=test_install
diff install_manifest.txt "${CURRENT_SOURCE_DIR}/install_manifest.txt"
