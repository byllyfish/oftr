// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/mpflowmonitorrequest.h"

using namespace ofp;

TEST(mpflowmonitorrequest, emptyMatch) {
  MPFlowMonitorRequestBuilder msg;

  msg.setMonitorId(0x11111111);
  msg.setOutPort(0x22222222);
  msg.setOutGroup(0x33333333);
  msg.setFlags(0x4444);
  msg.setTableId(0x55);
  msg.setCommand(0x66);

  MemoryChannel channel{OFP_VERSION_5};
  msg.write(&channel);

  EXPECT_EQ(24, channel.size());
  EXPECT_HEX("111111112222222233333333444455660001000400000000", channel.data(),
             channel.size());
}

TEST(mpflowmonitorrequest, withMatch) {
  MPFlowMonitorRequestBuilder msg;

  msg.setMonitorId(0x11111111);
  msg.setOutPort(0x22222222);
  msg.setOutGroup(0x33333333);
  msg.setFlags(0x4444);
  msg.setTableId(0x55);
  msg.setCommand(0x66);

  MatchBuilder match;
  match.add(OFB_ETH_DST{EnetAddress{"AA:11:22:33:44:55"}});
  msg.setMatch(match);

  MemoryChannel channel{OFP_VERSION_5};
  msg.write(&channel);

  EXPECT_EQ(32, channel.size());
  EXPECT_HEX("111111112222222233333333444455660001000E80000606AA11223344550000",
             channel.data(), channel.size());
}
