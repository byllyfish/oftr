# Python 2.7 script to convert text protocol to binary protocol.
#
# Usage:
#   python jsonrpc_to_binary.py < jsonrpc-t.bin > jsonrpc-b.bin

import sys
import struct

data = sys.stdin.read()
msgs = [line for line in data.split('\x00') if line.strip()]

for msg in msgs:
    hdr = struct.pack('>L', ((len(msg) + 4) << 8) | 0xA0)
    sys.stdout.write(hdr + msg)
