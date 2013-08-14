#include "ofp/unittest.h"
#include "ofp/packetout.h"
#include "ofp/actionlist.h"
#include "ofp/actions.h"

using namespace ofp;

TEST(packetout, test)
{
    PacketOutBuilder msg;

    ActionList actions;
    actions.add(AT_OUTPUT{1, 2});

    auto s = HexToRawData("0102 0304 0506 0708 0102 0304 0506 0708");
    ByteRange enetFrame{s.data(), s.size()};

    msg.setBufferID(3);
    msg.setInPort(4);
    msg.setActions(actions.toRange());
    msg.setEnetFrame(enetFrame);

    MockChannel channel{0x04};
    UInt32 xid = msg.send(&channel);

    EXPECT_EQ(1, xid);
    EXPECT_EQ(48, channel.size());

    const char *expected = "(04-0D-0030-00000001),"
                           "(00000003-00000004,0008|000000000000),"
                           "(0000-0008-0001-0002),"
                           "(0102030405060708,0102030405060708)";

    EXPECT_HEX(expected, channel.data(), channel.size());
}
