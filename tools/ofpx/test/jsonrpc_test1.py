# jsonrpc_test1

import jsonrpc_client as rpc
import certdata
import time

def onResult(conn, event):
    pass
    #print 'onResult', event

auxID = 0

def featuresReply(connid, xid):
    global auxID
    a = auxID
    auxID += 1
    return { 'type': 'OFPT_FEATURES_REPLY', 'xid': xid, 'conn_id': connid, 'msg': {
        'datapath_id':'1111-2222-3333-4444',
        'n_buffers': 1,
        'n_tables': 1,
        'auxiliary_id': a,
        'capabilities': 0,
        'ports': []
    }}

def packetIn(connid):
    return { 'type': 'OFPT_PACKET_IN', 'conn_id': connid, 'msg': {
        'buffer_id': 0,
        'total_len': 100,
        'in_port': 1,
        'in_phy_port': 1,
        'metadata': 0,
        'reason': 0,
        'table_id': 0,
        'cookie': 0,
        'data': 'DEADBEEF'
    }}

def onNotify(conn, event):
    #print event
    if event.method == 'ofp.message' and event.params.type == 'OFPT_FEATURES_REQUEST':
        conn.call('ofp.send', featuresReply(event.params.conn_id, event.params.xid)) 
        conn.call('ofp.send', packetIn(event.params.conn_id))
    

def onTimer(conn, event):
    conn.call('ofp.list_connections', {'conn_id': 0 }, onResult)
    conn.call('ofp.close', {'conn_id': 4 }, onResult)
    #conn.wait('timer1', 10.0, onTimer)
    conn.call('ofp.listen', {'endpoint': '8889', 'security_id': 1}, onResult)
    conn.call('ofp.connect', {'endpoint': '127.0.0.1:8889', 'security_id': 2, 'options':['--raw']}, onResult)


if __name__ == '__main__':
    conn = rpc.connect('subprocess:/Users/bfish/code/ofp/Build+Debug/tools/ofpx/ofpx', args=['jsonrpc'])
    
    # Call onNotify when a notification event from connection arrives.
    conn.notification(onNotify)
    
    #print "Sleeping for 30 seconds", conn
    #time.sleep(30)
    
    reply = conn.call_sync('ofp.add_identity', {
                'certificate': certdata.SERVER_PEM, 
                'password': certdata.KEY_PASSPHRASE, 
                'verifier': certdata.VERIFIER_PEM
            })
    
    listenSecurityId = reply.result.security_id
    
    reply = conn.call_sync('ofp.add_identity', {
                'certificate': certdata.CLIENT_PEM, 
                'password': certdata.KEY_PASSPHRASE, 
                'verifier': certdata.VERIFIER_PEM
            })
            
    connectSecurityId = reply.result.security_id
        
    reply = conn.call_sync('ofp.listen', {
                'endpoint': '8889', 
                'security_id': listenSecurityId
            })


    reply = conn.call_sync('ofp.listen', {
                'endpoint': '8888'
            })
    for i in range(10):
        reply = conn.call_sync('ofp.connect', {
                    'endpoint': '127.0.0.1:8889', 
                    'security_id': connectSecurityId, 
                    'options':['--raw']
                })
                
    reply = conn.call_sync('ofp.connect', {
                'endpoint': '127.0.0.1:8889', 
                'security_id': connectSecurityId, 
                'options':['--raw', '--udp']
            })
    
    #conn.call('ofp.listen', {'endpoint': '127.0.0.1:88'}, onResult)
    #conn.call('ofp.listen', {'endpoint': '127.0.0.1:8888'}, onResult)
    #conn.call('ofp.listen', {'endpoint': '8889', 'security_id': 1}, onResult)
    #conn.call('ofp.close', {'conn_id': 1 }, onResult)
    #conn.call('ofp.connect', {'endpoint': '127.0.0.1:8889', 'security_id': 2, 'options':['--raw']}, onResult)
    #conn.call('ofp.connect', {'endpoint': '127.0.0.1:8889', 'options':['--raw', '--udp']}, onResult)
    #conn.call('ofp.connect', {'endpoint': '[::1]:8889', 'options':['--raw', '--udp']}, onResult)
    #conn.call('ofp.connect', {'endpoint': '10.0.0.1:9999', 'options':['--raw']}, onResult)
    
    conn.wait('timer1', 10.0, onTimer)
    rpc.run(conn)

