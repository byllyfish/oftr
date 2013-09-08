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
    EXPECT_HEX("040E0058000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000010016800000040000000D80000A02080080001804C0A801010000000000000001000803000000", channel.data(), channel.size());

    const FlowMod *msg = reinterpret_cast<const FlowMod *>(channel.data());
    EXPECT_TRUE(msg->validateLength(channel.size()));

    auto result = yaml::write(msg);

    const char *expected =
        R"""(---
type:            OFPT_FLOW_MOD
version:         4
xid:             1
msg:             
  cookie:          0x0000000000000000
  cookie_mask:     0x0000000000000000
  table_id:        0
  command:         0
  idle_timeout:    0x0000
  hard_timeout:    0x0000
  priority:        0x0000
  buffer_id:       0x00000000
  out_port:        0x00000000
  out_group:       0x00000000
  flags:           0x0000
  match:           
    - type:            OFB_IN_PORT
      value:           13
    - type:            OFB_ETH_TYPE
      value:           2048
    - type:            OFB_IPV4_DST
      value:           192.168.1.1
...
)""";

    EXPECT_EQ(expected, result);

    FlowModBuilder builder;
    yaml::read(result, &builder);

    MemoryChannel channel2{OFP_VERSION_4};
    builder.send(&channel2);

    log::debug("size:", channel2.size());
    log::debug(RawDataToHex(channel2.data(), channel2.size()));
}
