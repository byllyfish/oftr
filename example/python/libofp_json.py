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

class _JsonObject:
    def __init__(self, d):
        self.__dict__ = d


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


    To schedule a "wake up call":

        ofp.setTimer(datapath, timerID, timeout)

    datapath is the datapathID of the switch.
    timerID is an opaque unsigned integer.
    timeout is in milliseconds.

    When the timer expires, you will receive a LIBOFP_TIMER_EVENT.
    """

    def __init__(self, driverPath='/usr/local/bin/libofpexec',
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
        self._sendEditSetting('format', 'json')

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
        rpc = {
          'method': 'ofp.send', 
          'params': params
        }
        msg = json.dumps(rpc)
        self._write(msg + '\n')

    def _call(self, method, id=None, **params):
        rpc = {
          'method': method,
          'params': params
        }
        if id is not None:
            rpc['id'] = id
        msg = json.dumps(rpc)
        self._write(msg + '\n')

    def waitNextEvent(self):
        return self._eventGenerator.next()

    def setTimer(self, datapath, timerID, timeout):
        self._call('ofp.set_timer', datapath_id=datapath,
                            timer_id=timerID, timeout=timeout)

    def _sendEditSetting(self, name, value):
        self._call('ofp.config', options=['%s=%s' % (name, value)])

    def _sendListenRequest(self, openflowAddr):
        self._call('ofp.open', endpoint='%s %d' % openflowAddr)

    def _makeEventGenerator(self):
        msgLines = []
        # Simply using `for line in self._sockInput:` doesn't work for pipes in
        # Python 2.7.2.
        for line in iter(self._sockInput.readline, ''):
            obj = json.loads(line, object_hook=_JsonObject)
            yield obj

            # if line == '...\n' and msgLines:
            #     msg = ''.join(msgLines[1:])
            #     #print msg
            #     obj = _toObj(json.loads(msg))
            #     obj.text = msg
            #     if hasattr(obj,'method') and not hasattr(obj,'type'):
            #         obj.type = obj.method
            #     yield obj
            #     msgLines = []
            # else:
            #     msgLines.append(line)

    def _writeToSocket(self, msg):
        self._sock.sendall(msg)

    def _writeToFile(self, msg):
        self._sockOutput.write(msg)


if __name__ == '__main__':
    ofp = LibOFP('/Users/bfish/code/ofp/Build+Debug/libofpexec')
    while True:
        event = ofp.waitNextEvent()
        print event

