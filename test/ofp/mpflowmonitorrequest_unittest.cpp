// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/mpflowmonitorrequest.h"

using namespace ofp;

const OFPFlowMonitorFlags kFakeFlags = static_cast<OFPFlowMonitorFlags>(0x4444);
const OFPFlowMonitorCommand kFakeCommand =
    static_cast<OFPFlowMonitorCommand>(0x66);

TEST(mpflowmonitorrequest, emptyMatch) {
  MPFlowMonitorRequestBuilder msg;

  msg.setMonitorId(0x11111111);
  msg.setOutPort(0x22222222);
  msg.setOutGroup(0x33333333);
  msg.setFlags(kFakeFlags);
  msg.setTableId(0x55);
  msg.setCommand(kFakeCommand);

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
  msg.setFlags(kFakeFlags);
  msg.setTableId(0x55);
  msg.setCommand(kFakeCommand);

  MatchBuilder match;
  match.add(OFB_ETH_DST{MacAddress{"AA:11:22:33:44:55"}});
  msg.setMatch(match);

  MemoryChannel channel{OFP_VERSION_5};
  msg.write(&channel);

  EXPECT_EQ(32, channel.size());
  EXPECT_HEX("111111112222222233333333444455660001000E80000606AA11223344550000",
             channel.data(), channel.size());
}
