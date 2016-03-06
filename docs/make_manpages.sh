#!/bin/bash

set -e

echo "Build libofp.1"
a2x --doctype manpage --format manpage libofp.1.adoc

echo "Build libofp-jsonrpc.1"
a2x --doctype manpage --format manpage libofp-jsonrpc.1.adoc

echo "Build libofp-schema.1"
a2x --doctype manpage --format manpage libofp-schema.1.adoc

exit 0
