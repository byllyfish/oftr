#include <gtest/gtest.h>
#include "ofp/oxm_value.h"

using namespace ofp;

using ofb_in_port = oxm_value<0x8000, 0, Big16, 16, false>;
using ofb_vlan_vid = oxm_value<0x8000, 6, Big16, 13, true>;

using ofb_tcp_src_port = oxm_value<0x8000, 19, Big16, 16, false>;

//Range<UInt8*> Prereq<ofb_tcp_src_port>();


TEST(oxm_value, ofb_in_port)
{	
	ofb_in_port port{80};
	EXPECT_EQ(2, sizeof(port));
	EXPECT_EQ(0, std::memcmp(&port, "\x00\x50", 2));
	EXPECT_EQ(80, port);
	
	port = 443;
	EXPECT_EQ(0, std::memcmp(&port, "\x01\xbb", 2));
	EXPECT_EQ(443, port);
	
	UInt32 nativeValue = BigEndianFromNative(0x80000002);
	EXPECT_EQ(nativeValue, ofb_in_port::type());
	EXPECT_EQ(16, ofb_in_port::bits());
	EXPECT_FALSE(ofb_in_port::maskSupported());
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
	
	UInt32 nativeValue = BigEndianFromNative(0x80000c02);
	EXPECT_EQ(nativeValue, ofb_vlan_vid::type());
	EXPECT_EQ(13, ofb_vlan_vid::bits());
	EXPECT_TRUE(ofb_vlan_vid::maskSupported());
}

TEST(oxm_value, use_in_switch_stmt)
{
	bool found = false;
	auto type = oxm_type{0x8000, 6, 13};
	
	switch (type) {
		case ofb_in_port::type():
		case ofb_tcp_src_port::type():
			break;
			
		case ofb_vlan_vid::type():
			found = true;
			break;
	}
	
	EXPECT_TRUE(found);
}
