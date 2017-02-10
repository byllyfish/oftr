# Python Controller
#
# Set environment variable LIBOFP_PATH before running this program. If that
# variable is not set, this program will use /usr/local/bin/libofp.

import os
import sys
import libofp_json as libofp

def setConfig(datapath, length):
    return {
      'type': 'SET_CONFIG',
      'datapath_id': datapath,
      'msg': {
        'flags': [ 'FRAG_NORMAL' ],
        'miss_send_len': length
      }
    }

def clearFlows(datapath):
    return {
      'type': 'FLOW_MOD',
      'datapath_id': datapath,
      'msg': {
        'cookie':       0,
        'cookie_mask':  0,
        'table_id':     0,
        'command':      3,
        'idle_timeout': 0,
        'hard_timeout': 0,
        'priority':     0x8000,
        'buffer_id':    0xFFFFFFFF,
        'out_port':     0xFFFFFFFF,
        'out_group':    0,
        'flags':        [],
        'match':  [],
        'instructions': []
      }
    }

def barrierRequest(datapath):
    return {
      'type': 'BARRIER_REQUEST',
      'datapath_id': datapath
    }

def isMulticast(enetAddr):
    return enetAddr[1] in "13579BDF"

# Tell switch to send the packet out all ports.
def flood(event):
    return {
      'type': 'PACKET_OUT',
      'datapath_id': event.datapath_id,
      'msg': {
        'buffer_id': event.msg.buffer_id,
        'in_port': event.msg.in_port,
        'actions': [
          { 'action': 'OUTPUT', 
            'port_no': 0xfffffffb, 
            'max_len': 0 }
        ],
        'data': ''
      }
    }


# Tell the switch to drop the packet.
def drop(event):
    return {
      'type': 'PACKET_OUT',
      'datapath_id': event.datapath_id,
      'msg': {
        'buffer_id': event.msg.buffer_id,
        'in_port': event.msg.in_port,
        'actions': [],
        'data': ''
      }
    }

def addFlow(event, ethSource, ethDest, outPort):
    return {
      'type': 'FLOW_MOD',
      'datapath_id': event.datapath_id,
      'msg': {
        'cookie':          0,
        'cookie_mask':     0,
        'table_id':        0,
        'command':         0,
        'idle_timeout':    10,
        'hard_timeout':    30,
        'priority':        0x7FFF,
        'buffer_id':       event.msg.buffer_id,
        'out_port':        0,
        'out_group':       0,
        'flags':           [],
        'match': [
          { 'field': 'IN_PORT', 
            'value': event.msg.in_port },
          { 'field': 'ETH_SRC', 
            'value': ethSource },
          { 'field': 'ETH_DST', 
            'value': ethDest }
        ],
        'instructions': [
          { 'instruction': 'APPLY_ACTIONS',
            'actions': [
              { 'action': 'OUTPUT', 
                'port_no': outPort, 
                'max_len': 0 }
            ]
          }
        ]
      }
    }


def handlePacketIn(ofp, event):
    ethDest = event.msg.data[0:12]
    ethSource = event.msg.data[12:24]
    ethType = event.msg.data[24:28]

    if not isMulticast(ethSource):
        key = event.datapath_id + ethSource
        forwardTable[key] = event.msg.in_port

    if isMulticast(ethDest):
        ofp.send(flood(event))
    else:
        key = event.datapath_id + ethDest
        if key in forwardTable:
            outPort = forwardTable[key]
            if outPort == event.msg.in_port:
                ofp.send(drop(event))
            else:
                ofp.send(addFlow(event, ethSource, ethDest, forwardTable[key]))
        else:
            ofp.send(flood(event))


if __name__ == '__main__':
    forwardTable = {}

    ofpx = os.environ.get('LIBOFP_PATH')
    if not ofpx:
        ofpx = '/usr/local/bin/oftr'

    ofp = libofp.LibOFP(ofpx)
    for event in ofp:
        
        if hasattr(event, 'error'):
            print >>sys.stderr, event
        else:
            assert event.method == 'OFP.MESSAGE'
            if event.params.type == 'CHANNEL_UP':
                ofp.send(setConfig(event.params.datapath_id, 14))
                ofp.send(clearFlows(event.params.datapath_id))
                ofp.send(barrierRequest(event.params.datapath_id))
            elif event.params.type == 'PACKET_IN':
                handlePacketIn(ofp, event.params)
