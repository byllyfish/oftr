Specifications
==============

Here are the detailed specifications for LibOFP.

Supports OpenFlow 1.x
---------------------
  - Supports specifications 1.0, 1.1, 1.2 and 1.3 (wire formats 0x01, 0x02, 0x03, 0x04).
  - Programming interface based on OpenFlow 1.3.2 specification.
  - Inversion of control event-driven listener interface for C++ clients.
  - TCP-based YAML interface for text-based clients.
  - Supports TCP and UDP over IPv4 and IPv6.
  - Auto-detects the OpenFlow protocol.
  - Handles multiple simultaneous connections and auxiliary connections.
  - Juggles different OpenFlow versions at the same time.
  - Transparently handles protocol version differences for the API client.
  - Automatically handles OpenFlow match field dependencies. 
  - Supports custom OpenFlow match fields by editing a tab-delimited file and recompiling.
  - Runs as an OpenFlow agent or controller, or both at the same time.

Implemented in C++11
----------------------
  - Relies on concrete classes and templates; minimal use of inheritance.
  - No use of exceptions (except for boost.asio) or RTTI.
  - Modern use of namespaces.
  - Most code relies on widely implemented C++11 interfaces only.
  - Uses Boost ASIO for event-driven network engine.
  - Compiles with "every" compiler warning enabled. (Except for a few.)
  - Simple logging support for observing and debugging programs.
  - Asserts.
  - Unit tests.

Portable 64 Bit Clean Code
--------------------------
  - Compiles with Clang or GCC.
  - Runs on Mac OS X or Linux with 32 or 64 bit pointers.
  - Transparently handles endian differences between platforms.
  - Uses memcpy for unaligned word memory access.

Scalable Thread-Aware Design
----------------------------
  - Single-threaded driver.
  - Event-Driven I/O supports many connections simultaneously within a single thread.
  - Library is designed to be integrated into larger multi-threaded programs.
  - No global variables except for a pluggable log output stream. Provides immutable variants of immutable classes.
  - No internal use of mutexes or threads. Clients that use threads must protect their mutable shared data.
  - Future versions may incorporate threads inside the driver, based on performance measurements and use cases.

  