#include "ofp/unittest.h"
#include "ofp/sys/boost_asio.h"
#include "ofp/ipv6address.h"

using namespace ofp;
using namespace ofp::sys;


TEST(boost_asio, tcp) 
{
	auto endpt = makeTCPEndpoint(IPv6Address{"192.168.1.1"}, 80);

	log::debug("endpt", endpt.address().to_string());

	IPv6Address addr = makeIPv6Address(endpt.address());
	UInt16 port = endpt.port();

	EXPECT_EQ("192.168.1.1", addr.toString());
	EXPECT_EQ(80, port);
}


TEST(boost_asio, udp) 
{
	auto endpt = makeUDPEndpoint(IPv6Address{"192.168.1.1"}, 80);

	IPv6Address addr = makeIPv6Address(endpt.address());
	UInt16 port = endpt.port();

	EXPECT_EQ("192.168.1.1", addr.toString());
	EXPECT_EQ(80, port);
}