// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/yaml/outputjson.h"
#include "ofp/yaml/decoder.h"
#include "ofp/hello.h"
#include "ofp/flowmod.h"

using namespace ofp;
using namespace yaml;

OFP_BEGIN_IGNORE_PADDING

struct TestStruct {
  int a;
  std::string b;
  bool c;
  Big32 d;
  double e;
};

OFP_END_IGNORE_PADDING

namespace llvm {  // <namespace llvm>
namespace yaml {  // <namespace yaml>

template <>
struct MappingTraits<TestStruct> {
  static void mapping(llvm::yaml::IO &io, TestStruct &item) {
    io.mapRequired("a", item.a);
    io.mapRequired("b", item.b);
    io.mapRequired("c", item.c);
    io.mapRequired("d", item.d);
    io.mapRequired("e", item.e);
  }
};

}  // </namespace yaml>
}  // </namespace llvm>

TEST(outputjson, hello) {
  MemoryChannel channel;
  HelloBuilder builder;
  builder.send(&channel);

  Message msg{channel.data(), channel.size()};
  Decoder decode{&msg, true};

  const char *expected =
      "{\"type\":\"OFPT_HELLO\",\"xid\":1,\"version\":"
      "4,\"msg\":{\"versions\":[1,2,3,4]}}";
  EXPECT_EQ(expected, decode.result().str());
}

TEST(outputjson, flowmod) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});
  match.add(OFB_TCP_SRC{80});
  match.add(OFB_ETH_TYPE{0x0800});

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
      "{\"type\":\"OFPT_FLOW_MOD\",\"xid\":1,\"version\":4,\"msg\":{"
      "\"cookie\":0,\"cookie_mask\":0,\"table_id\":0,\"command\":\"OFPFC_ADD\","
      "\"idle_"
      "timeout\":0,\"hard_timeout\":0,\"priority\":0,\"buffer_id\":0,\"out_"
      "port\":0,\"out_group\":0,\"flags\":0,\"match\":[{\"field\":\"IN_"
      "PORT\",\"value\":27},{\"field\":\"ETH_TYPE\",\"value\":2048},{"
      "\"field\":\"IP_PROTO\",\"value\":6},{\"field\":\"TCP_SRC\","
      "\"value\":80}],\"instructions\":[{\"instruction\":\"OFPIT_GOTO_TABLE\","
      "\"table_id\":3}]}}";
  EXPECT_EQ(expected, decodeJson.result().str());
}

TEST(outputjson, scalarString) {
  auto testOne = [](llvm::StringRef s) {
    // log::debug(s.str());
    std::string result;
    llvm::raw_string_ostream rss{result};
    OutputJson out{rss};
    out.scalarString(s, false);
    // log::debug("->", rss.str());
    return result;
  };

  EXPECT_EQ(R"~~("abc")~~", testOne(R"~~(abc)~~"));
  EXPECT_EQ(R"~~("")~~", testOne(R"~~()~~"));
  EXPECT_EQ(R"~~("\\")~~", testOne(R"~~(\)~~"));
  EXPECT_EQ(R"~~("\"")~~", testOne(R"~~(")~~"));
  EXPECT_EQ(R"~~("a\"")~~", testOne(R"~~(a")~~"));
  EXPECT_EQ(R"~~("\"b")~~", testOne(R"~~("b)~~"));
  EXPECT_EQ(R"~~("a\"b")~~", testOne(R"~~(a"b)~~"));
  EXPECT_EQ("\"\\n\"", testOne("\n"));
  EXPECT_EQ("\"\\\\a\\\\b\\\\\"", testOne("\\a\\b\\"));
  EXPECT_EQ("\"\\\"a\\\\n\\\"\"", testOne("\"a\\n\""));
  EXPECT_EQ(R"~~("\"a\\n\"\"b\"")~~", testOne(R"~~("a\n""b")~~"));

  EXPECT_EQ(R"~~("\u0001")~~", testOne("\x01"));
  EXPECT_EQ(R"~~("\b\t\n\f\r\"\\")~~", testOne("\b\t\n\f\r\"\\"));
}

TEST(outputjson, testStruct) {
  TestStruct s = {-54, "it works", true, 12345678, 3.141593};

  std::string result;
  llvm::raw_string_ostream rss{result};

  OutputJson yout{rss};
  yout << s;

  const char *expected =
      R"""({"a":-54,"b":"it works","c":true,"d":12345678,"e":3.141593})""";

  EXPECT_TRUE(result.empty());

  std::string ans = rss.str();
  EXPECT_EQ(ans, result);

  std::cout << result;
  EXPECT_EQ(expected, ans);
}
