#!/bin/bash

set -e

CURRENT_SOURCE_DIR=`dirname "$0"`
CURRENT_TEST_DIR=`pwd`

echo "Working Directory: $CURRENT_TEST_DIR"

cd $CURRENT_SOURCE_DIR/../../..

LIBOFP=$CURRENT_TEST_DIR/../oftr

# Make sure `sort` does the same thing on all systems.
export LC_ALL=C

# Decode all the *.pass messages in the openflow-messages repository. Collect
# resulting output and any error messages in the file openflow-messages.pass.out.

echo "Test openflow-messages '*.pass'"
ofmessages_pass_out="$CURRENT_TEST_DIR/openflow-messages$$.pass.out"
find external/openflow-messages -name '*.pass' -print | sort | xargs $LIBOFP_MEMCHECK $LIBOFP decode -kV --pkt-decode > $ofmessages_pass_out 2>&1

echo "Compare $ofmessages_pass_out to $CURRENT_SOURCE_DIR/openflow-messages.pass.out"
diff $ofmessages_pass_out "$CURRENT_SOURCE_DIR/openflow-messages.pass.out"
rm $ofmessages_pass_out

# Decode all the *.fail messages in the openflow-messages repository. Collect
# resulting error messages (and output) in the file openflow-messages.fail.out.

echo "Test openflow-messages '*.fail'"
ofmessages_fail_out="$CURRENT_TEST_DIR/openflow-messages$$.fail.out"
find external/openflow-messages -name '*.fail' -print | sort | xargs $LIBOFP_MEMCHECK $LIBOFP decode -kV --show-filename > $ofmessages_fail_out 2>&1

echo "Compare $ofmessages_fail_out to $CURRENT_SOURCE_DIR/openflow-messages.fail.out"
diff $ofmessages_fail_out "$CURRENT_SOURCE_DIR/openflow-messages.fail.out"
rm $ofmessages_fail_out

# Decode all messages in ovs-messages.pass. Collect resulting error messages 
# (and output) in the file ovs-messages.pass.out.

echo "Test ovs-messages.pass"
ovs_messages_pass_out="$CURRENT_TEST_DIR/ovs-messages$$.pass.out"
$LIBOFP_MEMCHECK $LIBOFP decode -kV --pkt-decode "$CURRENT_SOURCE_DIR/ovs-messages.pass" > $ovs_messages_pass_out 2>&1

echo "Compare $ovs_messages_pass_out to $CURRENT_SOURCE_DIR/ovs-messages.pass.out"
diff $ovs_messages_pass_out "$CURRENT_SOURCE_DIR/ovs-messages.pass.out"
rm $ovs_messages_pass_out

echo "Done."
exit 0
