#include "ofp/unittest.h"
#include "ofp/flowstatsreply.h"
#include "ofp/actions.h"

using namespace ofp;

TEST(flowstatsreply, test1_v4) {
  FlowStatsReplyBuilder reply;

  reply.setTableId(0x11);
  reply.setDurationSec(0x22222222);
  reply.setDurationNSec(0x33333333);
  reply.setPriority(0x4444);
  reply.setIdleTimeout(0x5555);
  reply.setHardTimeout(0x6666);
  reply.setFlags(0x7777);
  reply.setCookie(0x8888888888888888);
  reply.setPacketCount(0x9999999999999999);
  reply.setByteCount(0xAAAAAAAAAAAAAAAA);

  MemoryChannel channel{OFP_VERSION_4};
  reply.write(&channel);

  EXPECT_EQ(0x34, channel.size());
  EXPECT_HEX("00341100222222223333333344445555666677770000000088888888888888889"
             "999999999999999AAAAAAAAAAAAAAAA00010004",
             channel.data(), channel.size());
}

TEST(flowstatsreply, test1_v1) {
  FlowStatsReplyBuilder reply;

  reply.setTableId(0x11);
  reply.setDurationSec(0x22222222);
  reply.setDurationNSec(0x33333333);
  reply.setPriority(0x4444);
  reply.setIdleTimeout(0x5555);
  reply.setHardTimeout(0x6666);
  reply.setFlags(0x7777);
  reply.setCookie(0x8888888888888888);
  reply.setPacketCount(0x9999999999999999);
  reply.setByteCount(0xAAAAAAAAAAAAAAAA);

  MemoryChannel channel{OFP_VERSION_1};
  reply.write(&channel);

  EXPECT_EQ(0x58, channel.size());
  EXPECT_HEX("00581100003FFFFF0000000000000000000000000000000000000000000000000"
             "00000000000000000000000222222223333333344445555666600000000000088"
             "888888888888889999999999999999AAAAAAAAAAAAAAAA",
             channel.data(), channel.size());
}

TEST(flowstatsreply, test2_v4) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{7});

  ActionList actions;
  actions.add(AT_OUTPUT{99, 128});

  InstructionList instructions;
  instructions.add(IT_APPLY_ACTIONS{&actions});

  FlowStatsReplyBuilder reply;
  reply.setTableId(0x11);
  reply.setDurationSec(0x22222222);
  reply.setDurationNSec(0x33333333);
  reply.setPriority(0x4444);
  reply.setIdleTimeout(0x5555);
  reply.setHardTimeout(0x6666);
  reply.setFlags(0x7777);
  reply.setCookie(0x8888888888888888);
  reply.setPacketCount(0x9999999999999999);
  reply.setByteCount(0xAAAAAAAAAAAAAAAA);
  reply.setMatch(match);
  reply.setInstructions(instructions);

  MemoryChannel channel{OFP_VERSION_4};
  reply.write(&channel);

  EXPECT_EQ(0x54, channel.size());
  EXPECT_HEX("00541100222222223333333344445555666677770000000088888888888888889"
             "999999999999999AAAAAAAAAAAAAAAA0001000C80000004000000070004001800"
             "00000000000010000000630080000000000000",
             channel.data(), channel.size());
}

TEST(flowstatsreply, test2_v1) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{7});

  ActionList actions;
  actions.add(AT_OUTPUT{99, 128});

  InstructionList instructions;
  instructions.add(IT_APPLY_ACTIONS{&actions});

  FlowStatsReplyBuilder reply;
  reply.setTableId(0x11);
  reply.setDurationSec(0x22222222);
  reply.setDurationNSec(0x33333333);
  reply.setPriority(0x4444);
  reply.setIdleTimeout(0x5555);
  reply.setHardTimeout(0x6666);
  reply.setFlags(0x7777);
  reply.setCookie(0x8888888888888888);
  reply.setPacketCount(0x9999999999999999);
  reply.setByteCount(0xAAAAAAAAAAAAAAAA);
  reply.setMatch(match);
  reply.setInstructions(instructions);

  MemoryChannel channel{OFP_VERSION_1};
  reply.write(&channel);

  EXPECT_EQ(0x60, channel.size());
  EXPECT_HEX("00601100003FFFFE0007000000000000000000000000000000000000000000000"
             "00000000000000000000000222222223333333344445555666600000000000088"
             "888888888888889999999999999999AAAAAAAAAAAAAAAA0000000800630080",
             channel.data(), channel.size());
}
