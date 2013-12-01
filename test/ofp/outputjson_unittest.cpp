#include "ofp/unittest.h"
#include "ofp/yaml/outputjson.h"
#include "ofp/yaml/decoder.h"
#include "ofp/hello.h"
#include "ofp/flowmod.h"

using namespace ofp;
using namespace yaml;

TEST(outputjson, hello) {
  MemoryChannel channel;
  HelloBuilder builder;
  builder.send(&channel);

  Message msg{channel.data(), channel.size()};
  Decoder decode{&msg, true};

  const char *expected = "---\n{\"type\":\"OFPT_HELLO\",\"xid\":1,\"version\":"
                         "4,\"msg\":{\"versions\":[1,2,3,4]}}\n...\n";
  EXPECT_EQ(expected, decode.result());
}

TEST(outputjson, flowmod) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  InstructionList instructions;
  instructions.add(IT_GOTO_TABLE{3});

  FlowModBuilder flowMod;
  flowMod.setMatch(match);
  flowMod.setInstructions(instructions);

  MemoryChannel channel;
  flowMod.send(&channel);

  Message msg{channel.data(), channel.size()};
  Decoder decodeJson{&msg, true};

  const char *expected =
      "---\n{\"type\":\"OFPT_FLOW_MOD\",\"xid\":1,\"version\":4,\"msg\":{"
      "\"cookie\":0,\"cookie_mask\":0,\"table_id\":0,\"command\":0,\"idle_"
      "timeout\":0,\"hard_timeout\":0,\"priority\":0,\"buffer_id\":0,\"out_"
      "port\":0,\"out_group\":0,\"flags\":0,\"match\":[{\"type\":\"OFB_IN_"
      "PORT\",\"value\":27}],\"instructions\":[{\"type\":\"OFPIT_GOTO_TABLE\","
      "\"value\":{\"table_id\":3}}]}}\n...\n";
  EXPECT_EQ(expected, decodeJson.result());

  // Decoder decodeYaml{&msg};
  // std::cout << decodeYaml.result();
}