#include "ofp/unittest.h"
#include "ofp/oxmvalue.h"

using namespace ofp;

constexpr OXMInternalID cast(int n) { return static_cast<OXMInternalID>(n); }

using ofb_in_port = OXMValue<cast(0), 0x8000, 0, Big16, 2, false>;
using ofb_vlan_vid = OXMValue<cast(1), 0x8000, 6, Big16, 2, true>;
using ofb_tcp_src_port = OXMValue<cast(2), 0x8000, 19, Big16, 2, false>;


TEST(OXMValue, ofb_in_port)
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
	EXPECT_EQ(2, sizeof(ofb_in_port));
	EXPECT_FALSE(ofb_in_port::maskSupported());
}

TEST(OXMValue, ofb_vlan_vid)
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
	EXPECT_TRUE(ofb_vlan_vid::maskSupported());
}

TEST(OXMValue, use_in_switch_stmt)
{
	bool found = false;
	auto type = OXMType{0x8000, 6, 2};
	
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


TEST(OXMValue, construct_from_memory)
{
	const UInt8 raw[] = { 0x00, 0x65 };
	
	ofb_tcp_src_port srcPort = ofb_tcp_src_port::fromBytes(raw);
	EXPECT_EQ(101, srcPort);
}
