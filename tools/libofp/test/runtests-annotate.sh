#!/bin/bash

set -e

echo "Working Directory:" `pwd`

CURRENT_SOURCE_DIR=`dirname "$0"`

echo "Test annotate.py."

echo "  Run libofp encode to verify format of $CURRENT_SOURCE_DIR/annotate-input.yml"
$LIBOFP_MEMCHECK ../libofp encode -s $CURRENT_SOURCE_DIR/annotate-input.yml

echo "  Run annotate.py to produce annotate.tab"
python $CURRENT_SOURCE_DIR/annotate.py $CURRENT_SOURCE_DIR/annotate-input.yml > annotate.tab
echo "  Compare annotate.tab to $CURRENT_SOURCE_DIR/annotate.tab"
diff annotate.tab $CURRENT_SOURCE_DIR/annotate.tab

echo "Done."

exit 0
