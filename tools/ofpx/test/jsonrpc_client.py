# jsonrpc_client

import subprocess
import re
import os
import json
import select
import time
import heapq
from distutils.spawn import find_executable

EVENT_DELIMITER = '\n'


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
        envs = {'MallocStackLogging': '1', 'MallocScribble': '1'}
        self._process = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, env=envs)
        
        # Read connection input from child process stdout.
        self._sockInput = self._process.stdout
        
        # Write connection output to child process stdin.
        self._sockOutput = self._process.stdin

    def __del__(self):
        """
        Wait for child process to terminate.
        """
        
        self._process.wait()
        
    def close(self):
        """
        Close the connection to the child process.
        """
        
        self._process.terminate()
        
    def fileno(self):
        return self._sockInput.fileno()
    
    def __repr__(self):
        return 'SubprocessConnection: pid=%d' % self._process.pid
         
    def _send(self, event):
        msg = json.dumps(event, separators=(',', ':')) + EVENT_DELIMITER
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
