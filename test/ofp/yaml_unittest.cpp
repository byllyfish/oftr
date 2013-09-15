#include "ofp/unittest.h"
#include "ofp/yaml.h"
#include "ofp/log.h"
#include "ofp/flowmod.h"
#include "ofp/writable.h"

using namespace ofp;

TEST(yaml_flowmod, write)
{
    log::set(&std::cerr);

    MatchBuilder match;
    match.add(OFB_IN_PORT{13});
    match.add(OFB_IPV4_DST{IPv4Address{"192.168.1.1"}});

    InstructionList instructions;
    instructions.add(IT_GOTO_TABLE{3});

    FlowModBuilder flowMod;
    flowMod.setMatch(match);
    flowMod.setInstructions(instructions);

    MemoryChannel channel{OFP_VERSION_4};
    flowMod.send(&channel);

    EXPECT_EQ(88, channel.size());
    EXPECT_HEX("040E005800000001000000000000000000000000000000000000000000000000000000000000000000000000000000000001001A800000040000000D80000A02080080001804C0A801010000000000000001000803000000", channel.data(), channel.size());

    const FlowMod *msg = reinterpret_cast<const FlowMod *>(channel.data());
    EXPECT_TRUE(msg->validateLength(channel.size()));
    if (!msg->validateLength(channel.size()))
        return;

    auto result = yaml::write(msg);

    const char *expected =
        "---\ntype:            OFPT_FLOW_MOD\nversion:         4\nxid:             1\nmsg:             \n  cookie:          0x0000000000000000\n  cookie_mask:     0x0000000000000000\n  table_id:        0\n  command:         0\n  idle_timeout:    0x0000\n  hard_timeout:    0x0000\n  priority:        0x0000\n  buffer_id:       0x00000000\n  out_port:        0x00000000\n  out_group:       0x00000000\n  flags:           0x0000\n  match:           \n    - type:            OFB_IN_PORT\n      value:           13\n    - type:            OFB_ETH_TYPE\n      value:           2048\n    - type:            OFB_IPV4_DST\n      value:           192.168.1.1\n  instructions:    \n    - type:            OFPIT_GOTO_TABLE\n      value:           \n        table_id:        3\n...\n";

    EXPECT_EQ(expected, result);

    FlowModBuilder builder;
    yaml::read(result, &builder);

    MemoryChannel channel2{OFP_VERSION_4};
    builder.send(&channel2);

    log::debug("size:", channel2.size());
    log::debug(RawDataToHex(channel2.data(), channel2.size()));
}


TEST(yaml_unittest, escape_chars)
{
    const char *input = "---\ntype: OFPT_FLOW_MOD\x08\x08\n...\n";

    FlowModBuilder builder;
    yaml::read(input, &builder);
}


