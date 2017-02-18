#!/bin/bash

set -e

echo "Build oftr.1"
a2x --doctype manpage --format manpage oftr.1.adoc

echo "Build oftr-jsonrpc.1"
a2x --doctype manpage --format manpage oftr-jsonrpc.1.adoc

echo "Build oftr-schema.1"
a2x --doctype manpage --format manpage oftr-schema.1.adoc

exit 0
