#!/bin/bash

# Test script for nullcontroller and nullagent.

SOURCE_DIR=`dirname "$0"`
BINARY_DIR="$1"

$BINARY_DIR/nullcontroller &> $BINARY_DIR/nullc.out &
cpid=$!

$BINARY_DIR/nullagent 127.0.0.1 &> $BINARY_DIR/nulla.out &
apid=$!

sleep 2
kill $cpid
sleep 2
kill $apid

exit 0
