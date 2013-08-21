#include "ofp/unittest.h"
#include "ofp/yaml/flowmod.h"
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

	ByteList buf = MockChannel::serialize(flowMod, 4);
	
	log::debug("size:", buf.size());
	log::debug(RawDataToHex(buf.data(), buf.size()));

	const FlowMod *msg = reinterpret_cast<const FlowMod *>(buf.data());
	EXPECT_TRUE(msg->validateLength(buf.size()));
	
	auto result = yaml::write(msg);

	const char *expected = \
R"""(---
type:            14
version:         4
xid:             1
msg:             
  cookie:          0
  cookie_mask:     0
  table_id:        0
  command:         0
  idle_timeout:    0
  hard_timeout:    0
  priority:        0
  buffer_id:       0
  out_port:        0
  out_group:       0
  flags:           0
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

	ByteList buf2 = MockChannel::serialize(builder, 4);
	
	log::debug("size:", buf2.size());
	log::debug(RawDataToHex(buf2.data(), buf2.size()));
}
