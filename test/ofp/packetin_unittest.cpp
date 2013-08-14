#include "ofp/unittest.h"
#include "ofp/packetin.h"

using namespace ofp;

TEST(packetin, test)
{
    MatchBuilder match;
    match.add(OFB_IN_PORT{27});

    auto s = HexToRawData("0102 0304 0506 0708 0102 0304 0506 0708");
    ByteRange enetFrame{s.data(), s.size()};

    PacketInBuilder msg;
    msg.setBufferID(1);
    msg.setTotalLen(2);
    msg.setReason(3);
    msg.setTableID(4);
    msg.setCookie(5);

    msg.setMatch(match);
    msg.setEnetFrame(enetFrame);

    MockChannel channel{0x04};
    UInt32 xid = msg.send(&channel);

    EXPECT_EQ(1, xid);
    EXPECT_EQ(58, channel.size());

    const char *expected = "(04-0A-003A-00000001), "
                           "(00000001-0002-03-04,0000000000000005), "
                           "(00010008-80000004,0000001B|00000000), "
                           "(00000102-03040506,07080102-03040506,0708)";

    EXPECT_HEX(expected, channel.data(), channel.size());
}
