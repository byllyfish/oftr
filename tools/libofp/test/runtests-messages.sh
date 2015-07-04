#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`

echo "Working Directory: $CURRENT_TEST_DIR"

echo "Test openflow-messages '*.pass'"

cd $CURRENT_SOURCE_DIR/../../..

LIBOFP=$CURRENT_TEST_DIR/../libofp

# Decode all the *.pass messages in the openflow-messages repository. Collect
# resulting output and any error messages in the file openflow-messages.pass.out.

find external/openflow-messages -name '*.pass' -print | sort | xargs $LIBOFP decode -kV > "$CURRENT_TEST_DIR/openflow-messages.pass.out" 2>&1

diff "$CURRENT_TEST_DIR/openflow-messages.pass.out" "$CURRENT_SOURCE_DIR/openflow-messages.pass.out"

echo "Test openflow-messages '*.fail'"

# Decode all the *.fail messages in the openflow-messages repository. Collect
# resulting error messages (and output) in the file openflow-messages.fail.out.

find external/openflow-messages -name '*.fail' -print | sort | xargs $LIBOFP decode -kV > "$CURRENT_TEST_DIR/openflow-messages.fail.out" 2>&1

diff "$CURRENT_TEST_DIR/openflow-messages.fail.out" "$CURRENT_SOURCE_DIR/openflow-messages.fail.out"

echo "Done."
exit 0
