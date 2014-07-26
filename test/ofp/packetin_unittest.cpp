#include "ofp/unittest.h"
#include "ofp/packetin.h"

using namespace ofp;

TEST(packetin, version4) {
  auto s = HexToRawData("0102 0304 0506 0708 0102 0304 0506 0708");
  ByteList enetFrame{s.data(), s.size()};

  PacketInBuilder msg;
  msg.setBufferId(1);
  msg.setTotalLen(2);
  msg.setReason(OFPR_NO_MATCH);
  msg.setTableID(4);
  msg.setCookie(5);
  msg.setInPort(27);
  msg.setEnetFrame(enetFrame);

  MemoryChannel channel{OFP_VERSION_4};
  UInt32 xid = msg.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x3A, channel.size());

  const char *expected = "040A003A00000001000000010002000400000000000000050001000C800000040000001B00000000000001020304050607080102030405060708";

  EXPECT_HEX(expected, channel.data(), channel.size());

  {
    Message message{channel.data(), channel.size()};
    message.transmogrify();

    auto packetIn = PacketIn::cast(&message);

    Validation context{message.data(), message.size()};
    ASSERT_TRUE(packetIn->validateInput(&context));

    if (packetIn->validateInput(&context)) {
      EXPECT_EQ(1, packetIn->bufferId());
      EXPECT_EQ(2, packetIn->totalLen());
      EXPECT_EQ(OFPR_NO_MATCH, packetIn->reason());
      EXPECT_EQ(27, packetIn->inPort());
    }
  }
}

TEST(packetin, version1) {
  PacketInBuilder builder;
  builder.setBufferId(0x01020304);
  builder.setTotalLen(0x0101);
  builder.setInPort(0x2222);
  builder.setInPhyPort(0x3333);
  builder.setReason(OFPR_NO_MATCH);
  builder.setTableID(0xAA);
  builder.setCookie(0x05060708090a0b0c);

  ByteList pkt("77777777777777777777", 20);
  builder.setEnetFrame(pkt);

  MemoryChannel channel{OFP_VERSION_1};
  builder.send(&channel);

  EXPECT_EQ(0x0026, channel.size());

  const char *expected = "(01-0A-0026-00000001)"
                         "01020304-0101-2222-00-00"
                         "3737373737373737373737373737373737373737";

  EXPECT_HEX(expected, channel.data(), channel.size());

  // Now check parsing.

  Message message{channel.data(), channel.size()};
  EXPECT_EQ(OFP_VERSION_1, message.version());
  EXPECT_EQ(PacketIn::type(), message.type());

  const PacketIn *msg = PacketIn::cast(&message);
  EXPECT_TRUE(msg);

  if (msg) {
    EXPECT_EQ(OFP_VERSION_1, msg->version());
    EXPECT_EQ(0x01020304, msg->bufferId());
    EXPECT_EQ(0x0101, msg->totalLen());
    EXPECT_EQ(0x2222, msg->inPort());
    EXPECT_EQ(OFPR_NO_MATCH, msg->reason());

    ByteRange enetFrame = msg->enetFrame();
    EXPECT_HEX("3737373737373737373737373737373737373737", enetFrame.data(),
               enetFrame.size());

    // Not supported in v1.
    EXPECT_EQ(0, msg->inPhyPort());
    EXPECT_EQ(0, msg->tableID());
    EXPECT_EQ(0, msg->cookie());
  }
}
