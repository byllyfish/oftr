#include "ofp/unittest.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using ofp::yaml::Encoder;

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
      "YAML:18:30: error: Invalid OXM type.\n          - field:           "
      "IN_DORT\n                             ^~~~~~~\n",
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
