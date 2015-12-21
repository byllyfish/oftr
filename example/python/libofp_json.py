"""
Simple Python Wrapper for LibOFP.

No external dependencies; uses builtin json module.
"""

import json
import socket
import subprocess
import sys
import os

def getDefaultPort():
    try:
        return int(os.environ.get('LIBOFP_DEFAULT_PORT'))
    except:
        return 6653


DEFAULT_OPENFLOW_PORT = getDefaultPort()
EVENT_DELIMITER = '\n'

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
            if event.type == 'PACKET_IN':
                handlePacketIn(ofp, event)


    To send a message:

       ofp.send('''
          type:            OFPT_PACKET_OUT
          datapath_id:     0000-0000-0000-0001
          msg:
            buffer_id:       0x33333333
            in_port:         0x44444444
            actions:
              - action: OFPAT_OUTPUT
                port:   5
                maxlen: 20
              - action: OFPAT_SET_FIELD
                type:   OFB_IPV4_DST
                value:  192.168.1.1
            enet_frame: FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
       '''
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

        if listen:
            self._sendListenRequest(openflowAddr)
        self._eventGenerator = self._makeEventGenerator()

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
        self._call('ofp.send', id=None, **params)

    def __iter__(self):
        return self

    def next(self):
        line = self._sockInput.readline()
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
        self._write(json.dumps(rpc) + EVENT_DELIMITER)

    def _sendListenRequest(self, openflowAddr):
        self._call('ofp.listen', endpoint='[%s]:%d' % openflowAddr)

    def _makeEventGenerator(self):
        # Simply using `for line in self._sockInput:` doesn't work for pipes in
        # Python 2.7.2.
        for line in iter(self._sockInput.readline, ''):
            yield json.loads(line, object_hook=_JsonObject)

    def _write(self, msg):
        self._sockOutput.write(msg)


