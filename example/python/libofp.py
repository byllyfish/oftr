"""
Simple Python Wrapper for LibOFP.

Requires:  PyYAML
"""

import yaml
import socket


OPENFLOW_PORT = 6633
MESSAGE_BEGIN = '---\n'
MESSAGE_END = '...\n'


def _libofp(evt, **msgdict):
    return dict(event=evt, msg=msgdict)


class _toObj(object):
    def __init__(self, d):
        for a, b in d.items():
            if isinstance(b, (list, tuple)):
               setattr(self, a, [_toObj(x) if isinstance(x, dict) else x for x in b])
            else:
               setattr(self, a, _toObj(b) if isinstance(b, dict) else b)


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
---
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
...
'''
          
        
    To schedule a "wake up call":
    
        ofp.setTimer(datapath, timerID, timeout)
        
    datapath is the datapathID of the switch.
    timerID is an opaque unsigned integer.
    timeout is in milliseconds.
    
    When the timer expires, you will receive a LIBOFP_TIMER_EVENT.
    """
    
    def __init__(self, host='localhost', port=9191):
        self._sockInput = None
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._sock.connect((host, port))
        self._sockInput = self._sock.makefile()
        self._sendListenRequest()
        self._eventGenerator = self._makeEventGenerator()
        
    def __del__(self):
        if self._sockInput:
            self._sockInput.close()
        self._sock.close()

    def send(self, data):
        msg = yaml.dump(data) if not isinstance(data, str) else data
        msg = MESSAGE_BEGIN + msg + MESSAGE_END
        print msg
        self._sock.sendall(msg)
        
    def waitNextEvent(self):
        return self._eventGenerator.next()

    def setTimer(self, datapath, timerID, timeout):
        self._send(_libofp('LIBOFP_SET_TIMER', datapath_id=datapath, 
                            timer_id=timerID, timeout=timeout))
        
    def _sendListenRequest(self):
        self.send(_libofp('LIBOFP_LISTEN_REQUEST', port=OPENFLOW_PORT))

    def _makeEventGenerator(self):
        msgLines = []
        for line in self._sockInput:
            if line == '...\n' and msgLines:
                msg = ''.join(msgLines)
                print msg
                obj = _toObj(yaml.safe_load(msg))
                obj.text = msg
                if hasattr(obj,'event') and not hasattr(obj,'type'):
                    obj.type = obj.event
                yield obj
                msgLines = []
            else:
                msgLines.append(line)    
