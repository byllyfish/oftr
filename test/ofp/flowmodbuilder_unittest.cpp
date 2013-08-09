#include <ofp/unittest.h>
#include "ofp/flowmodbuilder.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructions.h"
#include "ofp/bytelist.h"

using namespace ofp;

class MockChannel {
public:
    MockChannel(UInt8 version) : version_{version}
    {
    }

    const UInt8 *data() const
    {
        return buf_.data();
    }

    size_t size() const
    {
        return buf_.size();
    }

    UInt8 version() const
    {
        return version_;
    }

    void write(const void *data, size_t length)
    {
        buf_.add(data, length);
    }

    UInt32 nextXid()
    {
        return ++nextXid_;
    }

private:
    ByteList buf_;
    UInt32 nextXid_ = 0;
    UInt8 version_;
};

TEST(flowmodbuilder, version1_3)
{
    MatchBuilder match;
    match.add(OFB_IN_PORT{27});

    InstructionSet instructions;
    instructions.add(IT_GOTO_TABLE{3});

    FlowModBuilder flowMod;
    flowMod.setMatch(match);
    flowMod.setInstructions(instructions);

    MockChannel channel{0x04};
    UInt32 xid = flowMod.write(&channel);

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

    InstructionSet instructions;
    instructions.add(IT_GOTO_TABLE{3});

    FlowModBuilder flowMod;
    flowMod.setMatch(match);
    flowMod.setInstructions(instructions);

    MockChannel channel{0x01};
    UInt32 xid = flowMod.write(&channel);

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
