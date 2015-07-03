#include "ofp/unittest.h"
#include "ofp/yaml/decoder.h"

using namespace ofp;
using namespace ofp::yaml;

static void testDecodeFail(const char *hex, const char *err) {
  auto s = HexToRawData(hex);

  MessageInfo msgInfo;
  Message msg{s.data(), s.size()};
  msg.setInfo(&msgInfo);
  msg.transmogrify();

  log::debug("msg=", msg);
  
  Decoder decoder{&msg};

  EXPECT_EQ(err, decoder.error());
  EXPECT_EQ("", decoder.result());
}

TEST(decoderfail, malformed_port_desc_v1) {
  testDecodeFail("011500189C3CBB55000D00000000000089A6001000080000", "Range element size overruns end");
}

TEST(decoderfail, malformed_port_desc_v2) {
  testDecodeFail("021700189C3CBB55000D00000000000089A6001000080000", "Range element size overruns end");
}
