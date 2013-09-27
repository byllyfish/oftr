#!/bin/bash

# To produce test files, `sh runtest.sh ../Build+Debug/libofpexec testfiles`

SOURCE_TEST_DIR=`dirname "$0"`
LIBOFPEXEC="$1"
BINARY_TEST_DIR="$2"

# Test conversion of YAML to binary OpenFlow packets.
for input in $SOURCE_TEST_DIR/testfiles/*.txt ; do
  name=`basename "$input"`
  output="$BINARY_TEST_DIR/$name.out"
  $LIBOFPEXEC < $input > $output
  diff $output "$SOURCE_TEST_DIR/testfiles/$name.out"
done

# Test convertion of binary OpenFlow packets to YAML and back.
for input in $SOURCE_TEST_DIR/testfiles/*.out ; do
  name=`basename "$input"`
  output="$BINARY_TEST_DIR/$name.yaml"
  $LIBOFPEXEC < $input > $output
  diff $output "$SOURCE_TEST_DIR/testfiles/$name.yaml"
  $LIBOFPEXEC < $output > $output.tmp
  diff $output.tmp $input
  rm -f "$output.tmp"
done

