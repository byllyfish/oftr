#include <gtest/gtest.h>
#include "ofp/openflow_match.h"
#include "openflow_test.h"

using namespace ofp;
using namespace spec;

TEST(openflow_match, ofp_match_header)
{
	ofp_match_header match{};
	EXPECT_EQ(0, std::memcmp(&match, "\0\0\0\0\0\0\0\0", sizeof(match)));
	
	match.type = OFPMT_STANDARD;
	match.length = 0x1234;
	
	ofp_match_header expected;
	HexToRawData("0000 1234 0000 0000", &expected, sizeof(expected));
	EXPECT_EQ(expected, match);
	
	EXPECT_EQ(OFPMT_STANDARD, match.type);
	EXPECT_EQ(0x1234, match.length);
}
