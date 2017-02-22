"""
Simple Python Wrapper for LibOFP.

No external dependencies; uses builtin json module.
"""

import json
import subprocess
import os
import sys

def getDefaultPort():
    try:
        return int(os.environ.get('LIBOFP_DEFAULT_PORT'))
    except:
        return 6653


DEFAULT_OPENFLOW_PORT = getDefaultPort()
EVENT_DELIMITER = '\x00'

class _JsonObject:
    def __init__(self, d):
        self.__dict__ = d

    def __repr__(self):
        return str(self.__dict__)

class LibOFP(object):
    """
    Implements Python Driver to LibOFP.

    Example:

        ofp = libofp.LibOFP()
        for event in ofp:
            if event.method == 'OFP.MESSAGE' and event.params.type == 'PACKET_IN':
                handlePacketIn(ofp, event)


    To send a message:

       ofp.send({
            'type': 'BARRIER_REQUEST',
            'datapath_id': datapath
        })
    }
    """

    def __init__(self, driverPath='/usr/local/bin/libofp',
                       openflowAddr=('', DEFAULT_OPENFLOW_PORT),
                       listen=True):
        """
        Open a connection to the driver and prepare to receive events.

        openflowAddr - Address for OpenFlow driver to listen on. Specified
                    as a 2-tuple (host, port). Specify host of '' for all
                    local addresses.
        """

        self._sockInput = None
        self._sockOutput = None
        self._process = None
        self._openDriver(driverPath)
        self._xid = 2

        self._sendDescriptionRequest()

        if listen:
            self._sendListenRequest(openflowAddr)

    def _openDriver(self, driverPath):
        assert driverPath

        # Driver's path is specified, so launch the executable.
        # Communicate with the driver using stdin and stdout (in line mode).
        self._process = subprocess.Popen([driverPath, 'jsonrpc', '--loglevel=info'],
                        stdin=subprocess.PIPE, stdout=subprocess.PIPE)

        # Our input is the other process's stdout.
        self._sockInput = self._process.stdout
        self._sockOutput = self._process.stdin

    def __del__(self):
        if self._sockInput:
            self._sockInput.close()
        if self._sockOutput and self._sockOutput is not self._sockInput:
            self._sockOutput.close()

    def send(self, params):
        self._call('OFP.SEND', id=None, **params)

    def __iter__(self):
        return self

    def next(self):
        line = _read_until(self._sockInput, EVENT_DELIMITER)
        if not line:
            raise StopIteration()
        return json.loads(line, object_hook=_JsonObject)

    def _call(self, method, id=None, **params):
        rpc = {
          'method': method,
          'params': params
        }
        if id is not None:
            rpc['id'] = id
        if method == 'OFP.SEND' and 'xid' not in params:
            params['xid'] = self._xid
            self._xid += 1
        self._write(json.dumps(rpc) + EVENT_DELIMITER)

    def _sendListenRequest(self, openflowAddr):
        self._call('OFP.LISTEN', endpoint='[%s]:%d' % openflowAddr, options=['FEATURES_REQ'])

    def _sendDescriptionRequest(self):
        self._call('OFP.DESCRIPTION', id=99999)
        msg = self.next()
        print >>sys.stderr, msg
        assert msg.id == 99999
        assert msg.result.api_version == '0.9'
        assert len(msg.result.sw_desc) > 0
        assert msg.result.versions == [1, 2, 3, 4, 5, 6]

    def _write(self, msg):
        self._sockOutput.write(msg)



def _read_until(stream, delimiter):
    """Read next line from stream.
    """
    buf = ''
    while True:
        ch = stream.read(1)
        if not ch or ch == delimiter:
            return buf
        buf += ch
