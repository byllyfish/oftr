#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../oftr

# Test text-based RPC protocol. 

echo "Test text-based RPC protocol"
$LIBOFP jsonrpc < ${CURRENT_SOURCE_DIR}/jsonrpc-t.bin > jsonrpc-t.out

echo "Compare jsonrpc-t.out to ${CURRENT_SOURCE_DIR}/jsonrpc-t.out"
diff jsonrpc-t.out "${CURRENT_SOURCE_DIR}/jsonrpc-t.out"

# Test the binary RPC protocol.

echo "Test binary RPC protocol"
$LIBOFP jsonrpc --binary-protocol < ${CURRENT_SOURCE_DIR}/jsonrpc-b.bin > jsonrpc-b.out

echo "Compare jsonrpc-b.out to ${CURRENT_SOURCE_DIR}/jsonrpc-b.out"
diff jsonrpc-b.out "${CURRENT_SOURCE_DIR}/jsonrpc-b.out"

exit 0
