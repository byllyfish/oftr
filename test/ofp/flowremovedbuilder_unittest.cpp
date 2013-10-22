#include "ofp/unittest.h"
#include "ofp/flowremoved.h"

using namespace ofp;

TEST(flowremovedbuilder, v1_3) 
{
	MatchBuilder match;
    match.add(OFB_IN_PORT{27});

	FlowRemovedBuilder flowRemoved;

	flowRemoved.setCookie(0x1111111111111111);
	flowRemoved.setPriority(0x2222);
	flowRemoved.setReason(0x33);
	flowRemoved.setTableId(0x44);
	flowRemoved.setDurationSec(0x55555555);
	flowRemoved.setDurationNSec(0x66666666);
	flowRemoved.setIdleTimeout(0x7777);
	flowRemoved.setHardTimeout(0x8888);
	flowRemoved.setPacketCount(0x9999999999999999);
	flowRemoved.setByteCount(0xAAAAAAAAAAAAAAAA);
	flowRemoved.setMatch(match);

	MemoryChannel channel{0x04};
    UInt32 xid = flowRemoved.send(&channel);

    EXPECT_EQ(1, xid);
    EXPECT_EQ(0x03C, channel.size());

    const char *expected = "040B003C000000011111111111111111222233445555555566666666777788889999999999999999AAAAAAAAAAAAAAAA0001000C800000040000001B";
    EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowremovedbuilder, v1_0)
{
	MatchBuilder match;
    match.add(OFB_IN_PORT{27});

	FlowRemovedBuilder flowRemoved;

	flowRemoved.setCookie(0x1111111111111111);
	flowRemoved.setPriority(0x2222);
	flowRemoved.setReason(0x33);
	flowRemoved.setTableId(0x44);
	flowRemoved.setDurationSec(0x55555555);
	flowRemoved.setDurationNSec(0x66666666);
	flowRemoved.setIdleTimeout(0x7777);
	flowRemoved.setHardTimeout(0x8888);
	flowRemoved.setPacketCount(0x9999999999999999);
	flowRemoved.setByteCount(0xAAAAAAAAAAAAAAAA);
	flowRemoved.setMatch(match);

	MemoryChannel channel{0x01};
    UInt32 xid = flowRemoved.send(&channel);

    EXPECT_EQ(1, xid);
    EXPECT_EQ(88, channel.size());

    const char *expected = "010B005800000001003FFFFE001B000000000000000000000000000000000000000000000000000000000000000000001111111111111111222233005555555566666666777700009999999999999999AAAAAAAAAAAAAAAA";
    EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowremovedbuilder, v1_1)
{
	MatchBuilder match;
    match.add(OFB_IN_PORT{27});

	FlowRemovedBuilder flowRemoved;

	flowRemoved.setCookie(0x1111111111111111);
	flowRemoved.setPriority(0x2222);
	flowRemoved.setReason(0x33);
	flowRemoved.setTableId(0x44);
	flowRemoved.setDurationSec(0x55555555);
	flowRemoved.setDurationNSec(0x66666666);
	flowRemoved.setIdleTimeout(0x7777);
	flowRemoved.setHardTimeout(0x8888);
	flowRemoved.setPacketCount(0x9999999999999999);
	flowRemoved.setByteCount(0xAAAAAAAAAAAAAAAA);
	flowRemoved.setMatch(match);

	MemoryChannel channel{0x02};
    UInt32 xid = flowRemoved.send(&channel);

    EXPECT_EQ(1, xid);
    EXPECT_EQ(136, channel.size());

    const char *expected = "020B0088000000011111111111111111222233445555555566666666777700009999999999999999AAAAAAAAAAAAAAAA000000580000001B000003FE00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowremovedbuilder, v1_2)
{
	MatchBuilder match;
    match.add(OFB_IN_PORT{27});

	FlowRemovedBuilder flowRemoved;

	flowRemoved.setCookie(0x1111111111111111);
	flowRemoved.setPriority(0x2222);
	flowRemoved.setReason(0x33);
	flowRemoved.setTableId(0x44);
	flowRemoved.setDurationSec(0x55555555);
	flowRemoved.setDurationNSec(0x66666666);
	flowRemoved.setIdleTimeout(0x7777);
	flowRemoved.setHardTimeout(0x8888);
	flowRemoved.setPacketCount(0x9999999999999999);
	flowRemoved.setByteCount(0xAAAAAAAAAAAAAAAA);
	flowRemoved.setMatch(match);

	MemoryChannel channel{0x03};
    UInt32 xid = flowRemoved.send(&channel);

    EXPECT_EQ(1, xid);
    EXPECT_EQ(0x3C, channel.size());

    const char *expected = "030B003C000000011111111111111111222233445555555566666666777788889999999999999999AAAAAAAAAAAAAAAA0001000C800000040000001B";
    EXPECT_HEX(expected, channel.data(), channel.size());
}

