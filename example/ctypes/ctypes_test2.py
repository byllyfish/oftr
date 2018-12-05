import sys
import re
import binascii
from ctypes import cdll, c_void_p, c_size_t, c_uint32, create_string_buffer

executable = sys.argv[1]
assert executable[0] == '/'   # must be absolute path; don't search library path

dll = cdll.LoadLibrary(executable)
dll.oftr_call.argtypes = [c_uint32, c_void_p, c_size_t, c_void_p, c_size_t]


def _oftr_call(opcode, inp):
    buflen = 512
    buf = create_string_buffer(buflen)

    result = dll.oftr_call(opcode, inp, len(inp), buf, buflen)
    if result < -buflen:
        buflen = -result
        buf = create_string_buffer(buflen)
        result = dll.oftr_call(opcode, inp, len(inp), buf, buflen)

    if result >= 0:
        return buf[:result]
    if result >= -buflen:
        raise ValueError('error: %s' % buf[:-result])
    raise ValueError('error: buffer result too big')


def version():
    return _oftr_call(0, '')


def encode(text, version=4):
    opcode = (version << 24) + 1
    return _oftr_call(opcode, text)


def decode(data):
    return _oftr_call(2, data)


if __name__ == '__main__':
    # Test version() function.
    vers = version()
    version_regex = re.compile(r'^\d+\.\d+\.\d+ \([a-f0-9]+\)  \<https://github\.com/byllyfish/oftr\>$')
    if not version_regex.match(vers):
        print('Unexpected version format: %s' % vers)
        sys.exit(1)

    # Test encode() function.
    msg = encode('type: HELLO\nversion: 4')
    if msg != binascii.unhexlify('0400000800000000'):
        print('Unexpected encode result: %s' % binascii.hexlify(msg))
        sys.exit(1)

    # Test decode() function.
    result = '''---
type:            HELLO
xid:             0x00000000
version:         0x04
msg:             
  versions:        [ 4 ]
...
'''
    msg = decode(binascii.unhexlify('04000010000000000001000800000010'))
    if msg != result:
        print('Unexpected decode result: %s' % msg)
        sys.exit(1)
