#!/bin/bash

set -e

CURRENT_TEST_DIR=`pwd`
LIBOFP=$CURRENT_TEST_DIR/../oftr

echo "Run oftr version"
$LIBOFP_MEMCHECK $LIBOFP version --loglevel=info

echo "Run oftr jsonrpc"
$LIBOFP_MEMCHECK $LIBOFP jsonrpc --loglevel=info << EOF
{"method":"OFP.DESCRIPTION","id":1}
{}
EOF

echo "Run oftr encode"
$LIBOFP_MEMCHECK $LIBOFP encode --loglevel=info < /dev/null

echo "Run oftr decode"
$LIBOFP_MEMCHECK $LIBOFP decode --loglevel=info < /dev/null

echo "Run oftr jsonrpc and test QUIT signal backtrace"
$LIBOFP_MEMCHECK $LIBOFP jsonrpc --initial-sleep=5 &> backtrace.txt &
PID=$!
sleep 1
kill -QUIT $PID
grep "4ofpx10Subprogram23parseCommandLineOptions" backtrace.txt

exit 0
