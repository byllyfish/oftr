=================================
OFTR: OpenFlow to YAML Translator
=================================

|TRAVIS|  |COVERITY|  |CODECOV|  |LAUNCHPAD|

-------------
What is oftr?
-------------

The *oftr* program translates binary OpenFlow_ messages to YAML_ and back again. It provides a uniform syntax for working with the OpenFlow protocol programmatically.

- Supports Linux and OS X
- Supports OpenFlow versions 1.0, 1.1, 1.2, 1.3, 1.4, 1.5.
- Optional JSON RPC Microservice supports:
    - multiple incoming or outgoing connections
    - auxiliary connections
    - TLS and DTLS

Decode the OpenFlow messages in a ".pcap" file while retaining metadata such as timestamps:

::

    $ oftr decode "filename.pcap"


oftr includes a JSON-RPC microservice that can operate as an OpenFlow 
controller, agent, or both simultaneously. The JSON-RPC microservice can control
the OpenFlow protocol over an IPC channel using asynchronous JSON events.

::

    $ oftr jsonrpc


----------------
YAML to OpenFlow
----------------

The following YAML input will produce an OpenFlow 1.3 message:

::

    type: PACKET_OUT
    version: 4    # OpenFlow 1.3
    msg:
      buffer_id: NO_BUFFER
      in_port: CONTROLLER    # Use strings for special values and flags
      actions: 
        - action: OUTPUT
          port: 0x11    # YAML supports hexadecimal numbers
          max_len: MAX
      data: 'ffffffffffff0000000000010800'

The YAML syntax is based on OpenFlow 1.4, but the same input
can generate earlier protocol version messages. oftr is intelligent about
translating between OpenFlows versions. For example,
here's a FlowMod message specified using OpenFlow 1.4 syntax, but which will 
output a "1.0" message because version 1 is explicitly specified.

::

    type: FLOW_MOD
    version: 1    # OpenFlow 1.0
    msg:
      cookie: 0
      cookie_mask: 0xFFFFFFFFFFFFFFFF   # Ignored; not present in OpenFlow 1.0
      table_id: 0    # Ignored; not present in OpenFlow 1.0
      command: ADD
      idle_timeout: 10
      hard_timeout: 30
      priority: 2
      buffer_id: NO_BUFFER
      out_port: ANY
      out_group: ANY
      flags: [ SEND_FLOW_REM, CHECK_OVERLAP ]   # Flags as array of strings
      match:
        - field: IN_PORT
          value: 1
        - field: IPV4_SRC     # Match field dependencies added automatically
          value: '192.168.1.1'
      instructions:
        - instruction: APPLY_ACTIONS
          actions:
            - action: OUTPUT
              port: 2
              max_len: MAX

----------------
OpenFlow to YAML
----------------

Given a binary OpenFlow message, `oftr` can translate it back to a human-readable
YAML object. Here's the binary for an OpenFlow OFPT_ERROR message in hexadecimal:

::

    01010013 00000062 00010001 FFFF1234 567890

Decoding this binary OpenFlow message yields:

::

  type:            ERROR
  xid:             0x00000062
  version:         0x01
  msg:             
    type:            OFPET_BAD_REQUEST
    code:            OFPBRC_BAD_TYPE
    data:            FFFF1234567890


------------
Installation
------------

Before you build, you need a C++11 compiler, cmake and git. You will also need 
Python, Perl and Go.

On Ubuntu, use apt-get to install the dependencies.

::

    $ sudo apt-get -yq update
    $ sudo apt-get install build-essential cmake git golang libpcap-dev

On Mac OS X, you can obtain cmake from http://cmake.org/download/.

To build the software:

::

    $ git clone --recursive https://github.com/byllyfish/oftr.git
    $ mkdir oftr/Build
    $ cd oftr/Build
    $ cmake ..
    $ make
    $ make test
    $ make install

On Ubuntu, you can also install the software from Launchpad_.

--------
Examples
--------

To translate a text file into binary OpenFlow messages:

::

    $ oftr encode -o output.bin input.yml


To translate a text file into binary OpenFlow messages, while ignoring
sets of match fields with missing prerequisites:

::

    $ oftr encode --unchecked-match -o output.bin input.yml


To translate a text file of compact, single line JSON objects separated by
linefeeds:

::

    $ oftr encode --json input.json


To decode a file of binary OpenFlow messages to YAML:

::

    $ oftr decode input.bin


To decode a ".pcap" file containing OpenFlow messages, treating all TCP streams
as potential OpenFlow connections:

::

    $ oftr decode --pcap-filter='' "input.pcap" 


To decode a file of binary OpenFlow messages to compact single-line JSON objects:

::

    $ oftr decode --json "filename"


To decode binary OpenFlow messages to a valid JSON array of objects:

::

    $ oftr decode --json-array "filename"


To decode binary OpenFlow messages with the additional check that the resulting
YAML produces the same binary message when re-encoded:

::

    $ oftr decode --verify-output "filename"


The `oftr help` tool provides information about the schema used for reading/writing 
OpenFlow messages in YAML. To obtain information about the syntax for the 
'FLOW_MOD' message:

::

    $ oftr help flow_mod


To list all supported OXM fields:

::

    $ oftr help --fields


See the man pages for details:  `oftr.1`_, `oftr-jsonrpc.1`_, `oftr-schema.1`_

-------
License
-------

This software is licensed under the terms of the *MIT License*.

*oftr* relies on the following third-party source code (static linked):

- yamlio_ from the llvm_ project
- asio_ (forked to support boringssl)
- boringssl_

For unit tests, *oftr* uses googletest:

- googletest_

For testing, *oftr* uses OpenFlow message samples from these projects:

- `openflow-messages`_
- ryu_



.. |TRAVIS| image:: https://travis-ci.org/byllyfish/oftr.svg?branch=develop
   :alt: Travis-CI Build Status
   :target: https://travis-ci.org/byllyfish/oftr

.. |COVERITY| image:: https://scan.coverity.com/projects/5587/badge.svg
   :alt: Coverity Scan Build Status
   :target: https://scan.coverity.com/projects/5587

.. |CODECOV| image:: https://codecov.io/github/byllyfish/oftr/coverage.svg?branch=codecov
   :alt: Code Coverage Status
   :target: https://codecov.io/gh/byllyfish/oftr/branch/codecov

.. |LAUNCHPAD| image:: https://img.shields.io/badge/Launchpad-ppa-f7c426.svg
    :alt: Launchpad Logo
    :target: Launchpad_

.. _Launchpad: https://launchpad.net/~byllyfish/+archive/ubuntu/oftr

.. _OpenFlow: https://www.opennetworking.org/sdn-resources/openflow

.. _YAML: http://www.yaml.org/spec/1.2/spec.html

.. _oftr.1 : https://github.com/byllyfish/oftr/blob/master/docs/oftr.1.adoc

.. _oftr-jsonrpc.1 : https://github.com/byllyfish/oftr/blob/master/docs/oftr-jsonrpc.1.adoc

.. _oftr-schema.1 : https://github.com/byllyfish/oftr/blob/master/docs/oftr-schema.1.adoc

.. _yamlio: http://llvm.org/docs/YamlIO.html

.. _llvm: http://llvm.org

.. _asio: https://github.com/byllyfish/asio.git

.. _boringssl: https://boringssl.googlesource.com/boringssl

.. _googletest: https://github.com/google/googletest.git

.. _openflow-messages : https://github.com/flowgrammable/openflow-messages

.. _ryu: https://github.com/osrg/ryu

