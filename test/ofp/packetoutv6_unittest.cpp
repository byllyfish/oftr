// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/packetoutv6.h"
#include "ofp/actionlist.h"
#include "ofp/actions.h"
#include "ofp/constants.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(packetoutv6, v6) {
  PacketOutV6Builder msg;

  ActionList actions;
  actions.add(AT_OUTPUT{0x77777771, 0x2221});
  actions.add(AT_GROUP{0x88888881});

  auto s = HexToRawData("0102 0304 0506 0708 0102 0304 0506 0708");
  ByteList enetFrame{s.data(), s.size()};

  MatchBuilder match;
  match.add(OFB_IN_PORT{0x44444441});

  msg.setBufferId(0x33333331);
  msg.setMatch(match);
  msg.setActions(actions);
  msg.setEnetFrame(enetFrame);

  MemoryChannel channel{OFP_VERSION_6};
  UInt32 xid = msg.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x048, channel.size());
  EXPECT_HEX(
      "060D00480000000133333331001800000001000C80000004444444410000000000000010"
      "77777771222100000000000000160008888888810102030405060708010203040506070"
      "8",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.normalize();

  auto packetout = PacketOutV6::cast(&message);
  ASSERT_TRUE(packetout != nullptr);

  EXPECT_EQ(0x33333331, packetout->bufferId());
  EXPECT_EQ(actions.toRange(), packetout->actions());
  EXPECT_EQ(enetFrame.toRange(), packetout->enetFrame());
}

TEST(packetoutv6, minimal) {
  PacketOutV6Builder msg;
  msg.setBufferId(0x33333331);

  MemoryChannel channel{OFP_VERSION_6};
  UInt32 xid = msg.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x18, channel.size());
  EXPECT_HEX("060D00180000000133333331000000000001000400000000", channel.data(),
             channel.size());

  Message message{channel.data(), channel.size()};
  message.normalize();

  auto packetout = PacketOutV6::cast(&message);
  ASSERT_TRUE(packetout != nullptr);

  EXPECT_EQ(0x33333331, packetout->bufferId());
  EXPECT_EQ(ActionRange{}, packetout->actions());
  EXPECT_EQ(ByteRange{}, packetout->enetFrame());
}
