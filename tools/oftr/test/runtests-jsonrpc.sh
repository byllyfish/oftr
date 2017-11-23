#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../oftr

# Test text-based RPC protocol. 
# 
# Use sed to strip the version/git revision information from the OFP.DESCRIPTION
# response.

echo "Test text-based RPC protocol"
$LIBOFP jsonrpc < ${CURRENT_SOURCE_DIR}/jsonrpc-text.bin | sed 's,"[0-9.]* ([0-9a-f]*)","",' > jsonrpc-text.out

echo "Compare jsonrpc-text.out to ${CURRENT_SOURCE_DIR}/jsonrpc-text.out"
diff jsonrpc-text.out "${CURRENT_SOURCE_DIR}/jsonrpc-text.out"

exit 0
