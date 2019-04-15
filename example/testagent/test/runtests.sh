#!/bin/bash

# This script is invoked by other test scripts. You must pass in the 
# path to the `testagent` executable.

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
TESTAGENT=$1

LIBOFP_DEFAULT_PORT=${LIBOFP_DEFAULT_PORT:-6653}

echo "Working Directory:" `pwd`

for input in $CURRENT_SOURCE_DIR/*.in ; do
  name=`basename $input .in`
  output=$name.out
  output_tmp="$name-${LIBOFP_DEFAULT_PORT}.out"
  echo "Run testagent to convert $input into $output"
  # Remove lines with timestamps from output.
  $TESTAGENT "127.0.0.1:$LIBOFP_DEFAULT_PORT" < $input | grep -Ev "^time: +[1-9][0-9]*\.[0-9]+$" > $output_tmp
  echo "Compare $output and $CURRENT_SOURCE_DIR/$output"
  diff $output_tmp $CURRENT_SOURCE_DIR/$output 
done

exit 0
