# Python Controller
#
# Before running this program, you must start `libofpexec`. It must be running
# on localhost so this program can connect to it.
#

import libofp


def setConfig(datapath, len):
    return '''
---
    type: OFPT_SET_CONFIG
    datapath_id: {}
    msg:
      flags: 0
      miss_send_len: {}
...
'''.format(datapath, len)


def isMulticast(enetAddr):
    return enetAddr[1] in "13579BDF" 

# Tell switch to send the packet out all ports.
def flood(ofp, event):
    print 'flood'
    return '''
---
      type:            OFPT_PACKET_OUT
      datapath_id:     {}
      msg:             
        buffer_id:       {}
        in_port:         {}
        actions:
          - action: OFPAT_OUTPUT
            port:   0xfffffffb
            maxlen: 0
        enet_frame:     ''

...
'''.format(event.datapath_id, event.msg.buffer_id, event.msg.in_port)

# Tell the switch to drop the packet.
def drop(ofp, event):
    print 'drop'
    return '''
---
      type:            OFPT_PACKET_OUT
      datapath_id:     {}
      msg:             
        buffer_id:       {}
        in_port:         {}
        actions:       []
        enet_frame:    ''

...
'''.format(event.datapath_id, event.msg.buffer_id, event.msg.in_port)    
      

def addFlow(ofp, event, ethSource, ethDest, outPort):
    return '''
---
      type:            OFPT_FLOW_MOD
      datapath_id:     {}
      msg:             
        cookie:          0
        cookie_mask:     0
        table_id:        0
        command:         0
        idle_timeout:    10
        hard_timeout:    30
        priority:        0x7FFF
        buffer_id:       {}
        out_port:        0
        out_group:       0
        flags:           0
        match:           
          - type:            OFB_IN_PORT
            value:           {}
          - type:            OFB_ETH_SRC
            value:           {}
          - type:            OFB_ETH_DST
            value:           {}
        instructions:
          - type:    OFPIT_APPLY_ACTIONS
            value:
              - action: OFPAT_OUTPUT
                port:   {}
                maxlen: 0
...
'''.format(event.datapath_id, event.msg.buffer_id, event.msg.in_port, ethSource, ethDest, outPort)
   
      
def handlePacketIn(ofp, event):
    print 'handlePacketIn'
    ethDest = event.msg.enet_frame[0:12]
    ethSource = event.msg.enet_frame[12:24]
    ethType = event.msg.enet_frame[24:28]
    
    if not isMulticast(ethSource):
        key = event.datapath_id + ethSource
        forwardTable[key] = event.msg.in_port
        
    if isMulticast(ethDest):
        ofp.send(flood(ofp, event))
    else:
        key = event.datapath_id + ethDest
        if key in forwardTable:
            outPort = forwardTable[key]
            if outPort == event.msg.in_port:
                ofp.send(drop(ofp, event))
            else:
                ofp.send(addFlow(ofp, event, ethSource, ethDest, forwardTable[key]))
        else:
            ofp.send(flood(ofp, event))


if __name__ == '__main__':
    forwardTable = {}
    
    ofp = libofp.LibOFP('/Users/bfish/code/ofp/Build+Debug/libofpexec')
    while True:
        event = ofp.waitNextEvent()
        #print event.text
        
        if event.type == 'LIBOFP_DATAPATH_UP':
            ofp.send(setConfig(event.msg.datapath_id, 14))

        elif event.type == 'OFPT_PACKET_IN':
            handlePacketIn(ofp, event)
