// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpflowstatsreply.h"

#include "ofp/actions.h"
#include "ofp/unittest.h"

using namespace ofp;

const OFPFlowModFlags kFakeFlags = static_cast<OFPFlowModFlags>(0x7777);

TEST(mpflowstatsreply, test1_v4) {
  MPFlowStatsReplyBuilder reply;

  reply.setTableId(0x11);
  reply.setDuration({0x22222222, 0x33333333});
  reply.setPriority(0x4444);
  reply.setIdleTimeout(0x5555);
  reply.setHardTimeout(0x6666);
  reply.setFlags(kFakeFlags);
  reply.setCookie(0x8888888888888888);
  reply.setPacketCount(0x9999999999999999);
  reply.setByteCount(0xAAAAAAAAAAAAAAAA);

  MemoryChannel channel{OFP_VERSION_4};
  reply.write(&channel);

  EXPECT_EQ(0x38, channel.size());
  EXPECT_HEX(
      "00381100222222223333333344445555666677770000000088888888888888889"
      "999999999999999AAAAAAAAAAAAAAAA0001000400000000",
      channel.data(), channel.size());
}

TEST(mpflowstatsreply, test1_v1) {
  MPFlowStatsReplyBuilder reply;

  reply.setTableId(0x11);
  reply.setDuration({0x22222222, 0x33333333});
  reply.setPriority(0x4444);
  reply.setIdleTimeout(0x5555);
  reply.setHardTimeout(0x6666);
  reply.setFlags(kFakeFlags);
  reply.setCookie(0x8888888888888888);
  reply.setPacketCount(0x9999999999999999);
  reply.setByteCount(0xAAAAAAAAAAAAAAAA);

  MemoryChannel channel{OFP_VERSION_1};
  reply.write(&channel);

  EXPECT_EQ(0x58, channel.size());
  EXPECT_HEX(
      "00581100003820FF0000000000000000000000000000000000000000000000000"
      "00000000000000000000000222222223333333344445555666600000000000088"
      "888888888888889999999999999999AAAAAAAAAAAAAAAA",
      channel.data(), channel.size());
}

TEST(mpflowstatsreply, test2_v4) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{7});

  ActionList actions;
  actions.add(AT_OUTPUT{99, 128});

  InstructionList instructions;
  instructions.add(IT_APPLY_ACTIONS{&actions});

  MPFlowStatsReplyBuilder reply;
  reply.setTableId(0x11);
  reply.setDuration({0x22222222, 0x33333333});
  reply.setPriority(0x4444);
  reply.setIdleTimeout(0x5555);
  reply.setHardTimeout(0x6666);
  reply.setFlags(kFakeFlags);
  reply.setCookie(0x8888888888888888);
  reply.setPacketCount(0x9999999999999999);
  reply.setByteCount(0xAAAAAAAAAAAAAAAA);
  reply.setMatch(match);
  reply.setInstructions(instructions);

  MemoryChannel channel{OFP_VERSION_4};
  reply.write(&channel);

  EXPECT_EQ(0x58, channel.size());
  EXPECT_HEX(
      "00581100222222223333333344445555666677770000000088888888888888889"
      "999999999999999AAAAAAAAAAAAAAAA0001000C800000040000000700000000000400180"
      "0"
      "00000000000010000000630080000000000000",
      channel.data(), channel.size());
}

TEST(mpflowstatsreply, test2_v1) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{7});

  ActionList actions;
  actions.add(AT_OUTPUT{99, 128});

  InstructionList instructions;
  instructions.add(IT_APPLY_ACTIONS{&actions});

  MPFlowStatsReplyBuilder reply;
  reply.setTableId(0x11);
  reply.setDuration({0x22222222, 0x33333333});
  reply.setPriority(0x4444);
  reply.setIdleTimeout(0x5555);
  reply.setHardTimeout(0x6666);
  reply.setFlags(kFakeFlags);
  reply.setCookie(0x8888888888888888);
  reply.setPacketCount(0x9999999999999999);
  reply.setByteCount(0xAAAAAAAAAAAAAAAA);
  reply.setMatch(match);
  reply.setInstructions(instructions);

  MemoryChannel channel{OFP_VERSION_1};
  reply.write(&channel);

  EXPECT_EQ(0x60, channel.size());
  EXPECT_HEX(
      "00601100003820FE0007000000000000000000000000000000000000000000000"
      "00000000000000000000000222222223333333344445555666600000000000088"
      "888888888888889999999999999999AAAAAAAAAAAAAAAA0000000800630080",
      channel.data(), channel.size());
}
