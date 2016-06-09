#!/bin/bash

set -e

echo "Working Directory:" `pwd`

CURRENT_SOURCE_DIR=`dirname "$0"`

echo "Test decode of valid OpenFlow messages."

for input in $CURRENT_SOURCE_DIR/*-pass.bin ; do
  name=`basename "$input" .bin`
  output_yml="$name.yml"
  output_json="$name.json"

  echo "  Run libofp decode to convert $input to $output_yml"
  $LIBOFP_MEMCHECK ../libofp decode -V --pkt-decode $input > $output_yml
  echo "  Compare $output_yml to $CURRENT_SOURCE_DIR/$name.yml"
  diff $output_yml "$CURRENT_SOURCE_DIR/$name.yml"

  echo "  Run libofp decode to convert $input to $output_json"
  $LIBOFP_MEMCHECK ../libofp decode -Vj $input > $output_json
  echo "  Compare $output_json to $CURRENT_SOURCE_DIR/$name.json"
  diff $output_json "$CURRENT_SOURCE_DIR/$name.json"
done

echo "Test encode of YAML OpenFlow messages."

for input in $CURRENT_SOURCE_DIR/*-pass.yml ; do
  name=`basename "$input" .yml`
  output="$name.bin"

  echo "  Run libofp encode to convert $input to $output"
  $LIBOFP_MEMCHECK ../libofp encode -M $input > $output
  echo "  Compare $output to $CURRENT_SOURCE_DIR/$name.bin"
  diff $output "$CURRENT_SOURCE_DIR/$name.bin"
done

echo "Test encode of JSON OpenFlow messages."

for input in $CURRENT_SOURCE_DIR/*-pass.json ; do
  name=`basename "$input" .json`
  output="$name.bin"

  echo "  Run libofp encode to convert $input to $output"
  $LIBOFP_MEMCHECK ../libofp encode -Mj $input > $output
  echo "  Compare $output to $CURRENT_SOURCE_DIR/$name.bin"
  diff $output "$CURRENT_SOURCE_DIR/$name.bin"
done

echo "Done."

exit 0
