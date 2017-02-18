#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`

echo "Working Directory: $CURRENT_TEST_DIR"

LIBOFP=$CURRENT_TEST_DIR/../oftr

echo "Generate schema-all"
$LIBOFP_MEMCHECK $LIBOFP help -schema-all > schema-all.yml

echo "Compare schema-all.yml to $CURRENT_SOURCE_DIR/schema-all.yml"
diff "$CURRENT_TEST_DIR/schema-all.yml" "$CURRENT_SOURCE_DIR/schema-all.yml"

echo "Generate schema-lexicon"
$LIBOFP_MEMCHECK $LIBOFP help -schema-lexicon > schema-lexicon.txt

echo "Compare schema-lexicon.txt to $CURRENT_SOURCE_DIR/schema-lexicon.txt"
diff "$CURRENT_TEST_DIR/schema-lexicon.txt" "$CURRENT_SOURCE_DIR/schema-lexicon.txt"

echo "Done."
exit 0
