"""
Simple Python Wrapper for LibOFP.

No external dependencies; uses builtin json module.
"""

import json
import socket
import subprocess
import sys

DEFAULT_OPENFLOW_PORT = 6633
DEFAULT_DRIVER_PORT = 9191
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
        while True:
            event = ofp.waitNextEvent()
            if event.type == 'OFPT_PACKET_IN':
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

    def __init__(self, driverPath='/usr/local/bin/ofpx',
                       openflowAddr=('', DEFAULT_OPENFLOW_PORT),
                       driverAddr=None,
                       listen=True):
        """
        Open a connection to the driver and prepare to receive events.

        openflowAddr - Address for OpenFlow driver to listen on. Specified
                    as a 2-tuple (host, port). Specify host of '' for all
                    local addresses. Specify port of 0 for default
                    openflow port.
        driverAddr - Address for driver to listen on. Specified as a
                    2-tuple (host, port). Specify None to disable YAML
                    TCP server.
        """

        self._sockInput = None
        self._sockOutput = None
        self._sock = None
        self._process = None
        self._openDriver(driverPath, driverAddr)

        if listen:
            self._sendListenRequest(openflowAddr)
        self._eventGenerator = self._makeEventGenerator()

    def _openDriver(self, driverPath, driverAddr):

        if driverPath:
            # Driver's path is specified, so launch the executable.
            if driverAddr:
                # We're connecting to the driver over TCP.
                self._process = subprocess.Popen([driverPath])
            else:
                # We're not connecting over TCP, so communicate with the driver
                # using stdin and stdout (in line mode).
                self._process = subprocess.Popen([driverPath],
                                stdin=subprocess.PIPE, stdout=subprocess.PIPE)

        if driverAddr:
            self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self._sock.connect(driverAddr)
            self._sockInput = self._sock.makefile()
            self._write = self._writeToSocket

        else:
            # Our input is the other process's stdout.
            self._sockInput = self._process.stdout
            self._sockOutput = self._process.stdin
            self._write = self._writeToFile

    def __del__(self):
        if self._sockInput:
            self._sockInput.close()
        if self._sockOutput and self._sockOutput is not self._sockInput:
            self._sockOutput.close()
        if self._sock:
            self._sock.close()

    def send(self, params):
        self._call('ofp.send', id=None, **params)

    def __iter__(self):
        return self

    def next(self):
        line = self._sockInput.readline()
        if not line:
            raise StopIteration()
        return json.loads(line, object_hook=_JsonObject)
        #return self._eventGenerator.next()

    def _call(self, method, id=None, **params):
        rpc = {
          'method': method,
          'params': params
        }
        if id is not None:
            rpc['id'] = id
        self._write(json.dumps(rpc) + EVENT_DELIMITER)

    def _sendListenRequest(self, openflowAddr):
        self._call('ofp.listen', endpoint='%s %d' % openflowAddr)

    def _makeEventGenerator(self):
        # Simply using `for line in self._sockInput:` doesn't work for pipes in
        # Python 2.7.2.
        for line in iter(self._sockInput.readline, ''):
            yield json.loads(line, object_hook=_JsonObject)

    def _writeToSocket(self, msg):
        self._sock.sendall(msg)

    def _writeToFile(self, msg):
        self._sockOutput.write(msg)


if __name__ == '__main__':
    ofp = LibOFP('/Users/bfish/code/ofp/Build+Debug/tools/ofpx/ofpx')
    for event in ofp:
        print event

