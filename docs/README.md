net-ofp
=======
Author: Bill Fisher <william.w.fisher@gmail.com>


net-ofp is a flexible and portable open source protocol driver that implements OpenFlow 1.3.2 and earlier versions. net-ofp is written in C++11 using a loosely-coupled event-driven network engine (Boost.ASIO).

net-ofp-c is a C library built on top of net-ofp. net-ofp-c provides a stable high-level API that uses JSON strings to represent OpenFlow Protocol messages. It is designed for use by programs written in other languages such as Python and Ruby, which support a foreign function API like `ctypes` or `FFI`.

Dependencies
------------

  - GNU Make
  - Modern C++11 Compiler (Clang or GCC)
  - External
    - Latest Boost.ASIO 1.54 (included) { Boost Open Source License }
    - Latest Google Test (included; required for unit tests only)
    - YAML IO (llvm.org)

Specs
-----

- Supports all versions of OpenFlow.
  - Supports specifications 1.0, 1.1, 1.2 and 1.3 (wire formats 0x01, 0x02, 0x03, 0x04).
  - Client interface uses OpenFlow Spec 1.3.2.
  - Inversion of Control Pub/Sub listener interface for clients.
  - Supports TCP and UDP over IPv4 and IPv6.
  - Optionally supports TLS over TCP.
  - Auto-detects the OpenFlow protocol.
  - Handles multiple simultaneous connections and auxiliary connections.
  - Juggles different OpenFlow versions at the same time.
  - Transparently handles protocol version differences for the API client.
  - Automatically handles OpenFlow match field dependencies. 
  - Supports custom OpenFlow match fields by editing a tab-delimited file and recompiling.
  - Runs as an OpenFlow agent or controller, or both at the same time.

- Implemented in C++11.
  - Relies on concrete classes and templates; minimal use of inheritance.
  - No use of exceptions or  RTTI (except for boost.asio).
  - Modern use of namespaces.
  - Uses Boost ASIO for event-driven network engine. (The boost api's are quarantined to the network engine only; the network engine is designed to be replaceable.)

- Portable 64 Bit Clean Code
  - Compiles with Clang or GCC.
  - Runs on Mac OS X or Linux with 32 or 64 bit pointers.
  - Transparently handles endian differences between platforms.
  - Uses memcpy for unaligned word memory access.
  - The code will probably run on Windows with minimal effort.

- Scalable Thread-Aware Design
  - Event-Driven I/O can support many connections simultaneously within a single thread.
  - Library is designed to be integrated into larger multi-threaded programs using an Actor-based framework.
  - No global variables except for a pluggable log output stream. Provides immutable variants of immutable classes.
  - No internal use of mutexes or threads. Clients that use threads must protect their mutable shared data.
  - Future versions will support a Driver option to inject a thread pool/policy from the hosting application. This would allow the network engine to use multiple threads to service its connections, while assigning "related" connections to the same thread.

- Maintainable & Debuggable
  - Provides pluggable logging support for observing and debugging programs.
  - Asserts.
  - Unit tests.


net-ofp-c
Design
======



ByteOrder
Protocol Versions
IPv6
TLS



DTLS is not supported


ofp/
  
  

  impl/
    Engine
    TCP_Server
    TCP_Connection
    UDP_Server
    UDP_Connection


References

http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3388.pdf
http://llvm.org/docs/YamlIO.html
http://blip.tv/boostcon/thinking-asynchronously-designing-applications-with-boost-asio-5250947

Tells how to upgrade MiniNet to openvswitch 1.3!

https://wiki.opendaylight.org/view/Openflow_Protocol_Library:OpenVirtualSwitch
