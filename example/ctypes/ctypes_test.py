from ctypes import *
import binascii
import re
import sys


class libofp_buffer(Structure):
    """
    typedef struct {
        char *data;
        size_t length;
    } libofp_buffer;
    """
    _fields_ = [('data', c_void_p), 
                ('length', c_size_t)]

    def __init__(self, buf = None):
        if buf is None:
            self._original = None
            self.data = cast(0, c_void_p)
            self.length = 0
        else:
            if isinstance(buf, str):
                buf = buf.encode('utf-8')
            elif not isinstance(buf, bytes):
                raise ValueError('Invalid buf type')
            # Keep reference to original buffer, since we point to its memory.
            # We do not want it to get garbage collected early.
            self._original = buf
            self.data = cast(buf, c_void_p)
            self.length = len(buf)

    def __enter__(self):
        assert self._original is None
        return self

    def __exit__(self, type, value, traceback):
        dll.libofp_buffer_free(self)

    def bytes(self):
        return string_at(self.data, self.length)

    def hex(self):
        return binascii.hexlify(self.bytes()).decode('utf-8')

    def str(self):
        return self.bytes().decode('utf-8', 'replace')

    def __str__(self):
        return self.str()


executable = sys.argv[1]
assert executable[0] == '/'   # must be absolute path; don't search library path
dll = cdll.LoadLibrary(executable)

dll.libofp_version.argtypes = [POINTER(libofp_buffer)]
dll.libofp_encode.argtypes = [POINTER(libofp_buffer), c_char_p, c_uint32]
dll.libofp_decode.argtypes = [POINTER(libofp_buffer), POINTER(libofp_buffer), c_uint32]
dll.libofp_buffer_free.argtypes = [POINTER(libofp_buffer)]


def version():
    with libofp_buffer() as result:
        dll.libofp_version(result)
        return result.str()

def encode(text):
    with libofp_buffer() as result:
        if dll.libofp_encode(result, text, 0) < 0:
            raise ValueError(result.str())
        return result.bytes()

def decode(data):
    with libofp_buffer() as result:
        if dll.libofp_decode(result, libofp_buffer(data), 0) < 0:
            raise ValueError(result.str())
        return result.str()


if __name__ == '__main__':
    # Test version() function.
    vers = version()
    version_regex = re.compile(r'^\d+\.\d+\.\d+ \([a-f0-9]+\)  \<https://github\.com/byllyfish/oftr\>$')
    if not version_regex.match(vers):
        print('Unexpected version format: %s' % vers)
        sys.exit(1)

    # Test encode() function.
    msg = encode('type: HELLO\nversion: 4')
    if msg != binascii.unhexlify('04000010000000000001000800000010'):
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

