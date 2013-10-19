#!/bin/bash

# Test script for python controller (relies on testagent). You must pass in the 
# path to the testagent executable and the libofpexec executable.

#set -m  # Enable job control.
set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
TESTAGENT="$1"
LIBOFPEXEC="$2"

echo "Working Directory:" `pwd`

echo "Start controller."
LIBOFPEXEC_PATH="$LIBOFPEXEC" python $CURRENT_SOURCE_DIR/../controller.py &
cpid=$!

echo "Run agent tests."
if bash $CURRENT_SOURCE_DIR/../../testagent/test/runtests.sh $TESTAGENT ; then
  echo "Agent tests succeeded."
  status=0
else
  echo "Agent tests failed."
  status=1
fi

echo "Stop controller."
kill $cpid
echo "Stopped."

exit $status
