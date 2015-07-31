// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/yaml/encoder.h"

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

  testEncoderSuccess(input, 16, "04000010000000000001000800000010");
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
      datapath_id: '0000:0102:0304:0506'
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
      datapath_id: '0000:0102:0304:0506'
      n_buffers: 256
      n_tables: 255
      auxiliary_id: 0
      capabilities: []
      actions: []
      ports:
        - port_no: 0x1111
          hw_addr: 22-22-22-22-22-22
          name: Port 1
          config: [ '0x33333333' ]
          state:  [ '0x44444444' ]
          ethernet:
            curr:   [ '0x55555555' ]
            advertised: [ '0x66666666' ]
            supported:  [ '0x77777777' ]
            peer:  [ '0x88888888' ]
            curr_speed: 0x99999999
            max_speed: 0xAAAAAAAA
          properties:
        - port_no: 0xBBBB
          hw_addr: CC-CC-CC-CC-CC-CC
          name: Port 2
          config: [ '0x33333333' ]
          state:  [ '0x44444444' ]
          ethernet:
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
      datapath_id: '0000:0102:0304:0506'
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

TEST(encoder, ofmp_flowrequest_v4) {
  const char *input = R"""(
    type: MULTIPART_REQUEST
    version: 4
    xid: 0x11223344
    msg:
      type: FLOW
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
      type: FLOW
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
      type: AGGREGATE
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
      type: AGGREGATE
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
      type: FLOW
      flags: []
      body:
         - table_id: 1
           duration_sec: 2
           duration_nsec: 3
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
      type: FLOW
      flags: []
      body:
         - table_id: 1
           duration_sec: 2
           duration_nsec: 3
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
           duration_sec: 0x22
           duration_nsec: 0x33 
           priority: 0x44
           idle_timeout: 0x55
           hard_timeout: 0x66
           flags: [ 0x77 ]
           cookie: 0x88
           packet_count: 0x9999999999999999
           byte_count:   0xAAAAAAAAAAAAAAAA
           match:
             - field: ETH_SRC
               value: 10-20-30-40-50-60
             - field: ETH_DST
               value: aa-bb-cc-dd-ee-ff
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
      type: FLOW
      flags: []
      body:
         - table_id: 1
           duration_sec: 2
           duration_nsec: 3
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
                    port: 1
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
      type: FLOW
      flags: [ 0x2222 ]
      body:
         - table_id: 0x33
           duration_sec: 0x44444444
           duration_nsec: 0x55555555
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
                    port: 0xEEEEEEEE
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
      type: FLOW
      flags: []
      body:
         - table_id: 1
           duration_sec: 2
           duration_nsec: 3
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
                    port: 0xEEEEEEEE
                    max_len: 0xFFFF
         - table_id: 0x11
           duration_sec: 0x22
           duration_nsec: 0x33 
           priority: 0x44
           idle_timeout: 0x55
           hard_timeout: 0x66
           flags: [ 0x77 ]
           cookie: 0x88
           packet_count: 0x9999999999999999
           byte_count:   0xAAAAAAAAAAAAAAAA
           match:
             - field: ETH_SRC
               value: 10-20-30-40-50-60
             - field: ETH_DST
               value: aa-bb-cc-dd-ee-ff
           instructions:
             - instruction: GOTO_TABLE
               table_id: 1
             - instruction: APPLY_ACTIONS
               actions:
                  - action: OUTPUT
                    port: 0xEEEEEEEE
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
      type: AGGREGATE
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
      type: AGGREGATE
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

TEST(encoder, ofmp_tablestats_v4) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 4
    xid: 0x11111111
    msg:
      type: TABLE
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
      type: TABLE
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
          duration_sec:   0x11111110
          duration_nsec:  0x22222220
          rx_packets: 0x4444444444444440
          tx_packets: 0x5555555555555550
          rx_bytes:   0x6666666666666660
          tx_bytes:   0x7777777777777770
          rx_dropped: 0x8888888888888880
          tx_dropped: 0x9999999999999990
          rx_errors:  0xAAAAAAAAAAAAAAA0
          tx_errors:  0xBBBBBBBBBBBBBBB0
          ethernet:
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
          duration_sec:   0x11111110
          duration_nsec:  0x22222220
          rx_packets: 0x4444444444444440
          tx_packets: 0x5555555555555550
          rx_bytes:   0x6666666666666660
          tx_bytes:   0x7777777777777770
          rx_dropped: 0x8888888888888880
          tx_dropped: 0x9999999999999990
          rx_errors:  0xAAAAAAAAAAAAAAA0
          tx_errors:  0xBBBBBBBBBBBBBBB0
          ethernet:
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
          duration_sec:   0x11111110
          duration_nsec:  0x22222220
          rx_packets: 0x4444444444444440
          tx_packets: 0x5555555555555550
          rx_bytes:   0x6666666666666660
          tx_bytes:   0x7777777777777770
          rx_dropped: 0x8888888888888880
          tx_dropped: 0x9999999999999990
          rx_errors:  0xAAAAAAAAAAAAAAA0
          tx_errors:  0xBBBBBBBBBBBBBBB0
          ethernet:
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
          duration_sec:   0x11111110
          duration_nsec:  0x22222220
          rx_packets: 0x4444444444444440
          tx_packets: 0x5555555555555550
          rx_bytes:   0x6666666666666660
          tx_bytes:   0x7777777777777770
          rx_dropped: 0x8888888888888880
          tx_dropped: 0x9999999999999990
          rx_errors:  0xAAAAAAAAAAAAAAA0
          tx_errors:  0xBBBBBBBBBBBBBBB0
          ethernet:
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

TEST(encoder, ofmp_queuestats_v4) {
  const char *input = R"""(
    type: MULTIPART_REPLY
    version: 4
    xid: 0x11111111
    msg:
      type: QUEUE
      flags: [ 0x2222 ]
      body:
        - port_no: 0x33333330
          queue_id: 0x44444440
          tx_bytes:   0x5555555555555550
          tx_packets: 0x6666666666666660
          tx_errors:  0x7777777777777770
          duration_sec:   0x11111110
          duration_nsec:  0x22222220
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
      type: QUEUE
      flags: [ 0x2222 ]
      body:
        - port_no: 0x33333330
          queue_id: 0x44444440
          tx_bytes:   0x5555555555555550
          tx_packets: 0x6666666666666660
          tx_errors:  0x7777777777777770
          duration_sec:   0x11111110
          duration_nsec:  0x22222220
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
            port: 5
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
            port: 5
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

TEST(encoder, setconfigv4) {
  const char *input = R"""(
      version: 4
      type: SET_CONFIG
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        reason: 0x22
        port:
          port_no: 0x33333333
          hw_addr: 'aabbccddeeff'
          name: 'Port 1'
          config: [ 0x44444444 ]
          state: [ 0x55555555 ]
          ethernet:
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
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        reason: 0x22
        port:
          port_no: 0x33333333
          hw_addr: 'aabbccddeeff'
          name: 'Port 1'
          config: [ 0x44444444 ]
          state: [ 0x55555555 ]
          ethernet:
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
              - action: OUTPUT
                port: 5
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
              - action: OUTPUT
                port: 5
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
      )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0x28, encoder.size());
  EXPECT_HEX(
      "04100028111111112222222200000000333333333333000044444444555555556"
      "666666600000000",
      encoder.data(), encoder.size());
}

TEST(encoder, portmodv1) {
  const char *input = R"""(
      version: 1
      type: PORT_MOD
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        port_no: 0x2222
        hw_addr: '333333333333'
        config: [ 0x44444444 ]
        mask: [ 0x55555555 ]
        ethernet:
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
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
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
      type: ROLE_REPLY
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
      type: GET_ASYNC_REPLY
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        packet_in_mask_master: [0x22222222]
        packet_in_mask_slave: [0x33333333]
        port_status_mask_master: [0x44444444]
        port_status_mask_slave: [0x55555555]
        flow_removed_mask_master: [0x66666666]
        flow_removed_mask_slave: [0x77777777]
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
      type: QUEUE_GET_CONFIG_REQUEST
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

TEST(encoder, queuegetconfigreplyv4) {
  const char *input = R"""(
    version: 4
    type: QUEUE_GET_CONFIG_REPLY
    datapath_id: 0000-0000-0000-0001
    xid: 0x11111111
    msg:
      port: 0x22222222
      queues:
        - queue_id: 0x33333333
          port: 0x44444444
          min_rate: 0x5555
          max_rate: 0x6666
        - queue_id: 0x77777777
          port: 0x88888888
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
    datapath_id: 0000-0000-0000-0001
    xid: 0x11111111
    msg:
      port: 0x22222222
      queues:
        - queue_id: 0x33333333
          port: 0x44444444
          min_rate: 0x5555
          max_rate: 0x6666
          properties:
            - experimenter: 0xEEEEEEEE
              value: 000102030405
            - experimenter: 0xFFFFFFFF
              value: abcdef
        - queue_id: 0x77777777
          port: 0x88888888
          min_rate: 0x9999
          max_rate: 0xAAAA
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(0xA0, encoder.size());
  EXPECT_HEX(
      "041700A01111111122222222000000003333333344444444006000000000000000010010"
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
    datapath_id: 0000-0000-0000-0001
    xid: 0x11111110
    msg:
      port: 0x22222221
      queues:
        - queue_id: 0x33333331
          port: 0x44444441
          min_rate: 0x5551
          max_rate: 0x6661
        - queue_id: 0x77777771
          port: 0x88888881
          min_rate: 0x9991
          max_rate: 0xAAA1
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(96, encoder.size());
  EXPECT_HEX(
      "01150060111111102221000000000000333333310028000000010010000000005551000000000000000200100000000066610000000000007777777100280000000100100000000099910000000000000002001000000000AAA1000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, queuegetconfigreplyv2) {
  const char *input = R"""(
    version: 2
    type: QUEUE_GET_CONFIG_REPLY
    datapath_id: 0000-0000-0000-0001
    xid: 0x11111110
    msg:
      port: 0x22222221
      queues:
        - queue_id: 0x33333331
          port: 0x44444441
          min_rate: 0x5551
          max_rate: 0x6661
        - queue_id: 0x77777771
          port: 0x88888881
          min_rate: 0x9991
          max_rate: 0xAAA1
    )""";

  Encoder encoder{input};
  EXPECT_EQ("", encoder.error());
  EXPECT_EQ(96, encoder.size());
  EXPECT_HEX(
      "02170060111111102222222100000000333333310028000000010010000000005551000000000000000200100000000066610000000000007777777100280000000100100000000099910000000000000002001000000000AAA1000000000000",
      encoder.data(), encoder.size());
}

TEST(encoder, getconfigreplyv4) {
  const char *input = R"""(
      version: 4
      type: GET_CONFIG_REPLY
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
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

TEST(encoder, ofmp_desc_request) {
  const char *input = R"""(
      version: 4
      type: MULTIPART_REQUEST
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
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

TEST(encoder, ofmp_portstats_v4_request) {
  const char *input = R"""(
      version: 4
      type: MULTIPART_REQUEST
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        type: QUEUE
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
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        type: QUEUE
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
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        type: GROUP_FEATURES
        flags: []
        body:
          types: 0x11111111
          capabilities: 0x22222222
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
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
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
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        role: 0x22222222
        reason: 0x01
        generation_id: 0x3333333333333333
        properties:
          - property: 0xffff
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
      datapath_id: 0000-0000-0000-0001
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
                  port: 5
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
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        bundle_id: 0x22222222
        type: 0x3333
        flags: 0x4444
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
      datapath_id: 0000-0000-0000-0001
      xid: 0x11111111
      msg:
        bundle_id: 0x22222222
        flags: 0x3333
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
    type: REPLY.PORT_DESC
    xid: 0x11111111
    msg:
      - port_no: 0x1111
        hw_addr: 22-22-22-22-22-22
        name: Port 1
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        ethernet:
          curr:   [ '0x55555555' ]
          advertised: [ '0x66666666' ]
          supported:  [ '0x77777777' ]
          peer:  [ '0x88888888' ]
          curr_speed: 0x99999999
          max_speed: 0xAAAAAAAA
        properties:
      - port_no: 0xBBBB
        hw_addr: CC-CC-CC-CC-CC-CC
        name: Port 2
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        ethernet:
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
    type: REPLY.PORT_DESC
    xid: 0x11111111
    msg:
      - port_no: 0x1111
        hw_addr: 22-22-22-22-22-22
        name: Port 1
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        ethernet:
          curr:   [ '0x55555555' ]
          advertised: [ '0x66666666' ]
          supported:  [ '0x77777777' ]
          peer:  [ '0x88888888' ]
          curr_speed: 0x99999999
          max_speed: 0xAAAAAAAA
        properties:
      - port_no: 0xBBBB
        hw_addr: CC-CC-CC-CC-CC-CC
        name: Port 2
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        ethernet:
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
    type: REPLY.PORT_DESC
    xid: 0x11111111
    msg:
      - port_no: 0x1111
        hw_addr: 22-22-22-22-22-22
        name: Port 1
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        ethernet:
          curr:   [ '0x55555555' ]
          advertised: [ '0x66666666' ]
          supported:  [ '0x77777777' ]
          peer:  [ '0x88888888' ]
          curr_speed: 0x99999999
          max_speed: 0xAAAAAAAA
        properties:
      - port_no: 0xBBBB
        hw_addr: CC-CC-CC-CC-CC-CC
        name: Port 2
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        ethernet:
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
    type: REPLY.PORT_DESC
    xid: 0x11111111
    msg:
      - port_no: 0x1111
        hw_addr: 22-22-22-22-22-22
        name: Port 1
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        ethernet:
          curr:   [ '0x55555555' ]
          advertised: [ '0x66666666' ]
          supported:  [ '0x77777777' ]
          peer:  [ '0x88888888' ]
          curr_speed: 0x99999999
          max_speed: 0xAAAAAAAA
        properties:
      - port_no: 0xBBBB
        hw_addr: CC-CC-CC-CC-CC-CC
        name: Port 2
        config: [ '0x33333333' ]
        state:  [ '0x44444444' ]
        ethernet:
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
