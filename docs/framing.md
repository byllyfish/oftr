
Single Connection Mode
======================

Single-Connection mode will typicaly be used over a local domain socket, on the same machine. A connection in single-mode will receive all events from all datapaths. There is no filtering.

Framing
=======

There are two forms of protocol framing for delimiting messages: YAML document and Binary-Length.

YAML Document Framing (for Convenience)
--------------------------------------------

Protocol messages are delimited by "---" and "..." as used to frame YAML documents. The frame delimiters must be "---\n" or "...\n"; that is, the delimiters must be on a line by itself with no whitespace other than newlines.

The message content is UTF-8 text;

This protocol is convenient for human-readable text files.


YML-Framing Protocol

1. White space is ignored.
2. Messages are separated by '\n---\n'.
3. The end of the session is marked by '\n...\n'
4. The end of a message is marked by '---' or '...'.
5. If the first line of the data is <illegal utf-8 sequence/BOM-sequence>, this is an escape for proprietary extensions.
6. All text is UTF-8.
7. UTF-8 checking is done upon interpretation; so we only check it when its part of an output string.
8. Max line length?
9. It is recommended that the client first send a '---' line. Upon receipt of a starting line, the server should acknowledge it.
10. The end of a message is indicated by '---'.
11. Extraneous '---' are ignored. Empty documents delimited by '---' are ignored.
12. The end of the session is indicated by an empty document terminated with '...'.
13. Metadata yaml tag?  Handle first lines that begin with a '#!' or '%' or '!'

YML Syntax
----------
Comments are not supported? # comment
Tags are ignored?
% lines are ignored?


Binary-Length Framing (necessary?)
---------------------

--- \0 <B> <B> <B> <B> \n

Text Data

--- \1 <B> <B> <B> <B> \n

Binary Data (Alternate Format)


The choice of framing sent in reply is determined by the first framed message. If you start out by using binary-length framing, you will receive responses uses binary-length framing.

Perhaps the binary framing should be simpler and should be a setting that can be changed with a protocol message? LIBOFP_EDIT_SETTING.

Multiple Connection Mode
========================

Multiple connections run over zeromq sessions. Clients must first subscribe to asynchronous OpenFlow events that they want to receive.

Clients may send messages into the broker, and libofp will service them.

You need to tell the pubsub center to tag certain events with a prefix.

event: LIBOFP.PUBLISH_REQUEST
params:
  xid: 0
  name: ofp.packet_in.lldp
  priority: 100
  continue: false   # alternate name 'stop' or 'break' ?
  type: PACKET_IN
  filter:
    datapath_id: '^.*$'
    reasons: []
    cookie: 
    cookie_mask:
    table_id: 
    match: []

event: LIBOFP.PUBLISH_REPLY
params:
  xid: 0
  error: ''



event: LIBOFP.UNPUBLISH_REQUEST
params:
  xid: 0
  name: lldp

event: LIBOFP.UNPUBLISH_REPLY
params:
  xid: 0
  error: ''



event: LIBOFP.PUBLISH_REQUEST
params:
  xid: 0
  name: ofp.packet_in
  type: PACKET_IN
  priority: 0
  filter:
    reasons: ALL
    cookie_mask: 0xFFFFFFFF
    table_id: ALL
    match: []


Suppose we receive a TCP packet to a port that we want to process. We insert our
hook using publish_request with a high priority so the TCP packet is tagged with 
our special tag (mytcp). Suppose we decide we want to continue processing it using default
ofp.packet_in tags. We push it back into the server with the associated tag. The server 
begins the search using that tag to publish the packet using the next packet_in tag.

The republish event uses a custom zeromq routing header.

Call the tag 'mark' and the republish event 'resume'?
Call the tag 'topic' ?


Marks that begin with a '.' are reserved for internal use. They will designate an internal app. Marks that begin with "ofp." are global. 

Perhaps all "internal" pre-defined marks should begin with '.' ?
