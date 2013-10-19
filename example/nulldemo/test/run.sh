#!/bin/bash

# Test script for nullcontroller and nullagent.

set -e

echo "Working Directory:" `pwd`

echo "Start nullcontroller."
../nullcontroller &> nullc.out &
cpid=$!

echo "Start nullagent."
../nullagent 127.0.0.1 &> nulla.out &
apid=$!

echo "Sleep 1 second."
sleep 1

echo "Stop nullcontroller."
kill $cpid
wait $cpid
echo "nullcontroller stopped."

echo "Sleep 1 second."
sleep 1

echo "Stop nullagent."
kill $apid
wait $apid
echo "nullagent stopped."

exit 0
