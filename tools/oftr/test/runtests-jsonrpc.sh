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

exit 0
