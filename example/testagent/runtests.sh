#!/bin/bash

# Test script for testagent and simple controller.

SOURCE_DIR=`dirname "$0"`
BINARY_DIR="$1"

$BINARY_DIR/../controller/simple &> /dev/null &
cpid=$!

$BINARY_DIR/testagent < $SOURCE_DIR/testfiles/packet_in.yaml &> $BINARY_DIR/packet_in.yaml.out &
apid=$!

sleep 3
kill $cpid
kill $apid

diff $BINARY_DIR/packet_in.yaml.out $SOURCE_DIR/testfiles/packet_in.yaml.out

exit 0
