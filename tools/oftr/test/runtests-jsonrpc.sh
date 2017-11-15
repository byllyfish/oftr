#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../oftr

# Test Binary protocol.

echo -ne '\x00\x00\x05\xA0x' | $LIBOFP jsonrpc > jsonrpc-test.out
echo -ne '\x00\x00\x29\xA0{"id":1, "method": "OFP.DESCRIPTION"}' | $LIBOFP jsonrpc >> jsonrpc-test.out
echo -ne '\x00\x00\x28\xA0{"id":1, "method": "OFP.DESCRIPTION"}' | $LIBOFP jsonrpc >> jsonrpc-test.out
echo -ne '\x00\x00\x27\xA0{"id":1, "method": "OFP.DESCRIPTION"}' | $LIBOFP jsonrpc >> jsonrpc-test.out

echo "Compare jsonrpc-test.out to ${CURRENT_SOURCE_DIR}/jsonrpc-test.out"
diff jsonrpc-test.out "${CURRENT_SOURCE_DIR}/jsonrpc-test.out"

exit 0
