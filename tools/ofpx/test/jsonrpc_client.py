# jsonrpc_client

import subprocess
import re
import os
import json
import select
import time
import heapq
from distutils.spawn import find_executable


def connect(url, **options):
    """
    Connect to the JSON-RPC service at the specified URL and return a 
    Connection object.
    """
    
    if url.startswith('subprocess:'):
        return SubprocessConnection(url, **options)
    
    raise ValueError('Invalid URL: `%s`' % url)

def run(*allConns, **kwds):
    """
    Loop and respond to events on the connection.
    """
    
    conns = list(allConns)
    replyId = kwds.get('id', None)
    
    prev = 0
    while conns:
        try:
            (r, w, x) = select.select(conns, [], [], 0.1)
        except KeyboardInterrupt:
            return
        now = time.time()
        if now - prev >= 0.1:
            prev = now
            for conn in conns:
                conn._checkTimer(now)
        for conn in r:
            event = conn._recv()
            if event:
                if hasattr(event, 'id') and event.id is not None:
                    if replyId and event.id == replyId:
                        return event
                    else:
                        conn._reply(event)
                else:
                    conn._notify(event)
            else:
                conns.remove(conn)

class Connection(object):
    """
    Connection to JSON-RPC service.
    """
    
    _notificationCallback = None
    _nextId = 1
    _callbacks = {}
    _timers = []
    
    def notification(self, callback):
        """
        Specify callback for incoming notifications. The callback function takes 
        two arguments: `conn` and `event`. `conn` is this connection object. 
        `event` is a JsonObject representing the event or error.
        
        Returns the previous notification callback.
        """
        
        prev = self._notificationCallback
        self._notificationCallback = callback
        return prev
        
    def call(self, method, params, callback=None):
        """
        Send a JSON-RPC event to the connection. If the callback function is
        specified, send the event with a unique ID and invoke the callback
        when a response or error is received.
        """
        
        event = dict(method=method, params=params)
        if callback:
            event['id'] = self._nextId
            self._nextId += 1
            self._callbacks[event['id']] = callback
        self._send(event)
    
    def call_sync(self, method, params):
        """
        Send JSON-RPC event to the connection, wait for the response, and 
        return it.
        """
        
        event = dict(id=self._nextId, method=method, params=params)
        self._nextId += 1
        self._send(event)
        return run(self, id=event['id'])
        
    def wait(self, obj, timeout, callback):
        """
        Start a timer to wait for the timeout, then invoke the callback. Pass
        obj at the second parameter to the timer callback.
        """
        
        now = time.time()
        heapq.heappush(self._timers, (now + timeout, callback, obj))
        
    def _checkTimer(self, now):
        if self._timers and now >= self._timers[0][0]:
            self._timers[0][1](self, self._timers[0][2])
            heapq.heappop(self._timers)

    def close(self):
        pass
    
    def _send(self, event):
        pass
        
    def _recv(self):
        pass

    def _reply(self, event):
        callback = self._callbacks.get(event.id, None)
        if callback:
            del self._callbacks[event.id]
            callback(self, event)

    def _notify(self, event):
        if self._notificationCallback:
            self._notificationCallback(self, event)
 

class SubprocessConnection(Connection):
    """
    Connection to JSON-RPC service over stdin/stdout to a child process.
    """
    
    def __init__(self, url, **options):
        """
        Launch child process and set up input/output pipes.
        """
        
        if 'args' not in options:
            raise ValueError('No argument named "args"')

        m = re.match(r'^subprocess:(.+)$', url)
        if not m:
            raise ValueError('Invalid URL: `%s`' % url)
        executable = m.group(1)
        foundExecutable = find_executable(executable)
        if not foundExecutable:
            raise ValueError('Executable does not exist: `%s`' % executable)
        args = [foundExecutable] + options['args']
        self._process = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, env={'DYLD_INSERT_LIBRARIES': '/usr/lib/libgmalloc.dylib'})
        
        # Read connection input from child process stdout.
        self._sockInput = self._process.stdout
        
        # Write connection output to child process stdin.
        self._sockOutput = self._process.stdin

    def close(self):
        """
        Close the connection to the child process.
        """
        
        self._process.terminate()
        
    def fileno(self):
        return self._sockInput.fileno()
            
    def _send(self, event):
        msg = json.dumps(event, separators=(',', ':')) + '\n'
        self._sockOutput.write(msg)
        
    def _recv(self):
        """
        Return the next event from the input.
        """
        
        try:
            line = self._sockInput.readline()
        except KeyboardInterrupt:
            line = None
        
        if not line:
            return None

        return json.loads(line, object_hook=JsonObject)
    

class JsonObject(object):
    """
    Wrapper for dictionary that makes entries accessible by dot notation.
    """
    
    def __init__(self, d):
        self.__dict__ = d

    def __repr__(self):
        return str(self.__dict__)


def onResult(conn, event):
    pass
    print 'onResult', event

auxID = 0

def features_reply(connid, xid):
    global auxID
    a = auxID
    auxID += 1
    return { 'version': 4, 'type': 'OFPT_FEATURES_REPLY', 'xid': xid, 'conn_id': connid, 'msg': {
        'datapath_id':'1111-2222-3333-4444',
        'n_buffers': 1,
        'n_tables': 1,
        'auxiliary_id': a,
        'capabilities': 0,
        'ports': []
    }}

def packet_in(connid):
    return { 'version': 4, 'type': 'OFPT_PACKET_IN', 'conn_id': connid, 'msg': {
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
    #print 'onNotify', event
    if event.method == 'ofp.message' and event.params.type == 'OFPT_FEATURES_REQUEST':
        conn.call('ofp.send', features_reply(event.params.conn_id, event.params.xid), onResult) 
        conn.call('ofp.send', packet_in(event.params.conn_id), onResult)
    

def onTimer(conn, event):
    conn.call('ofp.list_connections', {'conn_id': 0 }, onResult)
    conn.call('ofp.close', {'conn_id': 0 }, onResult)
    #conn.wait('timer1', 10.0, onTimer)
    conn.call('ofp.listen', {'endpoint': '8889', 'security_id': 1}, onResult)
    conn.call('ofp.connect', {'endpoint': '127.0.0.1:8889', 'security_id': 2, 'options':['--raw']}, onResult)



import certdata


if __name__ == '__main__':
    conn = connect('subprocess:/Users/bfish/code/ofp/Build+Debug/tools/ofpx/ofpx', args=['jsonrpc'])
    #time.sleep(20)
    #conn.call('ofp.set_authority', {'certificate':'root.pem'}, onResult)
    reply = conn.call_sync('ofp.add_identity', {'certificate':certdata.SERVER_PEM, 'password':'passphrase', 'verifier': certdata.VERIFIER_PEM})
    conn.call_sync('ofp.listen', {'endpoint': '8889', 'security_id': reply.result.security_id})
    
    reply = conn.call_sync('ofp.add_identity', {'certificate':certdata.CLIENT_PEM, 'password':'passphrase', 'verifier':certdata.VERIFIER_PEM})
    conn.call_sync('ofp.connect', {'endpoint': '127.0.0.1:8889', 'security_id': reply.result.security_id, 'options':['--raw']})
    
    #conn.call('ofp.listen', {'endpoint': '127.0.0.1:88'}, onResult)
    #conn.call('ofp.listen', {'endpoint': '127.0.0.1:8888'}, onResult)
    #conn.call('ofp.listen', {'endpoint': '8889', 'security_id': 1}, onResult)
    #conn.call('ofp.close', {'conn_id': 1 }, onResult)
    conn.call('ofp.connect', {'endpoint': '127.0.0.1:8889', 'security_id': 2, 'options':['--raw']}, onResult)
    #conn.call('ofp.connect', {'endpoint': '127.0.0.1:8889', 'options':['--raw', '--udp']}, onResult)
    #conn.call('ofp.connect', {'endpoint': '[::1]:8889', 'options':['--raw', '--udp']}, onResult)
    #conn.call('ofp.connect', {'endpoint': '10.0.0.1:9999', 'options':['--raw']}, onResult)
    conn.notification(onNotify)
    conn.wait('timer1', 10.0, onTimer)
    run(conn)
