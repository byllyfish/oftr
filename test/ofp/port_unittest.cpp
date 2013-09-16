#include "ofp/unittest.h"
#include "ofp/port.h"

using namespace ofp;

TEST(port, test) 
{
	Port port;

	port.setPortNo(0x11111111);
	port.setHwAddr(EnetAddress{"010203040506"});
	port.setName("Port 1");
	port.setConfig(0x22222222);
	port.setState(0x33333333);
	port.setCurr(0x44444444);
	port.setAdvertised(0x55555555);
	port.setSupported(0x66666666);
	port.setPeer(0x77777777);
	port.setCurrSpeed(0x88888888);
	port.setMaxSpeed(0x99999999);

	EXPECT_HEX("11111111000000000102030405060000506F72742031000000000000000000002222222233333333444444445555555566666666777777778888888899999999", &port, sizeof(port));

	EXPECT_EQ(0x11111111, port.portNo());
	EXPECT_EQ(EnetAddress{"010203040506"}, port.hwAddr());
	EXPECT_TRUE(PortName{"Port 1"} == port.name());
	EXPECT_EQ(0x22222222, port.config());
	EXPECT_EQ(0x33333333, port.state());
	EXPECT_EQ(0x44444444, port.curr());
	EXPECT_EQ(0x55555555, port.advertised());
	EXPECT_EQ(0x66666666, port.supported());
	EXPECT_EQ(0x77777777, port.peer());
	EXPECT_EQ(0x88888888, port.currSpeed());
	EXPECT_EQ(0x99999999, port.maxSpeed());

	deprecated::PortV1 portv1{port};

	EXPECT_HEX("1111010203040506506F7274203100000000000000000000222222223333333344444444555555556666666677777777", &portv1, sizeof(portv1)); 

	EXPECT_EQ(0x1111, portv1.portNo());
	EXPECT_EQ(EnetAddress{"010203040506"}, portv1.hwAddr());
	EXPECT_TRUE(PortName{"Port 1"} == portv1.name());
	EXPECT_EQ(0x22222222, portv1.config());
	EXPECT_EQ(0x33333333, portv1.state());
	EXPECT_EQ(0x44444444, portv1.curr());
	EXPECT_EQ(0x55555555, portv1.advertised());
	EXPECT_EQ(0x66666666, portv1.supported());
	EXPECT_EQ(0x77777777, portv1.peer());

	Port port2{portv1};
	EXPECT_EQ(0x1111, port2.portNo());
	EXPECT_EQ(EnetAddress{"010203040506"}, port2.hwAddr());
	EXPECT_TRUE(PortName{"Port 1"} == port2.name());
	EXPECT_EQ(0x22222222, port2.config());
	EXPECT_EQ(0x33333333, port2.state());
	EXPECT_EQ(0x44444444, port2.curr());
	EXPECT_EQ(0x55555555, port2.advertised());
	EXPECT_EQ(0x66666666, port2.supported());
	EXPECT_EQ(0x77777777, port2.peer());
	EXPECT_EQ(0, port2.currSpeed());
	EXPECT_EQ(0, port2.maxSpeed());	
}
