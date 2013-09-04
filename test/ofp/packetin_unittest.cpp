#include "ofp/unittest.h"
#include "ofp/packetin.h"

using namespace ofp;

TEST(packetin, version4)
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

    MemoryChannel channel{OFP_VERSION_4};
    UInt32 xid = msg.send(&channel);

    EXPECT_EQ(1, xid);
    EXPECT_EQ(58, channel.size());

    const char *expected = "(04-0A-003A-00000001), "
                           "(00000001-0002-03-04,0000000000000005), "
                           "(00010008-80000004,0000001B|00000000), "
                           "(00000102-03040506,07080102-03040506,0708)";

    EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(packetin, version1)
{
    PacketInBuilder builder;
    builder.setBufferID(0x01020304);
    builder.setTotalLen(0x0101);
    builder.setInPort(0x22);
    builder.setInPhyPort(0x0333);
    builder.setReason(0x09);
    builder.setTableID(0xAA);
    builder.setCookie(0x05060708090a0b0c);

    ByteRange pkt("77777777777777777777", 20);
    builder.setEnetFrame(pkt);

    MemoryChannel channel{OFP_VERSION_1};
    builder.send(&channel);

    EXPECT_EQ(0x0026, channel.size());

    const char *expected = "(01-0A-0026-00000001)"
                           "01020304-0101-0022-09-00"
                           "3737373737373737373737373737373737373737";

    EXPECT_HEX(expected, channel.data(), channel.size());

    // Now check parsing.

    Message message{channel.data(), channel.size()};
    EXPECT_EQ(OFP_VERSION_1, message.version());
    EXPECT_EQ(PacketIn::Type, message.type());

    const PacketIn *msg = PacketIn::cast(&message);
    EXPECT_TRUE(msg);

    if (msg) {
        EXPECT_EQ(OFP_VERSION_1, msg->version());
        EXPECT_EQ(0x01020304, msg->bufferID());
        EXPECT_EQ(0x0101, msg->totalLen());
        EXPECT_EQ(0x22, msg->inPort());
        EXPECT_EQ(0x09, msg->reason());

        ByteRange enetFrame = msg->enetFrame();
        EXPECT_HEX("3737373737373737373737373737373737373737", enetFrame.data(), enetFrame.size());

        // Not supported in v1.
        EXPECT_EQ(0, msg->inPhyPort());
        EXPECT_EQ(0, msg->tableID());
        EXPECT_EQ(0, msg->cookie());
    }
}
