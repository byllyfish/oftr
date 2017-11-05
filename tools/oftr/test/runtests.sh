#!/bin/bash

set -e

echo "Working Directory:" `pwd`

CURRENT_SOURCE_DIR=`dirname "$0"`

echo "Test decode of valid OpenFlow messages."

for input in $CURRENT_SOURCE_DIR/*-pass.bin ; do
  name=`basename "$input" .bin`
  output_yml="$name.yml"
  output_json="$name.json"
  output_array="$name.jsonarray"

  echo "  Run oftr decode to convert $input to $output_yml"
  $LIBOFP_MEMCHECK ../oftr decode -V --pkt-decode --fuzz-stress-test $input > $output_yml
  echo "  Compare $output_yml to $CURRENT_SOURCE_DIR/$name.yml"
  diff $output_yml "$CURRENT_SOURCE_DIR/$name.yml"

  echo "  Run oftr decode to convert $input to $output_json"
  $LIBOFP_MEMCHECK ../oftr decode -V --json $input > $output_json
  echo "  Compare $output_json to $CURRENT_SOURCE_DIR/$name.json"
  diff $output_json "$CURRENT_SOURCE_DIR/$name.json"

  if [ -f "$CURRENT_SOURCE_DIR/$name.jsonarray" ]; then
    # Not all test cases include a .jsonarray version.
    echo "  Run oftr decode to convert $input to $output_array"
    $LIBOFP_MEMCHECK ../oftr decode -V --json-array $input > $output_array
    echo "  Compare $output_array to $CURRENT_SOURCE_DIR/$name.jsonarray"
    diff $output_array "$CURRENT_SOURCE_DIR/$name.jsonarray"
  fi
done

echo "Test encode of YAML OpenFlow messages."

for input in $CURRENT_SOURCE_DIR/*-pass.yml ; do
  name=`basename "$input" .yml`
  output="$name.bin"

  echo "  Run oftr encode to convert $input to $output"
  $LIBOFP_MEMCHECK ../oftr encode -M $input > $output
  echo "  Compare $output to $CURRENT_SOURCE_DIR/$name.bin"
  diff $output "$CURRENT_SOURCE_DIR/$name.bin"
done

echo "Test encode of JSON OpenFlow messages."

for input in $CURRENT_SOURCE_DIR/*-pass.json ; do
  name=`basename "$input" .json`
  output="$name.bin"

  echo "  Run oftr encode to convert $input to $output"
  $LIBOFP_MEMCHECK ../oftr encode -Mj $input > $output
  echo "  Compare $output to $CURRENT_SOURCE_DIR/$name.bin"
  diff $output "$CURRENT_SOURCE_DIR/$name.bin"
done

echo "Test encode of JSON Array OpenFlow messages."

for input in $CURRENT_SOURCE_DIR/*-pass.jsonarray ; do
  name=`basename "$input" .jsonarray`
  output="$name.bin"

  echo "  Run oftr encode to convert $input to $output"
  $LIBOFP_MEMCHECK ../oftr encode -M --json-array $input > $output
  echo "  Compare $output to $CURRENT_SOURCE_DIR/$name.bin"
  diff $output "$CURRENT_SOURCE_DIR/$name.bin"
done

echo "Test encode of alternate JSON OpenFlow messages."

for input in $CURRENT_SOURCE_DIR/*-pass.json-alt ; do
  name=`basename "$input" .json-alt`
  output="$name.bin"

  echo "  Run oftr encode to convert $input to $output"
  $LIBOFP_MEMCHECK ../oftr encode -Mj $input > $output
  echo "  Compare $output to $CURRENT_SOURCE_DIR/$name.bin"
  diff $output "$CURRENT_SOURCE_DIR/$name.bin"
done

echo "Done."

exit 0
