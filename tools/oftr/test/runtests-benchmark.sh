#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`

CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../oftr

# Benchmark decoding binary OpenFlow messages, with specific emphasis on 
# PacketIn messages.

for i in 1 2; do
  echo "Benchmark decoding packet_in msgs to YAML ($i)"
  time $LIBOFP decode --pkt-decode -s $CURRENT_SOURCE_DIR/bench_packetin.bin
done

for i in 1 2; do
  echo "Benchmark decoding packet_in msgs to JSON ($i)"
  time $LIBOFP decode --pkt-decode -s -j $CURRENT_SOURCE_DIR/bench_packetin.bin
done

# Benchmark encoding JSON to OpenFlow, with specific emphasis on FlowMod, 
# PortMod, GroupMod and BarrierRequest messages.

echo "Decode flowmod msgs to JSON..."
# First, construct the benchmark file: bench_flowmod.json
gunzip -c $CURRENT_SOURCE_DIR/bench_flowmod.bin.gz | $LIBOFP decode -j > bench_flowmod.json

echo "Benchmark encoding flowmod msgs from JSON to binary"
time $LIBOFP encode -js bench_flowmod.json


exit 0
