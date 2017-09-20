#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`

CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../oftr

for i in 1 2; do
  echo "Benchmark decoding packet_in msgs to YAML ($i)"
  time $LIBOFP decode --pkt-decode -s $CURRENT_SOURCE_DIR/bench_packetin.bin
done

for i in 1 2; do
  echo "Benchmark decoding packet_in msgs to JSON ($i)"
  time $LIBOFP decode --pkt-decode -s -j $CURRENT_SOURCE_DIR/bench_packetin.bin
done

exit 0
