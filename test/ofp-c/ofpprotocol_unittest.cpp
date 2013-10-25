#include "ofp/unittest.h"
#include "ofp-c/ofpprotocol.h"

using namespace ofp;

TEST(ofpprotocol, empty) {
  OFPString input = ofpStringNewEmpty();
  OFPString output = ofpStringNewEmpty();
  OFPString error = ofpStringNewEmpty();

  int result = ofpProtocolEncode(&input, &output, &error);

  EXPECT_EQ(0, result);
  EXPECT_TRUE(ofpStringEqualsC(&error, "Empty message."));
  EXPECT_TRUE(ofpStringEqualsC(&output, ""));
}

TEST(ofpprotocol, test) {
  OFPString input = ofpStringNewEmpty();
  OFPString output = ofpStringNewEmpty();
  OFPString error = ofpStringNewEmpty();

  const char *expected = R"""(---
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
  instructions:
...
)""";

  ofpStringSetC(&input, expected);

  int result = ofpProtocolEncode(&input, &output, &error);

  EXPECT_EQ(1, result);
  EXPECT_HEX("040E00500000000100000000000000000000000000000000000000000000000"
             "0000000000000000000000000000000000001001A800000040000000D80000A"
             "02080080001804C0A80101000000000000",
             output.data, output.length);
  EXPECT_FALSE(error.data);

  ofpStringClear(&input);
  ofpStringClear(&output);
  ofpStringClear(&error);
}