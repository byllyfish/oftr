#include <gtest/gtest.h>
#include "ofp/ipv4address.h"

using namespace ofp;

TEST(ipv4address, test) 
{
	IPv4Address addr{"192.168.1.1"};

	EXPECT_TRUE(addr.valid());
	EXPECT_EQ("192.168.1.1", addr.toString());
}
