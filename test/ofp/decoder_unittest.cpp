// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/decoder.h"
#include "ofp/unittest.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using namespace yaml;

static void testDecodeEncode(const char *hex, const char *yaml) {
  auto s = HexToRawData(hex);

  Message msg{s.data(), s.size()};
  msg.normalize();

  {
    // YAML test
    Decoder decoder{&msg};

    EXPECT_EQ("", decoder.error());
    EXPECT_EQ(yaml, decoder.result().str());

    Encoder encoder{decoder.result()};

    EXPECT_EQ("", encoder.error());
    EXPECT_HEX(hex, encoder.data(), encoder.size());
  }

  {
    // JSON roundtrip test
    Decoder decoder{&msg, true};
    EXPECT_EQ("", decoder.error());

    Encoder encoder{decoder.result()};

    EXPECT_EQ("", encoder.error());
    EXPECT_HEX(hex, encoder.data(), encoder.size());
  }

  {
    // Simple fuzz test: remove 1 trailing byte
    if (msg.size() > sizeof(Header)) {
      msg.shrink(msg.size() - 1);
      msg.mutableHeader()->setLength(msg.size());
      Decoder decoder{&msg, true};

      // For the following message types, removing the last byte still yields a
      // valid message.
      std::array<OFPType, 6> allow = {{OFPT_ERROR, OFPT_ECHO_REQUEST,
                                       OFPT_ECHO_REPLY, OFPT_EXPERIMENTER,
                                       OFPT_PACKET_IN, OFPT_PACKET_OUT}};

      if (std::find(allow.begin(), allow.end(), msg.type()) == allow.end()) {
        // Expect an invalid message.
        EXPECT_NE("", decoder.error());
      } else {
        // Expect a valid message.
        EXPECT_EQ("", decoder.error());
      }
    }
  }
}

static void testDecodeOnly(const char *hex, const char *yaml) {
  auto s = HexToRawData(hex);

  Message msg{s.data(), s.size()};
  msg.normalize();

  // YAML test
  Decoder decoder{&msg};

  EXPECT_EQ("", decoder.error());
  EXPECT_EQ(yaml, decoder.result().str());
}

static void testDecodeFail(const char *hex) {
  auto s = HexToRawData(hex);

  Message msg{s.data(), s.size()};
  msg.normalize();

  Decoder decoder{&msg};

  EXPECT_EQ("Invalid data.", decoder.error());
  EXPECT_EQ("", decoder.result());
}

TEST(decoder, hellov1) {
  testDecodeEncode(
      "0100000800000001",
      "---\ntype:            HELLO\nxid:             "
      "0x00000001\nversion:         0x01\nmsg:             \n  versions:  "
      "      [  ]\n...\n");
}

TEST(decoder, hellov4) {
  testDecodeEncode(
      "04000010000000010001000800000012",
      "---\ntype:            HELLO\nxid:             "
      "0x00000001\nversion:         0x04\nmsg:             \n  versions:  "
      "      [ 1, 4 ]\n...\n");
}

TEST(decoder, errorv1) {
  testDecodeEncode(
      "010100130000006200010001FFFF1234567890",
      "---\ntype:            ERROR\nxid:             "
      "0x00000062\nversion:         0x01\nmsg:             \n  type:      "
      "      BAD_REQUEST\n  code:            OFPBRC_BAD_TYPE\n  data:    "
      "        "
      "FFFF1234567890\n...\n");
}

TEST(decoder, errorv1_2) {
  testDecodeEncode(
      "01010018000000620001000101111234567890AA00010000",
      "---\ntype:            ERROR\nxid:             0x00000062\nversion:      "
      "   0x01\nmsg:             \n  type:            BAD_REQUEST\n  code:     "
      "       OFPBRC_BAD_TYPE\n  data:            01111234567890AA00010000\n  "
      "_text:           'Type: OFPT_MULTIPART_REPLY.OFPMP_FLOW'\n...\n");
}

TEST(decoder, echoRequest) {
  testDecodeEncode("0102000E00000007AABBCCDDEEFF",
                   "---\ntype:            ECHO_REQUEST\nxid:            "
                   " 0x00000007\nversion:         0x01\nmsg:             \n  "
                   "data:            AABBCCDDEEFF\n...\n");
}

TEST(decoder, echoReply) {
  testDecodeEncode("0103000E00000007AABBCCDDEEFF",
                   "---\ntype:            ECHO_REPLY\nxid:             "
                   "0x00000007\nversion:         0x01\nmsg:             \n  "
                   "data:            AABBCCDDEEFF\n...\n");
}

TEST(decoder, experimenterv4) {
  testDecodeEncode("0404001800000018DEADBEEFAABBCCDDABCDEF0123456789",
                   "---\ntype:            EXPERIMENTER\nxid:             "
                   "0x00000018\nversion:         0x04\nmsg:             \n  "
                   "experimenter:    0xDEADBEEF\n  exp_type:        "
                   "0xAABBCCDD\n  data:            ABCDEF0123456789\n...\n");
}

TEST(decoder, experimenterv1) {
  testDecodeEncode(
      "010400140000001BDEADBEEFABCDEF0123456789",
      "---\ntype:            EXPERIMENTER\nxid:             "
      "0x0000001B\nversion:         0x01\nmsg:             \n  "
      "experimenter:    0xDEADBEEF\n  exp_type:        0x00000000\n  "
      "data:            ABCDEF0123456789\n...\n");
}

TEST(decoder, experimenterv2) {
  testDecodeEncode(
      "02040018000000FFDEADBEEF00000000ABCDEF0123456789",
      "---\ntype:            EXPERIMENTER\nxid:             "
      "0x000000FF\nversion:         0x02\nmsg:             \n  "
      "experimenter:    0xDEADBEEF\n  exp_type:        0x00000000\n  "
      "data:            ABCDEF0123456789\n...\n");
}

TEST(decoder, featuresrequest) {
  testDecodeEncode("04050008000000BF",
                   "---\ntype:            "
                   "FEATURES_REQUEST\nxid:          "
                   "   0x000000BF\nversion:         "
                   "0x04\nmsg:             \n...\n");
}

TEST(decoder, getconfigrequest) {
  testDecodeEncode("04070008000000BF",
                   "---\ntype:            "
                   "GET_CONFIG_REQUEST\nxid:        "
                   "     0x000000BF\nversion:         "
                   "0x04\nmsg:             \n...\n");
}

TEST(decoder, barrierrequestv4) {
  testDecodeEncode("04140008000000BF",
                   "---\ntype:            "
                   "BARRIER_REQUEST\nxid:           "
                   "  0x000000BF\nversion:         "
                   "0x04\nmsg:             \n...\n");
}

TEST(decoder, barrierrequestv1) {
  testDecodeEncode("01120008000000BF",
                   "---\ntype:            "
                   "BARRIER_REQUEST\nxid:           "
                   "  0x000000BF\nversion:         "
                   "0x01\nmsg:             \n...\n");
}

TEST(decoder, barrierreplyv4) {
  testDecodeEncode("04150008000000BF",
                   "---\ntype:            "
                   "BARRIER_REPLY\nxid:             "
                   "0x000000BF\nversion:         0x04\nmsg: "
                   "            \n...\n");
}

TEST(decoder, barrierreplyv1) {
  testDecodeEncode("01130008000000BF",
                   "---\ntype:            "
                   "BARRIER_REPLY\nxid:             "
                   "0x000000BF\nversion:         0x01\nmsg: "
                   "            \n...\n");
}

TEST(decoder, getasyncrequestv4) {
  testDecodeEncode("041A0008000000BF",
                   "---\ntype:            "
                   "GET_ASYNC_REQUEST\nxid:         "
                   "    0x000000BF\nversion:         "
                   "0x04\nmsg:             \n...\n");
}

TEST(decoder, getasyncrequestv1) {
  auto s = HexToRawData("011A0008000000BF");

  Message msg{s.data(), s.size()};
  msg.normalize();

  Decoder decoder{&msg};

  EXPECT_EQ("Invalid data.", decoder.error());
  EXPECT_EQ("", decoder.result());
}

TEST(decoder, featuresreplyv1) {
  testDecodeEncode(
      "01060020000000BF000001020304050600000100FF0000000000000000000000",
      "---\ntype:            FEATURES_REPLY\nxid:             "
      "0x000000BF\nversion:         0x01\nmsg:             \n  datapath_id:    "
      " '00:00:01:02:03:04:05:06'\n  n_buffers:       0x00000100\n  n_tables:  "
      "      0xFF\n  auxiliary_id:    0x00\n  capabilities:    [  ]\n  "
      "actions:         [  ]\n  ports:           \n...\n");
}

TEST(decoder, featuresreplyv1ports) {
  testDecodeEncode(
      "01060080000000BF000001020304050600000100FF000000000000000000000011112222"
      "22222222506F727420310000000000000000000033333333444444445555055566660666"
      "7777077788880888BBBBCCCCCCCCCCCC506F727420320000000000000000000033333333"
      "4444444455550555666606667777077788880888",
      "---\ntype:            FEATURES_REPLY\nxid:             "
      "0x000000BF\nversion:         0x01\nmsg:             \n  datapath_id:    "
      " '00:00:01:02:03:04:05:06'\n  n_buffers:       0x00000100\n  n_tables:  "
      "      0xFF\n  auxiliary_id:    0x00\n  capabilities:    [  ]\n  "
      "actions:         [  ]\n  ports:           \n    - port_no:         "
      "0x00001111\n      hw_addr:         '22:22:22:22:22:22'\n      name:     "
      "       Port 1\n      config:          [ PORT_DOWN, NO_STP, NO_FLOOD, "
      "NO_FWD, '0x33333300' ]\n      state:           [ LIVE, STP_LISTEN, "
      "'0x44444440' ]\n      ethernet:        \n        curr:            [ "
      "10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, FIBER, PAUSE, '0x55550000' ]\n      "
      "  advertised:      [ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, AUTONEG, "
      "PAUSE, '0x66660000' ]\n        supported:       [ 10MB_HD, 10MB_FD, "
      "100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, FIBER, AUTONEG, PAUSE, '0x77770000' "
      "]\n        peer:            [ 100MB_FD, COPPER, PAUSE_ASYM, "
      "'0x88880000' ]\n        curr_speed:      0x00000000\n        max_speed: "
      "      0x00000000\n      properties:      \n    - port_no:         "
      "0x0000BBBB\n      hw_addr:         'cc:cc:cc:cc:cc:cc'\n      name:     "
      "       Port 2\n      config:          [ PORT_DOWN, NO_STP, NO_FLOOD, "
      "NO_FWD, '0x33333300' ]\n      state:           [ LIVE, STP_LISTEN, "
      "'0x44444440' ]\n      ethernet:        \n        curr:            [ "
      "10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, FIBER, PAUSE, '0x55550000' ]\n      "
      "  advertised:      [ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, AUTONEG, "
      "PAUSE, '0x66660000' ]\n        supported:       [ 10MB_HD, 10MB_FD, "
      "100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, FIBER, AUTONEG, PAUSE, '0x77770000' "
      "]\n        peer:            [ 100MB_FD, COPPER, PAUSE_ASYM, "
      "'0x88880000' ]\n        curr_speed:      0x00000000\n        max_speed: "
      "      0x00000000\n      properties:      \n...\n");
}

TEST(decoder, featuresreplyv4) {
  testDecodeEncode(
      "04060020000000BF000001020304050600000100FF0000000000000000000000",
      "---\ntype:            FEATURES_REPLY\nxid:             "
      "0x000000BF\nversion:         0x04\nmsg:             \n  datapath_id:    "
      " '00:00:01:02:03:04:05:06'\n  n_buffers:       0x00000100\n  n_tables:  "
      "      0xFF\n  auxiliary_id:    0x00\n  capabilities:    [  ]\n  "
      "ports:           \n...\n");
}

TEST(decoder, ofmp_flowrequest_v4) {
  testDecodeEncode(
      "041200401122334400010000000000000100000000000002000000030000000000000000"
      "0000000400000000000000050001000C800000041234567800000000",
      "---\ntype:            REQUEST.FLOW\nflags:           [  ]\nxid:         "
      "    0x11223344\nversion:         0x04\nmsg:             \n  table_id:   "
      "     0x01\n  out_port:        0x00000002\n  out_group:       "
      "0x00000003\n  cookie:          0x0000000000000004\n  cookie_mask:     "
      "0x0000000000000005\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x12345678\n...\n");
}

TEST(decoder, ofmp_flowrequest_v1) {
  testDecodeEncode(
      "011000381122334400010000003820FE567"
      "8000000000000000000000000000000000000000000"
      "0000000000000000000000000011002222",
      "---\ntype:            REQUEST.FLOW\nflags:           [  ]\nxid:         "
      "    0x11223344\nversion:         0x01\nmsg:             \n  table_id:   "
      "     0x11\n  out_port:        0x00002222\n  out_group:       "
      "0x00000000\n  cookie:          0x0000000000000000\n  cookie_mask:     "
      "0x0000000000000000\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x00005678\n...\n");
}

TEST(decoder, ofmp_aggregaterequest_v4) {
  testDecodeEncode(
      "04120040112233440002000000000000110000002222222233333333000"
      "00000444444444444444455555555555555550001000C80000004123456"
      "7800000000",
      "---\ntype:            REQUEST.AGGREGATE\nflags:           [  ]\nxid:    "
      "         0x11223344\nversion:         0x04\nmsg:             \n  "
      "table_id:        0x11\n  out_port:        0x22222222\n  out_group:      "
      " 0x33333333\n  cookie:          0x4444444444444444\n  cookie_mask:     "
      "0x5555555555555555\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x12345678\n...\n");
}

TEST(decoder, ofmp_aggregaterequest_v1) {
  testDecodeEncode(
      "011000381122334400020000003820FE567"
      "8000000000000000000000000000000000000000000"
      "0000000000000000000000000011002222",
      "---\ntype:            REQUEST.AGGREGATE\nflags:           [  ]\nxid:    "
      "         0x11223344\nversion:         0x01\nmsg:             \n  "
      "table_id:        0x11\n  out_port:        0x00002222\n  out_group:      "
      " 0x00000000\n  cookie:          0x0000000000000000\n  cookie_mask:     "
      "0x0000000000000000\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x00005678\n...\n");
}

TEST(decoder, ofmp_flowreply_v4) {
  testDecodeEncode(
      "0413005811223344000100000000000000480100000000020000000300"
      "0400050006000700000000000000000000000800000000000000090000"
      "00000000000A0001000C8000000412345678000000000001000801000000",
      "---\ntype:            REPLY.FLOW\nflags:           [  ]\nxid:           "
      "  0x11223344\nversion:         0x04\nmsg:             \n  - table_id:   "
      "     0x01\n    duration:        2.000000003\n    priority:        "
      "0x0004\n    idle_timeout:    0x0005\n  "
      "  hard_timeout:    0x0006\n    flags:           [ SEND_FLOW_REM, "
      "CHECK_OVERLAP, RESET_COUNTS ]\n    cookie:          "
      "0x0000000000000008\n    packet_count:    0x0000000000000009\n    "
      "byte_count:      0x000000000000000A\n    match:           \n      - "
      "field:           IN_PORT\n        value:           0x12345678\n    "
      "instructions:    \n      - instruction:     GOTO_TABLE\n        "
      "table_id:        0x01\n...\n");
}

TEST(decoder, ofmp_flowreply2_v4) {
  testDecodeEncode(
      "041300A81122334400010000000000000040010000000002000000030"
      "004000500060007000000000000000000000008000000000000000900"
      "0000000000000A0001000C8000000412345678000000000058110000000022000"
      "000330044005500660077000000000000000000000088999999999999"
      "9999AAAAAAAAAAAAAAAA0001002080000004123456788000080610203"
      "040506080000606AABBCCDDEEFF0001000801000000",
      "---\ntype:            REPLY.FLOW\nflags:           [  ]\nxid:           "
      "  0x11223344\nversion:         0x04\nmsg:             \n  - table_id:   "
      "     0x01\n    duration:        2.000000003\n    priority:        "
      "0x0004\n    idle_timeout:    0x0005\n  "
      "  hard_timeout:    0x0006\n    flags:           [ SEND_FLOW_REM, "
      "CHECK_OVERLAP, RESET_COUNTS ]\n    cookie:          "
      "0x0000000000000008\n    packet_count:    0x0000000000000009\n    "
      "byte_count:      0x000000000000000A\n    match:           \n      - "
      "field:           IN_PORT\n        value:           0x12345678\n    "
      "instructions:    \n  - table_id:        0x11\n    duration:        "
      "34.000000051\n    priority:        "
      "0x0044\n    idle_timeout:    0x0055\n    hard_timeout:    0x0066\n    "
      "flags:           [ SEND_FLOW_REM, CHECK_OVERLAP, RESET_COUNTS, "
      "NO_BYT_COUNTS, '0x00000060' ]\n    cookie:          "
      "0x0000000000000088\n    packet_count:    0x9999999999999999\n    "
      "byte_count:      0xAAAAAAAAAAAAAAAA\n    match:           \n      - "
      "field:           IN_PORT\n        value:           0x12345678\n      - "
      "field:           ETH_SRC\n        value:           "
      "'10:20:30:40:50:60'\n      - field:           ETH_DST\n        value:   "
      "        'aa:bb:cc:dd:ee:ff'\n    instructions:    \n      - "
      "instruction:     GOTO_TABLE\n        table_id:        0x01\n...\n");
}

TEST(decoder, ofmp_flowreply_v1) {
  testDecodeEncode(
      "0111006C111111110001222200603300003820FEDDDD0000000000000000000000000000"
      "000000000000000000000000000000000000000044444444555555556666777788880000"
      "00000000AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCC00000008EEEEFFF"
      "F",
      "---\ntype:            REPLY.FLOW\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x01\nmsg:             "
      "\n  - table_id:        0x33\n    duration:        "
      "1145324612.x55555555\n    priority:        0x6666\n    "
      "idle_timeout:    0x7777\n    hard_timeout:    0x8888\n    flags:        "
      "   [  ]\n    cookie:          0xAAAAAAAAAAAAAAAA\n    packet_count:    "
      "0xBBBBBBBBBBBBBBBB\n    byte_count:      0xCCCCCCCCCCCCCCCC\n    match: "
      "          \n      - field:           IN_PORT\n        value:           "
      "0x0000DDDD\n    instructions:    \n      - instruction:     "
      "APPLY_ACTIONS\n        actions:         \n          - action:          "
      "OUTPUT\n            port_no:         0x0000EEEE\n            max_len:   "
      "      NO_BUFFER\n...\n");
}

TEST(decoder, ofmp_flowreply2_v1) {
  testDecodeEncode(
      "011100CC112233440001000000600100003820FE56780000000000000000000000000000"
      "000000000000000000000000000000000000000000000002000000030004000500060000"
      "0000000000000000000000080000000000000009000000000000000A00000008EEEEFFFF"
      "00601100003820F25678102030405060AABBCCDDEEFF0000000000000000000000000000"
      "000000000000000000000022000000330044005500660000000000000000000000000088"
      "9999999999999999AAAAAAAAAAAAAAAA00000008EEEEFFFF",
      "---\ntype:            REPLY.FLOW\nflags:           [  ]\nxid:           "
      "  0x11223344\nversion:         0x01\nmsg:             \n  - table_id:   "
      "     0x01\n    duration:        2.000000003\n    priority:        "
      "0x0004\n    idle_timeout:    0x0005\n  "
      "  hard_timeout:    0x0006\n    flags:           [  ]\n    cookie:       "
      "   0x0000000000000008\n    packet_count:    0x0000000000000009\n    "
      "byte_count:      0x000000000000000A\n    match:           \n      - "
      "field:           IN_PORT\n        value:           0x00005678\n    "
      "instructions:    \n      - instruction:     APPLY_ACTIONS\n        "
      "actions:         \n          - action:          OUTPUT\n            "
      "port_no:         0x0000EEEE\n            max_len:         NO_BUFFER\n  "
      "- table_id:        0x11\n    duration:        34.000000051\n    "
      "priority:        0x0044\n    "
      "idle_timeout:    0x0055\n    hard_timeout:    0x0066\n    flags:        "
      "   [  ]\n    cookie:          0x0000000000000088\n    packet_count:    "
      "0x9999999999999999\n    byte_count:      0xAAAAAAAAAAAAAAAA\n    match: "
      "          \n      - field:           IN_PORT\n        value:           "
      "0x00005678\n      - field:           ETH_SRC\n        value:           "
      "'10:20:30:40:50:60'\n      - field:           ETH_DST\n        value:   "
      "        'aa:bb:cc:dd:ee:ff'\n    instructions:    \n      - "
      "instruction:     APPLY_ACTIONS\n        actions:         \n          - "
      "action:          OUTPUT\n            port_no:         0x0000EEEE\n      "
      "      max_len:         NO_BUFFER\n...\n");
}

TEST(decoder, ofmp_aggregatereply_v4) {
  testDecodeEncode(
      "041300281111111100022222000000003333333333333330444444444444444055555550"
      "00000000",
      "---\ntype:            REPLY.AGGREGATE\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x04\nmsg:             "
      "\n  packet_count:    0x3333333333333330\n  byte_count:      "
      "0x4444444444444440\n  flow_count:      0x55555550\n...\n");
}

TEST(decoder, ofmp_aggregatereply_v1) {
  testDecodeEncode(
      "01110024111111110002222233333333333333304444444444444440555555500000000"
      "0",
      "---\ntype:            REPLY.AGGREGATE\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x01\nmsg:             "
      "\n  packet_count:    0x3333333333333330\n  byte_count:      "
      "0x4444444444444440\n  flow_count:      0x55555550\n...\n");
}

TEST(decoder, ofmp_aggregatereply_v6) {
  testDecodeEncode(
      "061300381111111100020000000000000000002480020808333333333333333080020A084444444444444440800206045555555000000000",
      "---\ntype:            REPLY.AGGREGATE\nflags:           [  ]\nxid:             0x11111111\nversion:         0x06\nmsg:             \n  packet_count:    0x3333333333333330\n  byte_count:      0x4444444444444440\n  flow_count:      0x55555550\n...\n");
}

TEST(decoder, ofmp_tablestats_v4) {
  testDecodeEncode(
      "041300401111111100032222000000003300000044444440555555555555555066666666"
      "6666666077000000888888809999999999999990AAAAAAAAAAAAAAA0",
      "---\ntype:            REPLY.TABLE\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x04\nmsg:             "
      "\n  - table_id:        0x33\n    name:            ''\n    wildcards:    "
      "   0x00000000\n    max_entries:     0x00000000\n    active_count:    "
      "0x44444440\n    lookup_count:    0x5555555555555550\n    matched_count: "
      "  0x6666666666666660\n  - table_id:        0x77\n    name:            "
      "''\n    wildcards:       0x00000000\n    max_entries:     0x00000000\n  "
      "  active_count:    0x88888880\n    lookup_count:    "
      "0x9999999999999990\n    matched_count:   0xAAAAAAAAAAAAAAA0\n...\n");
}

TEST(decoder, ofmp_tablestats_v1) {
  testDecodeEncode(
      "0111008C1111111100032222330000005461626C65203100000000000000000000000000"
      "000000000000000000000000444444405555555066666660777777777777777088888888"
      "88888880110000005461626C652032000000000000000000000000000000000000000000"
      "0000000099999990AAAAAAA0BBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0",
      "---\ntype:            REPLY.TABLE\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x01\nmsg:             "
      "\n  - table_id:        0x33\n    name:            Table 1\n    "
      "wildcards:       0x44444440\n    max_entries:     0x55555550\n    "
      "active_count:    0x66666660\n    lookup_count:    0x7777777777777770\n  "
      "  matched_count:   0x8888888888888880\n  - table_id:        0x11\n    "
      "name:            Table 2\n    wildcards:       0x99999990\n    "
      "max_entries:     0xAAAAAAA0\n    active_count:    0xBBBBBBB0\n    "
      "lookup_count:    0xCCCCCCCCCCCCCCC0\n    matched_count:   "
      "0xDDDDDDDDDDDDDDD0\n...\n");
}

TEST(decoder, ofmp_portstats_v4) {
  testDecodeEncode(
      "041300801111111100042222000000003333333000000000444444444444444055555555"
      "555555506666666666666660777777777777777088888888888888809999999999999990"
      "AAAAAAAAAAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEE"
      "EEEEEEE0FFFFFFFFFFFFFFF01111111022222220",
      "---\ntype:            REPLY.PORT_STATS\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x04\nmsg:             "
      "\n  - port_no:         0x33333330\n    duration:        "
      "286331152.572662304\n    rx_packets:      0x4444444444444440\n  "
      "  tx_packets:      0x5555555555555550\n    rx_bytes:        "
      "0x6666666666666660\n    tx_bytes:        0x7777777777777770\n    "
      "rx_dropped:      0x8888888888888880\n    tx_dropped:      "
      "0x9999999999999990\n    rx_errors:       0xAAAAAAAAAAAAAAA0\n    "
      "tx_errors:       0xBBBBBBBBBBBBBBB0\n    ethernet:        \n      "
      "rx_frame_err:    0xCCCCCCCCCCCCCCC0\n      rx_over_err:     "
      "0xDDDDDDDDDDDDDDD0\n      rx_crc_err:      0xEEEEEEEEEEEEEEE0\n      "
      "collisions:      0xFFFFFFFFFFFFFFF0\n    properties:      \n...\n");
}

TEST(decoder, ofmp_portstats_v3) {
  testDecodeEncode(
      "031300781111111100042222000000003333333000000000444444444444444055555555"
      "555555506666666666666660777777777777777088888888888888809999999999999990"
      "AAAAAAAAAAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEE"
      "EEEEEEE0FFFFFFFFFFFFFFF0",
      "---\ntype:            REPLY.PORT_STATS\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x03\nmsg:             "
      "\n  - port_no:         0x33333330\n    duration:        0\n    "
      "rx_packets:      0x4444444444444440\n  "
      "  tx_packets:      0x5555555555555550\n    rx_bytes:        "
      "0x6666666666666660\n    tx_bytes:        0x7777777777777770\n    "
      "rx_dropped:      0x8888888888888880\n    tx_dropped:      "
      "0x9999999999999990\n    rx_errors:       0xAAAAAAAAAAAAAAA0\n    "
      "tx_errors:       0xBBBBBBBBBBBBBBB0\n    ethernet:        \n      "
      "rx_frame_err:    0xCCCCCCCCCCCCCCC0\n      rx_over_err:     "
      "0xDDDDDDDDDDDDDDD0\n      rx_crc_err:      0xEEEEEEEEEEEEEEE0\n      "
      "collisions:      0xFFFFFFFFFFFFFFF0\n    properties:      \n...\n");
}

TEST(decoder, ofmp_portstats_v2) {
  testDecodeEncode(
      "021300781111111100042222000000003333333000000000444444444444444055555555"
      "555555506666666666666660777777777777777088888888888888809999999999999990"
      "AAAAAAAAAAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEE"
      "EEEEEEE0FFFFFFFFFFFFFFF0",
      "---\ntype:            REPLY.PORT_STATS\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x02\nmsg:             "
      "\n  - port_no:         0x33333330\n    duration:        0\n    "
      "rx_packets:      0x4444444444444440\n  "
      "  tx_packets:      0x5555555555555550\n    rx_bytes:        "
      "0x6666666666666660\n    tx_bytes:        0x7777777777777770\n    "
      "rx_dropped:      0x8888888888888880\n    tx_dropped:      "
      "0x9999999999999990\n    rx_errors:       0xAAAAAAAAAAAAAAA0\n    "
      "tx_errors:       0xBBBBBBBBBBBBBBB0\n    ethernet:        \n      "
      "rx_frame_err:    0xCCCCCCCCCCCCCCC0\n      rx_over_err:     "
      "0xDDDDDDDDDDDDDDD0\n      rx_crc_err:      0xEEEEEEEEEEEEEEE0\n      "
      "collisions:      0xFFFFFFFFFFFFFFF0\n    properties:      \n...\n");
}

TEST(decoder, ofmp_portstats_v1) {
  testDecodeEncode(
      "011100741111111100042222333000000000000044444444444444405555555555555550"
      "6666666666666660777777777777777088888888888888809999999999999990AAAAAAAA"
      "AAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEEEEEEEEE0"
      "FFFFFFFFFFFFFFF0",
      "---\ntype:            REPLY.PORT_STATS\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x01\nmsg:             "
      "\n  - port_no:         0x00003330\n    duration:        0\n    "
      "rx_packets:      0x4444444444444440\n  "
      "  tx_packets:      0x5555555555555550\n    rx_bytes:        "
      "0x6666666666666660\n    tx_bytes:        0x7777777777777770\n    "
      "rx_dropped:      0x8888888888888880\n    tx_dropped:      "
      "0x9999999999999990\n    rx_errors:       0xAAAAAAAAAAAAAAA0\n    "
      "tx_errors:       0xBBBBBBBBBBBBBBB0\n    ethernet:        \n      "
      "rx_frame_err:    0xCCCCCCCCCCCCCCC0\n      rx_over_err:     "
      "0xDDDDDDDDDDDDDDD0\n      rx_crc_err:      0xEEEEEEEEEEEEEEE0\n      "
      "collisions:      0xFFFFFFFFFFFFFFF0\n    properties:      \n...\n");
}

TEST(decoder, ofmp_queuestats_v4) {
  testDecodeEncode(
      "041300381111111100052222000000003333333044444440555555555555555066666666"
      "6666666077777777777777701111111022222220",
      "---\ntype:            REPLY.QUEUE\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x04\nmsg:             "
      "\n  - port_no:         0x33333330\n    queue_id:        0x44444440\n    "
      "tx_packets:      0x6666666666666660\n    tx_bytes:        "
      "0x5555555555555550\n    tx_errors:       0x7777777777777770\n    "
      "duration:        286331152.572662304\n...\n");
}

TEST(decoder, ofmp_queuestats_v1) {
  testDecodeEncode(
      "0111002C1111111100052222333000004444444055555555555555506666666666666660"
      "7777777777777770",
      "---\ntype:            REPLY.QUEUE\nflags:           [ '0x00002222' "
      "]\nxid:             0x11111111\nversion:         0x01\nmsg:             "
      "\n  - port_no:         0x00003330\n    queue_id:        0x44444440\n    "
      "tx_packets:      0x6666666666666660\n    tx_bytes:        "
      "0x5555555555555550\n    tx_errors:       0x7777777777777770\n    "
      "duration:        0\n...\n");
}

TEST(decoder, flowmodv4) {
  testDecodeEncode(
      "040E00680000000100000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000001001A800000040000000D80000A02080080001804C0A8"
      "010100000000000000040018000000000019001080001804C0A8020100000000",
      "---\ntype:            FLOW_MOD\nxid:             "
      "0x00000001\nversion:         0x04\nmsg:             \n  cookie:         "
      " "
      "0x0000000000000000\n  cookie_mask:     0x0000000000000000\n  table_id:  "
      "      0x00\n  command:         ADD\n  idle_timeout:    0x0000\n  "
      "hard_timeout:    0x0000\n  priority:        0x0000\n  buffer_id:       "
      "0x00000000\n  out_port:        0x00000000\n  out_group:       "
      "0x00000000\n  flags:           [  ]\n  match:           \n    - field:"
      "           IN_PORT\n      value:           0x0000000D\n    - field: "
      "        "
      "  ETH_TYPE\n      value:           0x0800\n    - field:           "
      "IPV4_DST\n      value:           192.168.1.1\n  instructions:    \n "
      "   - instruction:     APPLY_ACTIONS\n      actions:         \n    "
      "    - action:          SET_FIELD\n          field:           "
      "IPV4_DST\n          value:           192.168.2.1\n...\n");
}

TEST(decoder, flowmodv4_2) {
  testDecodeEncode(
      "040E00680000000111111111111111112222222222222222334455556666777788888888"
      "99999999AAAAAAAABBBB00000001001A80000004CCCCCCCC80000A02080080001804C0A8"
      "010100000000000000040018000000000019001080001804C0A8020100000000",
      "---\ntype:            FLOW_MOD\nxid:             "
      "0x00000001\nversion:         0x04\nmsg:             \n  cookie:         "
      " "
      "0x1111111111111111\n  cookie_mask:     0x2222222222222222\n  table_id:  "
      "      0x33\n  command:         0x44\n  idle_timeout:    0x5555\n  "
      "hard_timeout:    0x6666\n  priority:        0x7777\n  buffer_id:       "
      "0x88888888\n  out_port:        0x99999999\n  out_group:       "
      "0xAAAAAAAA\n  flags:           [ SEND_FLOW_REM, CHECK_OVERLAP, "
      "NO_PKT_COUNTS, NO_BYT_COUNTS, '0x0000BBA0' ]\n  match:           \n    "
      "- field:"
      "           IN_PORT\n      value:           0xCCCCCCCC\n    - field: "
      "          ETH_TYPE\n      value:           0x0800\n    - field:     "
      "  "
      "    IPV4_DST\n      value:           192.168.1.1\n  instructions:   "
      " \n    - instruction:     APPLY_ACTIONS\n      actions:         "
      "\n        - action:          SET_FIELD\n          field:          "
      " IPV4_DST\n          value:           192.168.2.1\n...\n");
}

TEST(decoder, flowmodv4_ipv6) {
  testDecodeEncode(
      "040E00680000000111111111111111102222222222222220304055506660777088888880"
      "99999990AAAAAAA0BBB100000001003280000A0286DD8000341000000000000000000000"
      "FFFFC0A800018000361020000000000000000000000000000001000000000000",
      "---\ntype:            FLOW_MOD\nxid:             0x00000001\nversion:   "
      "      0x04\nmsg:             \n  cookie:          0x1111111111111110\n  "
      "cookie_mask:     0x2222222222222220\n  table_id:        0x30\n  "
      "command:         0x40\n  idle_timeout:    0x5550\n  hard_timeout:    "
      "0x6660\n  priority:        0x7770\n  buffer_id:       0x88888880\n  "
      "out_port:        0x99999990\n  out_group:       0xAAAAAAA0\n  flags:    "
      "       [ SEND_FLOW_REM, NO_BYT_COUNTS, '0x0000BBA0' ]\n  match:         "
      "  \n    - field:           ETH_TYPE\n      value:           0x86DD\n    "
      "- field:           IPV6_SRC\n      value:           "
      "'::ffff:192.168.0.1'\n    - field:           IPV6_DST\n      value:     "
      "      '2000::1'\n  instructions:    \n...\n");
}

TEST(decoder, flowmodv4_experimenter_1) {
  testDecodeEncode(
      "040E00900000000100000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000001005FFFFF024400FFFFFF050102030405000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000FFFF060600FFFFFF1234FFFF020500FFFFFE010"
      "0",
      "---\ntype:            FLOW_MOD\nxid:             0x00000001\nversion:   "
      "      0x04\nmsg:             \n  cookie:          0x0000000000000000\n  "
      "cookie_mask:     0x0000000000000000\n  table_id:        0x00\n  "
      "command:         ADD\n  idle_timeout:    0x0000\n  hard_timeout:    "
      "0x0000\n  priority:        0x0000\n  buffer_id:       0x00000000\n  "
      "out_port:        0x00000000\n  out_group:       0x00000000\n  flags:    "
      "       [  ]\n  match:           \n    - field:           "
      "X_LLDP_CHASSIS_ID\n      value:           'chassis 02030405'\n    - "
      "field:           X_LLDP_TTL\n      value:           0x1234\n    - "
      "field:           0xFFFF0205.0x00FFFFFE\n      value:           01\n  "
      "instructions:    \n...\n");
}

TEST(decoder, flowmodv4_experimenter_mask_1) {
  testDecodeEncode(
      "040E00900000000100000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000010060FFFF024400FFFFFF050102030405000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000FFFF060600FFFFFF1234FFFF030600FFFFFE01F"
      "F",
      "---\ntype:            FLOW_MOD\nxid:             0x00000001\nversion:   "
      "      0x04\nmsg:             \n  cookie:          0x0000000000000000\n  "
      "cookie_mask:     0x0000000000000000\n  table_id:        0x00\n  "
      "command:         ADD\n  idle_timeout:    0x0000\n  hard_timeout:    "
      "0x0000\n  priority:        0x0000\n  buffer_id:       0x00000000\n  "
      "out_port:        0x00000000\n  out_group:       0x00000000\n  flags:    "
      "       [  ]\n  match:           \n    - field:           "
      "X_LLDP_CHASSIS_ID\n      value:           'chassis 02030405'\n    - "
      "field:           X_LLDP_TTL\n      value:           0x1234\n    - "
      "field:           0xFFFF0306.0x00FFFFFE\n      value:           01\n     "
      " mask:            FF\n  instructions:    \n...\n");
}

TEST(decoder, flowmodv4_experimenter_2) {
  testDecodeEncode(
      "040E00900000000100000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000010060FFFF024400FFFFFF050102030405000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000FFFF060600FFFFFF1234FFFF060600FFFFFE000"
      "1",
      "---\ntype:            FLOW_MOD\nxid:             0x00000001\nversion:   "
      "      0x04\nmsg:             \n  cookie:          0x0000000000000000\n  "
      "cookie_mask:     0x0000000000000000\n  table_id:        0x00\n  "
      "command:         ADD\n  idle_timeout:    0x0000\n  hard_timeout:    "
      "0x0000\n  priority:        0x0000\n  buffer_id:       0x00000000\n  "
      "out_port:        0x00000000\n  out_group:       0x00000000\n  flags:    "
      "       [  ]\n  match:           \n    - field:           "
      "X_LLDP_CHASSIS_ID\n      value:           'chassis 02030405'\n    - "
      "field:           X_LLDP_TTL\n      value:           0x1234\n    - "
      "field:           X_EXPERIMENTER_01\n      value:           0x0001\n  "
      "instructions:    \n...\n");
}

TEST(decoder, flowmodv4_experimenter_mask_2) {
  testDecodeEncode(
      "040E00980000000100000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000010062FFFF024400FFFFFF050102030405000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000FFFF060600FFFFFF1234FFFF070800FFFFFE0001"
      "00FF000000000000",
      "---\ntype:            FLOW_MOD\nxid:             0x00000001\nversion:   "
      "      0x04\nmsg:             \n  cookie:          0x0000000000000000\n  "
      "cookie_mask:     0x0000000000000000\n  table_id:        0x00\n  "
      "command:         ADD\n  idle_timeout:    0x0000\n  hard_timeout:    "
      "0x0000\n  priority:        0x0000\n  buffer_id:       0x00000000\n  "
      "out_port:        0x00000000\n  out_group:       0x00000000\n  flags:    "
      "       [  ]\n  match:           \n    - field:           "
      "X_LLDP_CHASSIS_ID\n      value:           'chassis 02030405'\n    - "
      "field:           X_LLDP_TTL\n      value:           0x1234\n    - "
      "field:           X_EXPERIMENTER_01\n      value:           0x0001\n     "
      " mask:            0x00FF\n  instructions:    \n...\n");
}

TEST(decoder, flowmodv4_minimal) {
  testDecodeEncode(
      "040E003800000001000000000000000000000000000000000000000000000000FFFFFFFF"
      "FFFFFFFFFFFFFFFF000000000001000400000000",
      "---\ntype:            FLOW_MOD\nxid:             0x00000001\nversion:   "
      "      0x04\nmsg:             \n  cookie:          0x0000000000000000\n  "
      "cookie_mask:     0x0000000000000000\n  table_id:        0x00\n  "
      "command:         ADD\n  idle_timeout:    0x0000\n  hard_timeout:    "
      "0x0000\n  priority:        0x0000\n  buffer_id:       NO_BUFFER\n  "
      "out_port:        ANY\n  out_group:       ANY\n  flags:           [  ]\n "
      " match:           \n  instructions:    \n...\n");
}

TEST(decoder, flowmodv1) {
  const char *hex =
      "010E005000000001003020EECCCC000000000000000000000000000000"
      "0008000000000000000000C0A801010000000011111111111111110044"
      "555566667777888888889999BBBB00070008C0A80201";
  const char *yaml =
      "---\ntype:            FLOW_MOD\nxid:             "
      "0x00000001\nversion:         0x01\nmsg:             \n  cookie:         "
      " "
      "0x1111111111111111\n  cookie_mask:     0xFFFFFFFFFFFFFFFF\n  table_id:  "
      "      0x00\n  command:         0x44\n  idle_timeout:    0x5555\n  "
      "hard_timeout:    0x6666\n  priority:        0x7777\n  buffer_id:       "
      "0x88888888\n  out_port:        0x00009999\n  out_group:       "
      "0x00000000\n  flags:           [ SEND_FLOW_REM, CHECK_OVERLAP, "
      "NO_PKT_COUNTS, NO_BYT_COUNTS, '0x0000BBA0' ]\n  match:           \n    "
      "- field:"
      "           IN_PORT\n      value:           0x0000CCCC\n    - field: "
      "     "
      "     ETH_TYPE\n      value:           0x0800\n    - field:          "
      " "
      "IPV4_DST\n      value:           192.168.1.1\n  instructions:    \n "
      "   - instruction:     APPLY_ACTIONS\n      actions:         \n    "
      "    - action:          SET_FIELD\n          field:           "
      "IPV4_DST\n          value:           192.168.2.1\n...\n";

  testDecodeEncode(hex, yaml);
}

TEST(decoder, flowmod1_2) {
  testDecodeEncode(
      "010E0058000000010030208ECCCC00000000000000000000000000000000080000010000"
      "00000000C0A8010100DD000011111111111111110044555566667777888888889999BBBB"
      "000A0008EE000000000B000800000000",
      "---\ntype:            FLOW_MOD\nxid:             "
      "0x00000001\nversion:         0x01\nmsg:             \n  cookie:         "
      " "
      "0x1111111111111111\n  cookie_mask:     0xFFFFFFFFFFFFFFFF\n  table_id:  "
      "      0x00\n  command:         0x44\n  idle_timeout:    0x5555\n  "
      "hard_timeout:    0x6666\n  priority:        0x7777\n  buffer_id:       "
      "0x88888888\n  out_port:        0x00009999\n  out_group:       "
      "0x00000000\n  flags:           [ SEND_FLOW_REM, CHECK_OVERLAP, "
      "NO_PKT_COUNTS, NO_BYT_COUNTS, '0x0000BBA0' ]\n  match:           \n    "
      "- field:"
      "           IN_PORT\n      value:           0x0000CCCC\n    - field: "
      "     "
      "     ETH_TYPE\n      value:           0x0800\n    - field:          "
      " "
      "IP_PROTO\n      value:           0x01\n    - field:           "
      "IPV4_DST\n      value:           192.168.1.1\n    - field:          "
      " ICMPV4_TYPE\n      value:           0xDD\n  instructions:    \n    "
      "- instruction:     APPLY_ACTIONS\n      actions:         \n       "
      " - action:          SET_FIELD\n          field:           "
      "ICMPV4_CODE\n          value:           0xEE\n        - action:     "
      " "
      "    COPY_TTL_OUT\n...\n");
}

TEST(decoder, flowmod1_modvlan) {
  testDecodeEncode(
      "010E005000000001003820FEC3C400000000000000000000000000000000000000000000"
      "00000000000000000000000011111111111111110044515261627172818283849394B1B2"
      "00010008822C0000",
      "---\ntype:            FLOW_MOD\nxid:             0x00000001\nversion:   "
      "      0x01\nmsg:             \n  cookie:          0x1111111111111111\n  "
      "cookie_mask:     0xFFFFFFFFFFFFFFFF\n  table_id:        0x00\n  "
      "command:         0x44\n  idle_timeout:    0x5152\n  hard_timeout:    "
      "0x6162\n  priority:        0x7172\n  buffer_id:       0x81828384\n  "
      "out_port:        0x00009394\n  out_group:       0x00000000\n  flags:    "
      "       [ CHECK_OVERLAP, NO_BYT_COUNTS, '0x0000B1A0' ]\n  match:         "
      "  \n    - field:           IN_PORT\n      value:           0x0000C3C4\n "
      " instructions:    \n    - instruction:     APPLY_ACTIONS\n      "
      "actions:         \n        - action:          SET_FIELD\n          "
      "field:           VLAN_VID\n          value:           0x822C\n...\n");
}

TEST(decoder, flowmodv6) {
  testDecodeEncode(
      "060E00680000000111111111111111102222222222222220304055506660777088888880"
      "99999990AAAAAAA0BBB1CCC10001003280000A0286DD8000341000000000000000000000"
      "FFFFC0A800018000361020000000000000000000000000000001000000000000",
      "---\ntype:            FLOW_MOD\nxid:             0x00000001\nversion:   "
      "      0x06\nmsg:             \n  cookie:          0x1111111111111110\n  "
      "cookie_mask:     0x2222222222222220\n  table_id:        0x30\n  "
      "command:         0x40\n  idle_timeout:    0x5550\n  hard_timeout:    "
      "0x6660\n  priority:        0x7770\n  buffer_id:       0x88888880\n  "
      "out_port:        0x99999990\n  out_group:       0xAAAAAAA0\n  flags:    "
      "       [ SEND_FLOW_REM, NO_BYT_COUNTS, '0x0000BBA0' ]\n  importance:    "
      "  0xCCC1\n  match:           \n    - field:           ETH_TYPE\n      "
      "value:           0x86DD\n    - field:           IPV6_SRC\n      value:  "
      "         '::ffff:192.168.0.1'\n    - field:           IPV6_DST\n      "
      "value:           '2000::1'\n  instructions:    \n...\n");
}

TEST(decoder, packetinv4) {
  testDecodeEncode(
      "040A0064000000013333333344440188999999999999999900010020 "
      "80000004555555558000020466666666800004087777777777777777000"
      "0FFFFFFFFFFFF000000000001080600010800060400010000000000010A"
      "0000010000000000000A000002",
      "---\ntype:            PACKET_IN\nxid:             "
      "0x00000001\nversion:         0x04\nmsg:             \n  buffer_id:      "
      " 0x33333333\n  total_len:       0x4444\n  in_port:         0x55555555\n "
      " in_phy_port:     0x66666666\n  metadata:        0x7777777777777777\n  "
      "reason:          APPLY_ACTION\n  table_id:        0x88\n  cookie:       "
      " "
      "  0x9999999999999999\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x55555555\n    - field:           "
      "IN_PHY_PORT\n      value:           0x66666666\n    - field:        "
      "   METADATA\n      value:           0x7777777777777777\n  data:     "
      "       "
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002\n...\n");
}

TEST(decoder, packetinv1) {
  testDecodeEncode(
      "010A003C0000000233333333444455550100FFFFFFFFFFFF00000000000"
      "1080600010800060400010000000000010A0000010000000000000A0000"
      "02",
      "---\ntype:            PACKET_IN\nxid:             "
      "0x00000002\nversion:         0x01\nmsg:             \n  buffer_id:      "
      " 0x33333333\n  total_len:       0x4444\n  in_port:         0x00005555\n "
      " in_phy_port:     0x00000000\n  metadata:        0x0000000000000000\n  "
      "reason:          APPLY_ACTION\n  table_id:        0x00\n  cookie:       "
      " "
      "  0x0000000000000000\n  match:           \n  data:            "
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002\n...\n");
}

TEST(decoder, packetinv2) {
  testDecodeEncode(
      "020A00420000000233333333555555556666666644440188FFFFFFFFFFFF000000000001"
      "080600010800060400010000000000010A0000010000000000000A000002",
      "---\ntype:            PACKET_IN\nxid:             0x00000002\nversion:  "
      "       0x02\nmsg:             \n  buffer_id:       0x33333333\n  "
      "total_len:       0x4444\n  in_port:         0x55555555\n  in_phy_port:  "
      "   0x66666666\n  metadata:        0x0000000000000000\n  reason:         "
      " APPLY_ACTION\n  table_id:        0x88\n  cookie:          "
      "0x0000000000000000\n  match:           \n  data:            "
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002\n...\n");
}

TEST(decoder, packetoutv4) {
  testDecodeEncode(
      "040D00620000000133333333444444440020000000000000000000100000000500140000"
      "000000000019001080001804C0A8010100000000FFFFFFFFFFFF00000000000108060001"
      "0800060400010000000000010A0000010000000000000A000002",
      "---\ntype:            PACKET_OUT\nxid:             "
      "0x00000001\nversion:         0x04\nmsg:             \n  buffer_id:      "
      " "
      "0x33333333\n  in_port:         0x44444444\n  actions:         \n    - "
      "action:          OUTPUT\n      port_no:         0x00000005\n      "
      "max_len: "
      "        0x0014\n    - action:          SET_FIELD\n      field:    "
      "    "
      "   IPV4_DST\n      value:           192.168.1.1\n  data:            "
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002\n...\n");
}

TEST(decoder, packetoutv1) {
  const char *hex =
      "010D004A000000013333333344440010000000080005001400070008C0"
      "A80101FFFFFFFFFFFF0000000000010806000108000604000100000000"
      "00010A0000010000000000000A000002";
  const char *yaml =
      "---\ntype:            PACKET_OUT\nxid:             "
      "0x00000001\nversion:         0x01\nmsg:             \n  buffer_id:      "
      " "
      "0x33333333\n  in_port:         0x00004444\n  actions:         \n    - "
      "action: "
      "         OUTPUT\n      port_no:         0x00000005\n      "
      "max_len:         "
      "0x0014\n    - action:          SET_FIELD\n      field:           "
      "IPV4_DST\n      value:           192.168.1.1\n  data:            "
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002\n...\n";

  testDecodeEncode(hex, yaml);
}

TEST(decoder, packetoutv6) {
  testDecodeEncode(
      "060D007A0000000133333333002000000001001A80000A02080080001604C0A801028000"
      "000444444444000000000000000000100000000500140000000000000019001080001804"
      "C0A8010100000000FFFFFFFFFFFF00000000000108060001080006040001000000000001"
      "0A0000010000000000000A000002",
      "---\ntype:            PACKET_OUT\nxid:             0x00000001\nversion: "
      "        0x06\nmsg:             \n  buffer_id:       0x33333333\n  "
      "in_port:         0x44444444\n  match:           \n    - field:          "
      " ETH_TYPE\n      value:           0x0800\n    - field:           "
      "IPV4_SRC\n      value:           192.168.1.2\n    - field:           "
      "IN_PORT\n      value:           0x44444444\n  actions:         \n    - "
      "action:          OUTPUT\n      port_no:         0x00000005\n      "
      "max_len:         0x0014\n    - action:          SET_FIELD\n      field: "
      "          IPV4_DST\n      value:           192.168.1.1\n  data:         "
      "   "
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002\n...\n");
}

TEST(decoder, setconfigv4) {
  testDecodeEncode("0409000C11111111AAAABBBB",
                   "---\ntype:            SET_CONFIG\nxid:             "
                   "0x11111111\nversion:         0x04\nmsg:             \n  "
                   "flags:           [ FRAG_REASM, '0x0000AAA8' ]\n  "
                   "miss_send_len:   0xBBBB\n...\n");
}

TEST(decoder, portstatusv4) {
  testDecodeEncode(
      "040C00501111111122000000000000003333333300000000AABBCCDDEEFF0000506F7274"
      "203100000000000000000000444444445555555566666666777777778888888899999999"
      "AAAAAAAABBBBBBBB",
      "---\ntype:            PORT_STATUS\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  reason:         "
      " 0x22\n  port_no:         0x33333333\n  hw_addr:         "
      "'aa:bb:cc:dd:ee:ff'\n  name:            Port 1\n  config:          [ "
      "NO_RECV, NO_PACKET_IN, '0x44444400' ]\n  state:           [ LINK_DOWN, "
      "LIVE, '0x55555550' ]\n  ethernet:        \n    curr:            [ "
      "10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, "
      "'0x66660000' ]\n    advertised:      [ 10MB_HD, 10MB_FD, 100MB_HD, "
      "1GB_HD, 1GB_FD, 10GB_FD, 100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, "
      "PAUSE, '0x77770000' ]\n    supported:       [ 100MB_FD, 40GB_FD, "
      "COPPER, PAUSE_ASYM, '0x88880000' ]\n    peer:            [ 10MB_HD, "
      "100MB_FD, 1GB_HD, 40GB_FD, 100GB_FD, COPPER, FIBER, PAUSE_ASYM, "
      "'0x99990000' ]\n    curr_speed:      0xAAAAAAAA\n    max_speed:       "
      "0xBBBBBBBB\n  properties:      \n...\n");
}

TEST(decoder, portstatusv1) {
  testDecodeEncode(
      "010C00401111111122000000000000003333AABBCCDDEEFF506F72742031000000000000"
      "00000000444444445555555566660666777707778888088899990999",
      "---\ntype:            PORT_STATUS\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  reason:         "
      " 0x22\n  port_no:         0x00003333\n  hw_addr:         "
      "'aa:bb:cc:dd:ee:ff'\n  name:            Port 1\n  config:          [ "
      "NO_RECV, NO_PACKET_IN, '0x44444400' ]\n  state:           [ LINK_DOWN, "
      "LIVE, STP_LEARN, '0x55555450' ]\n  ethernet:        \n    curr:         "
      "   [ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, AUTONEG, PAUSE, '0x66660000' "
      "]\n    advertised:      [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, "
      "10GB_FD, FIBER, AUTONEG, PAUSE, '0x77770000' ]\n    supported:       [ "
      "100MB_FD, COPPER, PAUSE_ASYM, '0x88880000' ]\n    peer:            [ "
      "10MB_HD, 100MB_FD, 1GB_HD, COPPER, FIBER, PAUSE_ASYM, '0x99990000' ]\n  "
      "  curr_speed:      0x00000000\n    max_speed:       0x00000000\n  "
      "properties:      \n...\n");
}

TEST(decoder, groupmodv4) {
  testDecodeEncode(
      "040F00681111111122223300444444440030555566666666777777770000000000000010"
      "0000000500140000000000000019001080001804C0A80101000000000028888899999999"
      "AAAAAAAA000000000019001080002801EE00000000000000000B000800000000",
      "---\ntype:            GROUP_MOD\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  command:        "
      " "
      "0x2222\n  type:            0x33\n  group_id:        0x44444444\n  "
      "buckets:         \n    - weight:          0x5555\n      watch_port:     "
      " "
      "0x66666666\n      watch_group:     0x77777777\n      actions:         "
      "\n        - action:          OUTPUT\n          port_no:         "
      "0x00000005\n          max_len:         0x0014\n        - action:        "
      "  "
      "SET_FIELD\n          field:           IPV4_DST\n          "
      "value:           192.168.1.1\n    - weight:          0x8888\n      "
      "watch_port:      0x99999999\n      watch_group:     0xAAAAAAAA\n      "
      "actions:         \n        - action:          SET_FIELD\n         "
      " field:           ICMPV4_CODE\n          value:           0xEE\n    "
      " "
      "   - action:          COPY_TTL_OUT\n...\n");
}

TEST(decoder, portmodv4) {
  testDecodeEncode(
      "04100028111111112222222200000000333333333333000044444444555"
      "555556666666600000000",
      "---\ntype:            PORT_MOD\nxid:             0x11111111\nversion:   "
      "      0x04\nmsg:             \n  port_no:         0x22222222\n  "
      "hw_addr:         '33:33:33:33:33:33'\n  config:          [ NO_RECV, "
      "NO_PACKET_IN, '0x44444400' ]\n  mask:            [ PORT_DOWN, NO_RECV, "
      "NO_FLOOD, NO_PACKET_IN, '0x55555500' ]\n  ethernet:        \n    "
      "advertise:       [ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, 1TB_FD, OTHER, "
      "AUTONEG, PAUSE, '0x66660000' ]\n  properties:      \n...\n");
}

TEST(decoder, portmodv1) {
  testDecodeEncode(
      "010F002011111111222233333333333344444444555555556666066600000000",
      "---\ntype:            PORT_MOD\nxid:             0x11111111\nversion:   "
      "      0x01\nmsg:             \n  port_no:         0x00002222\n  "
      "hw_addr:         '33:33:33:33:33:33'\n  config:          [ NO_RECV, "
      "NO_PACKET_IN, '0x44444400' ]\n  mask:            [ PORT_DOWN, NO_RECV, "
      "NO_FLOOD, NO_PACKET_IN, '0x55555500' ]\n  ethernet:        \n    "
      "advertise:       [ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, AUTONEG, PAUSE, "
      "'0x66660000' ]\n  properties:      \n...\n");
}

TEST(decoder, portmodv5) {
  testDecodeEncode(
      "051000780000000000000001000000000011000011110000000000000000000000000008"
      "000010000001001800000003000005DC000007D000000BB80000012CFFFF000C00000065"
      "0000000000000000FFFF0010000000650000000100000001FFFF00140000006500000002"
      "000000010000000200000000",
      "---\ntype:            PORT_MOD\nxid:             0x00000000\nversion:   "
      "      0x05\nmsg:             \n  port_no:         0x00000001\n  "
      "hw_addr:         '00:11:00:00:11:11'\n  config:          [  ]\n  mask:  "
      "          [  ]\n  ethernet:        \n    advertise:       [ FIBER ]\n  "
      "optical:         \n    configure:       [ RX_TUNE, TX_TUNE ]\n    "
      "freq_lmda:       0x000005DC\n    fl_offset:       2000\n    grid_span:  "
      "     0x00000BB8\n    tx_pwr:          0x0000012C\n  properties:      \n "
      "   - property:        EXPERIMENTER\n      experimenter:    0x00000065\n "
      "     exp_type:        0x00000000\n      data:            ''\n    - "
      "property:        EXPERIMENTER\n      experimenter:    0x00000065\n      "
      "exp_type:        0x00000001\n      data:            00000001\n    - "
      "property:        EXPERIMENTER\n      experimenter:    0x00000065\n      "
      "exp_type:        0x00000002\n      data:            "
      "0000000100000002\n...\n");
}

TEST(decoder, tablemodv4) {
  testDecodeEncode("04110010111111112200000033333330",
                   "---\ntype:            TABLE_MOD\nxid:             "
                   "0x11111111\nversion:         0x04\nmsg:             \n  "
                   "table_id:        0x22\n  config:          [ '0x33333330' "
                   "]\n  properties:      \n...\n");
}

TEST(decoder, tablemodv2) {
  testDecodeEncode("02110010111111112200000033333330",
                   "---\ntype:            TABLE_MOD\nxid:             "
                   "0x11111111\nversion:         0x02\nmsg:             \n  "
                   "table_id:        0x22\n  config:          [ "
                   "TABLE_MISS_CONTROLLER, '0x33333330' ]\n  properties:      "
                   "\n...\n");
}

TEST(decoder, rolerequestv4) {
  testDecodeEncode(
      "041800181111111122222222000000003333333333333333",
      "---\ntype:            ROLE_REQUEST\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  role: "
      "           0x22222222\n  generation_id:   "
      "0x3333333333333333\n...\n");
}

TEST(decoder, rolereplyv4) {
  testDecodeEncode(
      "041900181111111122222222000000003333333333333333",
      "---\ntype:            ROLE_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  role: "
      "           0x22222222\n  generation_id:   "
      "0x3333333333333333\n...\n");
}

TEST(decoder, getasyncreplyv4) {
  testDecodeEncode(
      "041B002011111111222222223333333344444444555555556666666677777777",
      "---\ntype:            GET_ASYNC_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  "
      "packet_in_slave: [ TABLE_MISS, APPLY_ACTION, GROUP, PACKET_OUT, "
      "'0x33333300' ]\n  packet_in_master: [ APPLY_ACTION, PACKET_OUT, "
      "'0x22222200' ]\n  port_status_slave: [ ADD, MODIFY, '0x55555550' ]\n  "
      "port_status_master: [ MODIFY, '0x44444440' ]\n  flow_removed_slave: [ "
      "IDLE_TIMEOUT, HARD_TIMEOUT, DELETE, METER_DELETE, EVICTION, "
      "'0x77777740' ]\n  flow_removed_master: [ HARD_TIMEOUT, DELETE, "
      "EVICTION, '0x66666640' ]\n  properties:      \n...\n");
}

TEST(decoder, getasyncreplyv5) {
  testDecodeEncode(
      "051B00381111111100000008333333330001000822222222000200085555555500030008"
      "4444444400040008777777770005000866666666",
      "---\ntype:            GET_ASYNC_REPLY\nxid:             "
      "0x11111111\nversion:         0x05\nmsg:             \n  "
      "packet_in_slave: [ TABLE_MISS, APPLY_ACTION, GROUP, PACKET_OUT, "
      "'0x33333300' ]\n  packet_in_master: [ APPLY_ACTION, PACKET_OUT, "
      "'0x22222200' ]\n  port_status_slave: [ ADD, MODIFY, '0x55555550' ]\n  "
      "port_status_master: [ MODIFY, '0x44444440' ]\n  flow_removed_slave: [ "
      "IDLE_TIMEOUT, HARD_TIMEOUT, DELETE, METER_DELETE, EVICTION, "
      "'0x77777740' ]\n  flow_removed_master: [ HARD_TIMEOUT, DELETE, "
      "EVICTION, '0x66666640' ]\n  properties:      \n...\n");
}

TEST(decoder, queuegetconfigrequestv4) {
  testDecodeEncode(
      "04160010111111112222222200000000",
      "---\ntype:            QUEUE_GET_CONFIG_REQUEST\nxid:  "
      "           0x11111111\nversion:         0x04\nmsg:            "
      " \n  port_no:         0x22222222\n...\n");
}

TEST(decoder, queuegetconfigreplyv4) {
  testDecodeEncode(
      "041700701111111122222222000000003333333344444444003000000000000000010010"
      "000000005555000000000000000200100000000066660000000000007777777788888888"
      "0030000000000000000100100000000099990000000000000002001000000000AAAA0000"
      "00000000",
      "---\ntype:            QUEUE_GET_CONFIG_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  port_no:        "
      " 0x22222222\n  queues:          \n    - queue_id:        0x33333333\n   "
      "   port_no:         0x44444444\n      min_rate:        0x5555\n      "
      "max_rate:        0x6666\n      properties:      \n    - queue_id:       "
      " 0x77777777\n      port_no:         0x88888888\n      min_rate:        "
      "0x9999\n      max_rate:        0xAAAA\n      properties:      \n...\n");
}

TEST(decoder, queuegetconfigreplyv4_experimenter) {
  testDecodeEncode(
      "041700991111111122222222000000003333333344444444005900000000000000010010"
      "00000000555500000000000000020010000000006666000000000000FFFF001600000000"
      "EEEEEEEE00000000000102030405FFFF001300000000FFFFFFFF00000000ABCDEF777777"
      "778888888800300000000000000001001000000000999900000000000000020010000000"
      "00AAAA000000000000",
      "---\ntype:            QUEUE_GET_CONFIG_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  port_no:        "
      " 0x22222222\n  queues:          \n    - queue_id:        0x33333333\n   "
      "   port_no:         0x44444444\n      min_rate:        0x5555\n      "
      "max_rate:        0x6666\n      properties:      \n        - "
      "experimenter:    0xEEEEEEEE\n          value:           000102030405\n  "
      "      - experimenter:    0xFFFFFFFF\n          value:           "
      "ABCDEF\n    - queue_id:        0x77777777\n      port_no:         "
      "0x88888888\n      min_rate:        0x9999\n      max_rate:        "
      "0xAAAA\n      properties:      \n...\n");
}

TEST(decoder, queuegetconfigreplyv5_experimenter) {
  testDecodeEncode(
      "051700A01111111122222222000000003333333344444444006000000000000000010010"
      "00000000555500000000000000020010000000006666000000000000FFFF001600000000"
      "EEEEEEEE000000000001020304050000FFFF001300000000FFFFFFFF00000000ABCDEF00"
      "000000007777777788888888003000000000000000010010000000009999000000000000"
      "0002001000000000AAAA000000000000",
      "---\ntype:            QUEUE_GET_CONFIG_REPLY\nxid:             "
      "0x11111111\nversion:         0x05\nmsg:             \n  port_no:        "
      " 0x22222222\n  queues:          \n    - queue_id:        0x33333333\n   "
      "   port_no:         0x44444444\n      min_rate:        0x5555\n      "
      "max_rate:        0x6666\n      properties:      \n        - "
      "experimenter:    0xEEEEEEEE\n          value:           000102030405\n  "
      "      - experimenter:    0xFFFFFFFF\n          value:           "
      "ABCDEF\n    - queue_id:        0x77777777\n      port_no:         "
      "0x88888888\n      min_rate:        0x9999\n      max_rate:        "
      "0xAAAA\n      properties:      \n...\n");
}

TEST(decoder, queuegetconfigreplyv1) {
  testDecodeEncode(
      "011500601111111022210000000000003333333100280000000100100000000055510000"
      "000000000002001000000000666100000000000077777771002800000001001000000000"
      "99910000000000000002001000000000AAA1000000000000",
      "---\ntype:            QUEUE_GET_CONFIG_REPLY\nxid:             "
      "0x11111110\nversion:         0x01\nmsg:             \n  port_no:        "
      " 0x00002221\n  queues:          \n    - queue_id:        0x33333331\n   "
      "   port_no:         0x00000000\n      min_rate:        0x5551\n      "
      "max_rate:        0x6661\n      properties:      \n    - queue_id:       "
      " 0x77777771\n      port_no:         0x00000000\n      min_rate:        "
      "0x9991\n      max_rate:        0xAAA1\n      properties:      \n...\n");
}

TEST(decoder, queuegetconfigreplyv2) {
  testDecodeEncode(
      "021700601111111022222221000000003333333100280000000100100000000055510000"
      "000000000002001000000000666100000000000077777771002800000001001000000000"
      "99910000000000000002001000000000AAA1000000000000",
      "---\ntype:            QUEUE_GET_CONFIG_REPLY\nxid:             "
      "0x11111110\nversion:         0x02\nmsg:             \n  port_no:        "
      " 0x22222221\n  queues:          \n    - queue_id:        0x33333331\n   "
      "   port_no:         0x00000000\n      min_rate:        0x5551\n      "
      "max_rate:        0x6661\n      properties:      \n    - queue_id:       "
      " 0x77777771\n      port_no:         0x00000000\n      min_rate:        "
      "0x9991\n      max_rate:        0xAAA1\n      properties:      \n...\n");
}

TEST(decoder, getconfigreplyv4) {
  testDecodeEncode("0408000C11111111AAAABBBB",
                   "---\ntype:            GET_CONFIG_REPLY\nxid:          "
                   "   0x11111111\nversion:         0x04\nmsg:             \n  "
                   "flags:           [ FRAG_REASM, '0x0000AAA8' ]\n  "
                   "miss_send_len:   0xBBBB\n...\n");
}

TEST(decoder, setasyncv4) {
  testDecodeEncode(
      "041C002011111111222222223333333344444444555555556666666677777777",
      "---\ntype:            SET_ASYNC\nxid:             0x11111111\nversion:  "
      "       0x04\nmsg:             \n  packet_in_slave: [ TABLE_MISS, "
      "APPLY_ACTION, GROUP, PACKET_OUT, '0x33333300' ]\n  packet_in_master: [ "
      "APPLY_ACTION, PACKET_OUT, '0x22222200' ]\n  port_status_slave: [ ADD, "
      "MODIFY, '0x55555550' ]\n  port_status_master: [ MODIFY, '0x44444440' "
      "]\n  flow_removed_slave: [ IDLE_TIMEOUT, HARD_TIMEOUT, DELETE, "
      "METER_DELETE, EVICTION, '0x77777740' ]\n  flow_removed_master: [ "
      "HARD_TIMEOUT, DELETE, EVICTION, '0x66666640' ]\n  properties:      "
      "\n...\n");
}

TEST(decoder, setasyncv5) {
  testDecodeEncode(
      "051C00381111111100000008333333330001000822222222000200085555555500030008"
      "4444444400040008777777770005000866666666",
      "---\ntype:            SET_ASYNC\nxid:             0x11111111\nversion:  "
      "       0x05\nmsg:             \n  packet_in_slave: [ TABLE_MISS, "
      "APPLY_ACTION, GROUP, PACKET_OUT, '0x33333300' ]\n  packet_in_master: [ "
      "APPLY_ACTION, PACKET_OUT, '0x22222200' ]\n  port_status_slave: [ ADD, "
      "MODIFY, '0x55555550' ]\n  port_status_master: [ MODIFY, '0x44444440' "
      "]\n  flow_removed_slave: [ IDLE_TIMEOUT, HARD_TIMEOUT, DELETE, "
      "METER_DELETE, EVICTION, '0x77777740' ]\n  flow_removed_master: [ "
      "HARD_TIMEOUT, DELETE, EVICTION, '0x66666640' ]\n  properties:      "
      "\n...\n");
}

TEST(decoder, setasyncv5_2) {
  testDecodeEncode(
      "051C00381111111100010008222222210002000855555551000300084444444100040008"
      "7777777100050008666666610007000888888881",
      "---\ntype:            SET_ASYNC\nxid:             0x11111111\nversion:  "
      "       0x05\nmsg:             \n  packet_in_master: [ TABLE_MISS, "
      "PACKET_OUT, '0x22222200' ]\n  port_status_slave: [ ADD, '0x55555550' "
      "]\n  port_status_master: [ ADD, '0x44444440' ]\n  flow_removed_slave: [ "
      "IDLE_TIMEOUT, METER_DELETE, EVICTION, '0x77777740' ]\n  "
      "flow_removed_master: [ IDLE_TIMEOUT, EVICTION, '0x66666640' ]\n  "
      "role_status_master: [ MASTER_REQUEST, '0x88888880' ]\n  properties:     "
      " \n...\n");
}

TEST(decoder, flowremovedv4) {
  testDecodeEncode(
      "040B004011111111222222222222222233334455666666667777777788889999AAAAAAAA"
      "AAAAAAAABBBBBBBBBBBBBBBB0001000C800000041234567800000000",
      "---\ntype:            FLOW_REMOVED\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  cookie:         "
      " "
      "0x2222222222222222\n  priority:        0x3333\n  reason:          "
      "0x44\n  "
      "table_id:        0x55\n  duration:        1717986918.x77777777\n  "
      "idle_timeout:    0x8888\n  hard_timeout:    0x9999\n  "
      "packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x12345678\n...\n");
}

TEST(decoder, flowremovedv1) {
  testDecodeEncode(
      "010B005811111111003820FE567800000000000000000000000000000000000000000000"
      "000000000000000000000000222222222222222233334400666666667777777788880000"
      "AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBB",
      "---\ntype:            FLOW_REMOVED\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  cookie:         "
      " "
      "0x2222222222222222\n  priority:        0x3333\n  reason:          "
      "0x44\n  "
      "table_id:        0x00\n  duration:        1717986918.x77777777\n  "
      "idle_timeout:    0x8888\n  hard_timeout:    0x0000\n  "
      "packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x00005678\n...\n");
}

TEST(decoder, flowremovedv2) {
  testDecodeEncode(
      "020B008811111111222222222222222233334455666666667777777788880000AAAAAAAA"
      "AAAAAAAABBBBBBBBBBBBBBBB0000005812345678000003FE000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000",
      "---\ntype:            FLOW_REMOVED\nxid:             "
      "0x11111111\nversion:         0x02\nmsg:             \n  cookie:         "
      " "
      "0x2222222222222222\n  priority:        0x3333\n  reason:          "
      "0x44\n  "
      "table_id:        0x55\n  duration:        1717986918.x77777777\n  "
      "idle_timeout:    0x8888\n  hard_timeout:    0x0000\n  "
      "packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x12345678\n...\n");
}

TEST(decoder, flowremovedv3) {
  testDecodeEncode(
      "030B004011111111222222222222222233334455666666667777777788889999AAAAAAAA"
      "AAAAAAAABBBBBBBBBBBBBBBB0001000C800000041234567800000000",
      "---\ntype:            FLOW_REMOVED\nxid:             "
      "0x11111111\nversion:         0x03\nmsg:             \n  cookie:         "
      " "
      "0x2222222222222222\n  priority:        0x3333\n  reason:          "
      "0x44\n  "
      "table_id:        0x55\n  duration:        1717986918.x77777777\n  "
      "idle_timeout:    0x8888\n  hard_timeout:    0x9999\n  "
      "packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x12345678\n...\n");
}

TEST(decoder, flowremovedv6) {
  testDecodeEncode(
      "060B005011111111554433338888999922222222222222220001000C8000000412345678"
      "000000000000002880020008666666667777777780020808AAAAAAAAAAAAAAAA80020A08"
      "BBBBBBBBBBBBBBBB",
      "---\ntype:            FLOW_REMOVED\nxid:             "
      "0x11111111\nversion:         0x06\nmsg:             \n  cookie:         "
      " 0x2222222222222222\n  priority:        0x3333\n  reason:          "
      "0x44\n  table_id:        0x55\n  duration:        "
      "1717986918.x77777777\n  idle_timeout:    0x8888\n  hard_timeout:    "
      "0x9999\n  packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x12345678\n  stat:            \n...\n");
}

TEST(decoder, ofmp_desc_request_v4) {
  testDecodeEncode("04120010111111110000000000000000",
                   "---\ntype:            REQUEST.DESC\nflags:           [  "
                   "]\nxid:             0x11111111\nversion:         "
                   "0x04\nmsg:             \n...\n");
}

TEST(decoder, ofmp_desc_request_v1) {
  testDecodeEncode("0110000C1111111100000000",
                   "---\ntype:            REQUEST.DESC\nflags:           [  "
                   "]\nxid:             0x11111111\nversion:         "
                   "0x01\nmsg:             \n...\n");
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
      "---\ntype:            REPLY.DESC\nflags:           [  ]\nxid:           "
      "  0x11111111\nversion:         0x04\nmsg:             \n  mfr_desc:     "
      "   ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n  hw_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n  sw_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n  serial_num:      "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n  dp_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n...\n");
}

TEST(decoder, ofmp_desc_reply_v1) {
  testDecodeEncode(
      "0111042C11111111000000004142434445464748494A4B4C4D4E4F505152535455565758"
      "595A20313233343536373839000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000004142434445464748494A4B4C4D4E4F5051525354"
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
      "4D4E4F505152535455565758595A0000000000004142434445464748494A4B4C4D4E4F50"
      "5152535455565758595A2031323334353637383900000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000",
      "---\ntype:            REPLY.DESC\nflags:           [  ]\nxid:           "
      "  0x11111111\nversion:         0x01\nmsg:             \n  mfr_desc:     "
      "   ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n  hw_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n  sw_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n  serial_num:      "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n  dp_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n...\n");
}

TEST(decoder, ofmp_portstats_v4_request) {
  testDecodeEncode("041200181111111100040000000000002222222200000000",
                   "---\ntype:            REQUEST.PORT_STATS\nflags:           "
                   "[  ]\nxid:             0x11111111\nversion:         "
                   "0x04\nmsg:             \n  port_no:         "
                   "0x22222222\n...\n");
}

TEST(decoder, ofmp_portstats_v1_request) {
  testDecodeEncode("0110001411111111000400002222000000000000",
                   "---\ntype:            REQUEST.PORT_STATS\nflags:           "
                   "[  ]\nxid:             0x11111111\nversion:         "
                   "0x01\nmsg:             \n  port_no:         "
                   "0x00002222\n...\n");
}

TEST(decoder, ofmp_queuestats_v4_request) {
  testDecodeEncode("041200181111111100050000000000002222222233333333",
                   "---\ntype:            REQUEST.QUEUE\nflags:           [  "
                   "]\nxid:             0x11111111\nversion:         "
                   "0x04\nmsg:             \n  port_no:         0x22222222\n  "
                   "queue_id:        0x33333333\n...\n");
}

TEST(decoder, ofmp_queuestats_v1_request) {
  testDecodeEncode("0110001411111111000500002222000033333333",
                   "---\ntype:            REQUEST.QUEUE\nflags:           [  "
                   "]\nxid:             0x11111111\nversion:         "
                   "0x01\nmsg:             \n  port_no:         0x00002222\n  "
                   "queue_id:        0x33333333\n...\n");
}

TEST(decoder, ofp_metermod_v4) {
  testDecodeEncode(
      "041D00301111111100012222333333330001001044444444555555550000000000020010"
      "666666667777777788000000",
      "---\ntype:            METER_MOD\nxid:             0x11111111\nversion:  "
      "       0x04\nmsg:             \n  command:         MODIFY\n  flags:     "
      "      [ PKTPS, '0x00002220' ]\n  meter_id:        0x33333333\n  bands:  "
      "         \n    - type:            DROP\n      rate:            "
      "0x44444444\n      burst_size:      0x55555555\n    - type:            "
      "DSCP_REMARK\n      rate:            0x66666666\n      burst_size:      "
      "0x77777777\n      prec_level:      0x88\n...\n");
}

TEST(decoder, ofmp_aggregatestatsrequest_v1) {
  testDecodeEncode(
      "0110003800000018 00020000 003820ff0000000000000000000000000000 "
      "00000000000000000000000000000000 0000000000000800ffff",
      "---\ntype:            REQUEST.AGGREGATE\nflags:           [  ]\nxid:    "
      "         0x00000018\nversion:         0x01\nmsg:             \n  "
      "table_id:        0x08\n  out_port:        ANY\n  out_group:       "
      "0x00000000\n  cookie:          0x0000000000000000\n  cookie_mask:     "
      "0x0000000000000000\n  match:           \n...\n");
}

TEST(decoder, ofmp_groupfeatures_reply) {
  testDecodeEncode(
      "041300381111111100080000000000001111111122222222333333334444444455555555"
      "666666663777777708888888199999992AAAAAAA",
      "---\ntype:            REPLY.GROUP_FEATURES\nflags:           [  ]\nxid: "
      "            0x11111111\nversion:         0x04\nmsg:             \n  "
      "types:           [ ALL, '0x11111110' ]\n  capabilities:    [ "
      "SELECT_LIVENESS, '0x22222220' ]\n  max_groups_all:  0x33333333\n  "
      "max_groups_sel:  0x44444444\n  max_groups_ind:  0x55555555\n  "
      "max_groups_ff:   0x66666666\n  actions_all:     [ OUTPUT, SET_VLAN_VID, "
      "SET_VLAN_PCP, SET_DL_DST, SET_NW_SRC, SET_NW_DST, SET_NW_ECN, "
      "SET_TP_SRC, SET_TP_DST, COPY_TTL_IN, SET_MPLS_LABEL, SET_MPLS_TC, "
      "DEC_MPLS_TTL, PUSH_VLAN, POP_VLAN, POP_MPLS, SET_QUEUE, GROUP, "
      "DEC_NW_TTL, SET_FIELD, PUSH_PBB, '0x30000000' ]\n  actions_sel:     [ "
      "SET_DL_SRC, SET_NW_TOS, COPY_TTL_OUT, SET_MPLS_TTL, PUSH_MPLS, "
      "SET_NW_TTL, POP_PBB ]\n  actions_ind:     [ OUTPUT, SET_DL_SRC, "
      "SET_DL_DST, SET_NW_TOS, SET_NW_ECN, COPY_TTL_OUT, COPY_TTL_IN, "
      "SET_MPLS_TTL, DEC_MPLS_TTL, PUSH_MPLS, POP_MPLS, SET_NW_TTL, "
      "DEC_NW_TTL, POP_PBB, '0x10000000' ]\n  actions_ff:      [ SET_VLAN_VID, "
      "SET_DL_SRC, SET_NW_SRC, SET_NW_TOS, SET_TP_SRC, COPY_TTL_OUT, "
      "SET_MPLS_LABEL, SET_MPLS_TTL, PUSH_VLAN, PUSH_MPLS, SET_QUEUE, "
      "SET_NW_TTL, SET_FIELD, POP_PBB, '0x20000000' ]\n...\n");
}

TEST(decoder, ofmp_flowmonitor_request) {
  testDecodeEncode(
      "05120030111111110010000000000000111111112222222233333333444455660001000C"
      "800000041234567800000000",
      "---\ntype:            REQUEST.FLOW_MONITOR\nflags:           [  ]\nxid: "
      "            0x11111111\nversion:         0x05\nmsg:             \n  "
      "monitor_id:      0x11111111\n  out_port:        0x22222222\n  "
      "out_group:       0x33333333\n  flags:           [ REMOVED, ONLY_OWN, "
      "'0x00004440' ]\n  table_id:        0x55\n  command:         0x66\n  "
      "match:           \n    - field:           IN_PORT\n      value:         "
      "  0x12345678\n...\n");
}

TEST(decoder, ofmp_flowmonitor_reply) {
  testDecodeEncode(
      "051300581111111100100000000000000040000111223333444455550000000066666666"
      "666666660001000C80000004123456780000000000040018000000000019001080001804"
      "C0A80201000000000008000422222222",
      "---\ntype:            REPLY.FLOW_MONITOR\nflags:           [  ]\nxid:   "
      "          0x11111111\nversion:         0x05\nmsg:             \n  - "
      "event:           ADDED\n    table_id:        0x11\n    reason:          "
      "0x22\n    idle_timeout:    0x3333\n    hard_timeout:    0x4444\n    "
      "priority:        0x5555\n    cookie:          0x6666666666666666\n    "
      "match:           \n      - field:           IN_PORT\n        value:     "
      "      0x12345678\n    instructions:    \n      - instruction:     "
      "APPLY_ACTIONS\n        actions:         \n          - action:          "
      "SET_FIELD\n            field:           IPV4_DST\n            value:    "
      "       192.168.2.1\n  - event:           ABBREV\n    xid:             "
      "0x22222222\n...\n");
}

TEST(decoder, rolestatusv5) {
  testDecodeEncode(
      "051E00281111111122222222010000003333333333333333FFFF001012345678ABCDABCD"
      "0000F1F1",
      "---\ntype:            ROLE_STATUS\nxid:             "
      "0x11111111\nversion:         0x05\nmsg:             \n  role:           "
      " 0x22222222\n  reason:          CONFIG\n  generation_id:   "
      "0x3333333333333333\n  properties:      \n    - property:        "
      "EXPERIMENTER\n      experimenter:    0x12345678\n      exp_type:        "
      "0xABCDABCD\n      data:            0000F1F1\n...\n");
}

TEST(decoder, requestforwardv5) {
  testDecodeEncode(
      "0520004811111111050F0040AAAAAAAA2222330044444444003055556666666677777777"
      "00000000000000100000000500140000000000000019001080001804C0A801010000000"
      "0",
      "---\ntype:            REQUESTFORWARD\nxid:             "
      "0x11111111\nversion:         0x05\nmsg:             \n  type:           "
      " GROUP_MOD\n  xid:             0xAAAAAAAA\n  version:         0x05\n  "
      "msg:             \n    command:         0x2222\n    type:            "
      "0x33\n    group_id:        0x44444444\n    buckets:         \n      - "
      "weight:          0x5555\n        watch_port:      0x66666666\n        "
      "watch_group:     0x77777777\n        actions:         \n          - "
      "action:          OUTPUT\n            port_no:         0x00000005\n      "
      "      max_len:         0x0014\n          - action:          SET_FIELD\n "
      "           field:           IPV4_DST\n            value:           "
      "192.168.1.1\n...\n");
}

TEST(decoder, bundlecontrolv5) {
  testDecodeEncode(
      "05210020111111112222222233334444FFFF001012345678ABCDABCD0000F1F1",
      "---\ntype:            BUNDLE_CONTROL\nxid:             "
      "0x11111111\nversion:         0x05\nmsg:             \n  bundle_id:      "
      " 0x22222222\n  type:            0x3333\n  flags:           [ "
      "'0x00004444' ]\n  properties:      \n    - property:        "
      "EXPERIMENTER\n      experimenter:    0x12345678\n      exp_type:        "
      "0xABCDABCD\n      data:            0000F1F1\n...\n");
}

TEST(decoder, bundleaddmessagev5) {
  testDecodeEncode(
      "052200301111111122222222000033330502000A11111111ABCD000000000000FFFF0010"
      "12345678ABCDABCD0000F1F1",
      "---\ntype:            BUNDLE_ADD_MESSAGE\nxid:             "
      "0x11111111\nversion:         0x05\nmsg:             \n  bundle_id:      "
      " 0x22222222\n  flags:           [ ATOMIC, ORDERED, '0x00003330' ]\n  "
      "message:         \n    type:            ECHO_REQUEST\n    xid:          "
      "   0x11111111\n    version:         0x05\n    msg:             \n      "
      "data:            ABCD\n  properties:      \n    - property:        "
      "EXPERIMENTER\n      experimenter:    0x12345678\n      exp_type:        "
      "0xABCDABCD\n      data:            0000F1F1\n...\n");
}

TEST(decoder, ofmp_portdescv4_reply) {
  testDecodeEncode(
      "0413009000000000000D0000000000000000000700000000F20BA4D03F700000506F7274"
      "370000000000000000000000000000000000000400002808000028000000280800002808"
      "00001388000013880000000600000000F20BA47DF8EA0000506F72743600000000000000"
      "00000000000000000000000400002808000028000000280800002808000013880000138"
      "8",
      "---\ntype:            REPLY.PORT_DESC\nflags:           [  ]\nxid:      "
      "       0x00000000\nversion:         0x04\nmsg:             \n  - "
      "port_no:         0x00000007\n    hw_addr:         'f2:0b:a4:d0:3f:70'\n "
      "   name:            Port7\n    config:          [  ]\n    state:        "
      "   [ LIVE ]\n    ethernet:        \n      curr:            [ 100MB_FD, "
      "COPPER, AUTONEG ]\n      advertised:      [ COPPER, AUTONEG ]\n      "
      "supported:       [ 100MB_FD, COPPER, AUTONEG ]\n      peer:            "
      "[ 100MB_FD, COPPER, AUTONEG ]\n      curr_speed:      0x00001388\n      "
      "max_speed:       0x00001388\n    properties:      \n  - port_no:        "
      " 0x00000006\n    hw_addr:         'f2:0b:a4:7d:f8:ea'\n    name:        "
      "    Port6\n    config:          [  ]\n    state:           [ LIVE ]\n   "
      " ethernet:        \n      curr:            [ 100MB_FD, COPPER, AUTONEG "
      "]\n      advertised:      [ COPPER, AUTONEG ]\n      supported:       [ "
      "100MB_FD, COPPER, AUTONEG ]\n      peer:            [ 100MB_FD, COPPER, "
      "AUTONEG ]\n      curr_speed:      0x00001388\n      max_speed:       "
      "0x00001388\n    properties:      \n...\n");
}

TEST(decoder, ofmp_portdescv1_reply) {
  testDecodeEncode(
      "011100CC00000000000D0000000362D37D762C7073312D65746833000000000000000000"
      "0000000000000000000000C0000000000000000000000000000172DEF2D61E0C73312D65"
      "7468310000000000000000000000000000000000000000C0000000000000000000000000"
      "00027EE6C789081673312D657468320000000000000000000000000000000000000000C0"
      "000000000000000000000000FFFEBA265A8B1E4573310000000000000000000000000000"
      "000000000000000000000000000000000000000000000000",
      "---\ntype:            REPLY.PORT_DESC\nflags:           [  ]\nxid:      "
      "       0x00000000\nversion:         0x01\nmsg:             \n  - "
      "port_no:         0x00000003\n    hw_addr:         '62:d3:7d:76:2c:70'\n "
      "   name:            s1-eth3\n    config:          [  ]\n    state:      "
      "     [ STP_LISTEN ]\n    ethernet:        \n      curr:            [ "
      "10GB_FD, COPPER ]\n      advertised:      [  ]\n      supported:       "
      "[  ]\n      peer:            [  ]\n      curr_speed:      0x00000000\n  "
      "    max_speed:       0x00000000\n    properties:      \n  - port_no:    "
      "     0x00000001\n    hw_addr:         '72:de:f2:d6:1e:0c'\n    name:    "
      "        s1-eth1\n    config:          [  ]\n    state:           [ "
      "STP_LISTEN ]\n    ethernet:        \n      curr:            [ 10GB_FD, "
      "COPPER ]\n      advertised:      [  ]\n      supported:       [  ]\n    "
      "  peer:            [  ]\n      curr_speed:      0x00000000\n      "
      "max_speed:       0x00000000\n    properties:      \n  - port_no:        "
      " 0x00000002\n    hw_addr:         '7e:e6:c7:89:08:16'\n    name:        "
      "    s1-eth2\n    config:          [  ]\n    state:           [ "
      "STP_LISTEN ]\n    ethernet:        \n      curr:            [ 10GB_FD, "
      "COPPER ]\n      advertised:      [  ]\n      supported:       [  ]\n    "
      "  peer:            [  ]\n      curr_speed:      0x00000000\n      "
      "max_speed:       0x00000000\n    properties:      \n  - port_no:        "
      " LOCAL\n    hw_addr:         'ba:26:5a:8b:1e:45'\n    name:            "
      "s1\n    config:          [  ]\n    state:           [ STP_LISTEN ]\n    "
      "ethernet:        \n      curr:            [  ]\n      advertised:      "
      "[  ]\n      supported:       [  ]\n      peer:            [  ]\n      "
      "curr_speed:      0x00000000\n      max_speed:       0x00000000\n    "
      "properties:      \n...\n");
}

TEST(decoder, ofmp_portdescv2_reply) {
  testDecodeEncode(
      "0213009011111111000D00000000000000001111000000002222222222220000506F7274"
      "203100000000000000000000333333334444444455555555666666667777777788888888"
      "99999999AAAAAAAA0000BBBB00000000CCCCCCCCCCCC0000506F72742032000000000000"
      "0000000033333333444444445555555566666666777777778888888899999999AAAAAAA"
      "A",
      "---\ntype:            REPLY.PORT_DESC\nflags:           [  ]\nxid:      "
      "       0x11111111\nversion:         0x02\nmsg:             \n  - "
      "port_no:         0x00001111\n    hw_addr:         '22:22:22:22:22:22'\n "
      "   name:            Port 1\n    config:          [ PORT_DOWN, NO_STP, "
      "NO_FLOOD, NO_FWD, '0x33333300' ]\n    state:           [ LIVE, "
      "'0x44444440' ]\n    ethernet:        \n      curr:            [ "
      "10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, 100GB_FD, OTHER, FIBER, PAUSE, "
      "'0x55550000' ]\n      advertised:      [ 10MB_FD, 100MB_HD, 1GB_FD, "
      "10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' ]\n      "
      "supported:       [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, "
      "100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, PAUSE, '0x77770000' ]\n      "
      "peer:            [ 100MB_FD, 40GB_FD, COPPER, PAUSE_ASYM, '0x88880000' "
      "]\n      curr_speed:      0x99999999\n      max_speed:       "
      "0xAAAAAAAA\n    properties:      \n  - port_no:         0x0000BBBB\n    "
      "hw_addr:         'cc:cc:cc:cc:cc:cc'\n    name:            Port 2\n    "
      "config:          [ PORT_DOWN, NO_STP, NO_FLOOD, NO_FWD, '0x33333300' "
      "]\n    state:           [ LIVE, '0x44444440' ]\n    ethernet:        \n "
      "     curr:            [ 10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, 100GB_FD, "
      "OTHER, FIBER, PAUSE, '0x55550000' ]\n      advertised:      [ 10MB_FD, "
      "100MB_HD, 1GB_FD, 10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' "
      "]\n      supported:       [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, "
      "10GB_FD, 100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, PAUSE, '0x77770000' "
      "]\n      peer:            [ 100MB_FD, 40GB_FD, COPPER, PAUSE_ASYM, "
      "'0x88880000' ]\n      curr_speed:      0x99999999\n      max_speed:     "
      "  0xAAAAAAAA\n    properties:      \n...\n");
}

TEST(decoder, ofmp_portdescv3_reply) {
  testDecodeEncode(
      "0313009011111111000D00000000000000001111000000002222222222220000506F7274"
      "203100000000000000000000333333334444444455555555666666667777777788888888"
      "99999999AAAAAAAA0000BBBB00000000CCCCCCCCCCCC0000506F72742032000000000000"
      "0000000033333333444444445555555566666666777777778888888899999999AAAAAAA"
      "A",
      "---\ntype:            REPLY.PORT_DESC\nflags:           [  ]\nxid:      "
      "       0x11111111\nversion:         0x03\nmsg:             \n  - "
      "port_no:         0x00001111\n    hw_addr:         '22:22:22:22:22:22'\n "
      "   name:            Port 1\n    config:          [ PORT_DOWN, NO_STP, "
      "NO_FLOOD, NO_FWD, '0x33333300' ]\n    state:           [ LIVE, "
      "'0x44444440' ]\n    ethernet:        \n      curr:            [ "
      "10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, 100GB_FD, OTHER, FIBER, PAUSE, "
      "'0x55550000' ]\n      advertised:      [ 10MB_FD, 100MB_HD, 1GB_FD, "
      "10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' ]\n      "
      "supported:       [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, "
      "100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, PAUSE, '0x77770000' ]\n      "
      "peer:            [ 100MB_FD, 40GB_FD, COPPER, PAUSE_ASYM, '0x88880000' "
      "]\n      curr_speed:      0x99999999\n      max_speed:       "
      "0xAAAAAAAA\n    properties:      \n  - port_no:         0x0000BBBB\n    "
      "hw_addr:         'cc:cc:cc:cc:cc:cc'\n    name:            Port 2\n    "
      "config:          [ PORT_DOWN, NO_STP, NO_FLOOD, NO_FWD, '0x33333300' "
      "]\n    state:           [ LIVE, '0x44444440' ]\n    ethernet:        \n "
      "     curr:            [ 10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, 100GB_FD, "
      "OTHER, FIBER, PAUSE, '0x55550000' ]\n      advertised:      [ 10MB_FD, "
      "100MB_HD, 1GB_FD, 10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' "
      "]\n      supported:       [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, "
      "10GB_FD, 100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, PAUSE, '0x77770000' "
      "]\n      peer:            [ 100MB_FD, 40GB_FD, COPPER, PAUSE_ASYM, "
      "'0x88880000' ]\n      curr_speed:      0x99999999\n      max_speed:     "
      "  0xAAAAAAAA\n    properties:      \n...\n");
}

TEST(decoder, ofmp_portdescv4_reply2) {
  testDecodeEncode(
      "0413009011111111000D00000000000000001111000000002222222222220000506F7274"
      "203100000000000000000000333333334444444455555555666666667777777788888888"
      "99999999AAAAAAAA0000BBBB00000000CCCCCCCCCCCC0000506F72742032000000000000"
      "0000000033333333444444445555555566666666777777778888888899999999AAAAAAA"
      "A",
      "---\ntype:            REPLY.PORT_DESC\nflags:           [  ]\nxid:      "
      "       0x11111111\nversion:         0x04\nmsg:             \n  - "
      "port_no:         0x00001111\n    hw_addr:         '22:22:22:22:22:22'\n "
      "   name:            Port 1\n    config:          [ PORT_DOWN, NO_STP, "
      "NO_FLOOD, NO_FWD, '0x33333300' ]\n    state:           [ LIVE, "
      "'0x44444440' ]\n    ethernet:        \n      curr:            [ "
      "10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, 100GB_FD, OTHER, FIBER, PAUSE, "
      "'0x55550000' ]\n      advertised:      [ 10MB_FD, 100MB_HD, 1GB_FD, "
      "10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' ]\n      "
      "supported:       [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, "
      "100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, PAUSE, '0x77770000' ]\n      "
      "peer:            [ 100MB_FD, 40GB_FD, COPPER, PAUSE_ASYM, '0x88880000' "
      "]\n      curr_speed:      0x99999999\n      max_speed:       "
      "0xAAAAAAAA\n    properties:      \n  - port_no:         0x0000BBBB\n    "
      "hw_addr:         'cc:cc:cc:cc:cc:cc'\n    name:            Port 2\n    "
      "config:          [ PORT_DOWN, NO_STP, NO_FLOOD, NO_FWD, '0x33333300' "
      "]\n    state:           [ LIVE, '0x44444440' ]\n    ethernet:        \n "
      "     curr:            [ 10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, 100GB_FD, "
      "OTHER, FIBER, PAUSE, '0x55550000' ]\n      advertised:      [ 10MB_FD, "
      "100MB_HD, 1GB_FD, 10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' "
      "]\n      supported:       [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, "
      "10GB_FD, 100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, PAUSE, '0x77770000' "
      "]\n      peer:            [ 100MB_FD, 40GB_FD, COPPER, PAUSE_ASYM, "
      "'0x88880000' ]\n      curr_speed:      0x99999999\n      max_speed:     "
      "  0xAAAAAAAA\n    properties:      \n...\n");
}

TEST(decoder, ofmp_portdescv5_reply) {
  testDecodeEncode(
      "051300A011111111000D00000000000000001111004800002222222222220000506F7274"
      "203100000000000000000000333333334444444400000020000000005555555566666666"
      "777777778888888899999999AAAAAAAA0000BBBB00480000CCCCCCCCCCCC0000506F7274"
      "203200000000000000000000333333334444444400000020000000005555555566666666"
      "777777778888888899999999AAAAAAAA",
      "---\ntype:            REPLY.PORT_DESC\nflags:           [  ]\nxid:      "
      "       0x11111111\nversion:         0x05\nmsg:             \n  - "
      "port_no:         0x00001111\n    hw_addr:         '22:22:22:22:22:22'\n "
      "   name:            Port 1\n    config:          [ PORT_DOWN, NO_STP, "
      "NO_FLOOD, NO_FWD, '0x33333300' ]\n    state:           [ LIVE, "
      "'0x44444440' ]\n    ethernet:        \n      curr:            [ "
      "10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, 100GB_FD, OTHER, FIBER, PAUSE, "
      "'0x55550000' ]\n      advertised:      [ 10MB_FD, 100MB_HD, 1GB_FD, "
      "10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' ]\n      "
      "supported:       [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, "
      "100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, PAUSE, '0x77770000' ]\n      "
      "peer:            [ 100MB_FD, 40GB_FD, COPPER, PAUSE_ASYM, '0x88880000' "
      "]\n      curr_speed:      0x99999999\n      max_speed:       "
      "0xAAAAAAAA\n    properties:      \n  - port_no:         0x0000BBBB\n    "
      "hw_addr:         'cc:cc:cc:cc:cc:cc'\n    name:            Port 2\n    "
      "config:          [ PORT_DOWN, NO_STP, NO_FLOOD, NO_FWD, '0x33333300' "
      "]\n    state:           [ LIVE, '0x44444440' ]\n    ethernet:        \n "
      "     curr:            [ 10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, 100GB_FD, "
      "OTHER, FIBER, PAUSE, '0x55550000' ]\n      advertised:      [ 10MB_FD, "
      "100MB_HD, 1GB_FD, 10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' "
      "]\n      supported:       [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, "
      "10GB_FD, 100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, PAUSE, '0x77770000' "
      "]\n      peer:            [ 100MB_FD, 40GB_FD, COPPER, PAUSE_ASYM, "
      "'0x88880000' ]\n      curr_speed:      0x99999999\n      max_speed:     "
      "  0xAAAAAAAA\n    properties:      \n...\n");
}

TEST(decoder, ofmp_tablefeaturesv4_reply_unpadded_len) {
  // Accept a slightly malformed packet.
  testDecodeOnly(
      "0413005800007777000C00000000000000445500000000005461626C6520310000000000"
      "000000000000000000000000000000000000000000000000008888880000000000999999"
      "000000AA000000BB0000000400000000",
      "---\ntype:            REPLY.TABLE_FEATURES\nflags:           [  ]\nxid: "
      "            0x00007777\nversion:         0x04\nmsg:             \n  - "
      "table_id:        0x55\n    name:            Table 1\n    "
      "metadata_match:  0x0000000000888888\n    metadata_write:  "
      "0x0000000000999999\n    config:          [ VACANCY_EVENTS, '0x000000A2' "
      "]\n    max_entries:     0x000000BB\n    instructions:    [  ]\n    "
      "next_tables:     [  ]\n    write_actions:   [  ]\n    apply_actions:   "
      "[  ]\n    match:           [  ]\n    wildcards:       [  ]\n    "
      "write_set_field: [  ]\n    apply_set_field: [  ]\n    properties:      "
      "\n...\n");
}

TEST(decoder, queue_get_config_replyv4_fix) {
  // Make sure issue where property was assigned/sliced is fixed.
  testDecodeOnly(
      "04170098BEC0D5180000000D000000002FC870660000000D0084000000000000FFFF0074"
      "00000000C2E4427C00000000646464646464646464646464646464646464646464646464"
      "646464646464646464646464646464646464646464646464646464646464646464646464"
      "646464646464646464646464646464646464646464646464646464646464646464646464"
      "6464646400000000",
      "---\ntype:            QUEUE_GET_CONFIG_REPLY\nxid:             "
      "0xBEC0D518\nversion:         0x04\nmsg:             \n  port_no:        "
      " 0x0000000D\n  queues:          \n    - queue_id:        0x2FC87066\n   "
      "   port_no:         0x0000000D\n      min_rate:        0xFFFF\n      "
      "max_rate:        0xFFFF\n      properties:      \n        - "
      "experimenter:    0xC2E4427C\n          value:           "
      "646464646464646464646464646464646464646464646464646464646464646464646464"
      "646464646464646464646464646464646464646464646464646464646464646464646464"
      "64646464646464646464646464646464646464646464646464646464\n...\n");
}

TEST(decoder, queue_get_config_replyv1) {
  testDecodeEncode(
      "01150020C1C49F86000D00000000000000000012001000000000000800000000",
      "---\ntype:            QUEUE_GET_CONFIG_REPLY\nxid:             "
      "0xC1C49F86\nversion:         0x01\nmsg:             \n  port_no:        "
      " 0x0000000D\n  queues:          \n    - queue_id:        0x00000012\n   "
      "   port_no:         0x00000000\n      min_rate:        0xFFFF\n      "
      "max_rate:        0xFFFF\n      properties:      \n...\n");
}

TEST(decoder, tablemodv5) {
  testDecodeEncode(
      "0511005800000000FF0000000000000000020008112233440003000811223300FFFF000C"
      "444444415555555100000000FFFF0010666666617777777188888888FFFF001499999991"
      "AAAAAAA1000000010000000200000000",
      "---\ntype:            TABLE_MOD\nxid:             0x00000000\nversion:  "
      "       0x05\nmsg:             \n  table_id:        ALL\n  config:       "
      "   [  ]\n  eviction:        \n    flags:           0x11223344\n  "
      "vacancy:         \n    vacancy_down:    0x11\n    vacancy_up:      "
      "0x22\n    vacancy:         0x33\n  properties:      \n    - property:   "
      "     EXPERIMENTER\n      experimenter:    0x44444441\n      exp_type:   "
      "     0x55555551\n      data:            ''\n    - property:        "
      "EXPERIMENTER\n      experimenter:    0x66666661\n      exp_type:        "
      "0x77777771\n      data:            88888888\n    - property:        "
      "EXPERIMENTER\n      experimenter:    0x99999991\n      exp_type:        "
      "0xAAAAAAA1\n      data:            0000000100000002\n...\n");
}

TEST(decoder, tablemod_unrecognized_prop_v5) {
  testDecodeEncode(
      "0511002000000000 FF00000000000000 0001001011223344 5566778899AABBCC",
      "---\ntype:            TABLE_MOD\nxid:             0x00000000\nversion:  "
      "       0x05\nmsg:             \n  table_id:        ALL\n  config:       "
      "   [  ]\n  properties:      \n    - property:        0x0001\n      "
      "data:            112233445566778899AABBCC\n...\n");
}

TEST(decoder, tablestatusv5) {
  testDecodeEncode(
      "051F00600000000010000000000000000050FF0000000000000200081122334400030008"
      "11223300FFFF000C444444415555555100000000FFFF0010666666617777777188888888"
      "FFFF001499999991AAAAAAA1000000010000000200000000",
      "---\ntype:            TABLE_STATUS\nxid:             "
      "0x00000000\nversion:         0x05\nmsg:             \n  reason:         "
      " 0x10\n  table_id:        ALL\n  config:          [  ]\n  eviction:     "
      "   \n    flags:           0x11223344\n  vacancy:         \n    "
      "vacancy_down:    0x11\n    vacancy_up:      0x22\n    vacancy:         "
      "0x33\n  properties:      \n    - property:        EXPERIMENTER\n      "
      "experimenter:    0x44444441\n      exp_type:        0x55555551\n      "
      "data:            ''\n    - property:        EXPERIMENTER\n      "
      "experimenter:    0x66666661\n      exp_type:        0x77777771\n      "
      "data:            88888888\n    - property:        EXPERIMENTER\n      "
      "experimenter:    0x99999991\n      exp_type:        0xAAAAAAA1\n      "
      "data:            0000000100000002\n...\n");
}

TEST(decoder, mptabledescv5) {
  testDecodeEncode(
      "0513007000000000000E0000000000000050010000000000000200081122334400030008"
      "11223300FFFF000C444444415555555100000000FFFF0010666666617777777188888888"
      "FFFF001499999991AAAAAAA1000000010000000200000000001002000000000800030008"
      "10203000",
      "---\ntype:            REPLY.TABLE_DESC\nflags:           [  ]\nxid:     "
      "        0x00000000\nversion:         0x05\nmsg:             \n  - "
      "table_id:        0x01\n    config:          [  ]\n    eviction:        "
      "\n      flags:           0x11223344\n    vacancy:         \n      "
      "vacancy_down:    0x11\n      vacancy_up:      0x22\n      vacancy:      "
      "   0x33\n    properties:      \n      - property:        EXPERIMENTER\n "
      "       experimenter:    0x44444441\n        exp_type:        "
      "0x55555551\n        data:            ''\n      - property:        "
      "EXPERIMENTER\n        experimenter:    0x66666661\n        exp_type:    "
      "    0x77777771\n        data:            88888888\n      - property:    "
      "    EXPERIMENTER\n        experimenter:    0x99999991\n        "
      "exp_type:        0xAAAAAAA1\n        data:            "
      "0000000100000002\n  - table_id:        0x02\n    config:          [ "
      "VACANCY_EVENTS ]\n    vacancy:         \n      vacancy_down:    0x10\n  "
      "    vacancy_up:      0x20\n      vacancy:         0x30\n    properties: "
      "     \n...\n");
}

TEST(decoder, mpqueuedescrequestv5) {
  testDecodeEncode("0512001801020304000F0000000000001111111F2222222F",
                   "---\ntype:            REQUEST.QUEUE_DESC\nflags:           "
                   "[  ]\nxid:             0x01020304\nversion:         "
                   "0x05\nmsg:             \n  port_no:         0x1111111F\n  "
                   "queue_id:        0x2222222F\n...\n");
}

TEST(decoder, mpqueuedescreplyv5) {
  testDecodeEncode(
      "0513004801020304000F0000000000001111111F2222222F003800000000000000010008"
      "123400000002000856780000FFFF0011AABBCCDDDDEEFF11DEADBEEF120000000000000"
      "0",
      "---\ntype:            REPLY.QUEUE_DESC\nflags:           [  ]\nxid:     "
      "        0x01020304\nversion:         0x05\nmsg:             \n  - "
      "port_no:         0x1111111F\n    queue_id:        0x2222222F\n    "
      "min_rate:        0x1234\n    max_rate:        0x5678\n    properties:   "
      "   \n      - property:        EXPERIMENTER\n        experimenter:    "
      "0xAABBCCDD\n        exp_type:        0xDDEEFF11\n        data:          "
      "  DEADBEEF12\n...\n");
}

TEST(decoder, nicira_actions) {
  testDecodeEncode(
      "040E007000000001000000000000000000000000000000000100000000000000FFFFFFFF"
      "FFFFFFFFFFFFFFFF0000000000010004000000000004003800000000FFFF001800002320"
      "00060018000000188000060680000806FFFF0018000023200007000780000A0200000000"
      "00000089",
      "---\ntype:            FLOW_MOD\nxid:             0x00000001\nversion:   "
      "      0x04\nmsg:             \n  cookie:          0x0000000000000000\n  "
      "cookie_mask:     0x0000000000000000\n  table_id:        0x01\n  "
      "command:         ADD\n  idle_timeout:    0x0000\n  hard_timeout:    "
      "0x0000\n  priority:        0x0000\n  buffer_id:       NO_BUFFER\n  "
      "out_port:        ANY\n  out_group:       ANY\n  flags:           [  ]\n "
      " match:           \n  instructions:    \n    - instruction:     "
      "APPLY_ACTIONS\n      actions:         \n        - action:          "
      "NX_REG_MOVE\n          src:             'ETH_DST[0:24]'\n          dst: "
      "            'ETH_SRC[24:48]'\n        - action:          NX_REG_LOAD\n  "
      "        dst:             'ETH_TYPE[0:8]'\n          value:           "
      "0x0000000000000089\n...\n");
}

TEST(decoder, packet_in_nonzero_padding) {
  // This packet_in has non-zero padding in the match. We don't expect this
  // message to roundtrip successfully.
  testDecodeOnly(
      "040a0098000000000000015805ea00000000000000000000000100698000000400000001"
      "80000a020800800014010680004c08000000000000000080001a02d3a780000408000000"
      "000000000080001c021389800018040a000002800016040a00000180000606da1dfcec1d"
      "9980001001008000080642e50bb51ff5800012010000da1dfcec1d990000da1dfcec1d99"
      "42e50bb51ff50800",
      "---\ntype:            PACKET_IN\nxid:             0x00000000\nversion:  "
      "       0x04\nmsg:             \n  buffer_id:       0x00000158\n  "
      "total_len:       0x05EA\n  in_port:         0x00000001\n  in_phy_port:  "
      "   0x00000001\n  metadata:        0x0000000000000000\n  reason:         "
      " TABLE_MISS\n  table_id:        0x00\n  cookie:          "
      "0x0000000000000000\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x00000001\n    - field:           "
      "ETH_TYPE\n      value:           0x0800\n    - field:           "
      "IP_PROTO\n      value:           0x06\n    - field:           "
      "TUNNEL_ID\n      value:           0x0000000000000000\n    - field:      "
      "     TCP_SRC\n      value:           0xD3A7\n    - field:           "
      "METADATA\n      value:           0x0000000000000000\n    - field:       "
      "    TCP_DST\n      value:           0x1389\n    - field:           "
      "IPV4_DST\n      value:           10.0.0.2\n    - field:           "
      "IPV4_SRC\n      value:           10.0.0.1\n    - field:           "
      "ETH_DST\n      value:           'da:1d:fc:ec:1d:99'\n    - field:       "
      "    IP_DSCP\n      value:           0x00\n    - field:           "
      "ETH_SRC\n      value:           '42:e5:0b:b5:1f:f5'\n    - field:       "
      "    IP_ECN\n      value:           0x00\n  data:            "
      "DA1DFCEC1D9942E50BB51FF50800\n...\n");
}

TEST(decoder, ofmp_groupstats_v3) {
  testDecodeEncode(
      "031300400000000000060000000000000030000000000001000000020000000010000000"
      "00009999200000000000AAAA300000000000BBBB400000000000CCCC",
      "---\ntype:            REPLY.GROUP\nflags:           [  ]\nxid:          "
      "   0x00000000\nversion:         0x03\nmsg:             \n  - group_id:  "
      "      0x00000001\n    ref_count:       0x00000002\n    packet_count:    "
      "0x1000000000009999\n    byte_count:      0x200000000000AAAA\n    "
      "duration:        0\n    bucket_stats:    \n      - packet_count:    "
      "0x300000000000BBBB\n        byte_count:      0x400000000000CCCC\n...\n");
}

TEST(decoder, ofmp_groupstats_v3_2) {
  testDecodeEncode(
      "031300700000000000060000000000000030000000000001000000020000000010000000"
      "00009999200000000000AAAA300000000000BBBB400000000000CCCC0030000000000001"
      "00000002000000001000000000009999200000000000AAAA300000000000BBBB40000000"
      "0000CCCC",
      "---\ntype:            REPLY.GROUP\nflags:           [  ]\nxid:          "
      "   0x00000000\nversion:         0x03\nmsg:             \n  - group_id:  "
      "      0x00000001\n    ref_count:       0x00000002\n    packet_count:    "
      "0x1000000000009999\n    byte_count:      0x200000000000AAAA\n    "
      "duration:        0\n    bucket_stats:    \n      - packet_count:    "
      "0x300000000000BBBB\n        byte_count:      0x400000000000CCCC\n  - "
      "group_id:        0x00000001\n    ref_count:       0x00000002\n    "
      "packet_count:    0x1000000000009999\n    byte_count:      "
      "0x200000000000AAAA\n    duration:        0\n    bucket_stats:    \n     "
      " - packet_count:    0x300000000000BBBB\n        byte_count:      "
      "0x400000000000CCCC\n...\n");
}

TEST(decoder, ofmp_table_features) {
  testDecodeEncode(
      "0413018800000000000C0000000000000178000000000000506F72742041434C00000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000003000000320000001800010004000300040004000400050004000600040002000B"
      "0102030405060700000000000004001C0000000400110004001200040016000400170004"
      "00190004000000000006001C000000040011000400120004001600040017000400190004"
      "00000000000800348000000480000A028000070C8000090C80000C028000140180001908"
      "8000372080001E028000200280001A0280001C0200000000000A00348000000480000A02"
      "800006068000080680000C0280001401800018048000361080001E028000200280001A02"
      "80001C0200000000000C0030800006068000080680000C0280000E018000100180001604"
      "8000180480001A0280001C0280001E0280002002000E0030800006068000080680000C02"
      "80000E0180001001800016048000180480001A0280001C0280001E0280002002",
      "---\ntype:            REPLY.TABLE_FEATURES\nflags:           [  ]\nxid: "
      "            0x00000000\nversion:         0x04\nmsg:             \n  - "
      "table_id:        0x00\n    name:            Port ACL\n    "
      "metadata_match:  0x0000000000000000\n    metadata_write:  "
      "0x0000000000000000\n    config:          [ '0x00000003' ]\n    "
      "max_entries:     0x00000032\n    instructions:    [ GOTO_TABLE, "
      "WRITE_ACTIONS, APPLY_ACTIONS, CLEAR_ACTIONS, \n                       "
      "METER ]\n    next_tables:     [ 1, 2, 3, 4, 5, 6, 7 ]\n    "
      "write_actions:   [ OUTPUT, PUSH_VLAN, POP_VLAN, GROUP, SET_NW_TTL, \n   "
      "                    SET_FIELD ]\n    apply_actions:   [ OUTPUT, "
      "PUSH_VLAN, POP_VLAN, GROUP, SET_NW_TTL, \n                       "
      "SET_FIELD ]\n    match:           [ IN_PORT, ETH_TYPE, ETH_DST/, "
      "ETH_SRC/, VLAN_VID, \n                       IP_PROTO, IPV4_DST/, "
      "IPV6_DST/, UDP_SRC, UDP_DST, \n                       TCP_SRC, TCP_DST "
      "]\n    wildcards:       [ IN_PORT, ETH_TYPE, ETH_DST, ETH_SRC, "
      "VLAN_VID, IP_PROTO, \n                       IPV4_DST, IPV6_DST, "
      "UDP_SRC, UDP_DST, TCP_SRC, TCP_DST ]\n    write_set_field: [ ETH_DST, "
      "ETH_SRC, VLAN_VID, VLAN_PCP, IP_DSCP, IPV4_SRC, \n                      "
      " IPV4_DST, TCP_SRC, TCP_DST, UDP_SRC, UDP_DST ]\n    apply_set_field: [ "
      "ETH_DST, ETH_SRC, VLAN_VID, VLAN_PCP, IP_DSCP, IPV4_SRC, \n             "
      "          IPV4_DST, TCP_SRC, TCP_DST, UDP_SRC, UDP_DST ]\n    "
      "properties:      \n...\n");
}

TEST(decoder, packet_in_fuzz1) {
  // Invalid oxm range.
  testDecodeFail(
      "040a00940000000000000002 002a 010100010203000000000001 "
      "0058800020240000000680000aa0080680000606ffffffffbfff80000806f20ba47df8ea"
      "80002a02000180002c140a00000180002e040a00000300003006f20ba47df8ea80003206"
      "0000000084040000fffffffffffff20ba47df8ea08060001080206040001f20ba47df8ea"
      "0a0000030000000000000a000003");

  // Wrong match type.
  testDecodeFail(
      "040a00940000000000000002 002a 010100010203000000000000 "
      "0058800020240000000680000aa0080680000606ffffffffbfff80000806f20ba47df8ea"
      "80002a02000180002c140a00000180002e040a00000300003006f20ba47df8ea80003206"
      "0000000084040000fffffffffffff20ba47df8ea08060001080206040001f20ba47df8ea"
      "0a0000030000000000000a000003");
}

TEST(decoder, table_status_fuzz) {
  testDecodeFail(
      "041f002000000000000000050000000180000007000081030000200f00000003");
}

TEST(decoder, flowmod_fuzz) {
  testDecodeEncode(
      "030e00800000000000000000000000000000000000000000010000000000007b0000ffff"
      "ffffffffffffffff000000000001000e80000606f20ba47df8ea00000003002800000000"
      "0019001080000c0201020000000000000000001000000006ffff00000000000000040018"
      "0000000000190010800008060102030405060000",
      "---\ntype:            FLOW_MOD\nxid:             0x00000000\nversion:   "
      "      0x03\nmsg:             \n  cookie:          0x0000000000000000\n  "
      "cookie_mask:     0x0000000000000000\n  table_id:        0x01\n  "
      "command:         ADD\n  idle_timeout:    0x0000\n  hard_timeout:    "
      "0x0000\n  priority:        0x007B\n  buffer_id:       0x0000FFFF\n  "
      "out_port:        ANY\n  out_group:       ANY\n  flags:           [  ]\n "
      " match:           \n    - field:           ETH_DST\n      value:        "
      "   'f2:0b:a4:7d:f8:ea'\n  instructions:    \n    - instruction:     "
      "WRITE_ACTIONS\n      actions:         \n        - action:          "
      "SET_FIELD\n          field:           VLAN_VID\n          value:        "
      "   0x0102\n        - action:          OUTPUT\n          port_no:        "
      " 0x00000006\n          max_len:         NO_BUFFER\n    - instruction:   "
      "  APPLY_ACTIONS\n      actions:         \n        - action:          "
      "SET_FIELD\n          field:           ETH_SRC\n          value:         "
      "  '01:02:03:04:05:06'\n...\n");
  // This should fail.
  testDecodeFail(
      "030e00800000000000000000000000000000000000000000010000000000007b0000ffff"
      "ffffffffffffffff000000000001000e80000606f20ba47df8ea00000003002800000000"
      "0019001080000cff01020000000000000000001000000006ffff00000000000000040018"
      "0000000000190010800008060102030405060000");
}

TEST(decoder, portstatus_fuzz) {
  // This should fail.
  testDecodeFail(
      "050c00480000000000090000000000000000006400380000000000000000000000000000"
      "00000000000000000000000000000000000753000000000000000000000000000000000"
      "0");
}
