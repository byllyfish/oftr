# Python Controller
#
# Set environment variable LIBOFPEXEC_PATH before running this program. If that
# variable is not set, this program will use /usr/local/bin/libofpexec.

import os

# If the environment variable LIBOFP_YAML is set to 1, use YAML version;
# otherwise default to json version.

if os.environ.get('LIBOFP_YAML') == '1':
  import libofp_yaml as libofp
else:
  import libofp_json as libofp


def setConfig(datapath, length):
    return {
      'type': 'OFPT_SET_CONFIG',
      'datapath_id': datapath,
      'msg': {
        'flags': 0,
        'miss_send_len': length
      }
    }

def clearFlows(datapath):
    return {
      'type': 'OFPT_FLOW_MOD',
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
        'flags':        0,
        'match':  [],
        'instructions': []
      }
    }

def barrierRequest(datapath):
    return {
      'type': 'OFPT_BARRIER_REQUEST',
      'datapath_id': datapath
    }

def isMulticast(enetAddr):
    return enetAddr[1] in "13579BDF"

# Tell switch to send the packet out all ports.
def flood(event):
    return {
      'type': 'OFPT_PACKET_OUT',
      'datapath_id': event.datapath_id,
      'msg': {
        'buffer_id': event.msg.buffer_id,
        'in_port': event.msg.in_port,
        'actions': [
          { 'action': 'OFPAT_OUTPUT', 
            'port': 0xfffffffb, 
            'max_len': 0 }
        ],
        'data': ''
      }
    }


# Tell the switch to drop the packet.
def drop(event):
    return {
      'type': 'OFPT_PACKET_OUT',
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
      'type': 'OFPT_FLOW_MOD',
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
        'flags':           0,
        'match': [
          { 'field': 'OFB_IN_PORT', 
            'value': event.msg.in_port },
          { 'field': 'OFB_ETH_SRC', 
            'value': ethSource },
          { 'field': 'OFB_ETH_DST', 
            'value': ethDest }
        ],
        'instructions': [
          { 'instruction': 'OFPIT_APPLY_ACTIONS',
            'actions': [
              { 'action': 'OFPAT_OUTPUT', 
                'port': outPort, 
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

    libofpexec = os.environ.get('LIBOFPEXEC_PATH')
    if not libofpexec:
        libofpexec = '/usr/local/bin/libofpexec'

    ofp = libofp.LibOFP(libofpexec)
    while True:
        event = ofp.waitNextEvent()
        #print event.text

        if event.method == 'ofp.datapath' and event.params.status == 'UP':
            ofp.send(setConfig(event.params.datapath_id, 14))
            ofp.send(clearFlows(event.params.datapath_id))
            ofp.send(barrierRequest(event.params.datapath_id))
        elif event.method == 'ofp.message' and event.params.type == 'OFPT_PACKET_IN':
            handlePacketIn(ofp, event.params)
