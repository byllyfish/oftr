#include <gtest/gtest.h>
#include "ofp/flowmatchbuilder.h"
#include "ofp/oxmfields.h"

using namespace ofp;

#include <iostream>


TEST(flowmatchbuilder, OFB_IN_PORT)
{
	FlowMatchBuilder b;
	b.add(OFB_IN_PORT{0xDEADBEEF});
	
	// No prerequisites.
	auto expected = HexToRawData("80000004DEADBEEF");
	EXPECT_EQ(expected.size(), b.size());
	EXPECT_EQ(0, std::memcmp(expected.data(), b.data(), b.size()));
}


TEST(flowmatchbuilder, OFB_IN_PHY_PORT)
{
	FlowMatchBuilder b;
	b.add(OFB_IN_PHY_PORT{1});
	
	// N.B. the mask for prerequisite OFB_IN_PORT is doubled up.
	auto expected = HexToRawData("8000010800000000000000008000010800000000000000008000020400000001");
	EXPECT_EQ(expected.size(), b.size());
	EXPECT_EQ(0, std::memcmp(expected.data(), b.data(), b.size()));
}

