#include "ofp/unittest.h"
#include "ofp/mpflowmonitorreply.h"

using namespace ofp;

TEST(mpflowmonitorreply, test) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{7});

  ActionList actions;
  actions.add(AT_OUTPUT{99, 128});

  InstructionList instructions;
  instructions.add(IT_APPLY_ACTIONS{&actions});

  MPFlowMonitorReplyBuilder msg;
  msg.setEvent(0);
  msg.setTableId(0x11);
  msg.setReason(0x22);
  msg.setIdleTimeout(0x3333);
  msg.setHardTimeout(0x4444);
  msg.setPriority(0x5555);
  msg.setCookie(0x6666666666666666);
  msg.setMatch(match);
  msg.setInstructions(instructions);

  MemoryChannel channel{OFP_VERSION_5};
  msg.write(&channel);

  EXPECT_EQ(64, channel.size());
  EXPECT_HEX("0040000011223333444455550000000066666666666666660001000C800000040000000700000000000400180000000000000010000000630080000000000000", channel.data(), channel.size());
}