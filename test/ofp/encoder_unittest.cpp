// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/encoder.h"

#include "ofp/unittest.h"

using namespace ofp::yaml;

static void testEncoderSuccess(const char *yaml, size_t size, const char *hex) {
  Encoder encoder{yaml};
  if (size) {
    EXPECT_EQ("", encoder.error());
  } else {
    EXPECT_NE("", encoder.error());
  }
  EXPECT_EQ(size, encoder.size());
  EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(encoder, flowMod) {
  const char *input = "{ type: FLOW_MOD, version: 4, xid: 1 }";

  Encoder encoder{input};
  const std::string &err = encoder.error();

  EXPECT_EQ(
      "YAML:1:3: error: missing required key 'msg'\n{ type: "
      "FLOW_MOD, version: 4, xid: 1 }\n  ^\n",
      err);
}

TEST(encoder, hellov1) {
  const char *input = "{ type: HELLO, version: 1, xid: 1 }";

  Encoder encoder{input};
  const std::string &err = encoder.error();

  EXPECT_EQ("", err);
  EXPECT_EQ(0x08, encoder.size());
  EXPECT_HEX("0100000800000001", encoder.data(), encoder.size());
}

TEST(encoder, hellov4err) {
  const char *input =
      "{ type: HELLO, version: 4, xid: 1 msg:{versions{1, 4}} }";

  Encoder encoder{input};

  EXPECT_EQ(
      "YAML:1:38: error: Found unexpected ':' while scanning a plain "
      "scalar\n{ type: HELLO, version: 4, xid: 1 msg:{versions{1, "
      "4}} }\n                                     ^\n",
      encoder.error());
}

TEST(encoder, hellov4) {
  const char *input =
      "{ 'type': 'HELLO', 'version': 4, 'xid': 99, "
      "'msg':{ 'versions': [1,4] } }";

  Encoder encoder{input};

  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x10, encoder.size());
  EXPECT_HEX("04000010000000630001000800000012", encoder.data(),
             encoder.size());
}

TEST(encoder, helloNoVersionsV4) {
  const char *input = R"""(
type: HELLO
version: 4
msg: {}
)""";

  testEncoderSuccess(input, 8, "0400000800000000");
}

TEST(encoder, helloExactVersionV4) {
  const char *input = R"""(
type: HELLO
version: 4
msg: { 'versions': [4] }
)""";

  testEncoderSuccess(input, 8, "0400000800000000");
}

TEST(encoder, helloOneOtherVersionV4) {
  const char *input = R"""(
type: HELLO
version: 4
msg: { 'versions': [1] }
)""";

  testEncoderSuccess(input, 16, "04000010000000000001000800000002");
}

TEST(encoder, error) {
  const char *input = R"""(
type: ERROR
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
type: ERROR
version: 1
xid: 98
msg:
  type: 1
  code: 1
  data: 'Not Hex'
)""";

  Encoder encoder{input};

  EXPECT_EQ(
      "YAML:8:9: error: Invalid hexadecimal text.\n  data: 'Not Hex'\n "
      "       ^~~~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0x00, encoder.size());
}

TEST(encoder, error3) {
  const char *input = R"""(
type: ERROR
version: 3
xid: 98
msg:
  type: BAD_ACTION
  code: OFPBAC_BAD_TAG
  data: 'FFFF1234567890'
)""";

  Encoder encoder{input};

  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x13, encoder.size());
  EXPECT_HEX("03010013000000620002000CFFFF1234567890", encoder.data(),
             encoder.size());
}

TEST(encoder, echorequest) {
  const char *input = R"""(
type: ECHO_REQUEST
version: 1
xid: 7
msg:
  data: 'aabbccddeeff'
)""";

  Encoder encoder{input};

  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x0E, encoder.size());
  EXPECT_HEX("0102000E00000007AABBCCDDEEFF", encoder.data(), encoder.size());
  EXPECT_EQ(ofp::OFP_DEFAULT_MESSAGE_FLAGS, encoder.flags());
}

TEST(encoder, echoreply) {
  const char *input = R"""(
type: ECHO_REPLY
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
type: EXPERIMENTER
version: 4
xid: 0x18
msg:
  experimenter: 0xDEADBEEF
  exp_type: 0xAABBCCDD
  data: 'ABCDEF0123456789'
)""";

  testEncoderSuccess(input, 0x18,
                     "0404001800000018DEADBEEFAABBCCDDABCDEF0123456789");
}

TEST(encoder, experimenterv1) {
  const char *input = R"""(
type: EXPERIMENTER
version: 1
xid: 0x1B
msg:
  experimenter: 0xDEADBEEF
  exp_type: 0xAABBCCDD
  data: 'ABCDEF0123456789'
)""";

  testEncoderSuccess(input, 0x14, "010400140000001BDEADBEEFABCDEF0123456789");
}

TEST(encoder, experimenterv2) {
  const char *input = R"""(
type: EXPERIMENTER
version: 2
xid: 0xFF
msg:
  experimenter: 0xDEADBEEF
  exp_type: 0xAABBCCDD
  data: 'ABCDEF0123456789'
)""";

  testEncoderSuccess(input, 0x18,
                     "02040018000000FFDEADBEEF00000000ABCDEF0123456789");
}

TEST(encoder, featuresrequest) {
  const char *input = R"""(
   type: FEATURES_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "04050008000000BF");
}

TEST(encoder, getasyncrequestv4) {
  const char *input = R"""(
   type: GET_ASYNC_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "041A0008000000BF");
}

TEST(encoder, getasyncrequestv1) {
  const char *input = R"""(
   type: GET_ASYNC_REQUEST
   version: 1
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0, "");
}

TEST(encoder, getconfigrequest) {
  const char *input = R"""(
   type: GET_CONFIG_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "04070008000000BF");
}

TEST(encoder, barrierrequestv4) {
  const char *input = R"""(
   type: BARRIER_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "04140008000000BF");
}

TEST(encoder, barrierrequestv1) {
  const char *input = R"""(
   type: BARRIER_REQUEST
   version: 1
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "01120008000000BF");
}

TEST(encoder, barrierreplyv4) {
  const char *input = R"""(
   type: BARRIER_REPLY
   version: 4
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "04150008000000BF");
}

TEST(encoder, barrierreplyv1) {
  const char *input = R"""(
   type: BARRIER_REPLY
   version: 1
   xid: 0xBF
   msg: {}
      )""";

  testEncoderSuccess(input, 0x08, "01130008000000BF");
}

TEST(encoder, featuresreplyv1) {
  const char *input = R"""(
    type: FEATURES_REPLY
    version: 1
    xid: 0xBF
    msg:
      datapath_id: '00:00:01:02:03:04:05:06'
      n_buffers: 256
      n_tables: 255
      auxiliary_id: 0
      capabilities: []
      actions: []
      ports:
    )""";

  testEncoderSuccess(
      input, 0x20,
      "01060020000000BF000001020304050600000100FF0000000000000000000000");
}

TEST(encoder, featuresreplyv1ports) {
  const char *input = R"""(
    type: FEATURES_REPLY
    version: 1
    xid: 0xBF
    msg:
      datapath_id: '00:00:01:02:03:04:05:06'
      n_buffers: 256
      n_tables: 255
      auxiliary_id: 0
      capabilities: []
      actions: []
      ports:
        - port_no: 0x1111
          hw_addr: 22:22:22:22:22:22
          name: Port 1
          config: [ '0x33333333' ]
          state:  [ '0x44444444' ]
          curr:   [ '0x55555555' ]
          advertised: [ '0x66666666' ]
          supported:  [ '0x77777777' ]
          peer:  [ '0x88888888' ]
          curr_speed: 0x99999999
          max_speed: 0xAAAAAAAA
          properties:
        - port_no: 0xBBBB
          hw_addr: CC:CC:CC:CC:CC:CC
          name: Port 2
          config: [ '0x33333333' ]
          state:  [ '0x44444444' ]
          curr:   [ '0x55555555' ]
          advertised: [ '0x66666666' ]
          supported:  [ '0x77777777' ]
          peer:  [ '0x88888888' ]
          curr_speed: 0x99999999
          max_speed: 0xAAAAAAAA
          properties:
    )""";

  testEncoderSuccess(input, 0x0080,
                     "01060080000000BF000001020304050600000100FF000000000000000"
                     "00000001111222222222222506F727420310000000000000000000033"
                     "3333334444444455550555666606667777077788880888BBBBCCCCCCC"
                     "CCCCC506F727420320000000000000000000033333333444444445555"
                     "0555666606667777077788880888");
}

TEST(encoder, featuresreplyv4) {
  const char *input = R"""(
    type: FEATURES_REPLY
    version: 4
    xid: 0xBF
    msg:
      datapath_id: '00:00:01:02:03:04:05:06'
      n_buffers: 256
      n_tables: 255
      auxiliary_id: 0
      capabilities: []
      actions: []
      ports:
    )""";

  testEncoderSuccess(
      input, 0x20,
      "04060020000000BF000001020304050600000100FF0000000000000000000000");
}

TEST(encoder, featuresreplyv4_no_ports) {
  const char *input = R"""(
    type: FEATURES_REPLY
    version: 4
    xid: 0xBF
    msg:
      datapath_id: '00:00:01:02:03:04:05:06'
      n_buffers: 256
      n_tables: 255
      auxiliary_id: 0
      capabilities: []
    )""";

  testEncoderSuccess(
      input, 0x20,
      "04060020000000BF000001020304050600000100FF0000000000000000000000");
}

TEST(encoder, ofmp_flowrequest_v4) {
  const char *input = R"""(
    type: MULTIPART_REQUEST
    version: 4
    xid: 0x11223344
    msg:
      type: FLOW_DESC
      flags: []
      body:
          table_id: 1
          out_port: 2
          out_group:  3
          cookie: 4
          cookie_mask: 5
          match:
             - field: IN_PORT
               value: 0x12345678
    )""";

  testEncoderSuccess(input, 0x040,
                     "041200401122334400010000000000000100000000000002000000030"
                     "0000000000000000000000400000000000000050001000C8000000412"
                     "34567800000000");
}

TEST(encoder, ofmp_flowrequest_v1) {
  const char *input = R"""(
    type: MULTIPART_REQUEST
    version: 1
    xid: 0x11223344
    msg:
      type: FLOW_DESC
      flags: []
      body:
          table_id: 0x11
          out_port: 0x22222222
          out_group:  0x33333333
          cookie: 0x4444444444444444
          cookie_mask: 0x5555555555555555
          match:
             - field: IN_PORT
               value: 0x12345678
    )""";

  testEncoderSuccess(input, 0x038,
                     "011000381122334400010000003820FE567"
                     "8000000000000000000000000000000000000000000"
                     "0000000000000000000000000011002222");
}

TEST(encoder, ofmp_aggregaterequest_v4) {
  const char *input = R"""(
    type: MULTIPART_REQUEST
    version: 4
    xid: 0x11223344
    msg:
      type: AGGREGATE_STATS
      flags: []
      body:
          table_id: 0x11
          out_port: 0x22222222
          out_group:  0x33333333
          cookie: 0x4444444444444444
          cookie_mask: 0x5555555555555555
          match:
             - field: IN_PORT
               value: 0x12345678
    )""";

  testEncoderSuccess(input, 0x040,
                     "041200401122334400020000000000001100000022222222333333330"
                     "0000000444444444444444455555555555555550001000C8000000412"
                     "34567800000000");
}

TEST(encoder, ofmp_aggregaterequest_v1) {
  const char *input = R"""(
    type: MULTIPART_REQUEST
    version: 1
    xid: 0x11223344
    msg:
      type: AGGREGATE_STATS
      flags: []
      body:
          table_id: 0x11
          out_port: 0x22222222
          out_group:  0x33333333
          cookie: 0x4444444444444444
          cookie_mask: 0x5555555555555555
          match:
             - field: IN_PORT
               value: 0x12345678
    )""";

  testEncoderSuccess(input, 0x038,
                     "011000381122334400020000003820FE567"
                     "8000000000000000000000000000000000000000000"
                     "0000000000000000000000000011002222");
}

TEST(encoder, ofmp_flowreply_v4) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 4
    xid: 0x11223344
    msg:
      type: FLOW_DESC
      flags: []
      body:
         - table_id: 1
           duration: 2.000000003
           priority: 4
           idle_timeout: 5
           hard_timeout: 6
           flags: [ 7 ]
           cookie: 8
           packet_count: 9
           byte_count: 10
           match:
             - field: IN_PORT
               value: 0x12345678
           instructions:
             - instruction: GOTO_TABLE
               table_id: 1
    )""";

  const char *hex =
      "0413005811223344000100000000000000480100000000020000000300"
      "0400050006000700000000000000000000000800000000000000090000"
      "00000000000A0001000C8000000412345678000000000001000801000000";
  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x58, encoder.size());
  EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(encoder, ofmp_flowreply2_v4) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 4
    xid: 0x11223344
    msg:
      type: FLOW_DESC
      flags: []
      body:
         - table_id: 1
           duration: 2.000000003
           priority: 4
           idle_timeout: 5
           hard_timeout: 6
           flags: [ 7 ]
           cookie: 8
           packet_count: 9
           byte_count: 10
           match:
             - field: IN_PORT
               value: 0x12345678
           instructions:
         - table_id: 0x11
           duration: 34.x33
           priority: 0x44
           idle_timeout: 0x55
           hard_timeout: 0x66
           flags: [ 0x77 ]
           cookie: 0x88
           packet_count: 0x9999999999999999
           byte_count:   0xAAAAAAAAAAAAAAAA
           match:
             - field: ETH_SRC
               value: 10:20:30:40:50:60
             - field: ETH_DST
               value: aa:bb:cc:dd:ee:ff
           instructions:
             - instruction: GOTO_TABLE
               table_id: 1
    )""";

  testEncoderSuccess(input, 0x0A0,
                     "041300A01122334400010000000000000040010000000002000000030"
                     "004000500060007000000000000000000000008000000000000000900"
                     "0000000000000A0001000C80000004123456780000000000501100000"
                     "000220000003300440055006600770000000000000000000000889999"
                     "999999999999AAAAAAAAAAAAAAAA00010018800008061020304050608"
                     "0000606AABBCCDDEEFF0001000801000000");
}

TEST(encoder, ofmp_flowreply3_v4) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 4
    xid: 0x11223344
    msg:
      type: FLOW_DESC
      flags: []
      body:
         - table_id: 1
           duration: 2.000000003
           priority: 4
           idle_timeout: 5
           hard_timeout: 6
           flags: [ 7 ]
           cookie: 8
           packet_count: 9
           byte_count: 10
           match:
           instructions:
             - instruction: APPLY_ACTIONS
               actions:
                  - action: OUTPUT
                    port_no: 1
                    max_len: 0xFFFF
                  - action: SET_NW_TTL
                    ttl: 64
    )""";

  const char *hex =
      "0413006811223344000100000000000000580100000000020000000300"
      "0400050006000700000000000000000000000800000000000000090000"
      "00000000000A000100040000000000040020000000000000001000000001FFFF00"
      "00000000000017000840000000";
  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x68, encoder.size());
  EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(encoder, ofmp_flowreply_v1) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 1
    xid: 0x11111111
    msg:
      type: FLOW_DESC
      flags: [ 0x2222 ]
      body:
         - table_id: 0x33
           duration: 1145324612.x55555555
           priority: 0x6666
           idle_timeout: 0x7777
           hard_timeout: 0x8888
           flags: [ 0x9999 ]
           cookie: 0xAAAAAAAAAAAAAAAA
           packet_count: 0xBBBBBBBBBBBBBBBB
           byte_count: 0xCCCCCCCCCCCCCCCC
           match:
             - field: IN_PORT
               value: 0xDDDDDDDD
           instructions:
             - instruction: APPLY_ACTIONS
               actions:
                  - action: OUTPUT
                    port_no: 0xEEEEEEEE
                    max_len: 0xFFFF
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x06C, encoder.size());
  EXPECT_HEX(
      "0111006C111111110001222200603300003820FEDDDD0000000000000000000000000000"
      "000000000000000000000000000000000000000044444444555555556666777788880000"
      "00000000AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCC00000008EEEEFFF"
      "F",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_flowreply2_v1) {
  // TODO(bfish): incorrect body for version 1?
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 1
    xid: 0x11223344
    msg:
      type: FLOW_DESC
      flags: []
      body:
         - table_id: 1
           duration: 2.000000003
           priority: 4
           idle_timeout: 5
           hard_timeout: 6
           flags: [ 7 ]
           cookie: 8
           packet_count: 9
           byte_count: 10
           match:
             - field: IN_PORT
               value: 0x12345678
           instructions:
             - instruction: APPLY_ACTIONS
               actions:
                  - action: OUTPUT
                    port_no: 0xEEEEEEEE
                    max_len: 0xFFFF
         - table_id: 0x11
           duration: 34.x33
           priority: 0x44
           idle_timeout: 0x55
           hard_timeout: 0x66
           flags: [ 0x77 ]
           cookie: 0x88
           packet_count: 0x9999999999999999
           byte_count:   0xAAAAAAAAAAAAAAAA
           match:
             - field: ETH_SRC
               value: 10:20:30:40:50:60
             - field: ETH_DST
               value: aa:bb:cc:dd:ee:ff
           instructions:
             - instruction: GOTO_TABLE
               table_id: 1
             - instruction: APPLY_ACTIONS
               actions:
                  - action: OUTPUT
                    port_no: 0xEEEEEEEE
                    max_len: 0xFFFF
    )""";

  testEncoderSuccess(
      input, 0x0CC,
      "011100CC112233440001000000600100003820FE56780000000000000000000000000000"
      "000000000000000000000000000000000000000000000002000000030004000500060000"
      "0000000000000000000000080000000000000009000000000000000A00000008EEEEFFFF"
      "00601100003820F30000102030405060AABBCCDDEEFF0000000000000000000000000000"
      "000000000000000000000022000000330044005500660000000000000000000000000088"
      "9999999999999999AAAAAAAAAAAAAAAA00000008EEEEFFFF");
}

TEST(encoder, ofmp_aggregatereply_v4) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 4
    xid: 0x11111111
    msg:
      type: AGGREGATE_STATS
      flags: [ 0x2222 ]
      body:
        packet_count: 0x3333333333333330
        byte_count: 0x4444444444444440
        flow_count: 0x55555550
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x28, encoder.size());
  EXPECT_HEX(
      "041300281111111100022222000000003333333333333330444444444444444055555550"
      "00000000",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_aggregatereply_v1) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 1
    xid: 0x11111111
    msg:
      type: AGGREGATE_STATS
      flags: [ 0x2222 ]
      body:
        packet_count: 0x3333333333333330
        byte_count: 0x4444444444444440
        flow_count: 0x55555550
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x24, encoder.size());
  EXPECT_HEX(
      "01110024111111110002222233333333333333304444444444444440555555500000000"
      "0",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_aggregatereply_v6) {
  const char *input = R"""(
    type: AGGREGATE_STATS_REPLY
    version: 6
    xid: 0x11111111
    msg:
      packet_count: 0x3333333333333330
      byte_count: 0x4444444444444440
      flow_count: 0x55555550
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x38, encoder.size());
  EXPECT_HEX(
      "061300381111111100020000000000000000002480020808333333333333333080020A08"
      "4444444444444440800206045555555000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_tablestats_v4) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 4
    xid: 0x11111111
    msg:
      type: TABLE_STATS
      flags: [ 0x2222 ]
      body:
        - table_id: 0x33
          active_count: 0x44444440
          lookup_count: 0x5555555555555550
          matched_count: 0x6666666666666660
        - table_id: 0x77
          active_count: 0x88888880
          lookup_count: 0x9999999999999990
          matched_count: 0xAAAAAAAAAAAAAAA0
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x40, encoder.size());
  EXPECT_HEX(
      "041300401111111100032222000000003300000044444440555555555555555066666666"
      "6666666077000000888888809999999999999990AAAAAAAAAAAAAAA0",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_tablestats_v1) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 1
    xid: 0x11111111
    msg:
      type: TABLE_STATS
      flags: [ 0x2222 ]
      body:
        - table_id: 0x33
          name: 'Table 1'
          wildcards: 0x44444440
          max_entries: 0x55555550
          active_count: 0x66666660
          lookup_count: 0x7777777777777770
          matched_count: 0x8888888888888880
        - table_id: 0x11
          name: 'Table 2'
          wildcards: 0x99999990
          max_entries: 0xAAAAAAA0
          active_count: 0xBBBBBBB0
          lookup_count: 0xCCCCCCCCCCCCCCC0
          matched_count: 0xDDDDDDDDDDDDDDD0
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x08C, encoder.size());
  EXPECT_HEX(
      "0111008C1111111100032222330000005461626C65203100000000000000000000000000"
      "000000000000000000000000444444405555555066666660777777777777777088888888"
      "88888880110000005461626C652032000000000000000000000000000000000000000000"
      "0000000099999990AAAAAAA0BBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portstats_v4) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 4
    xid: 0x11111111
    msg:
      type: PORT_STATS
      flags: [ 0x2222 ]
      body:
        - port_no: 0x33333330
          duration:   286331152.x22222220
          rx_packets: 0x4444444444444440
          tx_packets: 0x5555555555555550
          rx_bytes:   0x6666666666666660
          tx_bytes:   0x7777777777777770
          rx_dropped: 0x8888888888888880
          tx_dropped: 0x9999999999999990
          rx_errors:  0xAAAAAAAAAAAAAAA0
          tx_errors:  0xBBBBBBBBBBBBBBB0
          rx_frame_err: 0xCCCCCCCCCCCCCCC0
          rx_over_err:  0xDDDDDDDDDDDDDDD0
          rx_crc_err:   0xEEEEEEEEEEEEEEE0
          collisions:   0xFFFFFFFFFFFFFFF0
          properties:
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x80, encoder.size());
  EXPECT_HEX(
      "041300801111111100042222000000003333333000000000444444444444444055555555"
      "555555506666666666666660777777777777777088888888888888809999999999999990"
      "AAAAAAAAAAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEE"
      "EEEEEEE0FFFFFFFFFFFFFFF01111111022222220",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portstats_v3) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 3
    xid: 0x11111111
    msg:
      type: PORT_STATS
      flags: [ 0x2222 ]
      body:
        - port_no: 0x33333330
          duration:   286331152.x22222220
          rx_packets: 0x4444444444444440
          tx_packets: 0x5555555555555550
          rx_bytes:   0x6666666666666660
          tx_bytes:   0x7777777777777770
          rx_dropped: 0x8888888888888880
          tx_dropped: 0x9999999999999990
          rx_errors:  0xAAAAAAAAAAAAAAA0
          tx_errors:  0xBBBBBBBBBBBBBBB0
          rx_frame_err: 0xCCCCCCCCCCCCCCC0
          rx_over_err:  0xDDDDDDDDDDDDDDD0
          rx_crc_err:   0xEEEEEEEEEEEEEEE0
          collisions:   0xFFFFFFFFFFFFFFF0
          properties:
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x78, encoder.size());
  EXPECT_HEX(
      "031300781111111100042222000000003333333000000000444444444444444055555555"
      "555555506666666666666660777777777777777088888888888888809999999999999990"
      "AAAAAAAAAAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEE"
      "EEEEEEE0FFFFFFFFFFFFFFF0",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portstats_v2) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 2
    xid: 0x11111111
    msg:
      type: PORT_STATS
      flags: [ 0x2222 ]
      body:
        - port_no: 0x33333330
          duration:   286331152.x22222220
          rx_packets: 0x4444444444444440
          tx_packets: 0x5555555555555550
          rx_bytes:   0x6666666666666660
          tx_bytes:   0x7777777777777770
          rx_dropped: 0x8888888888888880
          tx_dropped: 0x9999999999999990
          rx_errors:  0xAAAAAAAAAAAAAAA0
          tx_errors:  0xBBBBBBBBBBBBBBB0
          rx_frame_err: 0xCCCCCCCCCCCCCCC0
          rx_over_err:  0xDDDDDDDDDDDDDDD0
          rx_crc_err:   0xEEEEEEEEEEEEEEE0
          collisions:   0xFFFFFFFFFFFFFFF0
          properties:
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x78, encoder.size());
  EXPECT_HEX(
      "021300781111111100042222000000003333333000000000444444444444444055555555"
      "555555506666666666666660777777777777777088888888888888809999999999999990"
      "AAAAAAAAAAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEE"
      "EEEEEEE0FFFFFFFFFFFFFFF0",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portstats_v1) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 1
    xid: 0x11111111
    msg:
      type: PORT_STATS
      flags: [ 0x2222 ]
      body:
        - port_no: 0x33333330
          duration:   286331152.x22222220
          rx_packets: 0x4444444444444440
          tx_packets: 0x5555555555555550
          rx_bytes:   0x6666666666666660
          tx_bytes:   0x7777777777777770
          rx_dropped: 0x8888888888888880
          tx_dropped: 0x9999999999999990
          rx_errors:  0xAAAAAAAAAAAAAAA0
          tx_errors:  0xBBBBBBBBBBBBBBB0
          rx_frame_err: 0xCCCCCCCCCCCCCCC0
          rx_over_err:  0xDDDDDDDDDDDDDDD0
          rx_crc_err:   0xEEEEEEEEEEEEEEE0
          collisions:   0xFFFFFFFFFFFFFFF0
          properties:
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x74, encoder.size());
  EXPECT_HEX(
      "011100741111111100042222333000000000000044444444444444405555555555555550"
      "6666666666666660777777777777777088888888888888809999999999999990AAAAAAAA"
      "AAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEEEEEEEEE0"
      "FFFFFFFFFFFFFFF0",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portstats_v5) {
  const char *input = R"""(
    type: PORT_STATS_REPLY
    version: 5
    xid: 0x11111111
    flags: [ 0x2222 ]
    msg:
      - port_no: 0x33333330
        duration:   286331152.x22222220
        rx_packets: 0x4444444444444440
        tx_packets: 0x5555555555555550
        rx_bytes:   0x6666666666666660
        tx_bytes:   0x7777777777777770
        rx_dropped: 0x8888888888888880
        tx_dropped: 0x9999999999999990
        rx_errors:  0xAAAAAAAAAAAAAAA0
        tx_errors:  0xBBBBBBBBBBBBBBB0
        rx_frame_err: 0xCCCCCCCCCCCCCCC0
        rx_over_err:  0xDDDDDDDDDDDDDDD0
        rx_crc_err:   0xEEEEEEEEEEEEEEE0
        collisions:   0xFFFFFFFFFFFFFFF0
        optical:
          flags: 0x11111110
          tx_freq_lmda: 0x22222220
          tx_offset: 0x33333330
          tx_grid_span: 0x44444440
          rx_freq_lmda: 0x55555550
          rx_offset: 0x66666660
          rx_grid_span: 0x77777770
          tx_pwr: 0x8880
          rx_pwr: 0x9990
          bias_current: 0xAAA0
          temperature: 0xBBB0
        properties:
          - property: EXPERIMENTER
            experimenter: 0x77777701
            exp_type: 0x88888801
            data: 99999901
          - property: 0x1234
            data: 0123456789ABCDEF
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(216, encoder.size());
  EXPECT_HEX(
      "051300D811111111000422220000000000C8000033333330111111102222222044444444"
      "444444405555555555555550666666666666666077777777777777708888888888888880"
      "9999999999999990AAAAAAAAAAAAAAA0BBBBBBBBBBBBBBB00000002800000000CCCCCCCC"
      "CCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEEEEEEEEE0FFFFFFFFFFFFFFF00001002C00000000"
      "1111111022222220333333304444444055555550666666607777777088809990AAA0BBB0"
      "00000000FFFF00107777770188888801999999011234000C0123456789ABCDEF0000000"
      "0",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_queuestats_v4) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 4
    xid: 0x11111111
    msg:
      type: QUEUE_STATS
      flags: [ 0x2222 ]
      body:
        - port_no: 0x33333330
          queue_id: 0x44444440
          tx_bytes:   0x5555555555555550
          tx_packets: 0x6666666666666660
          tx_errors:  0x7777777777777770
          duration:   286331152.x22222220
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x38, encoder.size());
  EXPECT_HEX(
      "041300381111111100052222000000003333333044444440555555555555555066666666"
      "6666666077777777777777701111111022222220",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_queuestats_v1) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 1
    xid: 0x11111111
    msg:
      type: QUEUE_STATS
      flags: [ 0x2222 ]
      body:
        - port_no: 0x33333330
          queue_id: 0x44444440
          tx_bytes:   0x5555555555555550
          tx_packets: 0x6666666666666660
          tx_errors:  0x7777777777777770
          duration:   286331152.x22222220
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x2C, encoder.size());
  EXPECT_HEX(
      "0111002C1111111100052222333000004444444055555555555555506666666666666660"
      "7777777777777770",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4) {
  const char *input = R"""(
      type:            FLOW_MOD
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
        flags:           []
        match:           
          - field:           IN_PORT
            value:           13
          - field:           ETH_TYPE
            value:           2048
          - field:           IPV4_DST
            value:           192.168.1.1
        instructions:
          - instruction:    APPLY_ACTIONS
            actions:
               - action: SET_FIELD
                 field: IPV4_DST
                 value: 192.168.2.1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x68, encoder.size());
  EXPECT_HEX(
      "040E0068000000010000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000001001A800000040000000D80000A0208"
      "0080001804C0A8010100000000000000040018000000000019001080001804C0A"
      "8020100000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4_2) {
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
          - field:           IN_PORT
            value:           0xCCCCCCCC
          - field:           ETH_TYPE
            value:           2048
          - field:           IPV4_DST
            value:           192.168.1.1
        instructions:
          - instruction:    APPLY_ACTIONS
            actions:
               - action: SET_FIELD
                 field: IPV4_DST
                 value: 192.168.2.1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x68, encoder.size());
  EXPECT_HEX(
      "040E0068000000011111111111111111222222222222222233445555666677778"
      "888888899999999AAAAAAAABBBB00000001001A80000004CCCCCCCC80000A0208"
      "0080001804C0A8010100000000000000040018000000000019001080001804C0A"
      "8020100000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4_ipv6) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        cookie:          0x1111111111111110
        cookie_mask:     0x2222222222222220
        table_id:        0x30
        command:         0x40
        idle_timeout:    0x5550
        hard_timeout:    0x6660
        priority:        0x7770
        buffer_id:       0x88888880
        out_port:        0x99999990
        out_group:       0xAAAAAAA0
        flags:           [ '0xBBB1' ]
        match:           
          - field:           IPV6_SRC
            value:           ::ffff:C0A8:0001
          - field:           IPV6_DST
            value:           2000::1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x068, encoder.size());
  EXPECT_HEX(
      "040E00680000000111111111111111102222222222222220304055506660777088888880"
      "99999990AAAAAAA0BBB100000001003280000A0286DD8000341000000000000000000000"
      "FFFFC0A800018000361020000000000000000000000000000001000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4_fail) {
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
          - field:           IN_PORT
            value:           13
          - field:           TCP_DST
            value:           80
        instructions:
          - instruction:    APPLY_ACTIONS
            actions:
               - action: SET_FIELD
                 field: IPV4_DST
                 value: 192.168.2.1
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:18:11: error: Invalid match: Missing prerequisites\n          - "
      "field:           IN_PORT\n          ^\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4_outOfOrder) {
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
            value:           80
          - field:           ETH_TYPE
            value:           0x0800
        instructions:
          - instruction:    APPLY_ACTIONS
            actions:
               - action: SET_FIELD
                 field: IPV4_DST
                 value: 192.168.2.1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(96, encoder.size());
  EXPECT_HEX(
      "040E00600000000111111111111111112222222222222222334455556666777788888888"
      "99999999AAAAAAAABBBB00000001001580000A020800800014010680001C020050000000"
      "00040018000000000019001080001804C0A8020100000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4_experimenter) {
  const char *input = R"""(
      type:            FLOW_MOD
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
        flags:           []
        match:           
          - field:           X_LLDP_CHASSIS_ID
            value:           unknown 0102030405
          - field:           X_LLDP_TTL
            value:           0x01234
          - field:           X_EXPERIMENTER_01
            value:           0x01
        instructions:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(144, encoder.size());
  EXPECT_HEX(
      "040E00900000000100000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000010060FFFF024400FFFFFF050102030405000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000FFFF060600FFFFFF1234FFFF060600FFFFFE000"
      "1",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4_experimenter_mask) {
  const char *input = R"""(
      type:            FLOW_MOD
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
        flags:           []
        match:           
          - field:           X_LLDP_CHASSIS_ID
            value:           unknown 0102030405
          - field:           X_LLDP_TTL
            value:           0x1234
          - field:           X_EXPERIMENTER_01
            value:           0x01
            mask:            0xFF
        instructions:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(152, encoder.size());
  EXPECT_HEX(
      "040E00980000000100000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000010062FFFF024400FFFFFF050102030405000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000FFFF060600FFFFFF1234FFFF070800FFFFFE0001"
      "00FF000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4_experimenter_2) {
  const char *input = R"""(
      type:            FLOW_MOD
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
        flags:           []
        match:           
          - field:           0xFFFFFE01
            value:           01
        instructions:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(64, encoder.size());
  EXPECT_HEX(
      "040E00400000000100000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000010009FFFFFE010100000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv4_minimal) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        table_id:        0
        command:         ADD
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(56, encoder.size());
  EXPECT_HEX(
      "040E003800000001000000000000000000000000000000000000000000000000FFFFFFFF"
      "FFFFFFFFFFFFFFFF000000000001000400000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv1) {
  const char *input = R"""(
      type:            FLOW_MOD
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
        flags:           [ '0xBBBB' ]
        match:           
          - field:           IN_PORT
            value:           0xCCCCCCCC
          - field:           IPV4_DST
            value:           192.168.1.1
        instructions:
          - instruction:    APPLY_ACTIONS
            actions:
               - action: SET_FIELD
                 field: IPV4_DST
                 value: 192.168.2.1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x050, encoder.size());
  EXPECT_HEX(
      "010E005000000001003020EECCCC0000000000000000000000000000000008000"
      "000000000000000C0A80101000000001111111111111111004455556666777788"
      "8888889999BBBB00070008C0A80201",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv1_2) {
  const char *input = R"""(
      type:            FLOW_MOD
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
        flags:           [ '0xBBBB' ]
        match:           
          - field:           IN_PORT
            value:           0xCCCCCCCC
          - field:           IPV4_DST
            value:           192.168.1.1
          - field:           ICMPV4_TYPE
            value:           0xDD
        instructions:
          - instruction:    APPLY_ACTIONS
            actions:
               - action: SET_FIELD
                 field: ICMPV4_CODE
                 value: 0xEE
               - action: COPY_TTL_OUT
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x058, encoder.size());
  EXPECT_HEX(
      "010E0058000000010030208ECCCC0000000000000000000000000000000008000"
      "001000000000000C0A8010100DD00001111111111111111004455556666777788"
      "8888889999BBBB000A0008EE000000000B000800000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv1_modvlan) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         1
      xid:             1
      msg:             
        cookie:          0x1111111111111111
        cookie_mask:     0x2222222222222222
        table_id:        0x33
        command:         0x44
        idle_timeout:    0x5152
        hard_timeout:    0x6162
        priority:        0x7172
        buffer_id:       0x81828384
        out_port:        0x91929394
        out_group:       0xA1A2A3A4
        flags:           [ '0xB1B2' ]
        match:           
          - field:           IN_PORT
            value:           0xC1C2C3C4
        instructions:
          - instruction:    APPLY_ACTIONS
            actions:
               - action: SET_FIELD
                 field: VLAN_VID
                 value: 0x822c
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x050, encoder.size());
  EXPECT_HEX(
      "010E005000000001003820FEC3C400000000000000000000000000000000000000000000"
      "00000000000000000000000011111111111111110044515261627172818283849394B1B2"
      "00010008822C0000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmodv6) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         6
      xid:             1
      msg:             
        cookie:          0x1111111111111110
        cookie_mask:     0x2222222222222220
        table_id:        0x30
        command:         0x40
        idle_timeout:    0x5550
        hard_timeout:    0x6660
        priority:        0x7770
        buffer_id:       0x88888880
        out_port:        0x99999990
        out_group:       0xAAAAAAA0
        flags:           [ '0xBBB1' ]
        importance:      0xCCC1
        match:           
          - field:           IPV6_SRC
            value:           ::ffff:C0A8:0001
          - field:           IPV6_DST
            value:           2000::1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x068, encoder.size());
  EXPECT_HEX(
      "060E00680000000111111111111111102222222222222220304055506660777088888880"
      "99999990AAAAAAA0BBB1CCC10001003280000A0286DD8000341000000000000000000000"
      "FFFFC0A800018000361020000000000000000000000000000001000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, packetinv1) {
  const char *input = R"""(
      type:            PACKET_IN
      version:         1
      xid:             2
      msg:             
        buffer_id:       0x33333333
        total_len:       0x4444
        in_port:         0x55555555
        in_phy_port:     0x66666666
        metadata:        0x7777777777777777
        reason:          APPLY_ACTION
        table_id:        0x88
        cookie:          0x9999999999999999
        match:
        data:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x03C, encoder.size());
  EXPECT_HEX(
      "010A003C0000000233333333444455550100FFFFFFFFFFFF00000000000108060"
      "0010800060400010000000000010A0000010000000000000A000002",
      encoder.data(), encoder.size());
}

TEST(encoder, packetinv2) {
  const char *input = R"""(
      type:            PACKET_IN
      version:         2
      xid:             2
      msg:             
        buffer_id:       0x33333333
        total_len:       0x4444
        in_port:         0x55555555
        in_phy_port:     0x66666666
        metadata:        0x7777777777777777
        reason:          APPLY_ACTION
        table_id:        0x88
        cookie:          0x9999999999999999
        match:
        data:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x042, encoder.size());
  EXPECT_HEX(
      "020A00420000000233333333555555556666666644440188FFFFFFFFFFFF000000000001"
      "080600010800060400010000000000010A0000010000000000000A000002",
      encoder.data(), encoder.size());
}

TEST(encoder, packetinv4) {
  const char *input = R"""(
      type:            PACKET_IN
      version:         4
      xid:             1
      msg:             
        buffer_id:       0x33333333
        total_len:       0x4444
        in_port:         0x55555555
        in_phy_port:     0x66666666
        metadata:        0x7777777777777777
        reason:          APPLY_ACTION
        table_id:        0x88
        cookie:          0x9999999999999999
        match:
        data:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x064, encoder.size());
  EXPECT_HEX(
      "040A0064000000013333333344440188999999999999999900010020800000045"
      "555555580000204666666668000040877777777777777770000FFFFFFFFFFFF00"
      "0000000001080600010800060400010000000000010A0000010000000000000A0"
      "00002",
      encoder.data(), encoder.size());
}

TEST(encoder, packetinv4_no_phys_port) {
  const char *input = R"""(
      type:            PACKET_IN
      version:         4
      xid:             1
      msg:             
        buffer_id:       0x33333331
        total_len:       0x4441
        in_port:         0x55555551
        metadata:        0x7777777777777771
        reason:          APPLY_ACTION
        table_id:        0x88
        cookie:          0x9999999999999991
        match:
        data:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x05c, encoder.size());
  EXPECT_HEX(
      "040A005C0000000133333331444101889999999999999991000100188000000455555551"
      "8000040877777777777777710000FFFFFFFFFFFF00000000000108060001080006040001"
      "0000000000010A0000010000000000000A000002",
      encoder.data(), encoder.size());
}

TEST(encoder, packetinv4_pkt) {
  const char *input = R"""(
      type:            PACKET_IN
      version:         4
      xid:             1
      msg:             
        buffer_id:       0x33333330
        total_len:       0x4440
        in_port:         0x55555550
        in_phy_port:     0x66666660
        metadata:        0x7777777777777770
        reason:          APPLY_ACTION
        table_id:        0x80
        cookie:          0x9999999999999990
        match:
        data:            ''
        _pkt:
          - field: ETH_DST
            value: ff:ff:ff:ff:ff:ff
          - field: ETH_SRC
            value: 00:00:00:00:00:01
          - field: ETH_TYPE
            value: 0x0806
          - field: ARP_OP
            value: 1
          - field: ARP_SHA
            value: 00:00:00:00:00:01
          - field: ARP_SPA
            value: 10.0.0.1
          - field: ARP_TPA
            value: 10.0.0.2
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x076, encoder.size());
  EXPECT_HEX(
      "040A00760000000133333330444001809999999999999990000100208000000455555550"
      "80000204666666608000040877777777777777700000FFFFFFFFFFFF0000000000010806"
      "00010800060400010000000000010A0000010000000000000A0000020000000000000000"
      "00000000000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, packetinv4_pkt_no_total_len) {
  const char *input = R"""(
      type:            PACKET_IN
      version:         4
      xid:             1
      msg:             
        buffer_id:       0x33333330
        in_port:         0x55555550
        in_phy_port:     0x66666660
        metadata:        0x7777777777777770
        reason:          APPLY_ACTION
        table_id:        0x80
        cookie:          0x9999999999999990
        match:
        data:            ''
        _pkt:
          - field: ETH_DST
            value: ff:ff:ff:ff:ff:ff
          - field: ETH_SRC
            value: 00:00:00:00:00:01
          - field: ETH_TYPE
            value: 0x0806
          - field: ARP_OP
            value: 1
          - field: ARP_SHA
            value: 00:00:00:00:00:01
          - field: ARP_SPA
            value: 10.0.0.1
          - field: ARP_TPA
            value: 10.0.0.2
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x076, encoder.size());
  EXPECT_HEX(
      "040A00760000000133333330003C01809999999999999990000100208000000455555550"
      "80000204666666608000040877777777777777700000FFFFFFFFFFFF0000000000010806"
      "00010800060400010000000000010A0000010000000000000A0000020000000000000000"
      "00000000000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, packetoutv1) {
  const char *input = R"""(
      type:            PACKET_OUT
      version:         1
      xid:             1
      msg:             
        buffer_id:       0x33333333
        in_port:         0x44444444
        actions:
          - action: OUTPUT
            port_no: 5
            max_len: 20
          - action: SET_FIELD
            field:  IPV4_DST
            value:  192.168.1.1
        data:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x04A, encoder.size());
  EXPECT_HEX(
      "010D004A000000013333333344440010000000080005001400070008C0A80101F"
      "FFFFFFFFFFF000000000001080600010800060400010000000000010A00000100"
      "00000000000A000002",
      encoder.data(), encoder.size());
}

TEST(encoder, packetoutv4) {
  const char *input = R"""(
      type:            PACKET_OUT
      version:         4
      xid:             1
      msg:             
        buffer_id:       0x33333333
        in_port:         0x44444444
        actions:
          - action: OUTPUT
            port_no: 5
            max_len: 20
          - action: SET_FIELD
            field:  IPV4_DST
            value:  192.168.1.1
        data:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x062, encoder.size());
  EXPECT_HEX(
      "040D0062000000013333333344444444002000000000000000000010000000050"
      "0140000000000000019001080001804C0A8010100000000FFFFFFFFFFFF000000"
      "000001080600010800060400010000000000010A0000010000000000000A00000"
      "2",
      encoder.data(), encoder.size());
}

TEST(encoder, packetoutv6) {
  const char *input = R"""(
      type:            PACKET_OUT
      version:         6
      xid:             1
      msg:             
        buffer_id:       0x33333333
        in_port:         0x44444444
        match:
          - field: IPV4_SRC
            value: 192.168.1.2
        actions:
          - action: OUTPUT
            port_no: 5
            max_len: 20
          - action: SET_FIELD
            field:  IPV4_DST
            value:  192.168.1.1
        data:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x07A, encoder.size());
  EXPECT_HEX(
      "060D007A0000000133333333002000000001001A80000A02080080001604C0A801028000"
      "000444444444000000000000000000100000000500140000000000000019001080001804"
      "C0A8010100000000FFFFFFFFFFFF00000000000108060001080006040001000000000001"
      "0A0000010000000000000A000002",
      encoder.data(), encoder.size());
}

TEST(encoder, packetoutv6_minimal) {
  const char *input = R"""(
      type:            PACKET_OUT
      version:         6
      xid:             1
      msg:             
        in_port:         0x44444441
        actions:
          - action: OUTPUT
            port_no: 5
        data:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(90, encoder.size());
  EXPECT_HEX(
      "060D005A00000001FFFFFFFF001000000001000C80000004444444410000000000000010"
      "00000005FFE5000000000000FFFFFFFFFFFF000000000001080600010800060400010000"
      "000000010A0000010000000000000A000002",
      encoder.data(), encoder.size());
}

TEST(encoder, setconfigv4) {
  const char *input = R"""(
      version: 4
      type: SET_CONFIG
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        flags: [ '0xAAAA' ]
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
      type: PORT_STATUS
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        reason: 0x22
        port_no: 0x33333333
        hw_addr: 'aabbccddeeff'
        name: 'Port 1'
        config: [ 0x44444444 ]
        state: [ 0x55555555 ]
        curr: [ '0x66666666' ]
        advertised: [ '0x77777777' ]
        supported: [ '0x88888888' ]
        peer: [ '0x99999999' ]
        curr_speed: 0xAAAAAAAA
        max_speed: 0xBBBBBBBB
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x50, encoder.size());
  EXPECT_HEX(
      "040C00501111111122000000000000003333333300000000AABBCCDDEEFF00005"
      "06F72742031000000000000000000004444444455555555666666667777777788"
      "88888899999999AAAAAAAABBBBBBBB",
      encoder.data(), encoder.size());
}

TEST(encoder, portstatusv1) {
  const char *input = R"""(
      version: 1
      type: PORT_STATUS
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        reason: 0x22
        port_no: 0x33333333
        hw_addr: 'aabbccddeeff'
        name: 'Port 1'
        config: [ 0x44444444 ]
        state: [ 0x55555555 ]
        curr: [ '0x66666666' ]
        advertised: [ '0x77777777' ]
        supported: [ '0x88888888' ]
        peer: [ '0x99999999' ]
        curr_speed: 0xAAAAAAAA
        max_speed: 0xBBBBBBBB
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x40, encoder.size());
  EXPECT_HEX(
      "010C00401111111122000000000000003333AABBCCDDEEFF506F7274203100000"
      "000000000000000444444445555555566660666777707778888088899990999",
      encoder.data(), encoder.size());
}

TEST(encoder, groupmodv4) {
  const char *input = R"""(
      version: 4
      type: GROUP_MOD
      datapath_id: 00:00:00:00:00:00:00:01
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
              - action: OUTPUT
                port_no: 5
                max_len: 20
              - action: SET_FIELD
                field:  IPV4_DST
                value:  192.168.1.1
          - weight: 0x8888
            watch_port: 0x99999999
            watch_group: 0xAAAAAAAA
            actions:
              - action: SET_FIELD
                field: ICMPV4_CODE
                value: 0xEE
              - action: COPY_TTL_OUT
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x68, encoder.size());
  EXPECT_HEX(
      "040F0068111111112222330044444444003055556666666677777777000000000"
      "00000100000000500140000000000000019001080001804C0A801010000000000"
      "28888899999999AAAAAAAA000000000019001080002801EE00000000000000000"
      "B000800000000",
      encoder.data(), encoder.size());
}

TEST(encoder, groupmodv2) {
  const char *input = R"""(
      version: 2
      type: GROUP_MOD
      datapath_id: 00:00:00:00:00:00:00:01
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
              - action: OUTPUT
                port_no: 5
                max_len: 20
              - action: SET_FIELD
                field:  IPV4_DST
                value:  192.168.1.1
          - weight: 0x8888
            watch_port: 0x99999999
            watch_group: 0xAAAAAAAA
            actions:
              - action: SET_FIELD
                field: ICMPV4_CODE
                value: 0xEE
              - action: COPY_TTL_OUT
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x68, encoder.size());
  EXPECT_HEX(
      "020F0068111111112222330044444444003055556666666677777777000000000"
      "00000100000000500140000000000000019001080001804C0A801010000000000"
      "28888899999999AAAAAAAA000000000019001080002801EE00000000000000000"
      "B000800000000",
      encoder.data(), encoder.size());
}

TEST(encoder, portmodv5) {
  const char *input = R"""(
      version: 5
      type: PORT_MOD
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        port_no: 0x22222222
        hw_addr: '333333333333'
        config: [ 0x44444444 ]
        mask: [ 0x55555555 ]
        advertise: [ 0x66666666 ]
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x28, encoder.size());
  EXPECT_HEX(
      "051000281111111122222222000000003333333333330000444444445555555500000008"
      "66666666",
      encoder.data(), encoder.size());
}

TEST(encoder, portmodv4) {
  const char *input = R"""(
      version: 4
      type: PORT_MOD
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        port_no: 0x22222222
        hw_addr: '333333333333'
        config: [ 0x44444444 ]
        mask: [ 0x55555555 ]
        advertise: [ 0x66666666 ]
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x28, encoder.size());
  EXPECT_HEX(
      "04100028111111112222222200000000333333333333000044444444555555556"
      "666666600000000",
      encoder.data(), encoder.size());
}

TEST(encoder, portmodv5_experimenter) {
  const char *input = R"""(
      version: 5
      type: PORT_MOD
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        port_no: 0x22222222
        hw_addr: '333333333333'
        config: [ 0x44444444 ]
        mask: [ 0x55555555 ]
        advertise: [ 0x66666666 ]
        properties:
          - property: EXPERIMENTER
            experimenter: 0x77777700
            exp_type: 0x88888800
            data: 99999900
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x38, encoder.size());
  EXPECT_HEX(
      "051000381111111122222222000000003333333333330000444444445555555500000008"
      "66666666FFFF0010777777008888880099999900",
      encoder.data(), encoder.size());
}

TEST(encoder, portmodv1) {
  const char *input = R"""(
      version: 1
      type: PORT_MOD
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        port_no: 0x2222
        hw_addr: '333333333333'
        config: [ 0x44444444 ]
        mask: [ 0x55555555 ]
        advertise: [ 0x66666666 ]
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x20, encoder.size());
  EXPECT_HEX("010F002011111111222233333333333344444444555555556666066600000000",
             encoder.data(), encoder.size());
}

TEST(encoder, tablemodv4) {
  const char *input = R"""(
      version: 4
      type: TABLE_MOD
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        table_id: 0x22
        config: [0x33333333]
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
      type: TABLE_MOD
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        table_id: 0x22
        config: [0x33333333]
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
      type: ROLE_REQUEST
      datapath_id: 00:00:00:00:00:00:00:01
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
      type: ROLE_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
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
      type: GET_ASYNC_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        packet_in_master: [0x22222222]
        packet_in_slave: [0x33333333]
        port_status_master: [0x44444444]
        port_status_slave: [0x55555555]
        flow_removed_master: [0x66666666]
        flow_removed_slave: [0x77777777]
        properties: []
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x20, encoder.size());
  EXPECT_HEX("041B002011111111222222223333333344444444555555556666666677777777",
             encoder.data(), encoder.size());
}

TEST(encoder, getasyncreplyv5) {
  const char *input = R"""(
      version: 5
      type: GET_ASYNC_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        packet_in_master: [0x22222222]
        packet_in_slave: [0x33333333]
        port_status_master: [0x44444444]
        port_status_slave: [0x55555555]
        flow_removed_master: [0x66666666]
        flow_removed_slave: [0x77777777]
        properties: []
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(56, encoder.size());
  EXPECT_HEX(
      "051B00381111111100000008333333330001000822222222000200085555555500030008"
      "4444444400040008777777770005000866666666",
      encoder.data(), encoder.size());
}

TEST(encoder, getasyncreplyv5_2) {
  const char *input = R"""(
      version: 5
      type: GET_ASYNC_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        packet_in_master: [0x22222222]
        packet_in_slave: [0x33333333]
        port_status_master: [0x44444444]
        port_status_slave: [0x55555555]
        flow_removed_master: [0x66666666]
        flow_removed_slave: [0x77777777]
        properties:
          - property: EXPERIMENTER_SLAVE
            experimenter: 0x88888880
            exp_type: 0x99999990
            data: ABCD
          - property: EXPERIMENTER_MASTER
            experimenter: 0xAAAAAAA0
            exp_type: 0xBBBBBBB0
            data: CDEF
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(88, encoder.size());
  EXPECT_HEX(
      "051B00581111111100000008333333330001000822222222000200085555555500030008"
      "4444444400040008777777770005000866666666FFFE000E8888888099999990ABCD0000"
      "FFFF000EAAAAAAA0BBBBBBB0CDEF0000",
      encoder.data(), encoder.size());
}

TEST(encoder, queuegetconfigrequestv4) {
  const char *input = R"""(
      version: 4
      type: QUEUE_GET_CONFIG_REQUEST
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        port_no: 0x22222222
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x10, encoder.size());
  EXPECT_HEX("04160010111111112222222200000000", encoder.data(),
             encoder.size());
}

TEST(encoder, queuegetconfigreplyv4) {
  const char *input = R"""(
    version: 4
    type: QUEUE_GET_CONFIG_REPLY
    datapath_id: 00:00:00:00:00:00:00:01
    xid: 0x11111111
    msg:
      port_no: 0x22222222
      queues:
        - queue_id: 0x33333333
          port_no: 0x44444444
          min_rate: 0x5555
          max_rate: 0x6666
        - queue_id: 0x77777777
          port_no: 0x88888888
          min_rate: 0x9999
          max_rate: 0xAAAA
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x70, encoder.size());
  EXPECT_HEX(
      "0417007011111111 2222222200000000"
      "333333334444444400300000000000000001001000000000555500000000000000020010"
      "000000006666000000000000"
      "777777778888888800300000000000000001001000000000999900000000000000020010"
      "00000000AAAA000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, queuegetconfigreplyv4_experimenter) {
  const char *input = R"""(
    version: 4
    type: QUEUE_GET_CONFIG_REPLY
    datapath_id: 00:00:00:00:00:00:00:01
    xid: 0x11111111
    msg:
      port_no: 0x22222222
      queues:
        - queue_id: 0x33333333
          port_no: 0x44444444
          min_rate: 0x5555
          max_rate: 0x6666
          properties:
            - experimenter: 0xEEEEEEEE
              value: 000102030405
            - experimenter: 0xFFFFFFFF
              value: abcdef
        - queue_id: 0x77777777
          port_no: 0x88888888
          min_rate: 0x9999
          max_rate: 0xAAAA
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(153, encoder.size());
  EXPECT_HEX(
      "041700991111111122222222000000003333333344444444005900000000000000010010"
      "00000000555500000000000000020010000000006666000000000000FFFF001600000000"
      "EEEEEEEE00000000000102030405FFFF001300000000FFFFFFFF00000000ABCDEF777777"
      "778888888800300000000000000001001000000000999900000000000000020010000000"
      "00AAAA000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, queuegetconfigreplyv5_experimenter) {
  const char *input = R"""(
    version: 5
    type: QUEUE_GET_CONFIG_REPLY
    datapath_id: 00:00:00:00:00:00:00:01
    xid: 0x11111111
    msg:
      port_no: 0x22222222
      queues:
        - queue_id: 0x33333333
          port_no: 0x44444444
          min_rate: 0x5555
          max_rate: 0x6666
          properties:
            - experimenter: 0xEEEEEEEE
              value: 000102030405
            - experimenter: 0xFFFFFFFF
              value: abcdef
        - queue_id: 0x77777777
          port_no: 0x88888888
          min_rate: 0x9999
          max_rate: 0xAAAA
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0xA0, encoder.size());
  EXPECT_HEX(
      "051700A01111111122222222000000003333333344444444006000000000000000010010"
      "00000000555500000000000000020010000000006666000000000000FFFF001600000000"
      "EEEEEEEE000000000001020304050000FFFF001300000000FFFFFFFF00000000ABCDEF00"
      "000000007777777788888888003000000000000000010010000000009999000000000000"
      "0002001000000000AAAA000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, queuegetconfigreplyv1) {
  const char *input = R"""(
    version: 1
    type: QUEUE_GET_CONFIG_REPLY
    datapath_id: 00:00:00:00:00:00:00:01
    xid: 0x11111110
    msg:
      port_no: 0x22222221
      queues:
        - queue_id: 0x33333331
          port_no: 0x44444441
          min_rate: 0x5551
          max_rate: 0x6661
        - queue_id: 0x77777771
          port_no: 0x88888881
          min_rate: 0x9991
          max_rate: 0xAAA1
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(96, encoder.size());
  EXPECT_HEX(
      "011500601111111022210000000000003333333100280000000100100000000055510000"
      "000000000002001000000000666100000000000077777771002800000001001000000000"
      "99910000000000000002001000000000AAA1000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, queuegetconfigreplyv2) {
  const char *input = R"""(
    version: 2
    type: QUEUE_GET_CONFIG_REPLY
    datapath_id: 00:00:00:00:00:00:00:01
    xid: 0x11111110
    msg:
      port_no: 0x22222221
      queues:
        - queue_id: 0x33333331
          port_no: 0x44444441
          min_rate: 0x5551
          max_rate: 0x6661
        - queue_id: 0x77777771
          port_no: 0x88888881
          min_rate: 0x9991
          max_rate: 0xAAA1
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(96, encoder.size());
  EXPECT_HEX(
      "021700601111111022222221000000003333333100280000000100100000000055510000"
      "000000000002001000000000666100000000000077777771002800000001001000000000"
      "99910000000000000002001000000000AAA1000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, getconfigreplyv4) {
  const char *input = R"""(
      version: 4
      type: GET_CONFIG_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        flags: [ '0xAAAA' ]
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
      type: SET_ASYNC
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        packet_in_master: [0x22222222]
        packet_in_slave: [0x33333333]
        port_status_master: [0x44444444]
        port_status_slave: [0x55555555]
        flow_removed_master: [0x66666666]
        flow_removed_slave: [0x77777777]
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x20, encoder.size());
  EXPECT_HEX("041C002011111111222222223333333344444444555555556666666677777777",
             encoder.data(), encoder.size());
}

TEST(encoder, setasyncv5) {
  const char *input = R"""(
      version: 5
      type: SET_ASYNC
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        packet_in_master: [0x22222221]
        port_status_master: [0x44444441]
        port_status_slave: [0x55555551]
        flow_removed_master: [0x66666661]
        flow_removed_slave: [0x77777771]
        role_status_master: [0x88888881]
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x38, encoder.size());
  EXPECT_HEX(
      "051C00381111111100010008222222210002000855555551000300084444444100040008"
      "7777777100050008666666610007000888888881",
      encoder.data(), encoder.size());
}

TEST(encoder, flowremovedv4) {
  const char *input = R"""(
      version: 4
      type: FLOW_REMOVED
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        cookie: 0x2222222222222222
        priority: 0x3333
        reason: 0x44
        table_id: 0x55
        duration: 1717986918.x77777777
        idle_timeout: 0x8888
        hard_timeout: 0x9999
        packet_count: 0xAAAAAAAAAAAAAAAA
        byte_count: 0xBBBBBBBBBBBBBBBB
        match:
            - field: IN_PORT
              value: 0x12345678
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x40, encoder.size());
  EXPECT_HEX(
      "040B004011111111222222222222222233334455666666667777777788889999A"
      "AAAAAAAAAAAAAAABBBBBBBBBBBBBBBB0001000C800000041234567800000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowremovedv1) {
  const char *input = R"""(
      version: 1
      type: FLOW_REMOVED
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        cookie: 0x2222222222222222
        priority: 0x3333
        reason: 0x44
        table_id: 0x55
        duration: 1717986918.x77777777
        idle_timeout: 0x8888
        hard_timeout: 0x9999
        packet_count: 0xAAAAAAAAAAAAAAAA
        byte_count: 0xBBBBBBBBBBBBBBBB
        match:
            - field: IN_PORT
              value: 0x12345678
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x58, encoder.size());
  EXPECT_HEX(
      "010B005811111111003820FE56780000000000000000000000000000000000000"
      "00000000000000000000000000000002222222222222222333344006666666677"
      "77777788880000AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBB",
      encoder.data(), encoder.size());
}

TEST(encoder, flowremovedv2) {
  const char *input = R"""(
      version: 2
      type: FLOW_REMOVED
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        cookie: 0x2222222222222222
        priority: 0x3333
        reason: 0x44
        table_id: 0x55
        duration: 1717986918.x77777777
        idle_timeout: 0x8888
        hard_timeout: 0x9999
        packet_count: 0xAAAAAAAAAAAAAAAA
        byte_count: 0xBBBBBBBBBBBBBBBB
        match:
            - field: IN_PORT
              value: 0x12345678
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x88, encoder.size());
  EXPECT_HEX(
      "020B008811111111222222222222222233334455666666667777777788880000A"
      "AAAAAAAAAAAAAAABBBBBBBBBBBBBBBB0000005812345678000003FE0000000000"
      "00000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000000000000"
      "000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowremovedv3) {
  const char *input = R"""(
      version: 3
      type: FLOW_REMOVED
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        cookie: 0x2222222222222222
        priority: 0x3333
        reason: 0x44
        table_id: 0x55
        duration: 1717986918.x77777777
        idle_timeout: 0x8888
        hard_timeout: 0x9999
        packet_count: 0xAAAAAAAAAAAAAAAA
        byte_count: 0xBBBBBBBBBBBBBBBB
        match:
            - field: IN_PORT
              value: 0x12345678
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x40, encoder.size());
  EXPECT_HEX(
      "030B004011111111222222222222222233334455666666667777777788889999A"
      "AAAAAAAAAAAAAAABBBBBBBBBBBBBBBB0001000C800000041234567800000000",
      encoder.data(), encoder.size());
}

TEST(encoder, flowremovedv6) {
  const char *input = R"""(
      version: 6
      type: FLOW_REMOVED
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        cookie: 0x2222222222222222
        priority: 0x3333
        reason: 0x44
        table_id: 0x55
        duration: 1717986918.x77777777
        idle_timeout: 0x8888
        hard_timeout: 0x9999
        packet_count: 0xAAAAAAAAAAAAAAAA
        byte_count: 0xBBBBBBBBBBBBBBBB
        match:
            - field: IN_PORT
              value: 0x12345678
        stat:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x50, encoder.size());
  EXPECT_HEX(
      "060B005011111111554433338888999922222222222222220001000C8000000412345678"
      "000000000000002880020008666666667777777780020808AAAAAAAAAAAAAAAA80020A08"
      "BBBBBBBBBBBBBBBB",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_desc_request) {
  const char *input = R"""(
      version: 4
      type: MULTIPART_REQUEST
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: DESC
        flags: []
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
      type: MULTIPART_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: DESC
        flags: []
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
      type: MULTIPART_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: DESC
        flags: []
        body:
          mfr_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          hw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          sw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          serial_num: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          dp_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
      )""";

  Encoder encoder{input};
  EXPECT_EQ(
      "YAML:13:23: error: Value is too long\n          serial_num: "
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789\n                      "
      "^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n",
      encoder.error());
  EXPECT_EQ(0, encoder.size());
  EXPECT_HEX("", encoder.data(), encoder.size());
}

TEST(encoder, ofmp_desc_request_v1) {
  const char *input = R"""(
      version: 1
      type: MULTIPART_REQUEST
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: DESC
        flags: []
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x0C, encoder.size());
  EXPECT_HEX("0110000C1111111100000000", encoder.data(), encoder.size());
}

TEST(encoder, ofmp_desc_reply_v1) {
  const char *input = R"""(
      version: 1
      type: MULTIPART_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: DESC
        flags: []
        body:
          mfr_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          hw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          sw_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
          serial_num: ABCDEFGHIJKLMNOPQRSTUVWXYZ
          dp_desc: ABCDEFGHIJKLMNOPQRSTUVWXYZ 123456789
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x42C, encoder.size());
  EXPECT_HEX(
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
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_desc_reply_non_utf8) {
  const char *input = R"""(
      version: 4
      type: MULTIPART_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111112
      msg:
        type: DESC
        flags: []
        body:
          mfr_desc: "\x01\n\xff\ud8fe\udcfe\x80\x7f"
          hw_desc: "\x01\n\xff\xfe\x80\x7f"
          sw_desc: '\x01\n\xff\xfe\x80\x7f'
          serial_num: '\x01\n\xff\xfe\x80\x7f'
          dp_desc: '\x01\n\xff\xfe\x80\x7f'
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(1072, encoder.size());
  EXPECT_HEX(
      "04130430111111120000000000000000010AC3BFEDA3BEEDB3BEC2807F00000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000010AC3BFC3BEC2807F00000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000005C7830315C6E5C7866665C78"
      "66655C7838305C7837660000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000005C7830315C6E5C78"
      "66665C7866655C7838305C783766000000000000000000005C7830315C6E5C7866665C78"
      "66655C7838305C7837660000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portstats_v4_request) {
  const char *input = R"""(
      version: 4
      type: MULTIPART_REQUEST
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: PORT_STATS
        flags: []
        body:
          port_no: 0x22222222
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x18, encoder.size());
  EXPECT_HEX("041200181111111100040000000000002222222200000000", encoder.data(),
             encoder.size());
}

TEST(encoder, ofmp_portstats_v1_request) {
  const char *input = R"""(
      version: 1
      type: MULTIPART_REQUEST
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: PORT_STATS
        flags: []
        body:
          port_no: 0x22222222
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x14, encoder.size());
  EXPECT_HEX("0110001411111111000400002222000000000000", encoder.data(),
             encoder.size());
}

TEST(encoder, ofmp_queue_v4_request) {
  const char *input = R"""(
      version: 4
      type: MULTIPART_REQUEST
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: QUEUE_STATS
        flags: []
        body:
          port_no: 0x22222222
          queue_id: 0x33333333
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x18, encoder.size());
  EXPECT_HEX("041200181111111100050000000000002222222233333333", encoder.data(),
             encoder.size());
}

TEST(encoder, ofmp_queue_v1_request) {
  const char *input = R"""(
      version: 1
      type: MULTIPART_REQUEST
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: QUEUE_STATS
        flags: []
        body:
          port_no: 0x22222222
          queue_id: 0x33333333
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x14, encoder.size());
  EXPECT_HEX("0110001411111111000500002222000033333333", encoder.data(),
             encoder.size());
}

TEST(encoder, meter_mod_v4) {
  const char *input = R"""(
      version: 4
      type: METER_MOD
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        command: MODIFY
        flags: [ 0x2222 ]
        meter_id: 0x33333333
        bands:
          - type: DROP
            rate: 0x44444444
            burst_size: 0x55555555
          - type: DSCP_REMARK
            rate: 0x66666666
            burst_size: 0x77777777
            prec_level: 0x88
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x30, encoder.size());
  EXPECT_HEX(
      "041D00301111111100012222333333330001001044444444555555550000000000020010"
      "666666667777777788000000",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_groupfeatures_reply) {
  const char *input = R"""(
      version: 4
      type: MULTIPART_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: GROUP_FEATURES
        flags: []
        body:
          types: [ 0x11111111 ]
          capabilities: [ 0x22222222 ]
          max_groups_all: 0x33333333
          max_groups_sel: 0x44444444
          max_groups_ind: 0x55555555
          max_groups_ff: 0x66666666
          actions_all: [ '0x37777777' ]
          actions_sel: [ '0x08888888' ]
          actions_ind: [ '0x19999999' ]
          actions_ff: [ '0x2AAAAAAA' ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(56, encoder.size());
  EXPECT_HEX(
      "041300381111111100080000000000001111111122222222333333334444444455555555"
      "666666663777777708888888199999992AAAAAAA",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_flowmonitor_request) {
  const char *input = R"""(
      version: 5
      type: MULTIPART_REQUEST
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: FLOW_MONITOR
        flags: []
        body:
          monitor_id: 0x11111111
          out_port: 0x22222222
          out_group: 0x33333333
          flags: [0x4444]
          table_id: 0x55
          command: 0x66
          match:
            - field: IN_PORT
              value: 0x12345678
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(48, encoder.size());
  EXPECT_HEX(
      "05120030111111110010000000000000111111112222222233333333444455660001000C"
      "800000041234567800000000",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_flowmonitor_reply) {
  const char *input = R"""(
      version: 5
      type: MULTIPART_REPLY
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: FLOW_MONITOR
        flags: []
        body:
          - event: ADDED
            table_id: 0x11
            reason: 0x22
            idle_timeout: 0x3333
            hard_timeout: 0x4444
            priority: 0x5555
            cookie: 0x6666666666666666
            match:
              - field: IN_PORT
                value: 0x12345678
            instructions:
              - instruction:    APPLY_ACTIONS
                actions:
                   - action: SET_FIELD
                     field: IPV4_DST
                     value: 192.168.2.1
          - event: ABBREV
            xid: 0x22222222
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(88, encoder.size());
  EXPECT_HEX(
      "051300581111111100100000000000000040000111223333444455550000000066666666"
      "666666660001000C80000004123456780000000000040018000000000019001080001804"
      "C0A80201000000000008000422222222",
      encoder.data(), encoder.size());
}

TEST(encoder, rolestatusv5) {
  const char *input = R"""(
      version: 5
      type: ROLE_STATUS
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        role: 0x22222222
        reason: 0x01
        generation_id: 0x3333333333333333
        properties:
          - property: EXPERIMENTER
            experimenter: 0x12345678
            exp_type: 0xABCDABCD
            data: '0000F1F1'
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(40, encoder.size());
  EXPECT_HEX(
      "051E00281111111122222222010000003333333333333333FFFF001012345678ABCDABCD"
      "0000F1F1",
      encoder.data(), encoder.size());
}

TEST(encoder, requestforwardv5) {
  const char *input = R"""(
      version: 5
      type: REQUESTFORWARD
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        type: GROUP_MOD
        xid: 0xAAAAAAAA
        msg:
          command: 0x2222
          type: 0x33
          group_id: 0x44444444
          buckets:
            - weight: 0x5555
              watch_port: 0x66666666
              watch_group: 0x77777777
              actions:
                - action: OUTPUT
                  port_no: 5
                  max_len: 20
                - action: SET_FIELD
                  field:  IPV4_DST
                  value:  192.168.1.1
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(72, encoder.size());
  EXPECT_HEX(
      "0520004811111111050F0040AAAAAAAA2222330044444444003055556666666677777777"
      "00000000000000100000000500140000000000000019001080001804C0A801010000000"
      "0",
      encoder.data(), encoder.size());
}

TEST(encoder, bundlecontrolv5) {
  const char *input = R"""(
      version: 5
      type: BUNDLE_CONTROL
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        bundle_id: 0x22222222
        type: 0x3333
        flags: [0x4444]
        properties:
          - property: 0xffff
            experimenter: 0x12345678
            exp_type: 0xABCDABCD
            data: '0000F1F1'
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(32, encoder.size());
  EXPECT_HEX("05210020111111112222222233334444FFFF001012345678ABCDABCD0000F1F1",
             encoder.data(), encoder.size());
}

TEST(encoder, bundleaddmessagev5) {
  const char *input = R"""(
      version: 5
      type: BUNDLE_ADD_MESSAGE
      datapath_id: 00:00:00:00:00:00:00:01
      xid: 0x11111111
      msg:
        bundle_id: 0x22222222
        flags: [0x3333]
        message:
          type: ECHO_REQUEST
          msg:
            data: 'ABCD'
        properties:
          - property: 0xffff
            experimenter: 0x12345678
            exp_type: 0xABCDABCD
            data: '0000F1F1'
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(48, encoder.size());
  EXPECT_HEX(
      "052200301111111122222222000033330502000A11111111ABCD000000000000FFFF0010"
      "12345678ABCDABCD0000F1F1",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portdesc_replyv1) {
  const char *input = R"""(
    version: 1
    type: PORT_DESC_REPLY
    xid: 0x11111111
    msg:
      - port_no: 0x1111
        hw_addr: 22:22:22:22:22:22
        name: Port 1
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        properties:
      - port_no: 0xBBBB
        hw_addr: CC:CC:CC:CC:CC:CC
        name: Port 2
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(108, encoder.size());
  EXPECT_HEX(
      "0111006C11111111000D00001111222222222222506F7274203100000000000000000000"
      "333333334444444455550555666606667777077788880888BBBBCCCCCCCCCCCC506F7274"
      "20320000000000000000000033333333444444445555055566660666777707778888088"
      "8",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portdesc_replyv2) {
  const char *input = R"""(
    version: 2
    type: PORT_DESC_REPLY
    xid: 0x11111111
    msg:
      - port_no: 0x1111
        hw_addr: 22:22:22:22:22:22
        name: Port 1
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        properties:
      - port_no: 0xBBBB
        hw_addr: CC:CC:CC:CC:CC:CC
        name: Port 2
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(144, encoder.size());
  EXPECT_HEX(
      "0213009011111111000D00000000000000001111000000002222222222220000506F7274"
      "203100000000000000000000333333334444444455555555666666667777777788888888"
      "99999999AAAAAAAA0000BBBB00000000CCCCCCCCCCCC0000506F72742032000000000000"
      "0000000033333333444444445555555566666666777777778888888899999999AAAAAAA"
      "A",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portdesc_replyv4) {
  const char *input = R"""(
    version: 4
    type: PORT_DESC_REPLY
    xid: 0x11111111
    msg:
      - port_no: 0x1111
        hw_addr: 22:22:22:22:22:22
        name: Port 1
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        properties:
      - port_no: 0xBBBB
        hw_addr: CC:CC:CC:CC:CC:CC
        name: Port 2
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(144, encoder.size());
  EXPECT_HEX(
      "0413009011111111000D00000000000000001111000000002222222222220000506F7274"
      "203100000000000000000000333333334444444455555555666666667777777788888888"
      "99999999AAAAAAAA0000BBBB00000000CCCCCCCCCCCC0000506F72742032000000000000"
      "0000000033333333444444445555555566666666777777778888888899999999AAAAAAA"
      "A",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portdesc_replyv5) {
  const char *input = R"""(
    version: 5
    type: PORT_DESC_REPLY
    xid: 0x11111111
    msg:
      - port_no: 0x1111
        hw_addr: 22:22:22:22:22:22
        name: Port 1
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        properties:
      - port_no: 0xBBBB
        hw_addr: CC:CC:CC:CC:CC:CC
        name: Port 2
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(160, encoder.size());
  EXPECT_HEX(
      "051300A011111111000D00000000000000001111004800002222222222220000506F7274"
      "203100000000000000000000333333334444444400000020000000005555555566666666"
      "777777778888888899999999AAAAAAAA0000BBBB00480000CCCCCCCCCCCC0000506F7274"
      "203200000000000000000000333333334444444400000020000000005555555566666666"
      "777777778888888899999999AAAAAAAA",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portdesc_replyv5_2) {
  const char *input = R"""(
    version: 5
    type: PORT_DESC_REPLY
    xid: 0x11111111
    msg:
      - port_no: 0x1111
        hw_addr: 22:22:22:22:22:22
        name: Port 1
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        optical:
          supported: [ 0x11111110 ]
          tx_min_freq_lmda: 0x22222220
          tx_max_freq_lmda: 0x33333330
          tx_grid_freq_lmda: 0x44444440
          rx_min_freq_lmda: 0x55555550
          rx_max_freq_lmda: 0x66666660
          rx_grid_freq_lmda: 0x77777770
          tx_pwr_min: 0x8880
          tx_pwr_max: 0x9990
        properties:
          - property: EXPERIMENTER
            experimenter: 0xBBBBBBB0
            exp_type: 0xCCCCCCC0
            data: ABCDEF
      - port_no: 0xBBBB
        hw_addr: CC:CC:CC:CC:CC:CC
        name: Port 2
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        curr:   [ '0x55555555' ]
        advertised: [ '0x66666666' ]
        supported:  [ '0x77777777' ]
        peer:  [ '0x88888888' ]
        curr_speed: 0x99999999
        max_speed: 0xAAAAAAAA
        properties:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(216, encoder.size());
  EXPECT_HEX(
      "051300D811111111000D00000000000000001111008000002222222222220000506F7274"
      "203100000000000000000000333333334444444400000020000000005555555566666666"
      "777777778888888899999999AAAAAAAA0001002800000000111111102222222033333330"
      "4444444055555550666666607777777088809990FFFF000FBBBBBBB0CCCCCCC0ABCDEF00"
      "0000BBBB00480000CCCCCCCCCCCC0000506F727420320000000000000000000033333333"
      "4444444400000020000000005555555566666666777777778888888899999999AAAAAAA"
      "A",
      encoder.data(), encoder.size());
}

TEST(encoder, tablemodv5) {
  const char *input = R"""(
    type:            TABLE_MOD
    xid:             0x00000000
    version:         0x05
    msg:             
      table_id:        ALL
      config:          [  ]
      eviction:        
        flags:           0x11223344
      vacancy:         
        vacancy_down:    0x11
        vacancy_up:      0x22
        vacancy:         0x33
      properties:      
        - property:        EXPERIMENTER
          experimenter:    0x44444441
          exp_type:        0x55555551
          data:            ''
        - property:        EXPERIMENTER
          experimenter:    0x66666661
          exp_type:        0x77777771
          data:            88888888
        - property:        EXPERIMENTER
          experimenter:    0x99999991
          exp_type:        0xAAAAAAA1
          data:            0000000100000002
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(88, encoder.size());
  EXPECT_HEX(
      "0511005800000000FF0000000000000000020008112233440003000811223300FFFF000C"
      "444444415555555100000000FFFF0010666666617777777188888888FFFF001499999991"
      "AAAAAAA1000000010000000200000000",
      encoder.data(), encoder.size());
}

TEST(encoder, tablestatusv5) {
  const char *input = R"""(
    type:            TABLE_STATUS
    xid:             0x00000000
    version:         0x05
    msg: 
      reason:        0x10 
      table_id:        ALL
      config:          [  ]
      eviction:        
        flags:           0x11223344
      vacancy:         
        vacancy_down:    0x11
        vacancy_up:      0x22
        vacancy:         0x33
      properties:      
        - property:        EXPERIMENTER
          experimenter:    0x44444441
          exp_type:        0x55555551
          data:            ''
        - property:        EXPERIMENTER
          experimenter:    0x66666661
          exp_type:        0x77777771
          data:            88888888
        - property:        EXPERIMENTER
          experimenter:    0x99999991
          exp_type:        0xAAAAAAA1
          data:            0000000100000002
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(96, encoder.size());
  EXPECT_HEX(
      "051F00600000000010000000000000000050FF0000000000000200081122334400030008"
      "11223300FFFF000C444444415555555100000000FFFF0010666666617777777188888888"
      "FFFF001499999991AAAAAAA1000000010000000200000000",
      encoder.data(), encoder.size());
}

TEST(encoder, mptabledescv5) {
  const char *input = R"""(
    type:            TABLE_DESC_REPLY
    xid:             0x00000000
    version:         0x05
    msg:            
      - table_id:        1
        config:          [  ]
        eviction:        
          flags:           0x11223344
        vacancy:         
          vacancy_down:    0x11
          vacancy_up:      0x22
          vacancy:         0x33
        properties:      
          - property:        EXPERIMENTER
            experimenter:    0x44444441
            exp_type:        0x55555551
            data:            ''
          - property:        EXPERIMENTER
            experimenter:    0x66666661
            exp_type:        0x77777771
            data:            88888888
          - property:        EXPERIMENTER
            experimenter:    0x99999991
            exp_type:        0xAAAAAAA1
            data:            0000000100000002
      - table_id:        2
        config:          [ VACANCY_EVENTS ]
        vacancy:         
          vacancy_down:    0x10
          vacancy_up:      0x20
          vacancy:         0x30
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(112, encoder.size());
  EXPECT_HEX(
      "0513007000000000000E0000000000000050010000000000000200081122334400030008"
      "11223300FFFF000C444444415555555100000000FFFF0010666666617777777188888888"
      "FFFF001499999991AAAAAAA1000000010000000200000000001002000000000800030008"
      "10203000",
      encoder.data(), encoder.size());
}

TEST(encoder, mptabledescv5_request) {
  const char *input = R"""(
    type:            TABLE_DESC_REQUEST
    xid:             0x00000000
    version:         0x05
    msg:
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(16, encoder.size());
  EXPECT_HEX("0512001000000000000E000000000000", encoder.data(),
             encoder.size());
}

TEST(encoder, mpqueuedescrequest_v5) {
  const char *input = R"""(
    type:            QUEUE_DESC_REQUEST
    xid:             0x01020304
    version:         0x05
    msg:
      port_no: 0x1111111f
      queue_id: 0x2222222f
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(24, encoder.size());
  EXPECT_HEX("0512001801020304000F0000000000001111111F2222222F", encoder.data(),
             encoder.size());
}

TEST(encoder, mpqueuedescreply_v5) {
  const char *input = R"""(
    type:            QUEUE_DESC_REPLY
    xid:             0x01020304
    version:         0x05
    msg:
      - port_no: 0x1111111f
        queue_id: 0x2222222f
        min_rate: 0x1234
        max_rate: 0x5678
        properties:
          - property: EXPERIMENTER
            experimenter: 0xAABBCCDD
            exp_type: 0xDDEEFF11
            data: DEADBEEF12
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(72, encoder.size());
  EXPECT_HEX(
      "0513004801020304000F0000000000001111111F2222222F003800000000000000010008"
      "123400000002000856780000FFFF0011AABBCCDDDDEEFF11DEADBEEF120000000000000"
      "0",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_portstats_v4_flags) {
  const char *input = R"""(
    type: PORT_STATS_REPLY
    version: 4
    xid: 0x11111111
    flags: [ 0x2222 ]
    msg:
        - port_no: 0x33333330
          duration:   286331152.x22222220
          rx_packets: 0x4444444444444440
          tx_packets: 0x5555555555555550
          rx_bytes:   0x6666666666666660
          tx_bytes:   0x7777777777777770
          rx_dropped: 0x8888888888888880
          tx_dropped: 0x9999999999999990
          rx_errors:  0xAAAAAAAAAAAAAAA0
          tx_errors:  0xBBBBBBBBBBBBBBB0
          rx_frame_err: 0xCCCCCCCCCCCCCCC0
          rx_over_err:  0xDDDDDDDDDDDDDDD0
          rx_crc_err:   0xEEEEEEEEEEEEEEE0
          collisions:   0xFFFFFFFFFFFFFFF0
          properties:
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x80, encoder.size());
  EXPECT_HEX(
      "041300801111111100042222000000003333333000000000444444444444444055555555"
      "555555506666666666666660777777777777777088888888888888809999999999999990"
      "AAAAAAAAAAAAAAA0BBBBBBBBBBBBBBB0CCCCCCCCCCCCCCC0DDDDDDDDDDDDDDD0EEEEEEEE"
      "EEEEEEE0FFFFFFFFFFFFFFF01111111022222220",
      encoder.data(), encoder.size());
}

TEST(encoder, echorequest_no_flush) {
  const char *input = R"""(
type: ECHO_REQUEST
version: 1
xid: 7
flags: [NO_FLUSH]
msg:
  data: 'aabbccddeeff'
)""";

  Encoder encoder{input};

  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x0E, encoder.size());
  EXPECT_HEX("0102000E00000007AABBCCDDEEFF", encoder.data(), encoder.size());
  EXPECT_EQ(ofp::OFP_NO_FLUSH, encoder.flags());
}

TEST(encoder, nicira_actions) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:             
        table_id:        0x01
        command:         ADD
        match:           
        instructions:
          - instruction:    APPLY_ACTIONS
            actions:
               - action: NX_REG_MOVE
                 src: 'ETH_DST[0:24]'
                 dst: 'ETH_SRC[24:48]'
               - action: NX_REG_LOAD
                 dst: 'ETH_TYPE[0:8]'
                 value: 0x89
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x70, encoder.size());
  EXPECT_HEX(
      "040E007000000001000000000000000000000000000000000100000000000000FFFFFFFF"
      "FFFFFFFFFFFFFFFF0000000000010004000000000004003800000000FFFF001800002320"
      "00060018000000188000060680000806FFFF0018000023200007000780000A0200000000"
      "00000089",
      encoder.data(), encoder.size());
}

TEST(encoder, raw_message) {
  const char *input = R"""(
      type:            _RAW_MESSAGE
      version:         4
      xid:             1
      msg:
        type:  FEATURES_REPLY
        data:  FF00FF00FF00FF00
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(16, encoder.size());
  EXPECT_HEX("0406001000000001FF00FF00FF00FF00", encoder.data(),
             encoder.size());
}

TEST(encoder, ofmp_groupstats_v3) {
  const char *input = R"""(
      type:            GROUP_STATS_REPLY
      xid:             0x00000000
      version:         0x03
      msg:             
        - group_id:        0x00000001
          ref_count:       0x00000002
          packet_count:    0x1000000000009999
          byte_count:      0x200000000000AAAA
          duration:        0
          bucket_stats:    
            - packet_count:    0x300000000000BBBB
              byte_count:      0x400000000000CCCC
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x40, encoder.size());
  EXPECT_HEX(
      "031300400000000000060000000000000030000000000001000000020000000010000000"
      "00009999200000000000AAAA300000000000BBBB400000000000CCCC",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_groupstats_v3_2) {
  const char *input = R"""(
      type:            GROUP_STATS_REPLY
      xid:             0x00000000
      version:         0x03
      msg:             
        - group_id:        0x00000001
          ref_count:       0x00000002
          packet_count:    0x1000000000009999
          byte_count:      0x200000000000AAAA
          duration:        0
          bucket_stats:    
            - packet_count:    0x300000000000BBBB
              byte_count:      0x400000000000CCCC
        - group_id:        0x00000001
          ref_count:       0x00000002
          packet_count:    0x1000000000009999
          byte_count:      0x200000000000AAAA
          duration:        0
          bucket_stats:    
            - packet_count:    0x300000000000BBBB
              byte_count:      0x400000000000CCCC
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x70, encoder.size());
  EXPECT_HEX(
      "031300700000000000060000000000000030000000000001000000020000000010000000"
      "00009999200000000000AAAA300000000000BBBB400000000000CCCC0030000000000001"
      "00000002000000001000000000009999200000000000AAAA300000000000BBBB40000000"
      "0000CCCC",
      encoder.data(), encoder.size());
}

TEST(encoder, ofmp_table_features) {
  const char *input = R"""(
      type:            TABLE_FEATURES_REPLY
      xid:             0x00000000
      version:         0x04
      msg:             
        - table_id:        0x00
          name:            Port ACL
          metadata_match:  0x0000000000000000
          metadata_write:  0x0000000000000000
          config:          [ 0x03 ]
          max_entries:     0x00000032
          instructions:    [ GOTO_TABLE, WRITE_ACTIONS, APPLY_ACTIONS, CLEAR_ACTIONS, 
                             METER ]
          next_tables:     [ 1, 2, 3, 4, 5, 6, 7 ]
          write_actions:   [ OUTPUT, PUSH_VLAN, POP_VLAN, GROUP, SET_NW_TTL, 
                             SET_FIELD ]
          apply_actions:   [ OUTPUT, PUSH_VLAN, POP_VLAN, GROUP, SET_NW_TTL, 
                             SET_FIELD ]
          match:           [ IN_PORT, ETH_TYPE, ETH_DST/, ETH_SRC/, VLAN_VID, 
                             IP_PROTO, IPV4_DST/, IPV6_DST/, UDP_SRC, UDP_DST, 
                             TCP_SRC, TCP_DST ]
          wildcards:       [ IN_PORT, ETH_TYPE, ETH_DST, ETH_SRC, VLAN_VID, IP_PROTO, 
                             IPV4_DST, IPV6_DST, UDP_SRC, UDP_DST, TCP_SRC, TCP_DST ]
          write_set_field: [ ETH_DST, ETH_SRC, VLAN_VID, VLAN_PCP, IP_DSCP, IPV4_SRC, 
                             IPV4_DST, TCP_SRC, TCP_DST, UDP_SRC, UDP_DST ]
          apply_set_field: [ ETH_DST, ETH_SRC, VLAN_VID, VLAN_PCP, IP_DSCP, IPV4_SRC, 
                             IPV4_DST, TCP_SRC, TCP_DST, UDP_SRC, UDP_DST ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(392, encoder.size());
  EXPECT_HEX(
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
      encoder.data(), encoder.size());
}

TEST(encoder, flowmod_fuzz) {
  const char *input = R"""(
    type:            FLOW_MOD
    xid:             0x00000000
    version:         0x03
    msg:             
      cookie:          0x0000000000000000
      cookie_mask:     0x0000000000000000
      table_id:        0x01
      command:         ADD
      idle_timeout:    0x0000
      hard_timeout:    0x0000
      priority:        0x007B
      buffer_id:       0x0000FFFF
      out_port:        ANY
      out_group:       ANY
      flags:           [  ]
      match:           
        - field:           ETH_DST
          value:           'f2:0b:a4:7d:f8:ea'
      instructions:    
        - instruction:     WRITE_ACTIONS
          actions:         
            - action:          SET_FIELD
              field:           0x80000CFF
              value:           01020000000000000000001000000006FFFF000000000000000400180000000000190010800008060102030405060000BEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE
            - action:          OUTPUT
              port_no:         0x00000006
              max_len:         NO_BUFFER
        - instruction:     APPLY_ACTIONS
          actions:         
            - action:          SET_FIELD
              field:           ETH_SRC
              value:           '01:02:03:04:05:06'
  )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x178, encoder.size());
  EXPECT_HEX(
      "030E01780000000000000000000000000000000000000000010000000000007B0000FFFF"
      "FFFFFFFFFFFFFFFF000000000001000E80000606F20BA47DF8EA00000003012000000000"
      "0019010880000CFF01020000000000000000001000000006FFFF00000000000000040018"
      "0000000000190010800008060102030405060000BEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE"
      "BEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE"
      "BEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE"
      "BEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE"
      "BEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE"
      "BEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBEBE"
      "BEBEBEBEBEBEBEBEBEBEBE000000001000000006FFFF0000000000000004001800000000"
      "00190010800008060102030405060000",
      encoder.data(), encoder.size());
}

TEST(encoder, packetin_icmp4_frag1) {
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

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(126, encoder.size());
  EXPECT_HEX(
      "040A007E00000000FFFFFFFF05EE010600000000FFFFFFFF0001000C8000000400000001"
      "0000000000000E00000000010AC2BB024296810000640800450005DC0518200040013AA7"
      "0A0000010A6400FE080014572C2400059DA8FC590000000046D506000000000010111213"
      "1415161718191A1B1C1D1E1F202122232425",
      encoder.data(), encoder.size());
}

TEST(encoder, packetin_icmp4_frag2) {
  const char *input = R"""(
    type:            PACKET_IN
    xid:             0x00000000
    version:         0x04
    msg:             
      buffer_id:       NO_BUFFER
      total_len:       0x002A
      in_port:         0x00000001
      in_phy_port:     0x00000001
      metadata:        0x0000000000000000
      reason:          APPLY_ACTION
      table_id:        0x06
      cookie:          0x00000000FFFFFFFF
      match:           
        - field:           IN_PORT
          value:           0x00000001
      data:            0E00000000010AC2BB02429681000064080045000018051800B940015FB20A0000010A6400FEC0C1C2C3
  )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(84, encoder.size());
  EXPECT_HEX(
      "040A005400000000FFFFFFFF002A010600000000FFFFFFFF0001000C8000000400000001"
      "0000000000000E00000000010AC2BB02429681000064080045000018051800B940015FB2"
      "0A0000010A6400FEC0C1C2C3",
      encoder.data(), encoder.size());
}

TEST(encoder, flowmod_icmpv4) {
  const char *input = R"""(
      type:            FLOW_MOD
      version:         4
      xid:             1
      msg:
        table_id:        0
        command:         ADD
        match:
          # ICMPV4_TYPE should be sufficient to determine all prereqs.         
          - field:           ICMPV4_TYPE
            value:           0
        instructions:
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(72, encoder.size());
  EXPECT_HEX(
      "040E004800000001000000000000000000000000000000000000000000000000FFFFFFFF"
      "FFFFFFFFFFFFFFFF000000000001001480000A020800800014010180002601000000000"
      "0",
      encoder.data(), encoder.size());
}

TEST(encoder, tablefeatures_empty) {
  const char *input = R"""(
      type:            TABLE_FEATURES_REQUEST
      version:         4
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(16, encoder.size());
  EXPECT_HEX("0412001000000000000C000000000000", encoder.data(),
             encoder.size());
}

TEST(encoder, tablefeatures_request) {
  const char *input = R"""(
      type:            TABLE_FEATURES_REQUEST
      xid:             0x00000001
      version:         0x04
      msg:             
        - table_id:        0x00
          name:            Port ACL
          metadata_match:  0x0000000000000000
          metadata_write:  0x0000000000000000
          config:          [ 0x03 ]
          max_entries:     0x00000032
          instructions:    [ GOTO_TABLE, WRITE_ACTIONS, APPLY_ACTIONS, CLEAR_ACTIONS, 
                             METER ]
          next_tables:     [ 1, 2, 3, 4, 5, 6, 7 ]
          write_actions:   [ OUTPUT, PUSH_VLAN, POP_VLAN, GROUP, SET_NW_TTL, 
                             SET_FIELD ]
          apply_actions:   [ OUTPUT, PUSH_VLAN, POP_VLAN, GROUP, SET_NW_TTL, 
                             SET_FIELD ]
          match:           [ IN_PORT, ETH_TYPE, ETH_DST/, ETH_SRC/, VLAN_VID, 
                             IP_PROTO, IPV4_DST/, IPV6_DST/, UDP_SRC, UDP_DST, 
                             TCP_SRC, TCP_DST ]
          wildcards:       [ IN_PORT, ETH_TYPE, ETH_DST, ETH_SRC, VLAN_VID, IP_PROTO, 
                             IPV4_DST, IPV6_DST, UDP_SRC, UDP_DST, TCP_SRC, TCP_DST ]
          write_set_field: [ ETH_DST, ETH_SRC, VLAN_VID, VLAN_PCP, IP_DSCP, IPV4_SRC, 
                             IPV4_DST, TCP_SRC, TCP_DST, UDP_SRC, UDP_DST ]
          apply_set_field: [ ETH_DST, ETH_SRC, VLAN_VID, VLAN_PCP, IP_DSCP, IPV4_SRC, 
                             IPV4_DST, TCP_SRC, TCP_DST, UDP_SRC, UDP_DST ]
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(392, encoder.size());
  EXPECT_HEX(
      "0412018800000001000C0000000000000178000000000000506F72742041434C00000000"
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
      encoder.data(), encoder.size());
}
