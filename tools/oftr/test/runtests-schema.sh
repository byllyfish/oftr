#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`

echo "Working Directory: $CURRENT_TEST_DIR"

LIBOFP=$CURRENT_TEST_DIR/../oftr

echo "Generate schema-all"
schema_all="schema-all$$.yml"
$LIBOFP_MEMCHECK $LIBOFP help -schema-all > $schema_all

echo "Compare $schema_all to $CURRENT_SOURCE_DIR/schema-all.yml"
diff "$schema_all" "$CURRENT_SOURCE_DIR/schema-all.yml"
rm $schema_all

echo "Generate schema-lexicon"
schema_lexicon="schema-lexicon$$.txt"
$LIBOFP_MEMCHECK $LIBOFP help -schema-lexicon > $schema_lexicon

echo "Compare $schema_lexicon to $CURRENT_SOURCE_DIR/schema-lexicon.txt"
diff "$schema_lexicon" "$CURRENT_SOURCE_DIR/schema-lexicon.txt"
rm $schema_lexicon

echo "Generate schema-fields"
schema_fields="schema-fields$$.txt"
$LIBOFP_MEMCHECK $LIBOFP help --field-table > $schema_fields

echo "Compare $schema_fields to $CURRENT_SOURCE_DIR/schema-fields.txt"
diff "$schema_fields" "$CURRENT_SOURCE_DIR/schema-fields.txt"
rm $schema_fields

echo "Done."
exit 0
