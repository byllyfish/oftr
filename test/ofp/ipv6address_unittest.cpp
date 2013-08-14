#include "ofp/unittest.h"
#include "ofp/ipv6address.h"

using namespace ofp;

TEST(ipv6address, test) 
{
	IPv6Address addr{"::1"};

	EXPECT_TRUE(addr.valid());
	EXPECT_EQ("::1", addr.toString());
}
