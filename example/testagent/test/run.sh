#!/bin/bash

set -e

echo "Working Directory:" `pwd`

echo "Starting testagent."
../testagent &
apid=$!

echo "Sleep 1 second."
sleep 1

echo "Stop testagent."
kill $apid
wait $apid
echo "Stopped."

exit 0
