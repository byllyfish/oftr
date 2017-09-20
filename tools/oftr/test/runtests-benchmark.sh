#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`

CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../oftr

echo "Benchmark decoding packet_in msgs to YAML"

time $LIBOFP decode --pkt-decode -s $CURRENT_SOURCE_DIR/bench_packetin.bin

echo "Benchmark decoding packet_in msgs to JSON"

time $LIBOFP decode --pkt-decode -s -j $CURRENT_SOURCE_DIR/bench_packetin.bin

exit 0
