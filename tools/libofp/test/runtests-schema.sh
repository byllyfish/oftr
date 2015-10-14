#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`

echo "Working Directory: $CURRENT_TEST_DIR"

LIBOFP=$CURRENT_TEST_DIR/../libofp

$LIBOFP help -schema-all

echo "Done."
exit 0
