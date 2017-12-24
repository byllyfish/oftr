#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../oftr

# Test text-based RPC protocol. 

echo "Test text-based RPC protocol"
jsonrpc_t_out="jsonrpc-t$$.out"
$LIBOFP_MEMCHECK $LIBOFP jsonrpc < ${CURRENT_SOURCE_DIR}/jsonrpc-t.bin > $jsonrpc_t_out

echo "Compare $jsonrpc_t_out to ${CURRENT_SOURCE_DIR}/jsonrpc-t.out"
diff $jsonrpc_t_out "${CURRENT_SOURCE_DIR}/jsonrpc-t.out"
rm $jsonrpc_t_out

# Test the binary RPC protocol.

echo "Test binary RPC protocol"
jsonrpc_b_out="jsonrpc-b$$.out"
$LIBOFP_MEMCHECK $LIBOFP jsonrpc --binary-protocol < ${CURRENT_SOURCE_DIR}/jsonrpc-b.bin > $jsonrpc_b_out

echo "Compare $jsonrpc_b_out to ${CURRENT_SOURCE_DIR}/jsonrpc-b.out"
diff $jsonrpc_b_out "${CURRENT_SOURCE_DIR}/jsonrpc-b.out"
rm $jsonrpc_b_out

# Test the binary protocol using malformed (text) input.

echo "Test binary RPC protocol using malformed (text) input"
jsonrpc_b_malformed_out="jsonrpc-b-malformed$$.out"
$LIBOFP_MEMCHECK $LIBOFP jsonrpc --binary-protocol < ${CURRENT_SOURCE_DIR}/jsonrpc-t.bin > $jsonrpc_b_malformed_out

echo "Compare $jsonrpc_b_malformed_out to ${CURRENT_SOURCE_DIR}/jsonrpc-b-malformed.out"
diff $jsonrpc_b_malformed_out "${CURRENT_SOURCE_DIR}/jsonrpc-b-malformed.out"
rm $jsonrpc_b_malformed_out

exit 0
