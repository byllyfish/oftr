#include "ofp/unittest.h"
#include "ofp/ipv4address.h"

using namespace ofp;

TEST(ipv4address, test) 
{
	IPv4Address addr{"192.168.1.1"};

	EXPECT_TRUE(addr.valid());
	EXPECT_EQ("192.168.1.1", addr.toString());
}

TEST(ipv4address, mask) 
{
	IPv4Address a = IPv4Address::mask(0);
	EXPECT_EQ("0.0.0.0", a.toString());

	IPv4Address b = IPv4Address::mask(1);
	EXPECT_EQ("128.0.0.0", b.toString());

	IPv4Address c = IPv4Address::mask(7);
	EXPECT_EQ("254.0.0.0", c.toString());

	IPv4Address d = IPv4Address::mask(8);
	EXPECT_EQ("255.0.0.0", d.toString());

	IPv4Address e = IPv4Address::mask(9);
	EXPECT_EQ("255.128.0.0", e.toString());

	IPv4Address f = IPv4Address::mask(23);
	EXPECT_EQ("255.255.254.0", f.toString());

	IPv4Address g = IPv4Address::mask(24);
	EXPECT_EQ("255.255.255.0", g.toString());

	IPv4Address h = IPv4Address::mask(25);
	EXPECT_EQ("255.255.255.128", h.toString());

	IPv4Address i = IPv4Address::mask(31);
	EXPECT_EQ("255.255.255.254", i.toString());

	IPv4Address j = IPv4Address::mask(32);
	EXPECT_EQ("255.255.255.255", j.toString());

	EXPECT_FALSE(i.isBroadcast());
	EXPECT_TRUE(j.isBroadcast());
}


TEST(ipv4address, prefix) 
{
	for (unsigned i = 0; i < 33; ++i) {
		IPv4Address addr = IPv4Address::mask(i);
		EXPECT_EQ(i, addr.prefix());
	}
}
