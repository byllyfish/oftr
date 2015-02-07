// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using namespace yaml;

static void testDecodeEncode(const char *hex, const char *yaml) {
  auto s = HexToRawData(hex);

  Message msg{s.data(), s.size()};
  msg.transmogrify();

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
      "      OFPET_BAD_REQUEST\n  code:            OFPBRC_BAD_TYPE\n  data:            "
      "FFFF1234567890\n...\n");
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
  msg.transmogrify();

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
      "actions:         [  ]\n  ports:           \n    - port_no:        "
      " 0x00001111\n      hw_addr:         '22:22:22:22:22:22'\n      name:    "
      "        Port 1\n      config:          [ PORT_DOWN, NO_STP, NO_FLOOD, NO_FWD, '0x33333300' ]\n      state:         "
      "  [ LIVE, STP_LISTEN, '0x44444440' ]\n      curr:            [ 10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, FIBER, PAUSE, '0x55550000' ]\n      advertised:      "
      "[ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, AUTONEG, PAUSE, '0x66660000' ]\n      supported:       [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, FIBER, AUTONEG, PAUSE, '0x77770000' ]\n      peer:            "
      "[ 100MB_FD, COPPER, PAUSE_ASYM, '0x88880000' ]\n      curr_speed:      0x00000000\n      max_speed:       "
      "0x00000000\n    - port_no:         0x0000BBBB\n      hw_addr:         "
      "'cc:cc:cc:cc:cc:cc'\n      name:            Port 2\n      config:       "
      "   [ PORT_DOWN, NO_STP, NO_FLOOD, NO_FWD, '0x33333300' ]\n      state:           [ LIVE, STP_LISTEN, '0x44444440' ]\n      curr:           "
      " [ 10MB_HD, 100MB_HD, 1GB_HD, 10GB_FD, FIBER, PAUSE, '0x55550000' ]\n      advertised:      [ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, AUTONEG, PAUSE, '0x66660000' ]\n      supported:       "
      "[ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, FIBER, AUTONEG, PAUSE, '0x77770000' ]\n      peer:            [ 100MB_FD, COPPER, PAUSE_ASYM, '0x88880000' ]\n      curr_speed:      "
      "0x00000000\n      max_speed:       0x00000000\n...\n");
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
      "---\ntype:            MULTIPART_REQUEST\nxid:             "
      "0x11223344\nversion:         0x04\nmsg:             \n  type:           "
      " "
      "FLOW\n  flags:           [  ]\n  body:            \n    "
      "table_id:        0x01\n    out_port:        0x00000002\n    out_group:  "
      "   "
      "  0x00000003\n    cookie:          0x0000000000000004\n    cookie_mask: "
      "    0x0000000000000005\n    match:           \n      - field:           "
      "IN_PORT\n        value:           0x12345678\n...\n");
}

TEST(decoder, ofmp_flowrequest_v1) {
  testDecodeEncode(
      "011000381122334400010000003820FE567"
      "8000000000000000000000000000000000000000000"
      "0000000000000000000000000011002222",
      "---\ntype:            MULTIPART_REQUEST\nxid:             "
      "0x11223344\nversion:         0x01\nmsg:             \n  type:           "
      " "
      "FLOW\n  flags:           [  ]\n  body:            \n    "
      "table_id:        0x11\n    out_port:        0x00002222\n    out_group:  "
      "  "
      "   0x00000000\n    cookie:          0x0000000000000000\n    "
      "cookie_mask:     0x0000000000000000\n    match:           \n      - "
      "field:           IN_PORT\n        value:           "
      "0x00005678\n...\n");
}

TEST(decoder, ofmp_aggregaterequest_v4) {
  testDecodeEncode(
      "04120040112233440002000000000000110000002222222233333333000"
      "00000444444444444444455555555555555550001000C80000004123456"
      "7800000000",
      "---\ntype:            MULTIPART_REQUEST\nxid:         "
      "    0x11223344\nversion:         0x04\nmsg:             \n  "
      "type:            AGGREGATE\n  flags:           "
      "[  ]\n  body:            \n    table_id:        0x11\n    "
      "out_port:        0x22222222\n    out_group:       "
      "0x33333333\n    cookie:          0x4444444444444444\n    "
      "cookie_mask:     0x5555555555555555\n    match:           "
      "\n      - field:           IN_PORT\n        value:     "
      "      0x12345678\n...\n");
}

TEST(decoder, ofmp_aggregaterequest_v1) {
  testDecodeEncode(
      "011000381122334400020000003820FE567"
      "8000000000000000000000000000000000000000000"
      "0000000000000000000000000011002222",
      "---\ntype:            MULTIPART_REQUEST\nxid:             "
      "0x11223344\nversion:         0x01\nmsg:             \n  type:           "
      " "
      "AGGREGATE\n  flags:           [  ]\n  body:            \n    "
      "table_id:        0x11\n    out_port:        0x00002222\n    out_group:  "
      "  "
      "   0x00000000\n    cookie:          0x0000000000000000\n    "
      "cookie_mask:     0x0000000000000000\n    match:           \n      - "
      "field:           IN_PORT\n        value:           "
      "0x00005678\n...\n");
}

TEST(decoder, ofmp_flowreply_v4) {
  testDecodeEncode(
      "0413005811223344000100000000000000480100000000020000000300"
      "0400050006000700000000000000000000000800000000000000090000"
      "00000000000A0001000C8000000412345678000000000001000801000000",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11223344\nversion:         0x04\nmsg:             \n  type:           "
      " "
      "FLOW\n  flags:           [  ]\n  body:            \n    - "
      "table_id:        0x01\n      duration_sec:    0x00000002\n      "
      "duration_nsec:   0x00000003\n      priority:        0x0004\n      "
      "idle_timeout:    0x0005\n      hard_timeout:    0x0006\n      flags:    "
      "       [ SEND_FLOW_REM, CHECK_OVERLAP, RESET_COUNTS ]\n      cookie:          0x0000000000000008\n      "
      "packet_count:    0x0000000000000009\n      byte_count:      "
      "0x000000000000000A\n      match:           \n        - field:           "
      "IN_PORT\n          value:           0x12345678\n      instructions: "
      " "
      "  \n        - instruction:     GOTO_TABLE\n          table_id:    "
      "    0x01\n...\n");
}

TEST(decoder, ofmp_flowreply2_v4) {
  testDecodeEncode(
      "041300A81122334400010000000000000040010000000002000000030"
      "004000500060007000000000000000000000008000000000000000900"
      "0000000000000A0001000C8000000412345678000000000058110000000022000"
      "000330044005500660077000000000000000000000088999999999999"
      "9999AAAAAAAAAAAAAAAA0001002080000004123456788000080610203"
      "040506080000606AABBCCDDEEFF0001000801000000",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11223344\nversion:         0x04\nmsg:             \n  type:           "
      " "
      "FLOW\n  flags:           [  ]\n  body:            \n    - "
      "table_id:        0x01\n      duration_sec:    0x00000002\n      "
      "duration_nsec:   0x00000003\n      priority:        0x0004\n      "
      "idle_timeout:    0x0005\n      hard_timeout:    0x0006\n      flags:    "
      "       [ SEND_FLOW_REM, CHECK_OVERLAP, RESET_COUNTS ]\n      cookie:          0x0000000000000008\n      "
      "packet_count:    0x0000000000000009\n      byte_count:      "
      "0x000000000000000A\n      match:           \n        - field:           "
      "IN_PORT\n          value:           0x12345678\n      instructions: "
      " "
      "  \n    - table_id:        0x11\n      duration_sec:    0x00000022\n    "
      "  "
      "duration_nsec:   0x00000033\n      priority:        0x0044\n      "
      "idle_timeout:    0x0055\n      hard_timeout:    0x0066\n      flags:    "
      "       [ SEND_FLOW_REM, CHECK_OVERLAP, RESET_COUNTS, NO_BYT_COUNTS, '0x00000060' ]\n      cookie:          0x0000000000000088\n      "
      "packet_count:    0x9999999999999999\n      byte_count:      "
      "0xAAAAAAAAAAAAAAAA\n      match:           \n        - field:           "
      "IN_PORT\n          value:           0x12345678\n        - field:    "
      " "
      "      ETH_SRC\n          value:           '10:20:30:40:50:60'\n     "
      "  "
      " - field:           ETH_DST\n          value:           "
      "'aa:bb:cc:dd:ee:ff'\n      instructions:    \n        - instruction:    "
      " "
      "GOTO_TABLE\n          table_id:        0x01\n...\n");
}

TEST(decoder, ofmp_flowreply_v1) {
  testDecodeEncode(
      "0111006C111111110001222200603300003820FEDDDD0000000000000000000000000000"
      "000000000000000000000000000000000000000044444444555555556666777788880000"
      "00000000AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCC00000008EEEEFFF"
      "F",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  type:           "
      " "
      "FLOW\n  flags:           [ '0x00002222' ]\n  body:            \n    - "
      "table_id:        0x33\n      duration_sec:    0x44444444\n      "
      "duration_nsec:   0x55555555\n      priority:        0x6666\n      "
      "idle_timeout:    0x7777\n      hard_timeout:    0x8888\n      flags:    "
      "       [  ]\n      cookie:          0xAAAAAAAAAAAAAAAA\n      "
      "packet_count:    0xBBBBBBBBBBBBBBBB\n      byte_count:      "
      "0xCCCCCCCCCCCCCCCC\n      match:           \n        - field:           "
      "IN_PORT\n          value:           0x0000DDDD\n      instructions: "
      "   "
      "\n        - instruction:     APPLY_ACTIONS\n          actions:    "
      "     \n            - action:          OUTPUT\n              port: "
      "           0x0000EEEE\n              max_len:         NO_BUFFER\n...\n");
}

TEST(decoder, ofmp_flowreply2_v1) {
  testDecodeEncode(
      "011100CC112233440001000000600100003820FE56780000000000000000000000000000"
      "000000000000000000000000000000000000000000000002000000030004000500060000"
      "0000000000000000000000080000000000000009000000000000000A00000008EEEEFFFF"
      "00601100003820F25678102030405060AABBCCDDEEFF0000000000000000000000000000"
      "000000000000000000000022000000330044005500660000000000000000000000000088"
      "9999999999999999AAAAAAAAAAAAAAAA00000008EEEEFFFF",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11223344\nversion:         0x01\nmsg:             \n  type:           "
      " "
      "FLOW\n  flags:           [  ]\n  body:            \n    - "
      "table_id:        0x01\n      duration_sec:    0x00000002\n      "
      "duration_nsec:   0x00000003\n      priority:        0x0004\n      "
      "idle_timeout:    0x0005\n      hard_timeout:    0x0006\n      flags:    "
      "       [  ]\n      cookie:          0x0000000000000008\n      "
      "packet_count:    0x0000000000000009\n      byte_count:      "
      "0x000000000000000A\n      match:           \n        - field:           "
      "IN_PORT\n          value:           0x00005678\n      instructions: "
      "   "
      "\n        - instruction:     APPLY_ACTIONS\n          actions:    "
      "     \n            - action:          OUTPUT\n              port: "
      "           0x0000EEEE\n              max_len:         NO_BUFFER\n    - "
      "table_id: "
      "       0x11\n      duration_sec:    0x00000022\n      duration_nsec:   "
      "0x00000033\n      priority:        0x0044\n      idle_timeout:    "
      "0x0055\n      hard_timeout:    0x0066\n      flags:           [  ]\n  "
      "    cookie:          0x0000000000000088\n      packet_count:    "
      "0x9999999999999999\n      byte_count:      0xAAAAAAAAAAAAAAAA\n      "
      "match:           \n        - field:           IN_PORT\n          "
      "value:           0x00005678\n        - field:           ETH_SRC\n   "
      "     "
      "  value:           '10:20:30:40:50:60'\n        - field:           "
      "ETH_DST\n          value:           'aa:bb:cc:dd:ee:ff'\n      "
      "instructions:    \n        - instruction:     APPLY_ACTIONS\n     "
      "     actions:         \n            - action:          OUTPUT\n   "
      "           port:            0x0000EEEE\n              max_len:         "
      "NO_BUFFER\n...\n");
}

TEST(decoder, ofmp_aggregatereply_v4) {
  testDecodeEncode(
      "041300281111111100022222000000003333333333333330444444444444444055555550"
      "00000000",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  type:           "
      " "
      "AGGREGATE\n  flags:           [ '0x00002222' ]\n  body:            \n    "
      "packet_count:    0x3333333333333330\n    byte_count:      "
      "0x4444444444444440\n    flow_count:      0x55555550\n...\n");
}

TEST(decoder, ofmp_aggregatereply_v1) {
  testDecodeEncode(
      "01110024111111110002222233333333333333304444444444444440555555500000000"
      "0",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  type:           "
      " "
      "AGGREGATE\n  flags:           [ '0x00002222' ]\n  body:            \n    "
      "packet_count:    0x3333333333333330\n    byte_count:      "
      "0x4444444444444440\n    flow_count:      0x55555550\n...\n");
}

TEST(decoder, ofmp_tablestats_v4) {
  testDecodeEncode(
      "041300401111111100032222000000003300000044444440555555555555555066666666"
      "6666666077000000888888809999999999999990AAAAAAAAAAAAAAA0",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  type:           "
      " "
      "TABLE\n  flags:           [ '0x00002222' ]\n  body:            \n    - "
      "table_id:        0x33\n      name:            ''\n      wildcards:      "
      " "
      "0x00000000\n      max_entries:     0x00000000\n      active_count:    "
      "0x44444440\n      lookup_count:    0x5555555555555550\n      "
      "matched_count:   0x6666666666666660\n    - table_id:        0x77\n      "
      "name:            ''\n      wildcards:       0x00000000\n      "
      "max_entries:     0x00000000\n      active_count:    0x88888880\n      "
      "lookup_count:    0x9999999999999990\n      matched_count:   "
      "0xAAAAAAAAAAAAAAA0\n...\n");
}

TEST(decoder, ofmp_tablestats_v1) {
  testDecodeEncode(
      "0111008C1111111100032222330000005461626C65203100000000000000000000000000"
      "000000000000000000000000444444405555555066666660777777777777777088888888"
      "88888880110000005461626C652032000000000000000000000000000000000000000000"
      "0000000099999990AAAAAAA0BBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  type:           "
      " "
      "TABLE\n  flags:           [ '0x00002222' ]\n  body:            \n    - "
      "table_id:        0x33\n      name:            Table 1\n      wildcards: "
      "  "
      "    0x44444440\n      max_entries:     0x55555550\n      active_count:  "
      "  0x66666660\n      lookup_count:    0x7777777777777770\n      "
      "matched_count:   0x8888888888888880\n    - table_id:        0x11\n      "
      "name:            Table 2\n      wildcards:       0x99999990\n      "
      "max_entries:     0xAAAAAAA0\n      active_count:    0xBBBBBBB0\n      "
      "lookup_count:    0xCCCCCCCCCCCCCCC0\n      matched_count:   "
      "0xDDDDDDDDDDDDDDD0\n...\n");
}

TEST(decoder, ofmp_portstats_v4) {
  testDecodeEncode(
      "041300801111111100042222000000003333333000000000444444444444444055555555"
      "555555506666666666666660777777777777777088888888888888809999999999999990"
      "AAAAAAAAAAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEE"
      "EEEEEEE0FFFFFFFFFFFFFFF01111111022222220",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  type:           "
      " "
      "PORT_STATS\n  flags:           [ '0x00002222' ]\n  body:            \n    - "
      "port_no:         0x33333330\n      rx_packets:      "
      "0x4444444444444440\n      tx_packets:      0x5555555555555550\n      "
      "rx_bytes:        0x6666666666666660\n      tx_bytes:        "
      "0x7777777777777770\n      rx_dropped:      0x8888888888888880\n      "
      "tx_dropped:      0x9999999999999990\n      rx_errors:       "
      "0xAAAAAAAAAAAAAAA0\n      tx_errors:       0xBBBBBBBBBBBBBBB0\n      "
      "rx_frame_err:    0xCCCCCCCCCCCCCCC0\n      rx_over_err:     "
      "0xDDDDDDDDDDDDDDD0\n      rx_crc_err:      0xEEEEEEEEEEEEEEE0\n      "
      "collisions:      0xFFFFFFFFFFFFFFF0\n      duration_sec:    "
      "0x11111110\n      duration_nsec:   0x22222220\n...\n");
}

TEST(decoder, ofmp_portstats_v1) {
  testDecodeEncode(
      "011100741111111100042222333000000000000044444444444444405555555555555550"
      "6666666666666660777777777777777088888888888888809999999999999990AAAAAAAA"
      "AAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEEEEEEEEE0"
      "FFFFFFFFFFFFFFF0",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  type:           "
      " "
      "PORT_STATS\n  flags:           [ '0x00002222' ]\n  body:            \n    - "
      "port_no:         0x00003330\n      rx_packets:      "
      "0x4444444444444440\n      tx_packets:      0x5555555555555550\n      "
      "rx_bytes:        0x6666666666666660\n      tx_bytes:        "
      "0x7777777777777770\n      rx_dropped:      0x8888888888888880\n      "
      "tx_dropped:      0x9999999999999990\n      rx_errors:       "
      "0xAAAAAAAAAAAAAAA0\n      tx_errors:       0xBBBBBBBBBBBBBBB0\n      "
      "rx_frame_err:    0xCCCCCCCCCCCCCCC0\n      rx_over_err:     "
      "0xDDDDDDDDDDDDDDD0\n      rx_crc_err:      0xEEEEEEEEEEEEEEE0\n      "
      "collisions:      0xFFFFFFFFFFFFFFF0\n      duration_sec:    "
      "0x00000000\n      duration_nsec:   0x00000000\n...\n");
}

TEST(decoder, ofmp_queuestats_v4) {
  testDecodeEncode(
      "041300381111111100052222000000003333333044444440555555555555555066666666"
      "6666666077777777777777701111111022222220",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  type:           "
      " "
      "QUEUE\n  flags:           [ '0x00002222' ]\n  body:            \n    - "
      "port_no:         0x33333330\n      queue_id:        0x44444440\n      "
      "tx_packets:      0x6666666666666660\n      tx_bytes:        "
      "0x5555555555555550\n      tx_errors:       0x7777777777777770\n      "
      "duration_sec:    0x11111110\n      duration_nsec:   0x22222220\n...\n");
}

TEST(decoder, ofmp_queuestats_v1) {
  testDecodeEncode(
      "0111002C1111111100052222333000004444444055555555555555506666666666666660"
      "7777777777777770",
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  type:           "
      " "
      "QUEUE\n  flags:           [ '0x00002222' ]\n  body:            \n    - "
      "port_no:         0x00003330\n      queue_id:        0x44444440\n      "
      "tx_packets:      0x6666666666666660\n      tx_bytes:        "
      "0x5555555555555550\n      tx_errors:       0x7777777777777770\n      "
      "duration_sec:    0x00000000\n      duration_nsec:   0x00000000\n...\n");
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
      "0xAAAAAAAA\n  flags:           [ SEND_FLOW_REM, CHECK_OVERLAP, NO_PKT_COUNTS, NO_BYT_COUNTS, '0x0000BBA0' ]\n  match:           \n    - field:"
      "           IN_PORT\n      value:           0xCCCCCCCC\n    - field: "
      "          ETH_TYPE\n      value:           0x0800\n    - field:     "
      "  "
      "    IPV4_DST\n      value:           192.168.1.1\n  instructions:   "
      " \n    - instruction:     APPLY_ACTIONS\n      actions:         "
      "\n        - action:          SET_FIELD\n          field:          "
      " IPV4_DST\n          value:           192.168.2.1\n...\n");
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
      "0x00000000\n  flags:           [ SEND_FLOW_REM, CHECK_OVERLAP, NO_PKT_COUNTS, NO_BYT_COUNTS, '0x0000BBA0' ]\n  match:           \n    - field:"
      "           IN_PORT\n      value:           0x0000CCCC\n    - field: "
      "     "
      "     ETH_TYPE\n      value:           0x0800\n    - field:          "
      " "
      "IPV4_DST\n      value:           192.168.1.1\n  instructions:    \n "
      "   - instruction:     APPLY_ACTIONS\n      actions:         \n    "
      "    - action:          SET_FIELD\n          field:           "
      "IPV4_DST\n          value:           192.168.2.1\n...\n";


  testDecodeEncode(hex, yaml);
  #if 0
  auto s = HexToRawData(hex);

  Message msg{s.data(), s.size()};
  msg.transmogrify();
  EXPECT_EQ(0xA0, msg.size());
  EXPECT_HEX(
      "010E00A0000000011111111111111111FFFFFFFFFFFFFFFF00445555666677778"
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
  #endif //0
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
      "0x00000000\n  flags:           [ SEND_FLOW_REM, CHECK_OVERLAP, NO_PKT_COUNTS, NO_BYT_COUNTS, '0x0000BBA0' ]\n  match:           \n    - field:"
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
      "reason:          APPLY_ACTION\n  table_id:        0x88\n  cookie:        "
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
      "reason:          APPLY_ACTION\n  table_id:        0x00\n  cookie:        "
      "  0x0000000000000000\n  match:           \n  data:            "
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
      "action:          OUTPUT\n      port:            0x00000005\n      "
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
      "         OUTPUT\n      port:            0x00000005\n      "
      "max_len:         "
      "0x0014\n    - action:          SET_FIELD\n      field:           "
      "IPV4_DST\n      value:           192.168.1.1\n  data:            "
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002\n...\n";

  testDecodeEncode(hex, yaml);

#if 0
  auto s = HexToRawData(hex);

  Message msg{s.data(), s.size()};
  msg.transmogrify();
  EXPECT_HEX(
      "010D0062000000013333333300004444002000000000000000000010000000050"
      "0140000000000000019001080001804C0A8010100000000FFFFFFFFFFFF000000"
      "000001080600010800060400010000000000010A0000010000000000000A00000"
      "2",
      msg.data(), msg.size());

  Decoder decoder{&msg};

  EXPECT_EQ("", decoder.error());
  EXPECT_EQ(yaml, decoder.result().str());

  Encoder encoder{decoder.result()};

  EXPECT_EQ("", encoder.error());
  EXPECT_HEX(hex, encoder.data(), encoder.size());
#endif //0
}

TEST(decoder, setconfigv4) {
  testDecodeEncode("0409000C11111111AAAABBBB",
                   "---\ntype:            SET_CONFIG\nxid:             "
                   "0x11111111\nversion:         0x04\nmsg:             \n  "
                   "flags:           [ FRAG_REASM, '0x0000AAA8' ]\n  miss_send_len:   0xBBBB\n...\n");
}

TEST(decoder, portstatusv4) {
  testDecodeEncode(
      "040C00501111111122000000000000003333333300000000AABBCCDDEEFF0000506F7274"
      "203100000000000000000000444444445555555566666666777777778888888899999999"
      "AAAAAAAABBBBBBBB",
      "---\ntype:            PORT_STATUS\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  reason:         "
      " "
      "0x22\n  port:            \n    port_no:         0x33333333\n    hw_addr:  "
      "       'aa:bb:cc:dd:ee:ff'\n    name:            Port 1\n    config:    "
      "  "
      "    [ NO_RECV, NO_PACKET_IN, '0x44444400' ]\n    state:           [ LINK_DOWN, LIVE, '0x55555550' ]\n    curr:            "
      "[ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' ]\n    advertised:      [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, 100GB_FD, 1TB_FD, OTHER, FIBER, AUTONEG, PAUSE, '0x77770000' ]\n    supported:       "
      "[ 100MB_FD, 40GB_FD, COPPER, PAUSE_ASYM, '0x88880000' ]\n    peer:            [ 10MB_HD, 100MB_FD, 1GB_HD, 40GB_FD, 100GB_FD, COPPER, FIBER, PAUSE_ASYM, '0x99990000' ]\n    curr_speed:      "
      "0xAAAAAAAA\n    max_speed:       0xBBBBBBBB\n...\n");
}

TEST(decoder, portstatusv1) {
  testDecodeEncode(
      "010C00401111111122000000000000003333AABBCCDDEEFF506F72742031000000000000"
      "00000000444444445555555566660666777707778888088899990999",
      "---\ntype:            PORT_STATUS\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  reason:         "
      " "
      "0x22\n  port:            \n    port_no:         0x00003333\n    hw_addr:  "
      "       'aa:bb:cc:dd:ee:ff'\n    name:            Port 1\n    config:    "
      "  "
      "    [ NO_RECV, NO_PACKET_IN, '0x44444400' ]\n    state:           [ LINK_DOWN, LIVE, STP_LEARN, '0x55555450' ]\n    curr:            "
      "[ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, AUTONEG, PAUSE, '0x66660000' ]\n    advertised:      [ 10MB_HD, 10MB_FD, 100MB_HD, 1GB_HD, 1GB_FD, 10GB_FD, FIBER, AUTONEG, PAUSE, '0x77770000' ]\n    supported:       "
      "[ 100MB_FD, COPPER, PAUSE_ASYM, '0x88880000' ]\n    peer:            [ 10MB_HD, 100MB_FD, 1GB_HD, COPPER, FIBER, PAUSE_ASYM, '0x99990000' ]\n    curr_speed:      "
      "0x00000000\n    max_speed:       0x00000000\n...\n");
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
      "\n        - action:          OUTPUT\n          port:            "
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
      "---\ntype:            PORT_MOD\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  "
      "port_no:         0x22222222\n  hw_addr:         "
      "'33:33:33:33:33:33'\n  config:          [ NO_RECV, NO_PACKET_IN, '0x44444400' ]\n  mask:  "
      "          [ PORT_DOWN, NO_RECV, NO_FLOOD, NO_PACKET_IN, '0x55555500' ]\n  advertise:       "
      "[ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' ]\n...\n");
}

TEST(decoder, portmodv1) {
  testDecodeEncode(
      "010F002011111111222233333333333344444444555555556666666600000000",
      "---\ntype:            PORT_MOD\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  port_no:        "
      " "
      "0x00002222\n  hw_addr:         '33:33:33:33:33:33'\n  config:          "
      "[ NO_RECV, NO_PACKET_IN, '0x44444400' ]\n  mask:            [ PORT_DOWN, NO_RECV, NO_FLOOD, NO_PACKET_IN, '0x55555500' ]\n  advertise:       "
      "[ 10MB_FD, 100MB_HD, 1GB_FD, 10GB_FD, 1TB_FD, OTHER, AUTONEG, PAUSE, '0x66660000' ]\n...\n");
}

TEST(decoder, tablemodv4) {
  testDecodeEncode(
      "04110010111111112200000033333333",
      "---\ntype:            TABLE_MOD\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  "
      "table_id:        0x22\n  config:          0x33333333\n...\n");
}

TEST(decoder, tablemodv2) {
  testDecodeEncode(
      "02110010111111112200000033333333",
      "---\ntype:            TABLE_MOD\nxid:             "
      "0x11111111\nversion:         0x02\nmsg:             \n  "
      "table_id:        0x22\n  config:          0x33333333\n...\n");
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
      "packet_in_mask_master: [ APPLY_ACTION, PACKET_OUT, '0x22222200' ]\n  packet_in_mask_slave: [ TABLE_MISS, APPLY_ACTION, GROUP, PACKET_OUT, '0x33333300' ]\n "
      " port_status_mask_master: [ MODIFY, '0x44444440' ]\n  port_status_mask_slave: "
      "[ ADD, MODIFY, '0x55555550' ]\n  flow_removed_mask_master: [ HARD_TIMEOUT, DELETE, '0x66666660' ]\n  "
      "flow_removed_mask_slave: [ IDLE_TIMEOUT, HARD_TIMEOUT, DELETE, '0x77777770' ]\n...\n");
}

TEST(decoder, queuegetconfigrequestv4) {
  testDecodeEncode(
      "04160010111111112222222200000000",
      "---\ntype:            QUEUE_GET_CONFIG_REQUEST\nxid:  "
      "           0x11111111\nversion:         0x04\nmsg:            "
      " \n  port:            0x22222222\n...\n");
}

TEST(decoder, queuegetconfigreplyv4) {
  testDecodeEncode(
      "041700701111111122222222000000003333333344444444003000000000000000010010"
      "000000005555000000000000000200100000000066660000000000007777777788888888"
      "0030000000000000000100100000000099990000000000000002001000000000AAAA0000"
      "00000000",
      "---\ntype:            QUEUE_GET_CONFIG_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  port:           "
      " 0x22222222\n  queues:          \n    - queue_id:        0x33333333\n   "
      "   port:            0x44444444\n      min_rate:        0x5555\n      "
      "max_rate:        0x6666\n      properties:      \n    - queue_id:       "
      " 0x77777777\n      port:            0x88888888\n      min_rate:        "
      "0x9999\n      max_rate:        0xAAAA\n      properties:      \n...\n");
}

TEST(decoder, queuegetconfigreplyv4_experimenter) {
  testDecodeEncode(
      "041700A01111111122222222000000003333333344444444006000000000000000010010"
      "00000000555500000000000000020010000000006666000000000000FFFF001600000000"
      "EEEEEEEE000000000001020304050000FFFF001300000000FFFFFFFF00000000ABCDEF00"
      "000000007777777788888888003000000000000000010010000000009999000000000000"
      "0002001000000000AAAA000000000000",
      "---\ntype:            QUEUE_GET_CONFIG_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  port:           "
      " 0x22222222\n  queues:          \n    - queue_id:        0x33333333\n   "
      "   port:            0x44444444\n      min_rate:        0x5555\n      "
      "max_rate:        0x6666\n      properties:      \n        - "
      "experimenter:    4008636142\n          value:           000102030405\n  "
      "      - experimenter:    4294967295\n          value:           "
      "ABCDEF\n    - queue_id:        0x77777777\n      port:            "
      "0x88888888\n      min_rate:        0x9999\n      max_rate:        "
      "0xAAAA\n      properties:      \n...\n");
}

TEST(decoder, getconfigreplyv4) {
  testDecodeEncode("0408000C11111111AAAABBBB",
                   "---\ntype:            GET_CONFIG_REPLY\nxid:          "
                   "   0x11111111\nversion:         0x04\nmsg:             \n  "
                   "flags:           [ FRAG_REASM, '0x0000AAA8' ]\n  miss_send_len:   0xBBBB\n...\n");
}

TEST(decoder, setasyncv4) {
  testDecodeEncode(
      "041C002011111111222222223333333344444444555555556666666677777777",
      "---\ntype:            SET_ASYNC\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  "
      "packet_in_mask_master: [ APPLY_ACTION, PACKET_OUT, '0x22222200' ]\n  packet_in_mask_slave: [ TABLE_MISS, APPLY_ACTION, GROUP, PACKET_OUT, '0x33333300' ]\n "
      " port_status_mask_master: [ MODIFY, '0x44444440' ]\n  port_status_mask_slave: "
      "[ ADD, MODIFY, '0x55555550' ]\n  flow_removed_mask_master: [ HARD_TIMEOUT, DELETE, '0x66666660' ]\n  "
      "flow_removed_mask_slave: [ IDLE_TIMEOUT, HARD_TIMEOUT, DELETE, '0x77777770' ]\n...\n");
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
      "table_id:        0x55\n  duration_sec:    0x66666666\n  duration_nsec:  "
      " "
      "0x77777777\n  idle_timeout:    0x8888\n  hard_timeout:    0x9999\n  "
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
      "table_id:        0x00\n  duration_sec:    0x66666666\n  duration_nsec:  "
      " "
      "0x77777777\n  idle_timeout:    0x8888\n  hard_timeout:    0x0000\n  "
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
      "table_id:        0x55\n  duration_sec:    0x66666666\n  duration_nsec:  "
      " "
      "0x77777777\n  idle_timeout:    0x8888\n  hard_timeout:    0x0000\n  "
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
      "table_id:        0x55\n  duration_sec:    0x66666666\n  duration_nsec:  "
      " "
      "0x77777777\n  idle_timeout:    0x8888\n  hard_timeout:    0x9999\n  "
      "packet_count:    0xAAAAAAAAAAAAAAAA\n  byte_count:      "
      "0xBBBBBBBBBBBBBBBB\n  match:           \n    - field:           "
      "IN_PORT\n      value:           0x12345678\n...\n");
}

TEST(decoder, ofmp_desc_request_v4) {
  testDecodeEncode(
      "04120010111111110000000000000000",
      "---\ntype:            MULTIPART_REQUEST\nxid:         "
      "    0x11111111\nversion:         0x04\nmsg:             \n  "
      "type:            DESC\n  flags:           "
      "[  ]\n...\n");
}

TEST(decoder, ofmp_desc_request_v1) {
  testDecodeEncode("0110000C1111111100000000",
                   "---\ntype:            MULTIPART_REQUEST\nxid:         "
                   "    0x11111111\nversion:         0x01\nmsg:             \n "
                   " type:            DESC\n  flags:           "
                   "[  ]\n...\n");
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
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  type:           "
      " "
      "DESC\n  flags:           [  ]\n  body:            \n    "
      "mfr_desc:        ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n    hw_desc:     "
      "    ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n    sw_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n    serial_num:      "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n    dp_desc:         "
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
      "---\ntype:            MULTIPART_REPLY\nxid:             "
      "0x11111111\nversion:         0x01\nmsg:             \n  type:           "
      " DESC\n  flags:           [  ]\n  body:            \n    "
      "mfr_desc:        ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n    hw_desc:     "
      "    ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n    sw_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n    serial_num:      "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n    dp_desc:         "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n...\n");
}

TEST(decoder, ofmp_portstats_v4_request) {
  testDecodeEncode("041200181111111100040000000000002222222200000000",
                   "---\ntype:            MULTIPART_REQUEST\nxid:         "
                   "    0x11111111\nversion:         0x04\nmsg:             \n "
                   " type:            PORT_STATS\n  flags:           "
                   "[  ]\n  body:            \n    port_no:         "
                   "0x22222222\n...\n");
}

TEST(decoder, ofmp_portstats_v1_request) {
  testDecodeEncode("0110001411111111000400002222000000000000",
                   "---\ntype:            MULTIPART_REQUEST\nxid:         "
                   "    0x11111111\nversion:         0x01\nmsg:             \n "
                   " type:            PORT_STATS\n  flags:           "
                   "[  ]\n  body:            \n    port_no:         "
                   "0x00002222\n...\n");
}

TEST(decoder, ofmp_queuestats_v4_request) {
  testDecodeEncode("041200181111111100050000000000002222222233333333",
                   "---\ntype:            MULTIPART_REQUEST\nxid:         "
                   "    0x11111111\nversion:         0x04\nmsg:             \n "
                   " type:            QUEUE\n  flags:           [  ]\n "
                   " body:            \n    port_no:         0x22222222\n    "
                   "queue_id:        0x33333333\n...\n");
}

TEST(decoder, ofmp_queuestats_v1_request) {
  testDecodeEncode("0110001411111111000500002222000033333333",
                   "---\ntype:            MULTIPART_REQUEST\nxid:         "
                   "    0x11111111\nversion:         0x01\nmsg:             \n "
                   " type:            QUEUE\n  flags:           [  ]\n "
                   " body:            \n    port_no:         0x00002222\n    "
                   "queue_id:        0x33333333\n...\n");
}

TEST(decoder, ofp_metermod_v4) {
  testDecodeEncode(
      "041D00301111111100012222333333330001001044444444555555550000000000020010"
      "666666667777777788000000",
      "---\ntype:            METER_MOD\nxid:             "
      "0x11111111\nversion:         0x04\nmsg:             \n  command:        "
      " MODIFY\n  flags:           [ PKTPS, '0x00002220' ]\n  meter_id:        0x33333333\n  "
      "bands:           \n    - type:            DROP\n      rate:      "
      "      1145324612\n      burst_size:      1431655765\n    - type:        "
      "    DSCP_REMARK\n      rate:            1717986918\n      "
      "burst_size:      2004318071\n      prec_level:      136\n...\n");
}

TEST(decoder, ofp_mpaggregatestatsrequest_v1) {
  testDecodeEncode("0110003800000018 00020000 003820ff0000000000000000000000000000 00000000000000000000000000000000 0000000000000800ffff", "---\ntype:            MULTIPART_REQUEST\nxid:             0x00000018\nversion:         0x01\nmsg:             \n  type:            AGGREGATE\n  flags:           [  ]\n  body:            \n    table_id:        0x08\n    out_port:        ANY\n    out_group:       0x00000000\n    cookie:          0x0000000000000000\n    cookie_mask:     0x0000000000000000\n    match:           \n...\n");
}

TEST(decoder, ofmp_groupfeatures_reply) {
  testDecodeEncode("041300381111111100080000000000001111111122222222333333334444444455555555666666663777777708888888199999992AAAAAAA", "---\ntype:            MULTIPART_REPLY\nxid:             0x11111111\nversion:         0x04\nmsg:             \n  type:            GROUP_FEATURES\n  flags:           [  ]\n  body:            \n    types:           0x11111111\n    capabilities:    0x22222222\n    max_groups_all:  0x33333333\n    max_groups_sel:  0x44444444\n    max_groups_ind:  0x55555555\n    max_groups_ff:   0x66666666\n    actions_all:     [ OUTPUT, SET_VLAN_VID, SET_VLAN_PCP, SET_DL_DST, SET_NW_SRC, SET_NW_DST, SET_NW_ECN, SET_TP_SRC, SET_TP_DST, COPY_TTL_IN, SET_MPLS_LABEL, SET_MPLS_TC, DEC_MPLS_TTL, PUSH_VLAN, POP_VLAN, POP_MPLS, SET_QUEUE, GROUP, DEC_NW_TTL, SET_FIELD, PUSH_PBB, '0x30000000' ]\n    actions_sel:     [ SET_DL_SRC, SET_NW_TOS, COPY_TTL_OUT, SET_MPLS_TTL, PUSH_MPLS, SET_NW_TTL, POP_PBB ]\n    actions_ind:     [ OUTPUT, SET_DL_SRC, SET_DL_DST, SET_NW_TOS, SET_NW_ECN, COPY_TTL_OUT, COPY_TTL_IN, SET_MPLS_TTL, DEC_MPLS_TTL, PUSH_MPLS, POP_MPLS, SET_NW_TTL, DEC_NW_TTL, POP_PBB, '0x10000000' ]\n    actions_ff:      [ SET_VLAN_VID, SET_DL_SRC, SET_NW_SRC, SET_NW_TOS, SET_TP_SRC, COPY_TTL_OUT, SET_MPLS_LABEL, SET_MPLS_TTL, PUSH_VLAN, PUSH_MPLS, SET_QUEUE, SET_NW_TTL, SET_FIELD, POP_PBB, '0x20000000' ]\n...\n");
}
