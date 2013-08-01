#include <gtest/gtest.h>
#include "ofp/oxmfields.h"

using namespace ofp;


TEST(oxmfields, OFB_TCP_DST)
{
	OFB_TCP_DST tcpdst{80};
	
	EXPECT_EQ(80, tcpdst);
	EXPECT_EQ(0x80001c02, OFB_TCP_DST::type().oxmNative());
	EXPECT_EQ(16, OFB_TCP_DST::bits());
	EXPECT_FALSE(OFB_TCP_DST::maskSupported());
		
	for (auto x : *OFB_TCP_DST::prerequisites()) {
		EXPECT_EQ(x.type(), OFB_IP_PROTO::type());
	}
}
