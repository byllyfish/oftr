Running the Simple Controller with MiniNet Using the Pyretic VM


1. Boot VM and login as MiniNet.
2. Verify that you can reach controller machine. 

   ping 192.168.56.1

  If you have trouble:
     Check networking with ifconfig eth1 and run `sudo dhclient eth1` if necessary.
     Check that firewall is not blocking port 6633.
   


3. Run MiniNet.

  sudo mn --controller=remote,ip=192.168.56.1,port=6633



Runing pox:

cd into the pox directory and type:

  ./pox.py --verbose forwarding.l2_learning

