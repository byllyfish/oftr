#!/bin/bash

set -e

echo "Working Directory:" `pwd`

echo "Starting testagent."
../testagent 127.0.0.1:6633 &
apid=$!

echo "Sleep 1 second."
sleep 1

echo "Stop testagent."
kill $apid
wait $apid || true
echo "Stopped."

exit 0
