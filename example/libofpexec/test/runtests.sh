#!/bin/bash

set -e

echo "Working Directory:" `pwd`

CURRENT_SOURCE_DIR=`dirname "$0"`

echo "Test conversion of YAML to binary OpenFlow packets."

for input in $CURRENT_SOURCE_DIR/*.in ; do
  name=`basename "$input" .in`
  output="$name.out"
  echo "  Run libofpexec to convert $input to $output"
  ../libofpexec < $input > $output
  echo "  Compare $output to $CURRENT_SOURCE_DIR/$name.out"
  diff $output "$CURRENT_SOURCE_DIR/$name.out"
done

echo "Test conversion of binary OpenFlow packets to YAML and back."

for input in $CURRENT_SOURCE_DIR/*.out ; do
  name=`basename "$input" .out`
  output="$name.yaml"
  echo "  Run libofpexec to convert $input to $output"
  ../libofpexec < $input > $output
  echo "  Compare $output to $CURRENT_SOURCE_DIR/$name.yaml"
  diff $output "$CURRENT_SOURCE_DIR/$name.yaml"
  echo "  Run libofpexec to convert $output to $output.tmp"
  ../libofpexec < $output > $output.tmp
  echo "  Compare $output.tmp to $input"
  diff $output.tmp $input
  rm -f "$output.tmp"
done

echo "Done."

exit 0