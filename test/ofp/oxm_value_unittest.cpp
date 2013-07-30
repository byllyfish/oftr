#include <gtest/gtest.h>
#include "ofp/oxm_value.h"

using namespace ofp;

using ofb_in_port = oxm_value<0x8000, 0, Big16, 16, false>;
using ofb_vlan_vid = oxm_value<0x8000, 6, Big16, 13, true>;


TEST(oxm_value, ofb_in_port)
{	
	ofb_in_port port{80};
	EXPECT_EQ(2, sizeof(port));
	EXPECT_EQ(0, std::memcmp(&port, "\x00\x50", 2));
	EXPECT_EQ(80, port);
	
	port = 443;
	EXPECT_EQ(0, std::memcmp(&port, "\x01\xbb", 2));
	EXPECT_EQ(443, port);
	
	EXPECT_EQ(0x80000002UL, ofb_in_port::id);
	EXPECT_EQ(16, ofb_in_port::bits);
	EXPECT_FALSE(ofb_in_port::hasMask);
}

TEST(oxm_value, ofb_vlan_vid)
{
	ofb_vlan_vid vid{100};
	EXPECT_EQ(2, sizeof(vid));
	EXPECT_EQ(0, std::memcmp(&vid, "\x00\x64", 2));
	EXPECT_EQ(100, vid);
	
	vid = 900;
	EXPECT_EQ(0, std::memcmp(&vid, "\x03\x84", 2));
	EXPECT_EQ(900, vid);
	
	EXPECT_EQ(0x80000d02UL, ofb_vlan_vid::id);
	EXPECT_EQ(13, ofb_vlan_vid::bits);
	EXPECT_TRUE(ofb_vlan_vid::hasMask);
}
