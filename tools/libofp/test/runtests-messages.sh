#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`

echo "Working Directory: $CURRENT_TEST_DIR"

cd $CURRENT_SOURCE_DIR/../../..

LIBOFP=$CURRENT_TEST_DIR/../libofp

# Make sure `sort` does the same thing on all systems.
export LC_ALL=C

# Decode all the *.pass messages in the openflow-messages repository. Collect
# resulting output and any error messages in the file openflow-messages.pass.out.

echo "Test openflow-messages '*.pass'"
find external/openflow-messages -name '*.pass' -print | sort | xargs $LIBOFP_MEMCHECK $LIBOFP decode -kV > "$CURRENT_TEST_DIR/openflow-messages.pass.out" 2>&1

echo "Compare openflow-messages.pass.out to $CURRENT_SOURCE_DIR/openflow-messages.pass.out"
diff "$CURRENT_TEST_DIR/openflow-messages.pass.out" "$CURRENT_SOURCE_DIR/openflow-messages.pass.out"

# Decode all the *.fail messages in the openflow-messages repository. Collect
# resulting error messages (and output) in the file openflow-messages.fail.out.

echo "Test openflow-messages '*.fail'"
find external/openflow-messages -name '*.fail' -print | sort | xargs $LIBOFP_MEMCHECK $LIBOFP decode -kV --include-filename > "$CURRENT_TEST_DIR/openflow-messages.fail.out" 2>&1

echo "Compare openflow-messages.fail.out to $CURRENT_SOURCE_DIR/openflow-messages.fail.out"
diff "$CURRENT_TEST_DIR/openflow-messages.fail.out" "$CURRENT_SOURCE_DIR/openflow-messages.fail.out"

# Decode all messages in ovs-messages.pass. Collect resulting error messages 
# (and output) in the file ovs-messages.pass.out.

echo "Test ovs-messages.pass"
$LIBOFP_MEMCHECK $LIBOFP decode -kV "$CURRENT_SOURCE_DIR/ovs-messages.pass" > "$CURRENT_TEST_DIR/ovs-messages.pass.out" 2>&1

echo "Compare ovs-messages.pass.out to $CURRENT_SOURCE_DIR/ovs-messages.pass.out"
diff "$CURRENT_TEST_DIR/ovs-messages.pass.out" "$CURRENT_SOURCE_DIR/ovs-messages.pass.out"

echo "Done."
exit 0
