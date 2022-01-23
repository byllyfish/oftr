// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/flowremovedv6.h"

#include "ofp/statbuilder.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(flowremovedv6, test) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  StatBuilder stat;
  stat.add(OXS_DURATION{DurationSec{0x55555550, 0x66666660}});
  stat.add(OXS_PACKET_COUNT{0x9999999999999990});
  stat.add(OXS_BYTE_COUNT{0xAAAAAAAAAAAAAAA0});

  FlowRemovedV6Builder msg;
  msg.setCookie(0x1111111111111110);
  msg.setPriority(0x2220);
  msg.setReason(static_cast<OFPFlowRemovedReason>(0x30));
  msg.setTableId(0x40);
  msg.setIdleTimeout(0x7770);
  msg.setHardTimeout(0x8880);
  msg.setMatch(match);
  msg.setStat(stat);

  MemoryChannel channel{OFP_VERSION_6};
  UInt32 xid = msg.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x050, channel.size());

  const char *expected =
      "060B005000000001403022207770888011111111111111100001000C800000040000001B"
      "000000000000002880020008555555506666666080020808999999999999999080020A08"
      "AAAAAAAAAAAAAAA0";
  EXPECT_HEX(expected, channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.normalize();

  auto flowremoved = FlowRemovedV6::cast(&message);
  ASSERT_TRUE(flowremoved != nullptr);

  EXPECT_EQ(0x1111111111111110, flowremoved->cookie());
}
