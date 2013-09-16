C++ Tutorial
============

LibOFP is designed to be static-linked into your C++ program. There are several 
C++ examples in the /example directory that show how to do this.

  `nulldemo` contains sample code for a minimal agent and controller. Neither
  one does anything.

  `controller` contains a simple layer 2 OpenFlow controller named `simple`.

nulldemo
--------

Change directory into the /nulldemo directory and type `make`.

To run an agent which only accepts connections, type:

	./nullagent

To run an agent and connect to a remote controller, type:

	./nullagent 192.168.56.101

To run an agent and restrict it to a specific OpenFlow version 1-4, type:

	./nullagent 192.168.56.101 4

To run a controller which only accepts connections, type:

    ./nullcontroller

To run a controller and connect to a remote agent, type:

    ./nullcontroller 192.168.56.101


controller
----------

Change directory into the /controller directory and type `make`.

To run the controller, type:

	./simple

Point an OpenFlow switch at your IP address. With MiniNet, use the command:

	sudo mn --controller=remote,ip=192.168.56.1,port=6633

After MiniNet starts, you can type `h1 ping h2` or `pingall`.

To test OpenFlow 1.3, upgrade openvswitch using the directions here (*), and then
type:

	sudo mn --controller=remote,ip=192.168.56.1,port=6633 --switch ovsk,protocols=OpenFlow13

(*) https://wiki.opendaylight.org/view/Openflow_Protocol_Library:OpenVirtualSwitch

	