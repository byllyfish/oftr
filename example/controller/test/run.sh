#!/bin/bash

# Test script for controller (relies on testagent). You must pass in the path
# to the testagent executable.

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
TESTAGENT="$1"

echo "Working Directory:" `pwd`

echo "Start controller."
../controller &> /dev/null &
cpid=$!
sleep 1

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
wait $cpid
echo "Stopped."

exit $status
