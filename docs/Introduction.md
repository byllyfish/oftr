LibOFP Version 0.1
==================

LibOFP is a library that implements the OpenFlow protocol. It handles the 
wire protocol and connection details for controller and agent applications. A 
client can program to the latest version of the OpenFlow spec; backward 
compatibility is handled automatically.

LibOFP is written in C++11. The library provides an efficient C++ API for 
OpenFlow agents and controllers. 

LibOFP also provides a YAML-based interface for controllers. YAML is a human-readable 
data serialization format designed for interacting with high level languages. 
YAML is specified at http://yaml.org. By using YAML, LibOFP can be 
accessed by any language with a YAML library: Python, Perl, PHP, Java, 
JavaScript, and more.

LibOFP is extensible. You can add support for new match fields 
(a.k.a. oxm types) by editing a tab-delimited file and re-compiling. 

LibOFP is portable. The majority of the code uses only standard C++ libraries. 
The networking code is implemented using Boost.ASIO, but Boost API's are limited
to a small subset of the code. The networking sub-system can be replaced if 
needed without making other code changes. The YAML serialization code is 
optional.

Status
------

LibOFP provides an interface that uses the OpenFlow 1.3 specification. 
The library supports multiple TCP connections and auxiliary connections over TCP.

LibOFP also supports previous protocol versions; the library translates messages
to prior OpenFlow versions as needed. 

LibOFP currently supports the OpenFlow 1.3.2 specification with these omissions:

- TLS and DTLS are not implemented yet. { Work-around: Use openssl or netcat to proxy. }
- A few OpenFlow protocol messages are incomplete, missing unit tests and 
  generally untested. This is especially true for OpenFlow versions 1.1 and 1.2.
- UDP auxiliary connections are implemented, but not presently working.

Software License
----------------

Copyright 2013 William W. Fisher

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


Dependencies & Third Party Code
-------------------------------

The LibOFP software contains code written by third parties. Such software will
have its own individual LICENSE.txt file that describes the copyrights, license
and restrictions which apply to that code.

- Boost.ASIO 1.54
  License: Boost Software License - Version 1.0
  Location: external/boost_1_54_0_asio/LICENSE.txt
  Source: http://www.boost.org/libs/asio/

- LLVM Project's YAML-IO
  License: University of Illinois Open Source License
  Location: external/yaml-io/LICENSE.txt
  Source: http://llvm.org/docs/YamlIO.html

- Google Test (for unit tests)
  License: Google Open Source License
  Location: external/googletest/LICENSE
  Source: https://code.google.com/p/googletest/


Build
-----

The software requires the following tools:

	- Make (GNUMake specifically)
	- C++11 Compiler. The following are known to work:
	   - Clang (LLVM-3.2svn or later) on Mac OS X 10.8
	   - GCC 4.7 on Ubuntu 12.10

Files produced:

    - libofp.a (library)
    - libofpexec (executable)

To build:

	make

To run test cases:

	make test

To produce a 32-bit build on a 64-bit system:

	CXXFLAGS=-m32 make

To produce a debug build:

    CXXFLAGS=-g make

To clean up:

    make clean


Instant Tutorial
----------------

`libofpexec` is the name of the program that provides a YAML interface to 
clients that connect over TCP. At this time, it only allows one client (of the 
API) to connect at a time. However, that client sees all connecting OpenFlow 
datapaths.

To run libofpexec:

    ./libofpexec

It doesn't daemonize, so it will remain in the foreground and write log output 
to stderr. Open another terminal and type:

    telnet localhost 9191

Type (or paste) the following into Telnet. Both case and indentation are important.

---
  event: LIBOFP_LISTEN_REQUEST
  msg:
    port: 6633
...

You should receive a reply that looks like this:

  ---
  event:           LIBOFP_LISTEN_REPLY
  msg:             
    port:            6633
    error:           
  ...

The `error:` value should be empty. However, you may get an error if you specify
a privileged port number or the TCP port is already in use.

libofpexec is now listening on port 6633 for OpenFlow connections as a 
controller. Run MiniNet and start up a simple switched network, substituting 
your own IP address:

	sudo mn --controller=remote,ip=192.168.56.1,port=6633 --mac

You should receive the following output on your Telnet terminal:

	---
	event:           LIBOFP_DATAPATH_UP
	msg:             
	  datapath_id:     0000-0000-0000-0001
	  version:         1
	  n_buffers:       256
	  n_tables:        255
	  capabilities:    199
	  reserved:        4095
	...

An OpenFlow switch has opened a connection. The switch is identified by its 
datapath_id. In this case, that's `0000-0000-0000-0001`. The OpenFlow connection
will remain open and be kept alive automatically.

The LIBOFP_DATAPATH_UP will be followed shortly by some OFPT_PACKET_IN messages.

	---
	type:            OFPT_PACKET_IN
	xid:             0x00000000
	version:         1
	datapath_id:     0000-0000-0000-0001
	auxiliary_id:    0
	msg:             
	  buffer_id:       256
	  total_len:       90
	  in_port:         2
	  in_phy_port:     0
	  metadata:        0
	  reason:          OFPR_NO_MATCH
	  table_id:        0
	  cookie:          0
	  enet_frame:      33330000001600000000000286DD6000000000240001000{...}
	...

The enet_frame contains the beginning of the packet in hexadecimal. Let's tell 
configure how much packet we want. Type the following into Telnet:

---
  type: OFPT_SET_CONFIG
  datapath_id: 0000-0000-0000-0001
  msg:
    flags: 0
    miss_send_len: 14
...

There will be no reply to this command. We just told the switch to only send us 
the first 14 bytes of the ethernet frame. The first 14 bytes contain the source 
and destination ethernet addresses and the ethernet type.

You can go back to mininet and run a ping to see more packets:

    mininet>  h1 ping h2

This concludes the Instant Tutorial. To stop libofpexec, type control-C.

Congratulations! You're an OpenFlow Controller! 

Where To Go Next
----------------

To run a program that uses the YAML API, check out the `Python.Tutorial.md`.

To see the C++ API in action, check out the `C++.Tutorial.md`.

Feedback
--------

No software product is successful without the input of others. I greatly value 
your feedback. If you find this software useful, appealing, stupid, crappy, 
incomplete, or downright buggy, please let me know:

    william.w.fisher+libofp@gmail.com

I may not always reply immediately, but I will always listen. Thanks.

References
----------

  - http://archive.openflow.org/documents/openflow-spec-v1.0.0.pdf
  - http://archive.openflow.org/documents/openflow-spec-v1.1.0.pdf
  - https://www.opennetworking.org/images/stories/downloads/sdn-resources/onf-specifications/openflow/openflow-spec-v1.2.pdf
  - https://www.opennetworking.org/images/stories/downloads/sdn-resources/onf-specifications/openflow/openflow-spec-v1.3.0.pdf
  - http://www.boost.org/libs/asio/
  - http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3388.pdf
  - http://blip.tv/boostcon/thinking-asynchronously-designing-applications-with-boost-asio-5250947
  - http://llvm.org/docs/YamlIO.html
  - https://code.google.com/p/googletest/

  Tells how to upgrade MiniNet to openvswitch 1.3!

  - https://wiki.opendaylight.org/view/Openflow_Protocol_Library:OpenVirtualSwitch




