#!/bin/bash

set -e

echo "Working Directory:" `pwd`

CURRENT_SOURCE_DIR=`dirname "$0"`

echo "Test decode of valid OpenFlow messages."

for input in $CURRENT_SOURCE_DIR/*-pass.in ; do
  name=`basename "$input" .in`
  output="$name.out"
  echo "  Run ofpx decode to convert $input to $output"
  ../ofpx decode -V $input > $output
  echo "  Compare $output to $CURRENT_SOURCE_DIR/$name.out"
  diff $output "$CURRENT_SOURCE_DIR/$name.out"
done

echo "Done."

exit 0
