// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/filtertable.h"
#include "ofp/message.h"
#include "ofp/mockchannel.h"
#include "ofp/rpc/filteractiongenericreply.h"
#include "ofp/unittest.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using namespace ofp::rpc;

static Message encodeMessage(llvm::StringRef yaml) {
  ofp::yaml::Encoder encoder{yaml};
  assert(encoder.error() == "");
  return Message{encoder.data(), encoder.size()};
}

static std::string decodeMessage(ByteRange data) {
  assert(data.size() > 0);
  Message message{data.data(), data.size()};
  message.normalize();

  ofp::yaml::Decoder decoder{&message, false, true};
  return decoder.result();
}

TEST(filtertable, test) {
  std::vector<FilterTableEntry> params;
  params.emplace_back();
  params.back().setFilter("vlan and icmp");
  params.back().setAction(MakeUniquePtr<FilterActionGenericReply>());

  FilterTable filters;
  filters.setFilters(std::move(params));
  EXPECT_EQ(filters.size(), 1);
  EXPECT_EQ(params.size(), 0);

  const char *input = R"""(
    type:            PACKET_IN
    xid:             0x00000000
    version:         0x04
    msg:             
      buffer_id:       NO_BUFFER
      total_len:       0x05EE
      in_port:         0x00000001
      in_phy_port:     0x00000001
      metadata:        0x0000000000000000
      reason:          APPLY_ACTION
      table_id:        0x06
      cookie:          0x00000000FFFFFFFF
      match:           
        - field:           IN_PORT
          value:           0x00000001
      data:            0E00000000010AC2BB024296810000640800450005DC0518200040013AA70A0000010A6400FE080014572C2400059DA8FC590000000046D5060000000000101112131415161718191A1B1C1D1E1F202122232425
  )""";

  Message message = encodeMessage(input);
  ASSERT_TRUE(message.type() == OFPT_PACKET_IN);
  message.normalize();

  MockChannel outputChannel{OFP_VERSION_4};
  message.setSource(&outputChannel);

  // Test filter table here.
  bool escalate = true;
  bool result = filters.apply(&message, &escalate);
  EXPECT_TRUE(result);
  EXPECT_FALSE(escalate);
  EXPECT_NE(message.msgFlags() & OFP_REPLIED, 0);

  ASSERT_GT(outputChannel.size(), 0);
  std::string output =
      decodeMessage({outputChannel.data(), outputChannel.size()});
  EXPECT_EQ(
      output,
      "---\ntype:            PACKET_OUT\nxid:             0x00000001\nversion: "
      "        0x04\nmsg:             \n  buffer_id:       NO_BUFFER\n  "
      "in_port:         CONTROLLER\n  actions:         \n    - action:         "
      " OUTPUT\n      port_no:         0x00000001\n      max_len:         "
      "0x0000\n  data:            "
      "0AC2BB0242960E00000000018100006408004500004200000000400165590A6400FE0A00"
      "00010000CDD42C2400059DA8FC590000000046D506000000000010111213141516171819"
      "1A1B1C1D1E1F202122232425\n  _pkt:            \n    - field:           "
      "ETH_DST\n      value:           '0a:c2:bb:02:42:96'\n    - field:       "
      "    ETH_SRC\n      value:           '0e:00:00:00:00:01'\n    - field:   "
      "        VLAN_VID\n      value:           0x1064\n    - field:           "
      "ETH_TYPE\n      value:           0x0800\n    - field:           "
      "IP_PROTO\n      value:           0x01\n    - field:           "
      "IPV4_SRC\n      value:           10.100.0.254\n    - field:           "
      "IPV4_DST\n      value:           10.0.0.1\n    - field:           "
      "NX_IP_TTL\n      value:           0x40\n    - field:           "
      "ICMPV4_TYPE\n      value:           0x00\n    - field:           "
      "ICMPV4_CODE\n      value:           0x00\n    - field:           "
      "X_PKT_POS\n      value:           0x002A\n...\n");
}
