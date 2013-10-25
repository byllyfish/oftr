#include "ofp/unittest.h"
#include "ofp/packetout.h"
#include "ofp/actionlist.h"
#include "ofp/actions.h"

using namespace ofp;

TEST(packetout, v4) {
  PacketOutBuilder msg;

  ActionList actions;
  actions.add(AT_OUTPUT{0x77777777, 0x2222});

  auto s = HexToRawData("0102 0304 0506 0708 0102 0304 0506 0708");
  ByteList enetFrame{s.data(), s.size()};

  msg.setBufferId(0x33333333);
  msg.setInPort(0x44444444);
  msg.setActions(actions);
  msg.setEnetFrame(enetFrame);

  MemoryChannel channel{OFP_VERSION_4};
  UInt32 xid = msg.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x0038, channel.size());

  const char *expected = "(04-0D-0038-00000001),"
                         "(33333333-44444444,0010|000000000000),"
                         "(00000010-77777777-2222|000000000000),"
                         "(0102030405060708,0102030405060708)";

  EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(packetout, v1) {
  PacketOutBuilder msg;

  ActionList actions;
  actions.add(AT_OUTPUT{0x7777, 0x2222});

  auto s = HexToRawData("0102 0304 0506 0708 0102 0304 0506 0708");
  ByteList enetFrame{s.data(), s.size()};

  msg.setBufferId(0x33333333);
  msg.setInPort(0x4444);
  msg.setActions(actions);
  msg.setEnetFrame(enetFrame);

  MemoryChannel channel{OFP_VERSION_1};
  UInt32 xid = msg.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x0028, channel.size());

  const char *expected = "(01-0D-0028-00000001),"
                         "(33333333-4444,0008),"
                         "(00000008-7777-2222),"
                         "(0102030405060708,0102030405060708)";

  EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(packetout, v1flood) {
  MemoryChannel channel{OFP_VERSION_1};

  ActionList actions;
  actions.add(AT_OUTPUT{OFPP_FLOOD});

  PacketOutBuilder packetOut;
  packetOut.setBufferId(0x11111111);
  packetOut.setInPort(0x22222222);
  packetOut.setActions(actions);
  packetOut.send(&channel);

  EXPECT_EQ(0x018, channel.size());
  EXPECT_HEX("01-0D-0018-00000001 11111111-2222 0008-00000008-FFFB0000",
             channel.data(), channel.size());
}
