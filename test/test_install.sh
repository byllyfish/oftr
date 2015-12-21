#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`

# Make sure that we only install the files we expect.

make -C .. install/fast DESTDIR=test_install
diff -b ../install_manifest.txt "${CURRENT_SOURCE_DIR}/install_manifest.txt"
