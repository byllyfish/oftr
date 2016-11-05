#!/bin/bash

set -e

CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../libofp

echo "Run libofp version"
$LIBOFP_MEMCHECK $LIBOFP version --loglevel=info

echo "Run libofp jsonrpc"
$LIBOFP_MEMCHECK $LIBOFP jsonrpc --loglevel=info < /dev/null

echo "Run libofp encode"
$LIBOFP_MEMCHECK $LIBOFP encode --loglevel=info < /dev/null

echo "Run libofp decode"
$LIBOFP_MEMCHECK $LIBOFP decode --loglevel=info < /dev/null

echo "Run libofp help"
$LIBOFP_MEMCHECK $LIBOFP help --loglevel=info

exit 0
