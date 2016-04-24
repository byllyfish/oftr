#!/bin/bash

set -e

function verify_sha1 {
    if [ ! -f "$1" ]; then
        echo "verify_sha1 failed: file not found: $1"
        exit 1
    fi
    sha1=`openssl sha1 "$1" | cut -d ' ' -f 2`
    if [ "$2" != "$sha1" ]; then
        echo "verify_sha1 failed: $1: expected $2 but got $sha1"
        exit 1
    fi
    echo "verify_sha1 success: $1"
}


CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`
MSG_DIR="/tmp/libofp.msgs"

echo "Working Directory: $CURRENT_TEST_DIR"

if [ -d "$MSG_DIR" ]; then
    /bin/rm -f $MSG_DIR/_tcp-*
fi

LIBOFP=$CURRENT_TEST_DIR/../libofp

echo "Run libofp decode on $CURRENT_SOURCE_DIR/tcp.pcap"
$LIBOFP_MEMCHECK $LIBOFP decode --pcap-format --pcap-debug "$CURRENT_SOURCE_DIR/tcp.pcap"

verify_sha1 "$MSG_DIR/_tcp-1-127.0.0.1:61278-127.0.0.1:8888" "bc5b02ab2e2250f4a3682e09104b0cca526733fe"
verify_sha1 "$MSG_DIR/_tcp-1-127.0.0.1:8888-127.0.0.1:61278" "b33f72339f194199cd084c623a61f3f21f74e108"
verify_sha1 "$MSG_DIR/_tcp-2-127.0.0.1:61279-127.0.0.1:8888" "68545747cea884c1a063319f50bd7de4ffcc54d8"
verify_sha1 "$MSG_DIR/_tcp-2-127.0.0.1:8888-127.0.0.1:61279" "f0ee790074f334b7e6205a9d88c8f4befb726b34"
verify_sha1 "$MSG_DIR/_tcp-3-127.0.0.1:61282-127.0.0.1:8888" "b4068e9627420fbd0e60e6483c51e7971ef86897"
verify_sha1 "$MSG_DIR/_tcp-3-127.0.0.1:8888-127.0.0.1:61282" "84e93061448eadd802697f8b2a94f9b534740249"

echo "Done."
exit 0
