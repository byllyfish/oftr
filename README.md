Libofp - JSON/YAML interface to OpenFlow Protocol
=================================================

`libofp` implements a JSON-RPC microservice that can operate as an OpenFlow 
controller, agent, or both simultaneously. The JSON-RPC microservice can control
the OpenFlow protocol over an IPC channel using asynchronous JSON events.

`libofp` also includes tools that translate between binary OpenFlow messages
and JSON or YAML.

Features
--------

- Supports asynchronous JSON-RPC events over stdin/stdout.
- Uniform JSON interface to OpenFlow Protocol versions 1.0 - 1.4
- Supports OpenFlow connections over TCP, UDP, TLS and DTLS
- Supports auxiliary OpenFlow connections

License
-------

- MIT License

Build (Linux/Mininet VM)
------------------------

Before you build, you need a C++11 compiler, cmake and git.

  ```bash
  sudo apt-get -yq update
  sudo apt-get install build-essential cmake git
  ```

To build the software:

  ```bash
  git clone -b develop --recursive https://github.com/byllyfish/libofp.git
  mkdir libofp/Build
  cd libofp/Build
  cmake ..
  make
  make test
  make install
  ```

Running (Linux/Mininet VM)
-------------------------

To Do...


