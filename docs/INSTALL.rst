============
Installation
============


------
Ubuntu
------

.. code-block:: bash
    
    $ sudo apt-add-repository ppa:byllyfish/oftr
    $ sudo apt-get update
    $ sudo apt-get install oftr

---------------
Raspbian/Jessie
---------------

.. code-block:: bash

    $ sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys FFBE06FBB4EDC679174DD2CCD5AA2ED445128570
    $ sudo sh -c 'echo "deb http://ppa.launchpad.net/byllyfish/oftr/ubuntu trusty main" >> /etc/apt/sources.list'


-----------------
Build from Source
-----------------

Before you build, you need a C++11 compiler, cmake and git. You will also need 
Python, Perl and Go.

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
