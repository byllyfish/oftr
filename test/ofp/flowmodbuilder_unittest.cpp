//  ===== ---- ofp/flowmodbuilder_unittest.cpp -------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements unit tests for FlowModBuilder class.
//  ===== ------------------------------------------------------------ =====  //

#include <ofp/unittest.h>
#include "ofp/flowmod.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructions.h"
#include "ofp/bytelist.h"
#include "ofp/actions.h"

using namespace ofp;

TEST(flowmodbuilder, version1_3) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  InstructionList instructions;
  instructions.add(IT_GOTO_TABLE{3});

  FlowModBuilder flowMod;
  flowMod.setMatch(match);
  flowMod.setInstructions(instructions);

  MemoryChannel channel{0x04};
  UInt32 xid = flowMod.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x048, channel.size());

  const char *expected = "040E-0048-0000-0001-0000-0000-0000-0000"
                         "0000-0000-0000-0000-0000-0000-0000-0000"
                         "0000-0000-0000-0000-0000-0000-0000-0000"
                         "0001-000C-8000-0004-0000-001B-0000-0000"
                         "0001-0008-0300-0000";
  EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowmodbuilder, version1_1) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  InstructionList instructions;
  instructions.add(IT_GOTO_TABLE{3});

  FlowModBuilder flowMod;
  flowMod.setMatch(match);
  flowMod.setInstructions(instructions);

  MemoryChannel channel{0x02};
  UInt32 xid = flowMod.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x090, channel.size());

  const char *expected = "020E-0090-0000-0001-0000-0000-0000-0000"
                         "0000-0000-0000-0000-0000-0000-0000-0000"
                         "0000-0000-0000-0000-0000-0000-0000-0000"
                         "0000-0058-0000-001B-0000-03FE-0000-0000"
                         "0000-0000-0000-0000-0000-0000-0000-0000"
                         "0000-0000-0000-0000-0000-0000-0000-0000"
                         "0000-0000-0000-0000-0000-0000-0000-0000"
                         "0000-0000-0000-0000-0000-0000-0000-0000"
                         "0000-0000-0000-0000-0001-0008-0300-0000";
  EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowmodbuilder, version1_0) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  ActionList actions;
  actions.add(AT_OUTPUT{0xaaaa});

  InstructionList instructions;
  instructions.add(IT_APPLY_ACTIONS{&actions});

  FlowModBuilder flowMod;
  flowMod.setMatch(match);
  flowMod.setInstructions(instructions);

  MemoryChannel channel{0x01};
  UInt32 xid = flowMod.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x050, channel.size());

  const char *expected = "010E005000000001003FFFFE001B00000000000000000000000"
                         "000000000000000000000000000000000000000000000000000"
                         "000000000000000000000000000000000000000000"
                         "00000008AAAA0000";

  EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowmodbuilder, fastVersion1) {
  UInt32 inPort = 0x12121212;
  UInt32 outPort = 0x23232323;
  UInt32 bufferId = 0x59595959;
  EnetAddress dst{"ABCDEFABCDEF"};
  EnetAddress src{"123456123456"};

  FlowModBuilder flowMod;

  MatchBuilder &match = flowMod.match();
  match.add(OFB_IN_PORT{inPort});
  match.add(OFB_ETH_DST{dst});
  match.add(OFB_ETH_SRC{src});

  ActionList actions;
  actions.add(AT_OUTPUT{outPort});

  InstructionList &instructions = flowMod.instructions();
  instructions.add(IT_APPLY_ACTIONS{&actions});

  flowMod.setIdleTimeout(10);
  flowMod.setHardTimeout(30);
  flowMod.setPriority(0x8000);
  flowMod.setBufferId(bufferId);

  MemoryChannel channel{0x01};
  flowMod.send(&channel);

  const char *expected = "010E005000000001003FFFF21212123456123456ABCDEFABCDEF0000000000000000000000000000000000000000000000000000000000000000000A001E800059595959000000000000000823230000";

  EXPECT_EQ(0x50, channel.size());
  EXPECT_HEX(expected, channel.data(), channel.size());

  channel.setNextXid(1);
  channel.clear();

  FlowModBuilder::sendFastVersion1(&channel, inPort, outPort, bufferId, dst, src);

  EXPECT_EQ(0x50, channel.size());
  EXPECT_HEX(expected, channel.data(), channel.size());
}
