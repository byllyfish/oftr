#!/bin/bash

set -e

CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../libofp

echo "Run simple command line checks"

$LIBOFP_MEMCHECK $LIBOFP version --loglevel=info
$LIBOFP_MEMCHECK $LIBOFP jsonrpc --loglevel=info < /dev/null
$LIBOFP_MEMCHECK $LIBOFP encode --loglevel=info < /dev/null
$LIBOFP_MEMCHECK $LIBOFP decode --loglevel=info < /dev/null
$LIBOFP_MEMCHECK $LIBOFP help --loglevel=info

exit 0
