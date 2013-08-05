#include "ofp/unittest.h"
#include "ofp/standardmatch.h"

using namespace ofp;
using namespace deprecated;

TEST(standardmatch, test) 
{
	StandardMatch match{};

	EXPECT_EQ(OFPMT_STANDARD_LENGTH, sizeof(match));
	EXPECT_EQ(OFPMT_STANDARD, match.type);
	EXPECT_EQ(OFPMT_STANDARD_LENGTH, match.length);
	EXPECT_EQ(0, match.in_port);

	EXPECT_TRUE(IsMemFilled(&match.dl_src, sizeof(match) - 12, '\0'));

	UInt32 wc = OFPFW_ALL;
	match.in_port = 5;
	wc &= ~OFPFW_IN_PORT;
	match.wildcards = wc;

	OXMList list = match.toOXMList();

	EXPECT_HEX("8000000400000005", list.data(), list.size());
}
