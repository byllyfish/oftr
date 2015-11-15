#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`

echo "Working Directory: $CURRENT_TEST_DIR"

LIBOFP=$CURRENT_TEST_DIR/../libofp

echo "Generate schema-all"
$LIBOFP help -schema-all > schema-all.yml

echo "Compare schema-all.yml to $CURRENT_SOURCE_DIR/schema-all.yml"
diff "$CURRENT_TEST_DIR/schema-all.yml" "$CURRENT_SOURCE_DIR/schema-all.yml"

echo "Done."
exit 0
