#include <gtest/gtest.h>
#include "ofp/openflow_header.h"
#include "openflow_test.h"


TEST(openflow_header, test_layout)
{
	using namespace ofp;
	using namespace spec;
	
	ofp_header hdr;
	hdr.version = OFP_VERSION_1_0;
	hdr.type = OFPT_PACKET_OUT;
	hdr.length = 0x1122;
	hdr.xid = 0xDEADBEEF;
	
	ofp_header expected;
	sys::HexToRawData("010D 1122 DEAD BEEF", &expected, sizeof(expected));
	EXPECT_EQ(expected, hdr);
	
	EXPECT_EQ(OFP_VERSION_1_0, hdr.version);
	EXPECT_EQ(OFPT_PACKET_OUT, hdr.type);
	EXPECT_EQ(0x1122, hdr.length);
	EXPECT_EQ(0xDEADBEEF, hdr.xid);
}
