//  ===== ---- ofp/encoder_unittest.cpp --------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements unit tests for yaml::Encoder class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/yaml/encoder.h"

using namespace ofp::yaml;

static void testEncoderSuccess(const char *yaml, size_t size, const char *hex) {
  Encoder encoder{yaml};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(size, encoder.size());
  EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(encoder, flowMod) {
  const char *input = "{ type: OFPT_FLOW_MOD, version: 4, xid: 1 }";

  Encoder encoder{input};
  const std::string &err = encoder.error();

  EXPECT_EQ("YAML:1:3: error: missing required key 'msg'\n{ type: "
            "OFPT_FLOW_MOD, version: 4, xid: 1 }\n  ^\n",
            err);
}

TEST(encoder, hellov1) {
  const char *input = "{ type: OFPT_HELLO, version: 1, xid: 1 }";

  Encoder encoder{input};
  const std::string &err = encoder.error();

  EXPECT_EQ("", err);
  EXPECT_EQ(0x08, encoder.size());
  EXPECT_HEX("0100000800000001", encoder.data(), encoder.size());
}

TEST(encoder, hellov4err) {
  const char *input =
      "{ type: OFPT_HELLO, version: 4, xid: 1 msg:{versions{1, 4}} }";

  Encoder encoder{input};

  EXPECT_EQ("YAML:1:43: error: Found unexpected ':' while scanning a plain "
            "scalar\n{ type: OFPT_HELLO, version: 4, xid: 1 msg:{versions{1, "
            "4}} }\n                                          ^\n",
            encoder.error());
}

TEST(encoder, hellov4) {
  const char *input = "{ 'type': 'OFPT_HELLO', 'version': 4, 'xid': 99, "
                      "'msg':{ 'versions': [1,4] } }";

  Encoder encoder{input};

  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x10, encoder.size());
  EXPECT_HEX("04000010000000630001000800000012", encoder.data(),
             encoder.size());
}

TEST(encoder, error) {
  const char *input = R"""(
type: OFPT_ERROR
version: 1
xid: 98
msg:
  type: 1
  code: 1
  data: 'FFFF1234567890'
)""";

  Encoder encoder{input};

  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x13, encoder.size());
  EXPECT_HEX("010100130000006200010001FFFF1234567890", encoder.data(),
             encoder.size());
}

TEST(encoder, error2) {
  const char *input = R"""(
type: OFPT_ERROR
version: 1
xid: 98
msg:
  type: 1
  code: 1
  data: 'Not Hex'
)""";

  Encoder encoder{input};

  EXPECT_EQ("YAML:8:9: error: Invalid hexadecimal text.\n  data: 'Not Hex'\n "
            "       ^~~~~~~~~\n",
            encoder.error());
  EXPECT_EQ(0x00, encoder.size());
}

TEST(encoder, echorequest) {
  const char *input = R"""(
type: OFPT_ECHO_REQUEST
version: 1
xid: 7
msg:
  data: 'aabbccddeeff'
)""";

  Encoder encoder{input};

  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x0E, encoder.size());
  EXPECT_HEX("0102000E00000007AABBCCDDEEFF", encoder.data(), encoder.size());
}

TEST(encoder, echoreply) {
  const char *input = R"""(
type: OFPT_ECHO_REPLY
version: 1
xid: 7
msg:
  data: 'aabbccddeeff'
)""";

  Encoder encoder{input};

  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x0E, encoder.size());
  EXPECT_HEX("0103000E00000007AABBCCDDEEFF", encoder.data(), encoder.size());
}

TEST(encoder, experimenterv4) {
  const char *input = R"""(
type: OFPT_EXPERIMENTER
version: 4
xid: 0x18
msg:
  experimenter: 0xDEADBEEF
  exp_type: 0xAABBCCDD
  experimenter_data: 'ABCDEF0123456789'
)""";

  testEncoderSuccess(input, 0x18,
                     "0404001800000018DEADBEEFAABBCCDDABCDEF0123456789");
}

TEST(encoder, experimenterv1) {
  const char *input = R"""(
type: OFPT_EXPERIMENTER
version: 1
xid: 0x1B
msg:
  experimenter: 0xDEADBEEF
  exp_type: 0xAABBCCDD
  experimenter_data: 'ABCDEF0123456789'
)""";

  testEncoderSuccess(input, 0x14, "010400140000001BDEADBEEFABCDEF0123456789");
}

TEST(encoder, experimenterv2) {
  const char *input = R"""(
type: OFPT_EXPERIMENTER
version: 2
xid: 0xFF
msg:
  experimenter: 0xDEADBEEF
  exp_type: 0xAABBCCDD
  experimenter_data: 'ABCDEF0123456789'
)""";

  testEncoderSuccess(input, 0x18,
                     "02040018000000FFDEADBEEF00000000ABCDEF0123456789");
}

TEST(encoder, featuresrequest) {
  const char *input = R"""(
   type: OFPT_FEATURES_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "04050008000000BF");
}

TEST(encoder, getasyncrequestv4) {
  const char *input = R"""(
   type: OFPT_GET_ASYNC_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "041A0008000000BF");
}

TEST(encoder, getasyncrequestv1) {
  const char *input = R"""(
   type: OFPT_GET_ASYNC_REQUEST
   version: 1
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0, "");
}

TEST(encoder, getconfigrequest) {
  const char *input = R"""(
   type: OFPT_GET_CONFIG_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "04070008000000BF");
}

TEST(encoder, barrierrequestv4) {
  const char *input = R"""(
   type: OFPT_BARRIER_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "04140008000000BF");
}

TEST(encoder, barrierrequestv1) {
  const char *input = R"""(
   type: OFPT_BARRIER_REQUEST
   version: 1
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "01120008000000BF");
}

TEST(encoder, barrierreplyv4) {
  const char *input = R"""(
   type: OFPT_BARRIER_REPLY
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "04150008000000BF");
}

TEST(encoder, barrierreplyv1) {
  const char *input = R"""(
   type: OFPT_BARRIER_REPLY
   version: 1
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "01130008000000BF");
}

TEST(encoder, featuresreplyv1) {
  const char *input = R"""(
   type: OFPT_FEATURES_REPLY
   version: 1
   xid: 0xBF
   msg:
     datapath_id: '0000:0102:0304:0506'
     n_buffers: 256
     n_tables: 255
     auxiliary_id: 0
     capabilities: 0
     reserved: 0
   )""";

  testEncoderSuccess(
      input, 0x20,
      "01060020000000BF000001020304050600000100FF0000000000000000000000");
}

TEST(encoder, featuresreplyv4) {
  const char *input = R"""(
   type: OFPT_FEATURES_REPLY
   version: 4
   xid: 0xBF
   msg:
     datapath_id: '0000:0102:0304:0506'
     n_buffers: 256
     n_tables: 255
     auxiliary_id: 0
     capabilities: 0
     reserved: 0
   )""";

  testEncoderSuccess(
      input, 0x20,
      "04060020000000BF000001020304050600000100FF0000000000000000000000");
}

TEST(encoder, ofmp_flow_v4) {
  const char *input = R"""(
   type: OFPT_MULTIPART_REQUEST
   version: 4
   xid: 0x11223344
   msg:
     type: OFPMP_FLOW
     flags: 0
     body:
         table_id: 1
         out_port: 2
         out_group:  3
         cookie: 4
         cookie_mask: 5
         match:
            - type: OFB_IN_PORT
              value: 0x12345678
   )""";

  testEncoderSuccess(input, 0x03C, "0412003C112233440001000000000000010000000"
                                   "00000020000000300000000000000000000000400"
                                   "00000000000005000100088000000412345678");
}

TEST(encoder, ofmp_flow_v1) {
  const char *input = R"""(
   type: OFPT_MULTIPART_REQUEST
   version: 1
   xid: 0x11223344
   msg:
     type: OFPMP_FLOW
     flags: 0
     body:
         table_id: 0x11
         out_port: 0x22222222
         out_group:  0x33333333
         cookie: 0x4444444444444444
         cookie_mask: 0x5555555555555555
         match:
            - type: OFB_IN_PORT
              value: 0x12345678
   )""";

  testEncoderSuccess(input, 0x03C, "0112003C112233440001000000000000003FFFFE567"
                                   "8000000000000000000000000000000000000000000"
                                   "0000000000000000000000000011002222");
}

TEST(encoder, multipartreply) {
  const char *input = R"""(
   type: OFPT_MULTIPART_REPLY
   version: 4
   xid: 0x11223344
   msg:
     type: OFPMP_FLOW
     flags: 0
     body:
        - table_id: 1
          duration_sec: 2
          duration_nsec: 3
          priority: 4
          idle_timeout: 5
          hard_timeout: 6
          flags: 7
          cookie: 8
          packet_count: 9
          byte_count: 10
          match:
            - type: OFB_IN_PORT
              value: 0x12345678
          instructions:
            - type: OFPIT_GOTO_TABLE
              value: { 'table_id': 1 }
   )""";

  const char *hex = "0413005411223344000100000000000000440100000000020000000300"
                    "0400050006000700000000000000000000000800000000000000090000"
                    "00000000000A0001000880000004123456780001000801000000";
  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x54, encoder.size());
  EXPECT_HEX(hex, encoder.data(), encoder.size());

  // testEncoderSuccess(input, 0x04C,
  // "0413004C112233440001000000000000003C0100000000020000000300040005000600070000000000000000000000080000000000000009000000000000000A000100088000000412345678");
}

TEST(encoder, multipartreply2) {
  const char *input = R"""(
   type: OFPT_MULTIPART_REPLY
   version: 4
   xid: 0x11223344
   msg:
     type: OFPMP_FLOW
     flags: 0
     body:
        - table_id: 1
          duration_sec: 2
          duration_nsec: 3
          priority: 4
          idle_timeout: 5
          hard_timeout: 6
          flags: 7
          cookie: 8
          packet_count: 9
          byte_count: 10
          match:
            - type: OFB_IN_PORT
              value: 0x12345678
          instructions:
        - table_id: 0x11
          duration_sec: 0x22
          duration_nsec: 0x33 
          priority: 0x44
          idle_timeout: 0x55
          hard_timeout: 0x66
          flags: 0x77
          cookie: 0x88
          packet_count: 0x9999999999999999
          byte_count:   0xAAAAAAAAAAAAAAAA
          match:
            - type: OFB_ETH_SRC
              value: 10-20-30-40-50-60
            - type: OFB_ETH_DST
              value: aa-bb-cc-dd-ee-ff
          instructions:
            - type: OFPIT_GOTO_TABLE
              value: { 'table_id': 1 }
   )""";

  testEncoderSuccess(input, 0x0A4,
                     "041300A4112233440001000000000000003C010000000002000000030"
                     "004000500060007000000000000000000000008000000000000000900"
                     "0000000000000A0001000880000004123456780058110000000022000"
                     "000330044005500660077000000000000000000000088999999999999"
                     "9999AAAAAAAAAAAAAAAA0001001C80000004123456788000080610203"
                     "040506080000606AABBCCDDEEFF0001000801000000");
}

TEST(encoder, multipartreply3) {
  const char *input = R"""(
   type: OFPT_MULTIPART_REPLY
   version: 4
   xid: 0x11223344
   msg:
     type: OFPMP_FLOW
     flags: 0
     body:
        - table_id: 1
          duration_sec: 2
          duration_nsec: 3
          priority: 4
          idle_timeout: 5
          hard_timeout: 6
          flags: 7
          cookie: 8
          packet_count: 9
          byte_count: 10
          match:
          instructions:
            - type: OFPIT_APPLY_ACTIONS
              value:
                 - action: OFPAT_OUTPUT
                   port: 1
                   maxlen: 0xFFFF
                 - action: OFPAT_SET_NW_TTL
                   ttl: 64
   )""";

  const char *hex = "0413006411223344000100000000000000540100000000020000000300"
                    "0400050006000700000000000000000000000800000000000000090000"
                    "00000000000A0001000000040020000000000000001000000001FFFF00"
                    "00000000000017000840000000";
  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x64, encoder.size());
  EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(encoder, ofpmp_flow_reply_v1) {
  const char *input = R"""(
   type: OFPT_MULTIPART_REPLY
   version: 1
   xid: 0x11111111
   msg:
     type: OFPMP_FLOW
     flags: 0x2222
     body:
        - table_id: 0x33
          duration_sec: 0x44444444
          duration_nsec: 0x55555555
          priority: 0x6666
          idle_timeout: 0x7777
          hard_timeout: 0x8888
          flags: 0x9999
          cookie: 0xAAAAAAAAAAAAAAAA
          packet_count: 0xBBBBBBBBBBBBBBBB
          byte_count: 0xCCCCCCCCCCCCCCCC
          match:
            - type: OFB_IN_PORT
              value: 0xDDDDDDDD
          instructions:
            - type: OFPIT_APPLY_ACTIONS
              value:
                 - action: OFPAT_OUTPUT
                   port: 0xEEEEEEEE
                   maxlen: 0xFFFF
   )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x70, encoder.size());
  EXPECT_HEX("0113007011111111000122220000000000683300003FFFFEDDDD000000000000000000000000000000000000000000000000000000000000000000004444444455555555666677778888000000000000AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCC00000008EEEEFFFF", encoder.data(), encoder.size());
}


TEST(encoder, flowmodv4) {
  const char *input = R"""(
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
          - type:    OFPIT_APPLY_ACTIONS
            value:
               - action: OFPAT_SET_FIELD
                 type: OFB_IPV4_DST
                 value: 192.168.2.1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x68, encoder.size());
  EXPECT_HEX("040E0068000000010000000000000000000000000000000000000000000000000"
             "00000000000000000000000000000000001001A800000040000000D80000A0208"
             "0080001804C0A8010100000000000000040018000000000019001080001804C0A"
             "8020100000000",
             encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4_2) {
  const char *input = R"""(
      type:            OFPT_FLOW_MOD
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
        flags:           0xBBBB
        match:           
          - type:            OFB_IN_PORT
            value:           0xCCCCCCCC
          - type:            OFB_ETH_TYPE
            value:           2048
          - type:            OFB_IPV4_DST
            value:           192.168.1.1
        instructions:
          - type:    OFPIT_APPLY_ACTIONS
            value:
               - action: OFPAT_SET_FIELD
                 type: OFB_IPV4_DST
                 value: 192.168.2.1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x68, encoder.size());
  EXPECT_HEX("040E0068000000011111111111111111222222222222222233445555666677778"
             "888888899999999AAAAAAAABBBB00000001001A80000004CCCCCCCC80000A0208"
             "0080001804C0A8010100000000000000040018000000000019001080001804C0A"
             "8020100000000",
             encoder.data(), encoder.size());
}

TEST(encoder, flowmodv1) {
  const char *input = R"""(
      type:            OFPT_FLOW_MOD
      version:         1
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
        flags:           0xBBBB
        match:           
          - type:            OFB_IN_PORT
            value:           0xCCCCCCCC
          - type:            OFB_IPV4_DST
            value:           192.168.1.1
        instructions:
          - type:    OFPIT_APPLY_ACTIONS
            value:
               - action: OFPAT_SET_FIELD
                 type: OFB_IPV4_DST
                 value: 192.168.2.1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x050, encoder.size());
  EXPECT_HEX("010E00500000000100303FEECCCC0000000000000000000000000000000008000"
             "000000000000000C0A80101000000001111111111111111004455556666777788"
             "8888889999BBBB00070008C0A80201",
             encoder.data(), encoder.size());
}

TEST(encoder, flowmodv1_2) {
  const char *input = R"""(
      type:            OFPT_FLOW_MOD
      version:         1
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
        flags:           0xBBBB
        match:           
          - type:            OFB_IN_PORT
            value:           0xCCCCCCCC
          - type:            OFB_IPV4_DST
            value:           192.168.1.1
          - type:            OFB_ICMPV4_TYPE
            value:           0xDD
        instructions:
          - type:    OFPIT_APPLY_ACTIONS
            value:
               - action: OFPAT_SET_FIELD
                 type: OFB_ICMPV4_CODE
                 value: 0xEE
               - action: OFPAT_COPY_TTL_OUT
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x058, encoder.size());
  EXPECT_HEX("010E00580000000100303F8ECCCC0000000000000000000000000000000008000"
             "001000000000000C0A8010100DD00001111111111111111004455556666777788"
             "8888889999BBBB000A0008EE000000000B000800000000",
             encoder.data(), encoder.size());
}

TEST(encoder, packetinv1) {
  const char *input = R"""(
      type:            OFPT_PACKET_IN
      version:         1
      xid:             2
      msg:             
        buffer_id:       0x33333333
        total_len:       0x4444
        in_port:         0x55555555
        in_phy_port:     0x66666666
        metadata:        0x7777777777777777
        reason:          OFPR_ACTION
        table_id:        0x88
        cookie:          0x9999999999999999
        enet_frame:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x03C, encoder.size());
  EXPECT_HEX("010A003C0000000233333333444455550100FFFFFFFFFFFF00000000000108060"
             "0010800060400010000000000010A0000010000000000000A000002",
             encoder.data(), encoder.size());
}

TEST(encoder, packetinv4) {
  const char *input = R"""(
      type:            OFPT_PACKET_IN
      version:         4
      xid:             1
      msg:             
        buffer_id:       0x33333333
        total_len:       0x4444
        in_port:         0x55555555
        in_phy_port:     0x66666666
        metadata:        0x7777777777777777
        reason:          OFPR_ACTION
        table_id:        0x88
        cookie:          0x9999999999999999
        enet_frame:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x064, encoder.size());
  EXPECT_HEX("040A0064000000013333333344440188999999999999999900010020800000045"
             "555555580000204666666668000040877777777777777770000FFFFFFFFFFFF00"
             "0000000001080600010800060400010000000000010A0000010000000000000A0"
             "00002",
             encoder.data(), encoder.size());
}

TEST(encoder, packetoutv1) {
  const char *input = R"""(
      type:            OFPT_PACKET_OUT
      version:         1
      xid:             1
      msg:             
        buffer_id:       0x33333333
        in_port:         0x44444444
        actions:
          - action: OFPAT_OUTPUT
            port: 5
            maxlen: 20
          - action: OFPAT_SET_FIELD
            type:   OFB_IPV4_DST
            value:  192.168.1.1
        enet_frame:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x04A, encoder.size());
  EXPECT_HEX("010D004A000000013333333344440010000000080005001400070008C0A80101F"
             "FFFFFFFFFFF000000000001080600010800060400010000000000010A00000100"
             "00000000000A000002",
             encoder.data(), encoder.size());
}

TEST(encoder, packetoutv4) {
  const char *input = R"""(
      type:            OFPT_PACKET_OUT
      version:         4
      xid:             1
      msg:             
        buffer_id:       0x33333333
        in_port:         0x44444444
        actions:
          - action: OFPAT_OUTPUT
            port: 5
            maxlen: 20
          - action: OFPAT_SET_FIELD
            type:   OFB_IPV4_DST
            value:  192.168.1.1
        enet_frame:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x062, encoder.size());
  EXPECT_HEX("040D0062000000013333333344444444002000000000000000000010000000050"
             "0140000000000000019001080001804C0A8010100000000FFFFFFFFFFFF000000"
             "000001080600010800060400010000000000010A0000010000000000000A00000"
             "2",
             encoder.data(), encoder.size());
}

TEST(encoder, setconfigv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_SET_CONFIG
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        flags: 0xAAAA
        miss_send_len: 0xBBBB
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x0C, encoder.size());
  EXPECT_HEX("0409000C11111111AAAABBBB", encoder.data(), encoder.size());
}

TEST(encoder, portstatusv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_PORT_STATUS
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        reason: 0x22
        port:
          port_no: 0x33333333
          hw_addr: 'aabbccddeeff'
          name: 'Port 1'
          config: 0x44444444
          state: 0x55555555
          curr: 0x66666666
          advertised: 0x77777777
          supported: 0x88888888
          peer: 0x99999999
          curr_speed: 0xAAAAAAAA
          max_speed: 0xBBBBBBBB
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x50, encoder.size());
  EXPECT_HEX("040C00501111111122000000000000003333333300000000AABBCCDDEEFF00005"
             "06F72742031000000000000000000004444444455555555666666667777777788"
             "88888899999999AAAAAAAABBBBBBBB",
             encoder.data(), encoder.size());
}

TEST(encoder, portstatusv1) {
  const char *input = R"""(
      version: 1
      type: OFPT_PORT_STATUS
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        reason: 0x22
        port:
          port_no: 0x33333333
          hw_addr: 'aabbccddeeff'
          name: 'Port 1'
          config: 0x44444444
          state: 0x55555555
          curr: 0x66666666
          advertised: 0x77777777
          supported: 0x88888888
          peer: 0x99999999
          curr_speed: 0xAAAAAAAA
          max_speed: 0xBBBBBBBB
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x40, encoder.size());
  EXPECT_HEX("010C00401111111122000000000000003333AABBCCDDEEFF506F7274203100000"
             "000000000000000444444445555555566666666777777778888888899999999",
             encoder.data(), encoder.size());
}

TEST(encoder, groupmodv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_GROUP_MOD
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        command: 0x2222
        type: 0x33
        group_id: 0x44444444
        buckets:
          - weight: 0x5555
            watch_port: 0x66666666
            watch_group: 0x77777777
            actions:
              - action: OFPAT_OUTPUT
                port: 5
                maxlen: 20
              - action: OFPAT_SET_FIELD
                type:   OFB_IPV4_DST
                value:  192.168.1.1
          - weight: 0x8888
            watch_port: 0x99999999
            watch_group: 0xAAAAAAAA
            actions:
              - action: OFPAT_SET_FIELD
                type: OFB_ICMPV4_CODE
                value: 0xEE
              - action: OFPAT_COPY_TTL_OUT
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x68, encoder.size());
  EXPECT_HEX("040F0068111111112222330044444444003055556666666677777777000000000"
             "00000100000000500140000000000000019001080001804C0A801010000000000"
             "28888899999999AAAAAAAA000000000019001080002801EE00000000000000000"
             "B000800000000",
             encoder.data(), encoder.size());
}

TEST(encoder, groupmodv2) {
  const char *input = R"""(
      version: 2
      type: OFPT_GROUP_MOD
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        command: 0x2222
        type: 0x33
        group_id: 0x44444444
        buckets:
          - weight: 0x5555
            watch_port: 0x66666666
            watch_group: 0x77777777
            actions:
              - action: OFPAT_OUTPUT
                port: 5
                maxlen: 20
              - action: OFPAT_SET_FIELD
                type:   OFB_IPV4_DST
                value:  192.168.1.1
          - weight: 0x8888
            watch_port: 0x99999999
            watch_group: 0xAAAAAAAA
            actions:
              - action: OFPAT_SET_FIELD
                type: OFB_ICMPV4_CODE
                value: 0xEE
              - action: OFPAT_COPY_TTL_OUT
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x68, encoder.size());
  EXPECT_HEX("020F0068111111112222330044444444003055556666666677777777000000000"
             "00000100000000500140000000000000019001080001804C0A801010000000000"
             "28888899999999AAAAAAAA000000000019001080002801EE00000000000000000"
             "B000800000000",
             encoder.data(), encoder.size());
}

TEST(encoder, portmodv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_PORT_MOD
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        port_no: 0x22222222
        hw_addr: '333333333333'
        config: 0x44444444
        mask: 0x55555555
        advertise: 0x66666666
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x28, encoder.size());
  EXPECT_HEX("04100028111111112222222200000000333333333333000044444444555555556"
             "666666600000000",
             encoder.data(), encoder.size());
}

TEST(encoder, portmodv1) {
  const char *input = R"""(
      version: 1
      type: OFPT_PORT_MOD
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        port_no: 0x2222
        hw_addr: '333333333333'
        config: 0x44444444
        mask: 0x55555555
        advertise: 0x66666666
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x20, encoder.size());
  EXPECT_HEX("010F002011111111222233333333333344444444555555556666666600000000",
             encoder.data(), encoder.size());
}

TEST(encoder, tablemodv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_TABLE_MOD
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        table_id: 0x22
        config: 0x33333333
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x10, encoder.size());
  EXPECT_HEX("04110010111111112200000033333333", encoder.data(),
             encoder.size());
}

TEST(encoder, tablemodv2) {
  const char *input = R"""(
      version: 2
      type: OFPT_TABLE_MOD
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        table_id: 0x22
        config: 0x33333333
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x10, encoder.size());
  EXPECT_HEX("02110010111111112200000033333333", encoder.data(),
             encoder.size());
}

TEST(encoder, rolerequestv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_ROLE_REQUEST
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        role: 0x22222222
        generation_id: 0x3333333333333333
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x18, encoder.size());
  EXPECT_HEX("041800181111111122222222000000003333333333333333", encoder.data(),
             encoder.size());
}

TEST(encoder, rolereplyv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_ROLE_REPLY
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        role: 0x22222222
        generation_id: 0x3333333333333333
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x18, encoder.size());
  EXPECT_HEX("041900181111111122222222000000003333333333333333", encoder.data(),
             encoder.size());
}

TEST(encoder, getasyncreplyv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_GET_ASYNC_REPLY
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        packet_in_mask_master: 0x22222222
        packet_in_mask_slave: 0x33333333
        port_status_mask_master: 0x44444444
        port_status_mask_slave: 0x55555555
        flow_removed_mask_master: 0x66666666
        flow_removed_mask_slave: 0x77777777
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x20, encoder.size());
  EXPECT_HEX("041B002011111111222222223333333344444444555555556666666677777777",
             encoder.data(), encoder.size());
}

TEST(encoder, queuegetconfigrequestv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_QUEUE_GET_CONFIG_REQUEST
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        port: 0x22222222
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x10, encoder.size());
  EXPECT_HEX("04160010111111112222222200000000", encoder.data(),
             encoder.size());
}

TEST(encoder, getconfigreplyv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_GET_CONFIG_REPLY
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        flags: 0xAAAA
        miss_send_len: 0xBBBB
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x0C, encoder.size());
  EXPECT_HEX("0408000C11111111AAAABBBB", encoder.data(), encoder.size());
}

TEST(encoder, setasyncv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_SET_ASYNC
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        packet_in_mask_master: 0x22222222
        packet_in_mask_slave: 0x33333333
        port_status_mask_master: 0x44444444
        port_status_mask_slave: 0x55555555
        flow_removed_mask_master: 0x66666666
        flow_removed_mask_slave: 0x77777777
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x20, encoder.size());
  EXPECT_HEX("041C002011111111222222223333333344444444555555556666666677777777",
             encoder.data(), encoder.size());
}

TEST(encoder, flowremovedv4) {
  const char *input = R"""(
      version: 4
      type: OFPT_FLOW_REMOVED
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        cookie: 0x2222222222222222
        priority: 0x3333
        reason: 0x44
        table_id: 0x55
        duration_sec: 0x66666666
        duration_nsec: 0x77777777
        idle_timeout: 0x8888
        hard_timeout: 0x9999
        packet_count: 0xAAAAAAAAAAAAAAAA
        byte_count: 0xBBBBBBBBBBBBBBBB
        match:
            - type: OFB_IN_PORT
              value: 0x12345678
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x3C, encoder.size());
  EXPECT_HEX("040B003C11111111222222222222222233334455666666667777777788889999A"
             "AAAAAAAAAAAAAAABBBBBBBBBBBBBBBB0001000C8000000412345678",
             encoder.data(), encoder.size());
}

TEST(encoder, flowremovedv1) {
  const char *input = R"""(
      version: 1
      type: OFPT_FLOW_REMOVED
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        cookie: 0x2222222222222222
        priority: 0x3333
        reason: 0x44
        table_id: 0x55
        duration_sec: 0x66666666
        duration_nsec: 0x77777777
        idle_timeout: 0x8888
        hard_timeout: 0x9999
        packet_count: 0xAAAAAAAAAAAAAAAA
        byte_count: 0xBBBBBBBBBBBBBBBB
        match:
            - type: OFB_IN_PORT
              value: 0x12345678
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x58, encoder.size());
  EXPECT_HEX("010B005811111111003FFFFE56780000000000000000000000000000000000000"
             "00000000000000000000000000000002222222222222222333344006666666677"
             "77777788880000AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBB",
             encoder.data(), encoder.size());
}

TEST(encoder, flowremovedv2) {
  const char *input = R"""(
      version: 2
      type: OFPT_FLOW_REMOVED
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        cookie: 0x2222222222222222
        priority: 0x3333
        reason: 0x44
        table_id: 0x55
        duration_sec: 0x66666666
        duration_nsec: 0x77777777
        idle_timeout: 0x8888
        hard_timeout: 0x9999
        packet_count: 0xAAAAAAAAAAAAAAAA
        byte_count: 0xBBBBBBBBBBBBBBBB
        match:
            - type: OFB_IN_PORT
              value: 0x12345678
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x88, encoder.size());
  EXPECT_HEX("020B008811111111222222222222222233334455666666667777777788880000A"
             "AAAAAAAAAAAAAAABBBBBBBBBBBBBBBB0000005812345678000003FE0000000000"
             "00000000000000000000000000000000000000000000000000000000000000000"
             "00000000000000000000000000000000000000000000000000000000000000000"
             "000000000000",
             encoder.data(), encoder.size());
}

TEST(encoder, flowremovedv3) {
  const char *input = R"""(
      version: 3
      type: OFPT_FLOW_REMOVED
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        cookie: 0x2222222222222222
        priority: 0x3333
        reason: 0x44
        table_id: 0x55
        duration_sec: 0x66666666
        duration_nsec: 0x77777777
        idle_timeout: 0x8888
        hard_timeout: 0x9999
        packet_count: 0xAAAAAAAAAAAAAAAA
        byte_count: 0xBBBBBBBBBBBBBBBB
        match:
            - type: OFB_IN_PORT
              value: 0x12345678
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x3C, encoder.size());
  EXPECT_HEX("030B003C11111111222222222222222233334455666666667777777788889999A"
             "AAAAAAAAAAAAAAABBBBBBBBBBBBBBBB0001000C8000000412345678",
             encoder.data(), encoder.size());
}

TEST(encoder, ofmp_desc_request) {
  const char *input = R"""(
      version: 4
      type: OFPT_MULTIPART_REQUEST
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        type: OFPMP_DESC
        flags: 0
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x10, encoder.size());
  EXPECT_HEX("04120010111111110000000000000000", encoder.data(),
             encoder.size());
}

TEST(encoder, ofmp_desc_reply) {
  const char *input = R"""(
      version: 4
      type: OFPT_MULTIPART_REPLY
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        type: OFPMP_DESC
        flags: 0
        body:
          mfr_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          hw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          sw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          serial_num: ABCDEFGHIJKLMNOPQRSTUVWXYZ
          dp_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x430, encoder.size());
  EXPECT_HEX(
      "041304301111111100000000000000004142434445464748494A4B4C4D4E4F5051525354"
      "55565758595A203132333435363738390000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000004142434445464748494A4B4C4D4E4F50"
      "5152535455565758595A2031323334353637383900000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000004142434445464748494A4B4C"
      "4D4E4F505152535455565758595A20313233343536373839000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000004142434445464748"
      "494A4B4C4D4E4F505152535455565758595A0000000000004142434445464748494A4B4C"
      "4D4E4F505152535455565758595A20313233343536373839000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_desc_reply_error) {
  const char *input = R"""(
      version: 4
      type: OFPT_MULTIPART_REPLY
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        type: OFPMP_DESC
        flags: 0
        body:
          mfr_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          hw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          sw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          serial_num: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          dp_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
      )""";

  Encoder encoder{input};
  EXPECT_EQ("YAML:13:23: error: Value is too long\n          serial_num: "
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n                      "
            "^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n",
            encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoder, ofmp_desc_request_v1) {
  const char *input = R"""(
      version: 1
      type: OFPT_MULTIPART_REQUEST
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        type: OFPMP_DESC
        flags: 0
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x10, encoder.size());
  EXPECT_HEX("01120010111111110000000000000000", encoder.data(),
             encoder.size());
}

TEST(encoder, ofmp_desc_reply_v1) {
  const char *input = R"""(
      version: 1
      type: OFPT_MULTIPART_REPLY
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        type: OFPMP_DESC
        flags: 0
        body:
          mfr_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          hw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          sw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          serial_num: ABCDEFGHIJKLMNOPQRSTUVWXYZ
          dp_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x430, encoder.size());
  EXPECT_HEX(
      "011304301111111100000000000000004142434445464748494A4B4C4D4E4F5051525354"
      "55565758595A203132333435363738390000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000004142434445464748494A4B4C4D4E4F50"
      "5152535455565758595A2031323334353637383900000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000004142434445464748494A4B4C"
      "4D4E4F505152535455565758595A20313233343536373839000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000004142434445464748"
      "494A4B4C4D4E4F505152535455565758595A0000000000004142434445464748494A4B4C"
      "4D4E4F505152535455565758595A20313233343536373839000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000",
      encoder.data(), encoder.size());
}
