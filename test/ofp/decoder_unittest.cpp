//  ===== ---- ofp/decoder_unittest.cpp --------------------*- C++ -*- =====  //
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
/// \brief Implements unit tests for yaml::Decoder class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using namespace yaml;

static void testDecodeEncode(const char *hex, const char *yaml) {
  auto s = HexToRawData(hex);

  Message msg{s.data(), s.size()};
  msg.transmogrify();

  Decoder decoder{&msg};

  EXPECT_EQ("", decoder.error());
  EXPECT_EQ(yaml, decoder.result());

  Encoder encoder{decoder.result()};

  EXPECT_EQ("", encoder.error());
  EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(decoder, hellov1) {
  testDecodeEncode(
      "0100000800000001",
      "---\ntype:            OFPT_HELLO\nxid:             "
      "0x00000001\nversion:         1\nmsg:             \n  versions:  "
      "      [ 1 ]\n...\n");
}

TEST(decoder, hellov4) {
  testDecodeEncode(
      "04000010000000010001000800000012",
      "---\ntype:            OFPT_HELLO\nxid:             "
      "0x00000001\nversion:         4\nmsg:             \n  versions:  "
      "      [ 1, 4 ]\n...\n");
}

TEST(decoder, errorv1) {
  testDecodeEncode(
      "010100130000006200010001FFFF1234567890",
      "---\ntype:            OFPT_ERROR\nxid:             "
      "0x00000062\nversion:         1\nmsg:             \n  type:      "
      "      1\n  code:            1\n  data:            "
      "FFFF1234567890\n...\n");
}

TEST(decoder, echoRequest) {
  testDecodeEncode("0102000E00000007AABBCCDDEEFF",
                   "---\ntype:            OFPT_ECHO_REQUEST\nxid:            "
                   " 0x00000007\nversion:         1\nmsg:             \n  "
                   "data:            AABBCCDDEEFF\n...\n");
}

TEST(decoder, echoReply) {
  testDecodeEncode("0103000E00000007AABBCCDDEEFF",
                   "---\ntype:            OFPT_ECHO_REPLY\nxid:             "
                   "0x00000007\nversion:         1\nmsg:             \n  "
                   "data:            AABBCCDDEEFF\n...\n");
}

TEST(decoder, experimenterv4) {
  testDecodeEncode("0404001800000018DEADBEEFAABBCCDDABCDEF0123456789",
                   "---\ntype:            OFPT_EXPERIMENTER\nxid:             "
                   "0x00000018\nversion:         4\nmsg:             \n  "
                   "experimenter:    3735928559\n  exp_type:        "
                   "2864434397\n  experimenter_data: ABCDEF0123456789\n...\n");
}

TEST(decoder, experimenterv1) {
  testDecodeEncode("010400140000001BDEADBEEFABCDEF0123456789",
                   "---\ntype:            OFPT_EXPERIMENTER\nxid:             "
                   "0x0000001B\nversion:         1\nmsg:             \n  "
                   "experimenter:    3735928559\n  exp_type:        0\n  "
                   "experimenter_data: ABCDEF0123456789\n...\n");
}

TEST(decoder, experimenterv2) {
  testDecodeEncode("02040018000000FFDEADBEEF00000000ABCDEF0123456789",
                   "---\ntype:            OFPT_EXPERIMENTER\nxid:             "
                   "0x000000FF\nversion:         2\nmsg:             \n  "
                   "experimenter:    3735928559\n  exp_type:        0\n  "
                   "experimenter_data: ABCDEF0123456789\n...\n");
}

TEST(decoder, featuresrequest) {
  testDecodeEncode("04050008000000BF", "---\ntype:            "
                                       "OFPT_FEATURES_REQUEST\nxid:          "
                                       "   0x000000BF\nversion:         "
                                       "4\nmsg:             \n...\n");
}

TEST(decoder, getconfigrequest) {
  testDecodeEncode("04070008000000BF", "---\ntype:            "
                                       "OFPT_GET_CONFIG_REQUEST\nxid:        "
                                       "     0x000000BF\nversion:         "
                                       "4\nmsg:             \n...\n");
}

TEST(decoder, barrierrequestv4) {
  testDecodeEncode("04140008000000BF", "---\ntype:            "
                                       "OFPT_BARRIER_REQUEST\nxid:           "
                                       "  0x000000BF\nversion:         "
                                       "4\nmsg:             \n...\n");
}

TEST(decoder, barrierrequestv1) {
  testDecodeEncode("01120008000000BF", "---\ntype:            "
                                       "OFPT_BARRIER_REQUEST\nxid:           "
                                       "  0x000000BF\nversion:         "
                                       "1\nmsg:             \n...\n");
}

TEST(decoder, barrierreplyv4) {
  testDecodeEncode("04150008000000BF", "---\ntype:            "
                                       "OFPT_BARRIER_REPLY\nxid:             "
                                       "0x000000BF\nversion:         4\nmsg: "
                                       "            \n...\n");
}

TEST(decoder, barrierreplyv1) {
  testDecodeEncode("01130008000000BF", "---\ntype:            "
                                       "OFPT_BARRIER_REPLY\nxid:             "
                                       "0x000000BF\nversion:         1\nmsg: "
                                       "            \n...\n");
}

TEST(decoder, getasyncrequestv4) {
  testDecodeEncode("041A0008000000BF", "---\ntype:            "
                                       "OFPT_GET_ASYNC_REQUEST\nxid:         "
                                       "    0x000000BF\nversion:         "
                                       "4\nmsg:             \n...\n");
}

TEST(decoder, getasyncrequestv1) {
  auto s = HexToRawData("011A0008000000BF");

  Message msg{s.data(), s.size()};
  msg.transmogrify();

  Decoder decoder{&msg};

  EXPECT_EQ("Invalid data.", decoder.error());
  EXPECT_EQ("", decoder.result());
}

TEST(decoder, featuresreplyv1) {
  testDecodeEncode(
      "01060020000000BF000001020304050600000100FF0000000000000000000000",
      "---\ntype:            OFPT_FEATURES_REPLY\nxid:             "
      "0x000000BF\nversion:         1\nmsg:             \n  datapath_id:     "
      "0000-0102-0304-0506\n  n_buffers:       256\n  n_tables:        255\n "
      " auxiliary_id:    0\n  capabilities:    0\n  reserved:        "
      "0\n...\n");
}

TEST(decoder, featuresreplyv4) {
  testDecodeEncode(
      "04060020000000BF000001020304050600000100FF0000000000000000000000",
      "---\ntype:            OFPT_FEATURES_REPLY\nxid:             "
      "0x000000BF\nversion:         4\nmsg:             \n  datapath_id:     "
      "0000-0102-0304-0506\n  n_buffers:       256\n  n_tables:        255\n "
      " auxiliary_id:    0\n  capabilities:    0\n  reserved:        "
      "0\n...\n");
}

TEST(decoder, ofmp_flowrequest_v4) {
  testDecodeEncode(
      "0412003C1122334400010000000000000100000000000002000000030000000000000000"
      "000000040000000000000005000100088000000412345678",
      "---\ntype:            OFPT_MULTIPART_REQUEST\nxid:             "
      "0x11223344\nversion:         4\nmsg:             \n  type:            "
      "OFPMP_FLOW\n  flags:           0x0000\n  body:            \n    "
      "table_id:        1\n    out_port:        0x00000002\n    out_group:     "
      "  0x00000003\n    cookie:          0x0000000000000004\n    cookie_mask: "
      "    0x0000000000000005\n    match:           \n      - type:            "
      "OFB_IN_PORT\n        value:           305419896\n...\n");
}

TEST(decoder, ofmp_flowrequest_v1) {
  testDecodeEncode(
      "0110003C112233440001000000000000003FFFFE567"
      "8000000000000000000000000000000000000000000"
      "0000000000000000000000000011002222",
      "---\ntype:            OFPT_MULTIPART_REQUEST\nxid:             "
      "0x11223344\nversion:         1\nmsg:             \n  type:            "
      "OFPMP_FLOW\n  flags:           0x0000\n  body:            \n    "
      "table_id:        17\n    out_port:        0x00002222\n    out_group:    "
      "   0x00000000\n    cookie:          0x0000000000000000\n    "
      "cookie_mask:     0x0000000000000000\n    match:           \n      - "
      "type:            OFB_IN_PORT\n        value:           22136\n...\n");
}

TEST(decoder, ofmp_aggregaterequest_v4) {
  testDecodeEncode("0412003C11223344000200000000000011000000222"
                   "2222233333333000000004444444444444444555555"
                   "5555555555000100088000000412345678",
                   "---\ntype:            OFPT_MULTIPART_REQUEST\nxid:         "
                   "    0x11223344\nversion:         4\nmsg:             \n  "
                   "type:            OFPMP_AGGREGATE\n  flags:           "
                   "0x0000\n  body:            \n    table_id:        17\n    "
                   "out_port:        0x22222222\n    out_group:       "
                   "0x33333333\n    cookie:          0x4444444444444444\n    "
                   "cookie_mask:     0x5555555555555555\n    match:           "
                   "\n      - type:            OFB_IN_PORT\n        value:     "
                   "      305419896\n...\n");
}

TEST(decoder, ofmp_aggregaterequest_v1) {
  testDecodeEncode(
      "0110003C112233440002000000000000003FFFFE567"
      "8000000000000000000000000000000000000000000"
      "0000000000000000000000000011002222",
      "---\ntype:            OFPT_MULTIPART_REQUEST\nxid:             "
      "0x11223344\nversion:         1\nmsg:             \n  type:            "
      "OFPMP_AGGREGATE\n  flags:           0x0000\n  body:            \n    "
      "table_id:        17\n    out_port:        0x00002222\n    out_group:    "
      "   0x00000000\n    cookie:          0x0000000000000000\n    "
      "cookie_mask:     0x0000000000000000\n    match:           \n      - "
      "type:            OFB_IN_PORT\n        value:           22136\n...\n");
}

TEST(decoder, ofmp_flowreply_v4) {
  testDecodeEncode(
      "0413005411223344000100000000000000440100000000020000000300"
      "0400050006000700000000000000000000000800000000000000090000"
      "00000000000A0001000C80000004123456780001000801000000",
      "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             "
      "0x11223344\nversion:         4\nmsg:             \n  type:            "
      "OFPMP_FLOW\n  flags:           0x0000\n  body:            \n    - "
      "table_id:        1\n      duration_sec:    0x00000002\n      "
      "duration_nsec:   0x00000003\n      priority:        0x0004\n      "
      "idle_timeout:    0x0005\n      hard_timeout:    0x0006\n      flags:    "
      "       0x0007\n      cookie:          0x0000000000000008\n      "
      "packet_count:    0x0000000000000009\n      byte_count:      "
      "0x000000000000000A\n      match:           \n        - type:            "
      "OFB_IN_PORT\n          value:           305419896\n      instructions:  "
      "  \n        - type:            OFPIT_GOTO_TABLE\n          value:       "
      "    \n            table_id:        1\n...\n");
}

TEST(decoder, ofmp_flowreply2_v4) {
  testDecodeEncode(
      "041300A4112233440001000000000000003C010000000002000000030"
      "004000500060007000000000000000000000008000000000000000900"
      "0000000000000A0001000C80000004123456780058110000000022000"
      "000330044005500660077000000000000000000000088999999999999"
      "9999AAAAAAAAAAAAAAAA0001002080000004123456788000080610203"
      "040506080000606AABBCCDDEEFF0001000801000000",
      "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             "
      "0x11223344\nversion:         4\nmsg:             \n  type:            "
      "OFPMP_FLOW\n  flags:           0x0000\n  body:            \n    - "
      "table_id:        1\n      duration_sec:    0x00000002\n      "
      "duration_nsec:   0x00000003\n      priority:        0x0004\n      "
      "idle_timeout:    0x0005\n      hard_timeout:    0x0006\n      flags:    "
      "       0x0007\n      cookie:          0x0000000000000008\n      "
      "packet_count:    0x0000000000000009\n      byte_count:      "
      "0x000000000000000A\n      match:           \n        - type:            "
      "OFB_IN_PORT\n          value:           305419896\n      instructions:  "
      "  \n    - table_id:        17\n      duration_sec:    0x00000022\n      "
      "duration_nsec:   0x00000033\n      priority:        0x0044\n      "
      "idle_timeout:    0x0055\n      hard_timeout:    0x0066\n      flags:    "
      "       0x0077\n      cookie:          0x0000000000000088\n      "
      "packet_count:    0x9999999999999999\n      byte_count:      "
      "0xAAAAAAAAAAAAAAAA\n      match:           \n        - type:            "
      "OFB_IN_PORT\n          value:           305419896\n        - type:      "
      "      OFB_ETH_SRC\n          value:           10-20-30-40-50-60\n       "
      " - type:            OFB_ETH_DST\n          value:           "
      "AA-BB-CC-DD-EE-FF\n      instructions:    \n        - type:            "
      "OFPIT_GOTO_TABLE\n          value:           \n            table_id:    "
      "    1\n...\n");
}

TEST(decoder, ofmp_flowreply_v1) {
  testDecodeEncode(
      "0111007011111111000122220000000000603300003FFFFEDDDD0000000000000"
      "00000000000000000000000000000000000000000000000000000004444444455"
      "555555666677778888000000000000AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBCCC"
      "CCCCCCCCCCCCC00000008EEEEFFFF",
      "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         1\nmsg:             \n  type:            "
      "OFPMP_FLOW\n  flags:           0x2222\n  body:            \n    - "
      "table_id:        51\n      duration_sec:    0x44444444\n      "
      "duration_nsec:   0x55555555\n      priority:        0x6666\n      "
      "idle_timeout:    0x7777\n      hard_timeout:    0x8888\n      flags:    "
      "       0x0000\n      cookie:          0xAAAAAAAAAAAAAAAA\n      "
      "packet_count:    0xBBBBBBBBBBBBBBBB\n      byte_count:      "
      "0xCCCCCCCCCCCCCCCC\n      match:           \n        - type:            "
      "OFB_IN_PORT\n          value:           56797\n      instructions:    "
      "\n        - type:            OFPIT_APPLY_ACTIONS\n          value:      "
      "     \n            - action:          OFPAT_OUTPUT\n              port: "
      "           61166\n              maxlen:          65535\n...\n");
}

TEST(decoder, ofmp_flowreply2_v1) {
  testDecodeEncode(
      "011100D011223344000100000000000000600100003FFFFE567800000000000000000000"
      "000000000000000000000000000000000000000000000000000000020000000300040005"
      "000600000000000000000000000000080000000000000009000000000000000A00000008"
      "EEEEFFFF00601100003FFFF25678102030405060AABBCCDDEEFF00000000000000000000"
      "000000000000000000000000000000220000003300440055006600000000000000000000"
      "000000889999999999999999AAAAAAAAAAAAAAAA00000008EEEEFFFF",
      "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             "
      "0x11223344\nversion:         1\nmsg:             \n  type:            "
      "OFPMP_FLOW\n  flags:           0x0000\n  body:            \n    - "
      "table_id:        1\n      duration_sec:    0x00000002\n      "
      "duration_nsec:   0x00000003\n      priority:        0x0004\n      "
      "idle_timeout:    0x0005\n      hard_timeout:    0x0006\n      flags:    "
      "       0x0000\n      cookie:          0x0000000000000008\n      "
      "packet_count:    0x0000000000000009\n      byte_count:      "
      "0x000000000000000A\n      match:           \n        - type:            "
      "OFB_IN_PORT\n          value:           22136\n      instructions:    "
      "\n        - type:            OFPIT_APPLY_ACTIONS\n          value:      "
      "     \n            - action:          OFPAT_OUTPUT\n              port: "
      "           61166\n              maxlen:          65535\n    - table_id: "
      "       17\n      duration_sec:    0x00000022\n      duration_nsec:   "
      "0x00000033\n      priority:        0x0044\n      idle_timeout:    "
      "0x0055\n      hard_timeout:    0x0066\n      flags:           0x0000\n  "
      "    cookie:          0x0000000000000088\n      packet_count:    "
      "0x9999999999999999\n      byte_count:      0xAAAAAAAAAAAAAAAA\n      "
      "match:           \n        - type:            OFB_IN_PORT\n          "
      "value:           22136\n        - type:            OFB_ETH_SRC\n        "
      "  value:           10-20-30-40-50-60\n        - type:            "
      "OFB_ETH_DST\n          value:           AA-BB-CC-DD-EE-FF\n      "
      "instructions:    \n        - type:            OFPIT_APPLY_ACTIONS\n     "
      "     value:           \n            - action:          OFPAT_OUTPUT\n   "
      "           port:            61166\n              maxlen:          "
      "65535\n...\n");
}

TEST(decoder, aggregatereply_v4) {
  testDecodeEncode(
      "041300281111111100022222000000003333333333333330444444444444444055555550"
      "00000000",
      "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         4\nmsg:             \n  type:            "
      "OFPMP_AGGREGATE\n  flags:           0x2222\n  body:            \n    "
      "packet_count:    0x3333333333333330\n    byte_count:      "
      "0x4444444444444440\n    flow_count:      0x55555550\n...\n");
}

TEST(decoder, aggregatereply_v1) {
  testDecodeEncode(
      "011100281111111100022222000000003333333333333330444444444444444055555550"
      "00000000",
      "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         1\nmsg:             \n  type:            "
      "OFPMP_AGGREGATE\n  flags:           0x2222\n  body:            \n    "
      "packet_count:    0x3333333333333330\n    byte_count:      "
      "0x4444444444444440\n    flow_count:      0x55555550\n...\n");
}

TEST(decoder, tablestats_v4) {
  testDecodeEncode(
      "041300401111111100032222000000003300000044444440555555555555555066666666"
      "6666666077000000888888809999999999999990AAAAAAAAAAAAAAA0",
      "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         4\nmsg:             \n  type:            "
      "OFPMP_TABLE\n  flags:           0x2222\n  body:            \n    - "
      "table_id:        51\n      name:            ''\n      wildcards:       "
      "0x00000000\n      max_entries:     0x00000000\n      active_count:    "
      "0x44444440\n      lookup_count:    0x5555555555555550\n      "
      "matched_count:   0x6666666666666660\n    - table_id:        119\n      "
      "name:            ''\n      wildcards:       0x00000000\n      "
      "max_entries:     0x00000000\n      active_count:    0x88888880\n      "
      "lookup_count:    0x9999999999999990\n      matched_count:   "
      "0xAAAAAAAAAAAAAAA0\n...\n");
}

TEST(decoder, tablestats_v1) {
  testDecodeEncode(
      "01110090111111110003222200000000330000005461626C652031000000000000000000"
      "000000000000000000000000000000004444444055555550666666607777777777777770"
      "8888888888888880110000005461626C6520320000000000000000000000000000000000"
      "000000000000000099999990AAAAAAA0BBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD"
      "0",
      "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         1\nmsg:             \n  type:            "
      "OFPMP_TABLE\n  flags:           0x2222\n  body:            \n    - "
      "table_id:        51\n      name:            Table 1\n      wildcards:   "
      "    0x44444440\n      max_entries:     0x55555550\n      active_count:  "
      "  0x66666660\n      lookup_count:    0x7777777777777770\n      "
      "matched_count:   0x8888888888888880\n    - table_id:        17\n      "
      "name:            Table 2\n      wildcards:       0x99999990\n      "
      "max_entries:     0xAAAAAAA0\n      active_count:    0xBBBBBBB0\n      "
      "lookup_count:    0xCCCCCCCCCCCCCCC0\n      matched_count:   "
      "0xDDDDDDDDDDDDDDD0\n...\n");
}

TEST(decoder, flowmodv4) {
  testDecodeEncode(
      "040E00680000000100000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000001001A800000040000000D80000A02080080001804C0A8"
      "010100000000000000040018000000000019001080001804C0A8020100000000",
      "---\ntype:            OFPT_FLOW_MOD\nxid:             "
      "0x00000001\nversion:         4\nmsg:             \n  cookie:          "
      "0x0000000000000000\n  cookie_mask:     0x0000000000000000\n  table_id:  "
      "      0\n  command:         0\n  idle_timeout:    0x0000\n  "
      "hard_timeout:    0x0000\n  priority:        0x0000\n  buffer_id:       "
      "0x00000000\n  out_port:        0x00000000\n  out_group:       "
      "0x00000000\n  flags:           0x0000\n  match:           \n    - type: "
      "           OFB_IN_PORT\n      value:           13\n    - type:          "
      "  OFB_ETH_TYPE\n      value:           2048\n    - type:            "
      "OFB_IPV4_DST\n      value:           192.168.1.1\n  instructions:    \n "
      "   - type:            OFPIT_APPLY_ACTIONS\n      value:           \n    "
      "    - action:          OFPAT_SET_FIELD\n          type:            "
      "OFB_IPV4_DST\n          value:           192.168.2.1\n...\n");
}

TEST(decoder, flowmodv4_2) {
  testDecodeEncode(
      "040E00680000000111111111111111112222222222222222334455556666777788888888"
      "99999999AAAAAAAABBBB00000001001A80000004CCCCCCCC80000A02080080001804C0A8"
      "010100000000000000040018000000000019001080001804C0A8020100000000",
      "---\ntype:            OFPT_FLOW_MOD\nxid:             "
      "0x00000001\nversion:         4\nmsg:             \n  cookie:          "
      "0x1111111111111111\n  cookie_mask:     0x2222222222222222\n  table_id:  "
      "      51\n  command:         68\n  idle_timeout:    0x5555\n  "
      "hard_timeout:    0x6666\n  priority:        0x7777\n  buffer_id:       "
      "0x88888888\n  out_port:        0x99999999\n  out_group:       "
      "0xAAAAAAAA\n  flags:           0xBBBB\n  match:           \n    - type: "
      "           OFB_IN_PORT\n      value:           3435973836\n    - type:  "
      "          OFB_ETH_TYPE\n      value:           2048\n    - type:        "
      "    OFB_IPV4_DST\n      value:           192.168.1.1\n  instructions:   "
      " \n    - type:            OFPIT_APPLY_ACTIONS\n      value:           "
      "\n        - action:          OFPAT_SET_FIELD\n          type:           "
      " OFB_IPV4_DST\n          value:           192.168.2.1\n...\n");
}

TEST(decoder, flowmodv1) {
  const char *hex = "010E00500000000100303FEECCCC000000000000000000000000000000"
                    "0008000000000000000000C0A801010000000011111111111111110044"
                    "555566667777888888889999BBBB00070008C0A80201";
  const char *yaml =
      "---\ntype:            OFPT_FLOW_MOD\nxid:             "
      "0x00000001\nversion:         1\nmsg:             \n  cookie:          "
      "0x1111111111111111\n  cookie_mask:     0xFFFFFFFFFFFFFFFF\n  table_id:  "
      "      0\n  command:         68\n  idle_timeout:    0x5555\n  "
      "hard_timeout:    0x6666\n  priority:        0x7777\n  buffer_id:       "
      "0x88888888\n  out_port:        0x00009999\n  out_group:       "
      "0x00000000\n  flags:           0xBBBB\n  match:           \n    - type: "
      "           OFB_IN_PORT\n      value:           52428\n    - type:       "
      "     OFB_ETH_TYPE\n      value:           2048\n    - type:            "
      "OFB_IPV4_DST\n      value:           192.168.1.1\n  instructions:    \n "
      "   - type:            OFPIT_APPLY_ACTIONS\n      value:           \n    "
      "    - action:          OFPAT_SET_FIELD\n          type:            "
      "OFB_IPV4_DST\n          value:           192.168.2.1\n...\n";

  auto s = HexToRawData(hex);

  Message msg{s.data(), s.size()};
  msg.transmogrify();
  EXPECT_EQ(0xA0, msg.size());
  EXPECT_HEX("010E00A0000000011111111111111111FFFFFFFFFFFFFFFF00445555666677778"
             "88888880000999900000000BBBB0000000000580000CCCC000003F60000000000"
             "00000000000000000000000000000000000000000000000800000000000000000"
             "00000C0A80101FFFFFFFF00000000000000000000000000000000000000000000"
             "00000000000000040018000000000019001080001804C0A8020100000000",
             msg.data(), msg.size());

  Decoder decoder{&msg};

  EXPECT_EQ("", decoder.error());
  EXPECT_EQ(yaml, decoder.result());

  Encoder encoder{decoder.result()};

  EXPECT_EQ("", encoder.error());
  EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(decoder, flowmod1_2) {
  testDecodeEncode(
      "010E00580000000100303F8ECCCC00000000000000000000000000000000080000010000"
      "00000000C0A8010100DD000011111111111111110044555566667777888888889999BBBB"
      "000A0008EE000000000B000800000000",
      "---\ntype:            OFPT_FLOW_MOD\nxid:             "
      "0x00000001\nversion:         1\nmsg:             \n  cookie:          "
      "0x1111111111111111\n  cookie_mask:     0xFFFFFFFFFFFFFFFF\n  table_id:  "
      "      0\n  command:         68\n  idle_timeout:    0x5555\n  "
      "hard_timeout:    0x6666\n  priority:        0x7777\n  buffer_id:       "
      "0x88888888\n  out_port:        0x00009999\n  out_group:       "
      "0x00000000\n  flags:           0xBBBB\n  match:           \n    - type: "
      "           OFB_IN_PORT\n      value:           52428\n    - type:       "
      "     OFB_ETH_TYPE\n      value:           2048\n    - type:            "
      "OFB_IP_PROTO\n      value:           1\n    - type:            "
      "OFB_IPV4_DST\n      value:           192.168.1.1\n    - type:           "
      " OFB_ICMPV4_TYPE\n      value:           221\n  instructions:    \n    "
      "- type:            OFPIT_APPLY_ACTIONS\n      value:           \n       "
      " - action:          OFPAT_SET_FIELD\n          type:            "
      "OFB_ICMPV4_CODE\n          value:           238\n        - action:      "
      "    OFPAT_COPY_TTL_OUT\n...\n");
}

TEST(decoder, packetinv4) {
  testDecodeEncode("040A0064000000013333333344440188999999999999999900010020 "
                   "80000004555555558000020466666666800004087777777777777777000"
                   "0FFFFFFFFFFFF000000000001080600010800060400010000000000010A"
                   "0000010000000000000A000002",
                   "---\ntype:            OFPT_PACKET_IN\nxid:             "
                   "0x00000001\nversion:         4\nmsg:             \n  "
                   "buffer_id:       858993459\n  total_len:       17476\n  "
                   "in_port:         1431655765\n  in_phy_port:     "
                   "1717986918\n  metadata:        8608480567731124087\n  "
                   "reason:          OFPR_ACTION\n  table_id:        136\n  "
                   "cookie:          11068046444225730969\n  enet_frame:      "
                   "FFFFFFFFFFFF000000000001080600010800060400010000000000010A0"
                   "000010000000000000A000002\n...\n");
}

TEST(decoder, packetinv1) {
  testDecodeEncode("010A003C0000000233333333444455550100FFFFFFFFFFFF00000000000"
                   "1080600010800060400010000000000010A0000010000000000000A0000"
                   "02",
                   "---\ntype:            OFPT_PACKET_IN\nxid:             "
                   "0x00000002\nversion:         1\nmsg:             \n  "
                   "buffer_id:       858993459\n  total_len:       17476\n  "
                   "in_port:         21845\n  in_phy_port:     0\n  metadata:  "
                   "      0\n  reason:          OFPR_ACTION\n  table_id:       "
                   " 0\n  cookie:          0\n  enet_frame:      "
                   "FFFFFFFFFFFF000000000001080600010800060400010000000000010A0"
                   "000010000000000000A000002\n...\n");
}

TEST(decoder, packetoutv4) {
  testDecodeEncode(
      "040D00620000000133333333444444440020000000000000000000100000000500140000"
      "000000000019001080001804C0A8010100000000FFFFFFFFFFFF00000000000108060001"
      "0800060400010000000000010A0000010000000000000A000002",
      "---\ntype:            OFPT_PACKET_OUT\nxid:             "
      "0x00000001\nversion:         4\nmsg:             \n  buffer_id:       "
      "858993459\n  in_port:         1145324612\n  actions:         \n    - "
      "action:          OFPAT_OUTPUT\n      port:            5\n      maxlen:  "
      "        20\n    - action:          OFPAT_SET_FIELD\n      type:         "
      "   OFB_IPV4_DST\n      value:           192.168.1.1\n  enet_frame:      "
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002\n...\n");
}

TEST(decoder, packetoutv1) {
  const char *hex = "010D004A000000013333333344440010000000080005001400070008C0"
                    "A80101FFFFFFFFFFFF0000000000010806000108000604000100000000"
                    "00010A0000010000000000000A000002";
  const char *yaml =
      "---\ntype:            OFPT_PACKET_OUT\nxid:             "
      "0x00000001\nversion:         1\nmsg:             \n  buffer_id:       "
      "858993459\n  in_port:         17476\n  actions:         \n    - action: "
      "         OFPAT_OUTPUT\n      port:            5\n      maxlen:          "
      "20\n    - action:          OFPAT_SET_FIELD\n      type:            "
      "OFB_IPV4_DST\n      value:           192.168.1.1\n  enet_frame:      "
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002\n...\n";

  auto s = HexToRawData(hex);

  Message msg{s.data(), s.size()};
  msg.transmogrify();
  EXPECT_HEX("010D0062000000013333333300004444002000000000000000000010000000050"
             "0140000000000000019001080001804C0A8010100000000FFFFFFFFFFFF000000"
             "000001080600010800060400010000000000010A0000010000000000000A00000"
             "2",
             msg.data(), msg.size());

  Decoder decoder{&msg};

  EXPECT_EQ("", decoder.error());
  EXPECT_EQ(yaml, decoder.result());

  Encoder encoder{decoder.result()};

  EXPECT_EQ("", encoder.error());
  EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(decoder, setconfigv4) {
  testDecodeEncode("0409000C11111111AAAABBBB",
                   "---\ntype:            OFPT_SET_CONFIG\nxid:             "
                   "0x11111111\nversion:         4\nmsg:             \n  "
                   "flags:           0xAAAA\n  miss_send_len:   0xBBBB\n...\n");
}

TEST(decoder, portstatusv4) {
  testDecodeEncode(
      "040C00501111111122000000000000003333333300000000AABBCCDDEEFF0000506F7274"
      "203100000000000000000000444444445555555566666666777777778888888899999999"
      "AAAAAAAABBBBBBBB",
      "---\ntype:            OFPT_PORT_STATUS\nxid:             "
      "0x11111111\nversion:         4\nmsg:             \n  reason:          "
      "34\n  port:            \n    port_no:         0x33333333\n    hw_addr:  "
      "       AA-BB-CC-DD-EE-FF\n    name:            Port 1\n    config:      "
      "    0x44444444\n    state:           0x55555555\n    curr:            "
      "0x66666666\n    advertised:      0x77777777\n    supported:       "
      "0x88888888\n    peer:            0x99999999\n    curr_speed:      "
      "0xAAAAAAAA\n    max_speed:       0xBBBBBBBB\n...\n");
}

TEST(decoder, portstatusv1) {
  testDecodeEncode(
      "010C00401111111122000000000000003333AABBCCDDEEFF506F72742031000000000000"
      "00000000444444445555555566666666777777778888888899999999",
      "---\ntype:            OFPT_PORT_STATUS\nxid:             "
      "0x11111111\nversion:         1\nmsg:             \n  reason:          "
      "34\n  port:            \n    port_no:         0x00003333\n    hw_addr:  "
      "       AA-BB-CC-DD-EE-FF\n    name:            Port 1\n    config:      "
      "    0x44444444\n    state:           0x55555555\n    curr:            "
      "0x66666666\n    advertised:      0x77777777\n    supported:       "
      "0x88888888\n    peer:            0x99999999\n    curr_speed:      "
      "0x00000000\n    max_speed:       0x00000000\n...\n");
}

TEST(decoder, groupmodv4) {
  testDecodeEncode(
      "040F00681111111122223300444444440030555566666666777777770000000000000010"
      "0000000500140000000000000019001080001804C0A80101000000000028888899999999"
      "AAAAAAAA000000000019001080002801EE00000000000000000B000800000000",
      "---\ntype:            OFPT_GROUP_MOD\nxid:             "
      "0x11111111\nversion:         4\nmsg:             \n  command:         "
      "0x2222\n  type:            51\n  group_id:        0x44444444\n  "
      "buckets:         \n    - weight:          21845\n      watch_port:      "
      "1717986918\n      watch_group:     2004318071\n      actions:         "
      "\n        - action:          OFPAT_OUTPUT\n          port:            "
      "5\n          maxlen:          20\n        - action:          "
      "OFPAT_SET_FIELD\n          type:            OFB_IPV4_DST\n          "
      "value:           192.168.1.1\n    - weight:          34952\n      "
      "watch_port:      2576980377\n      watch_group:     2863311530\n      "
      "actions:         \n        - action:          OFPAT_SET_FIELD\n         "
      " type:            OFB_ICMPV4_CODE\n          value:           238\n     "
      "   - action:          OFPAT_COPY_TTL_OUT\n...\n");
}

TEST(decoder, portmodv4) {
  testDecodeEncode("04100028111111112222222200000000333333333333000044444444555"
                   "555556666666600000000",
                   "---\ntype:            OFPT_PORT_MOD\nxid:             "
                   "0x11111111\nversion:         4\nmsg:             \n  "
                   "port_no:         0x22222222\n  hw_addr:         "
                   "33-33-33-33-33-33\n  config:          0x44444444\n  mask:  "
                   "          0x55555555\n  advertise:       "
                   "0x66666666\n...\n");
}

TEST(decoder, portmodv1) {
  testDecodeEncode(
      "010F002011111111222233333333333344444444555555556666666600000000",
      "---\ntype:            OFPT_PORT_MOD\nxid:             "
      "0x11111111\nversion:         1\nmsg:             \n  port_no:         "
      "0x00002222\n  hw_addr:         33-33-33-33-33-33\n  config:          "
      "0x44444444\n  mask:            0x55555555\n  advertise:       "
      "0x66666666\n...\n");
}

TEST(decoder, tablemodv4) {
  testDecodeEncode("04110010111111112200000033333333",
                   "---\ntype:            OFPT_TABLE_MOD\nxid:             "
                   "0x11111111\nversion:         4\nmsg:             \n  "
                   "table_id:        34\n  config:          0x33333333\n...\n");
}

TEST(decoder, tablemodv2) {
  testDecodeEncode("02110010111111112200000033333333",
                   "---\ntype:            OFPT_TABLE_MOD\nxid:             "
                   "0x11111111\nversion:         2\nmsg:             \n  "
                   "table_id:        34\n  config:          0x33333333\n...\n");
}

TEST(decoder, rolerequestv4) {
  testDecodeEncode("041800181111111122222222000000003333333333333333",
                   "---\ntype:            OFPT_ROLE_REQUEST\nxid:             "
                   "0x11111111\nversion:         4\nmsg:             \n  role: "
                   "           0x22222222\n  generation_id:   "
                   "0x3333333333333333\n...\n");
}

TEST(decoder, rolereplyv4) {
  testDecodeEncode("041900181111111122222222000000003333333333333333",
                   "---\ntype:            OFPT_ROLE_REPLY\nxid:             "
                   "0x11111111\nversion:         4\nmsg:             \n  role: "
                   "           0x22222222\n  generation_id:   "
                   "0x3333333333333333\n...\n");
}

TEST(decoder, getasyncreplyv4) {
  testDecodeEncode(
      "041B002011111111222222223333333344444444555555556666666677777777",
      "---\ntype:            OFPT_GET_ASYNC_REPLY\nxid:             "
      "0x11111111\nversion:         4\nmsg:             \n  "
      "packet_in_mask_master: 0x22222222\n  packet_in_mask_slave: 0x33333333\n "
      " port_status_mask_master: 0x44444444\n  port_status_mask_slave: "
      "0x55555555\n  flow_removed_mask_master: 0x66666666\n  "
      "flow_removed_mask_slave: 0x77777777\n...\n");
}

TEST(decoder, queuegetconfigrequestv4) {
  testDecodeEncode("04160010111111112222222200000000",
                   "---\ntype:            OFPT_QUEUE_GET_CONFIG_REQUEST\nxid:  "
                   "           0x11111111\nversion:         4\nmsg:            "
                   " \n  port:            0x22222222\n...\n");
}

TEST(decoder, getconfigreplyv4) {
  testDecodeEncode("0408000C11111111AAAABBBB",
                   "---\ntype:            OFPT_GET_CONFIG_REPLY\nxid:          "
                   "   0x11111111\nversion:         4\nmsg:             \n  "
                   "flags:           0xAAAA\n  miss_send_len:   0xBBBB\n...\n");
}

TEST(decoder, setasyncv4) {
  testDecodeEncode(
      "041C002011111111222222223333333344444444555555556666666677777777",
      "---\ntype:            OFPT_SET_ASYNC\nxid:             "
      "0x11111111\nversion:         4\nmsg:             \n  "
      "packet_in_mask_master: 0x22222222\n  packet_in_mask_slave: 0x33333333\n "
      " port_status_mask_master: 0x44444444\n  port_status_mask_slave: "
      "0x55555555\n  flow_removed_mask_master: 0x66666666\n  "
      "flow_removed_mask_slave: 0x77777777\n...\n");
}

TEST(decoder, flowremovedv4) {
  testDecodeEncode(
      "040B003C11111111222222222222222233334455666666667777777788889999AAAAAAAA"
      "AAAAAAAABBBBBBBBBBBBBBBB0001000C8000000412345678",
      "---\ntype:            OFPT_FLOW_REMOVED\nxid:             "
      "0x11111111\nversion:         4\nmsg:             \n  cookie:          "
      "0x2222222222222222\n  priority:        0x3333\n  reason:          68\n  "
      "table_id:        85\n  duration_sec:    0x66666666\n  duration_nsec:   "
      "0x77777777\n  idle_timeout:    0x8888\n  hard_timeout:    0x9999\n  "
      "packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - type:            "
      "OFB_IN_PORT\n      value:           305419896\n...\n");
}

TEST(decoder, flowremovedv1) {
  testDecodeEncode(
      "010B005811111111003FFFFE567800000000000000000000000000000000000000000000"
      "000000000000000000000000222222222222222233334400666666667777777788880000"
      "AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBB",
      "---\ntype:            OFPT_FLOW_REMOVED\nxid:             "
      "0x11111111\nversion:         1\nmsg:             \n  cookie:          "
      "0x2222222222222222\n  priority:        0x3333\n  reason:          68\n  "
      "table_id:        0\n  duration_sec:    0x66666666\n  duration_nsec:   "
      "0x77777777\n  idle_timeout:    0x8888\n  hard_timeout:    0x0000\n  "
      "packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - type:            "
      "OFB_IN_PORT\n      value:           22136\n...\n");
}

TEST(decoder, flowremovedv2) {
  testDecodeEncode(
      "020B008811111111222222222222222233334455666666667777777788880000AAAAAAAA"
      "AAAAAAAABBBBBBBBBBBBBBBB0000005812345678000003FE000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000",
      "---\ntype:            OFPT_FLOW_REMOVED\nxid:             "
      "0x11111111\nversion:         2\nmsg:             \n  cookie:          "
      "0x2222222222222222\n  priority:        0x3333\n  reason:          68\n  "
      "table_id:        85\n  duration_sec:    0x66666666\n  duration_nsec:   "
      "0x77777777\n  idle_timeout:    0x8888\n  hard_timeout:    0x0000\n  "
      "packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - type:            "
      "OFB_IN_PORT\n      value:           305419896\n...\n");
}

TEST(decoder, flowremovedv3) {
  testDecodeEncode(
      "030B003C11111111222222222222222233334455666666667777777788889999AAAAAAAA"
      "AAAAAAAABBBBBBBBBBBBBBBB0001000C8000000412345678",
      "---\ntype:            OFPT_FLOW_REMOVED\nxid:             "
      "0x11111111\nversion:         3\nmsg:             \n  cookie:          "
      "0x2222222222222222\n  priority:        0x3333\n  reason:          68\n  "
      "table_id:        85\n  duration_sec:    0x66666666\n  duration_nsec:   "
      "0x77777777\n  idle_timeout:    0x8888\n  hard_timeout:    0x9999\n  "
      "packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - type:            "
      "OFB_IN_PORT\n      value:           305419896\n...\n");
}

TEST(decoder, ofmp_desc_request_v4) {
  testDecodeEncode("04120010111111110000000000000000",
                   "---\ntype:            OFPT_MULTIPART_REQUEST\nxid:         "
                   "    0x11111111\nversion:         4\nmsg:             \n  "
                   "type:            OFPMP_DESC\n  flags:           "
                   "0x0000\n...\n");
}

TEST(decoder, ofmp_desc_reply_v4) {
  testDecodeEncode(
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
      "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         4\nmsg:             \n  type:            "
      "OFPMP_DESC\n  flags:           0x0000\n  body:            \n    "
      "mfr_desc:        ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n    hw_desc:     "
      "    ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n    sw_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n    serial_num:      "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n    dp_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n...\n");
}
