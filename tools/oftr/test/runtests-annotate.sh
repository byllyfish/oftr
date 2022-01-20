#!/bin/bash

set -e

echo "Working Directory:" `pwd`

CURRENT_SOURCE_DIR=`dirname "$0"`

echo "Test annotate.py."
output_tab="annotate$$.tab"

echo "  Run oftr encode to verify format of $CURRENT_SOURCE_DIR/annotate-input.yml"
$LIBOFP_MEMCHECK ../oftr encode -s $CURRENT_SOURCE_DIR/annotate-input.yml

echo "  Run annotate.py to produce $output_tab"
python3 $CURRENT_SOURCE_DIR/annotate.py $CURRENT_SOURCE_DIR/annotate-input.yml > $output_tab
echo "  Compare $output_tab to $CURRENT_SOURCE_DIR/annotate.tab"
diff $output_tab $CURRENT_SOURCE_DIR/annotate.tab
rm $output_tab

echo "Done."

exit 0
