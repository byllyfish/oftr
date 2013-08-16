#include "ofp/unittest.h"
#include "ofp/ipv6address.h"

using namespace ofp;

TEST(ipv6address, basic) 
{
	IPv6Address addr{"::1"};

	EXPECT_TRUE(addr.valid());
	EXPECT_EQ("::1", addr.toString());
}


TEST(ipv6address, equals)
{
	IPv6Address x{"192.168.1.1"};
	IPv6Address y{"192.168.1.1"};

	EXPECT_TRUE(x == y);

	IPv6Address z = y;

	EXPECT_TRUE(y == z);
}
