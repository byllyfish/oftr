============
Installation
============


------
Ubuntu
------

There are builds for xenial and trusty in the Launchpad PPA.

.. code-block:: bash
    
    $ sudo apt-add-repository ppa:byllyfish/oftr
    $ sudo apt-get update
    $ sudo apt-get install oftr

---------------
Raspbian/Jessie
---------------

Specify the "trusty" build for Rasbian/Jessie. There is an armhf trusty build in the PPA for the Raspberry Pi.

.. code-block:: bash

    $ sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys FFBE06FBB4EDC679174DD2CCD5AA2ED445128570
    $ sudo sh -c 'echo "deb http://ppa.launchpad.net/byllyfish/oftr/ubuntu trusty main" >> /etc/apt/sources.list'
    $ sudo apt-get update
    $ sudo apt-get install oftr

-------------------
Mac OS X / Homebrew
-------------------

You can install using Homebrew on Mac OS X.

.. code-block:: bash

    $ brew tap byllyfish/oftr
    $ brew install oftr

-----------------
Build from Source
-----------------

Before you build, you need a C++11 compiler, cmake and git. You will also need 
Python, Perl and Go. (Perl and Go are required to build BoringSSL.)

On Ubuntu, use apt-get to install the dependencies.

.. code-block:: bash

    $ sudo apt-get -yq update
    $ sudo apt-get install build-essential cmake git golang libpcap-dev

On Mac OS X, you can obtain cmake from http://cmake.org/download/.

To build the software:

.. code-block:: bash

    $ git clone --recursive https://github.com/byllyfish/oftr.git
    $ mkdir oftr/Build
    $ cd oftr/Build
    $ cmake ..
    $ make
    $ make test
    $ make install    

-------------------------
Build without SSL Support
-------------------------

You can build oftr without SSL support. Replace the cmake line in the above 
instructions with the following.

.. code-block:: bash

    $ cmake -DLIBOFP_ENABLE_OPENSSL=false ..
