#include <ofp/unittest.h>
#include "ofp/flowmod.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructions.h"
#include "ofp/bytelist.h"

using namespace ofp;

// Put this here ... to get the vtable generated once.
UInt32 MockChannel::nextXid()
{
    return ++nextXid_;
}

TEST(flowmodbuilder, version1_3)
{
    MatchBuilder match;
    match.add(OFB_IN_PORT{27});

    InstructionList instructions;
    instructions.add(IT_GOTO_TABLE{3});

    FlowModBuilder flowMod;
    flowMod.setMatch(match);
    flowMod.setInstructions(instructions);

    MockChannel channel{0x04};
    UInt32 xid = flowMod.send(&channel);

    EXPECT_EQ(1, xid);
    EXPECT_EQ(72, channel.size());

    const char *expected = "040E-0048-0000-0001-0000-0000-0000-0000"
                           "0000-0000-0000-0000-0000-0000-0000-0000"
                           "0000-0000-0000-0000-0000-0000-0000-0000"
                           "0001-0008-8000-0004-0000-001B-0000-0000"
                           "0001-0008-0300-0000";
    EXPECT_HEX(expected, channel.data(), channel.size());
}

TEST(flowmodbuilder, version1_0)
{
    MatchBuilder match;
    match.add(OFB_IN_PORT{27});

    InstructionList instructions;
    instructions.add(IT_GOTO_TABLE{3});

    FlowModBuilder flowMod;
    flowMod.setMatch(match);
    flowMod.setInstructions(instructions);

    MockChannel channel{0x01};
    UInt32 xid = flowMod.send(&channel);

    EXPECT_EQ(1, xid);
    EXPECT_EQ(144, channel.size());

    const char *expected = "010E-0090-0000-0001-0000-0000-0000-0000"
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
