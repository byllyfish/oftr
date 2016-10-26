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
  echo "Run testagent to convert $input into $output"
  #cat $input | $TESTAGENT "127.0.0.1:$LIBOFP_DEFAULT_PORT" | tee $output
  $TESTAGENT "127.0.0.1:$LIBOFP_DEFAULT_PORT" < $input > $output
  echo "Compare $output and $CURRENT_SOURCE_DIR/$output"
  diff $output $CURRENT_SOURCE_DIR/$output 
done

exit 0
