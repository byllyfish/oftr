#!/bin/bash

set -e

echo "Working Directory:" `pwd`

CURRENT_SOURCE_DIR=`dirname "$0"`

echo "Test openflow-messages '*.pass'"

# Decode all the *.pass messages in the openflow-messages repository. Collect
# resulting output and any error messages in the file openflow-messages.pass.out.

find $CURRENT_SOURCE_DIR/../../../external/openflow-messages -name '*.pass' -exec ../libofp decode -kV {} + > openflow-messages.pass.out 2>&1

diff openflow-messages.pass.out "$CURRENT_SOURCE_DIR/openflow-messages.pass.out"

echo "Test openflow-messages '*.fail'"

# Decode all the *.fail messages in the openflow-messages repository. Collect
# resulting error messages (and output) in the file openflow-messages.fail.out.

find $CURRENT_SOURCE_DIR/../../../external/openflow-messages -name '*.pass' -exec ../libofp decode -kV {} + > openflow-messages.fail.out 2>&1

diff openflow-messages.fail.out "$CURRENT_SOURCE_DIR/openflow-messages.fail.out"

echo "Done."
exit 0
