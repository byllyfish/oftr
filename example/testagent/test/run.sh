#!/bin/bash

set -e

LIBOFP_DEFAULT_PORT=${LIBOFP_DEFAULT_PORT:-6653}

echo "Working Directory:" `pwd`

echo "Starting testagent."
../testagent "127.0.0.1:$LIBOFP_DEFAULT_PORT" &
apid=$!

echo "Sleep 1 second."
sleep 1

echo "Stop testagent."
kill $apid
wait $apid || true
echo "Stopped."

exit 0
