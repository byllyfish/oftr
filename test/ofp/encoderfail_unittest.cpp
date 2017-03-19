// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using ofp::yaml::Encoder;

TEST(encoderfail, unknownType) {
  const char *input = R"""(
    version: 1
    type: FEATURE_REQUEST
    msg:
      data: '1234'
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:3:11: error: unknown value \"FEATURE_REQUEST\" Did you mean "
      "\"FEATURES_REQUEST\"?\n    type: FEATURE_REQUEST\n          "
      "^~~~~~~~~~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, unknownVersion) {
  const char *input = R"""(
    type: FEATURES_REQUEST
    msg:
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:2:5: error: unspecified protocol version\n    type: "
      "FEATURES_REQUEST\n    ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, unknownKey) {
  const char *input = R"""(
    version: 1
    type: FEATURES_REQUEST
    foo: '1234'
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:4:10: error: unknown key 'foo'\n    foo: '1234'\n         ^~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, unknownMultipartRequest) {
  const char *input = R"""(
    type: REQUEST.DES
    version: 1
    msg:
      data: '1234'
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:2:11: error: unknown value \"DES\" Did you mean \"DESC\"?\n    "
      "type: REQUEST.DES\n          ^~~~~~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, unknownMultipartRequest2) {
  const char *input = R"""(
    type: REQUEST_DESC
    version: 5
    msg:
      data: '1234'
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:2:11: error: unknown value \"REQUEST_DESC\" Did you mean "
      "\"REQUESTFORWARD\"?\n    type: REQUEST_DESC\n          ^~~~~~~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, unknownMultipartReply) {
  const char *input = R"""(
    type: REPLY.DES
    version: 1
    msg:
      data: '1234'
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:2:11: error: unknown value \"DES\" Did you mean \"DESC\"?\n    "
      "type: REPLY.DES\n          ^~~~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, invalidVersionAndType) {
  const char *input = R"""(
    type: ROLE_REQUEST
    version: 1
    msg:
      role: 0
      generation_id: 0
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:2:5: error: invalid combination of version and type\n    type: "
      "ROLE_REQUEST\n    ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, unknownTypeRequestForward) {
  const char *input = R"""(
    type: REQUESTFORWARD
    version: 5
    msg:
      type: FOO_REQUEST
      version: 1
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:5:13: error: unknown value \"FOO_REQUEST\" Did you mean "
      "\"ECHO_REQUEST\"?\n      type: FOO_REQUEST\n            ^~~~~~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, unknownFlags) {
  const char *input = R"""(
    type: REQUEST.DESC
    version: 4
    flags: [ MOR ]
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:4:14: error: unknown value \"MOR\". Did you mean \"MORE\"?\n    "
      "flags: [ MOR ]\n             ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, unknownOXM) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           IN_DORT
            value:           13
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:30: error: unknown value \"IN_DORT\" Did you mean \"IN_PORT\"?\n          - field:           IN_DORT\n                             ^~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, ambiguousOXM) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           TCP_DST
            value:           13
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:11: error: Invalid match: Missing prerequisites\n          - "
      "field:           TCP_DST\n          ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, conflictingOXM) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           ICMPV6_TYPE
            value:           13
          - field:           TCP_DST
            value:           80
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:11: error: Invalid match: Conflicting prerequisites\n          "
      "- field:           ICMPV6_TYPE\n          ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, ambiguousOXM2) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           MPLS_TC
            value:           13
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:11: error: Invalid match: Missing prerequisites\n          - "
      "field:           MPLS_TC\n          ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, missingMaskedPrereq1) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           IN_PHY_PORT
            value:           13
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:11: error: Invalid match: Missing prerequisites\n          - "
      "field:           IN_PHY_PORT\n          ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, missingMaskedPrereq2) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           VLAN_VID
            value:           0x0001
          - field:           VLAN_PCP
            value:           13
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:11: error: Invalid match: Missing prerequisites\n          - "
      "field:           VLAN_VID\n          ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, invalidPrereq) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           ETH_TYPE
            value:           0x0801
          - field:           TCP_DST
            value:           80
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:11: error: Invalid match: Missing prerequisites\n          - "
      "field:           ETH_TYPE\n          ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, invalidPrereq2) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           ETH_TYPE
            value:           0x0800
          - field:           IP_PROTO
            value:           7
          - field:           TCP_DST
            value:           80
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:11: error: Invalid match: Conflicting prerequisites\n          "
      "- field:           ETH_TYPE\n          ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, duplicateFieldsAllowSameValue) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           ETH_TYPE
            value:           0x0800
          - field:           ETH_TYPE
            value:           0x0800
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(64, encoder.size());
  EXPECT_HEX(
      "040E00400000000111111111111111112222222222222222334455556666777788888888"
      "99999999AAAAAAAABBBB00000001000A80000A020800000000000000",
      encoder.data(), encoder.size());
}

TEST(encoderfail, duplicateFieldsDisallowDifferentValue) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5555
        hard_timeout:    0x6666
        priority:        0x7777
        buffer_id:       0x88888888
        out_port:        0x99999999
        out_group:       0xAAAAAAAA
        flags:           [ '0xBBBB' ]
        match:           
          - field:           ETH_TYPE
            value:           0x0801
          - field:           ETH_TYPE
            value:           0x0800
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:11: error: Invalid match: Conflicting prerequisites\n          "
      "- field:           ETH_TYPE\n          ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, end_brace_only) {
  const char *input = "  }";

  Encoder encoder{input};
  EXPECT_EQ("YAML:1:3: error: Unexpected token\n  }\n  ^\n", encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoderfail, xid_present) {
  // Make sure xid is still available after an encoder failure.
  const char *input = R"""(
    version: 1
    type: FEATURE_REQUEST
    xid: 54321
    msg:
      data: '1234'
    )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:3:11: error: unknown value \"FEATURE_REQUEST\" Did you mean "
      "\"FEATURES_REQUEST\"?\n    type: FEATURE_REQUEST\n          "
      "^~~~~~~~~~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
  EXPECT_EQ(54321, encoder.xid());
}

TEST(encoderfail, ipv4_mapped) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        table_id:        0x33
        command:         0x44
        match:           
          - field:           IPV6_SRC
            value:           10.0.0.1
        instructions:    [  ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:11:30: error: Invalid IPv6 address.\n            value:           "
      "10.0.0.1\n                             ^~~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

#if 0
// TODO(bfish): This test should fail -- properties aren't supported until
// OpenFlow version 1.4.

TEST(encoderfail, portmodv4_experimenter) {
  const char *input = R"""(
      version: 4
      type: PORT_MOD
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        port_no: 0x22222222
        hw_addr: '333333333333'
        config: [ 0x44444444 ]
        mask: [ 0x55555555 ]
        ethernet:
          advertise: [ 0x66666666 ]
        properties:
          - property: EXPERIMENTER
            experimenter: 0x77777700
            exp_type: 0x88888800
            data: 99999900
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX(
      "",
      encoder.data(), encoder.size());
}
#endif  // 0

TEST(encoderfail, packetout_data) {
  const char *input = R"""(
{"version":4,"datapath_id":"0x1","msg":{"in_port":"CONTROLLER","data":"","_pkt_decode":[{"value":"10.10.10.1","field":"IPV4_SRC"},{"value":0,"field":"ICMPV4_CODE"},{"value":0,"field":"ICMPV4_TYPE"},{"value":"089400014bb3a05800000000fa350c0000000000101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f3031323334353637","field":"DATA"},{"value":"f2:f4:54:60:60:53","field":"ETH_DST"},{"value":2048,"field":"ETH_TYPE"},{"value":"0e:00:00:00:00:01","field":"ETH_SRC"},{"value":"10.0.0.1","field":"IPV4_DST"},{"value":1,"field":"IP_PROTO"}],"actions":[{"action":"OUTPUT","port_no":1,"max_len":0}],"buffer_id":"NO_BUFFER"},"xid":8254,"type":"PACKET_OUT","conn_id":4}
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:2:339: error: unknown value \"DATA\" Did you mean \"METADATA\"?\n{\"version\":4,\"datapath_id\":\"0x1\",\"msg\":{\"in_port\":\"CONTROLLER\",\"data\":\"\",\"_pkt_decode\":[{\"value\":\"10.10.10.1\",\"field\":\"IPV4_SRC\"},{\"value\":0,\"field\":\"ICMPV4_CODE\"},{\"value\":0,\"field\":\"ICMPV4_TYPE\"},{\"value\":\"089400014bb3a05800000000fa350c0000000000101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f3031323334353637\",\"field\":\"DATA\"},{\"value\":\"f2:f4:54:60:60:53\",\"field\":\"ETH_DST\"},{\"value\":2048,\"field\":\"ETH_TYPE\"},{\"value\":\"0e:00:00:00:00:01\",\"field\":\"ETH_SRC\"},{\"value\":\"10.0.0.1\",\"field\":\"IPV4_DST\"},{\"value\":1,\"field\":\"IP_PROTO\"}],\"actions\":[{\"action\":\"OUTPUT\",\"port_no\":1,\"max_len\":0}],\"buffer_id\":\"NO_BUFFER\"},\"xid\":8254,\"type\":\"PACKET_OUT\",\"conn_id\":4}\n                                                                                                                                                                                                                                                                                                                                                  ^~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}
