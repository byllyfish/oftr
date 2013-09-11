#include "ofp/unittest.h"
#include "ofp/yaml/encoder.h"

using namespace ofp::yaml;

static void testEncoderSuccess(const char *yaml, size_t size, const char *hex)
{
    Encoder encoder{yaml};
    EXPECT_EQ("", encoder.error());
    EXPECT_EQ(size, encoder.size());
    EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(encoder, flowMod)
{
    const char *input = "{ type: OFPT_FLOW_MOD, version: 4, xid: 1 }";

    Encoder encoder{input};
    const std::string &err = encoder.error();

    EXPECT_EQ("YAML:1:3: error: missing required key 'msg'\n{ type: "
              "OFPT_FLOW_MOD, version: 4, xid: 1 }\n  ^\n",
              err);
}

TEST(encoder, hellov1)
{
    const char *input = "{ type: OFPT_HELLO, version: 1, xid: 1 }";

    Encoder encoder{input};
    const std::string &err = encoder.error();

    EXPECT_EQ("", err);
    EXPECT_EQ(0x08, encoder.size());
    EXPECT_HEX("0100000800000001", encoder.data(), encoder.size());
}

TEST(encoder, hellov4err)
{
    const char *input =
        "{ type: OFPT_HELLO, version: 4, xid: 1 msg:{versions{1, 4}} }";

    Encoder encoder{input};

    EXPECT_EQ("YAML:1:43: error: Found unexpected ':' while scanning a plain "
              "scalar\n{ type: OFPT_HELLO, version: 4, xid: 1 msg:{versions{1, "
              "4}} }\n                                          ^\n",
              encoder.error());
}

TEST(encoder, hellov4)
{
    const char *input = "{ 'type': 'OFPT_HELLO', 'version': 4, 'xid': 99, "
                        "'msg':{ 'versions': [1,4] } }";

    Encoder encoder{input};

    EXPECT_EQ("", encoder.error());
    EXPECT_EQ(0x10, encoder.size());
    EXPECT_HEX("04000010000000630001000800000012", encoder.data(),
               encoder.size());
}

TEST(encoder, error)
{
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

TEST(encoder, error2)
{
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
    EXPECT_EQ(0x0C, encoder.size());
    EXPECT_HEX("0101000C0000006200010001", encoder.data(), encoder.size());
}

TEST(encoder, echorequest)
{
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

TEST(encoder, echoreply)
{
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

TEST(encoder, experimenterv4)
{
    const char *input = R"""(
type: OFPT_EXPERIMENTER
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

TEST(encoder, experimenterv1)
{
    const char *input = R"""(
type: OFPT_EXPERIMENTER
version: 1
xid: 0x1B
msg:
  experimenter: 0xDEADBEEF
  exp_type: 0xAABBCCDD
  data: 'ABCDEF0123456789'
)""";

    testEncoderSuccess(input, 0x14, "010400140000001BDEADBEEFABCDEF0123456789");
}

TEST(encoder, experimenterv2)
{
    const char *input = R"""(
type: OFPT_EXPERIMENTER
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

TEST(encoder, featuresrequest)
{
    const char *input = R"""(
   type: OFPT_FEATURES_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

    testEncoderSuccess(input, 0x08, "04050008000000BF");
}

TEST(encoder, getasyncrequestv4)
{
    const char *input = R"""(
   type: OFPT_GET_ASYNC_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

    testEncoderSuccess(input, 0x08, "041A0008000000BF");
}

TEST(encoder, getasyncrequestv1)
{
    const char *input = R"""(
   type: OFPT_GET_ASYNC_REQUEST
   version: 1
   xid: 0xBF
   msg: {}
      )""";

    testEncoderSuccess(input, 0, "");
}

TEST(encoder, getconfigrequest)
{
    const char *input = R"""(
   type: OFPT_GET_CONFIG_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

    testEncoderSuccess(input, 0x08, "04070008000000BF");
}

TEST(encoder, barrierrequestv4)
{
    const char *input = R"""(
   type: OFPT_BARRIER_REQUEST
   version: 4
   xid: 0xBF
   msg: {}
      )""";

    testEncoderSuccess(input, 0x08, "04140008000000BF");
}

TEST(encoder, barrierrequestv1)
{
    const char *input = R"""(
   type: OFPT_BARRIER_REQUEST
   version: 1
   xid: 0xBF
   msg: {}
      )""";

    testEncoderSuccess(input, 0x08, "01120008000000BF");
}

TEST(encoder, barrierreplyv4)
{
    const char *input = R"""(
   type: OFPT_BARRIER_REPLY
   version: 4
   xid: 0xBF
   msg: {}
      )""";

    testEncoderSuccess(input, 0x08, "04150008000000BF");
}

TEST(encoder, barrierreplyv1)
{
    const char *input = R"""(
   type: OFPT_BARRIER_REPLY
   version: 1
   xid: 0xBF
   msg: {}
      )""";

    testEncoderSuccess(input, 0x08, "01130008000000BF");
}

TEST(encoder, featuresreplyv1)
{
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

TEST(encoder, featuresreplyv4)
{
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

TEST(encoder, multipartrequest)
{
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

TEST(encoder, multipartreply)
{
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

    const char *hex = "0413005411223344000100000000000000440100000000020000000300040005000600070000000000000000000000080000000000000009000000000000000A0001000880000004123456780001000801000000";
    Encoder encoder{input};
    EXPECT_EQ("", encoder.error());
    EXPECT_EQ(0x54, encoder.size());
    EXPECT_HEX(hex, encoder.data(), encoder.size());

    // testEncoderSuccess(input, 0x04C,
    // "0413004C112233440001000000000000003C0100000000020000000300040005000600070000000000000000000000080000000000000009000000000000000A000100088000000412345678");
}

TEST(encoder, multipartreply2)
{
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
                       "041300A4112233440001000000000000003C0100000000020000000300040005000600070000000000000000000000080000000000000009000000000000000A00010008800000041234567800581100000000220000003300440055006600770000000000000000000000889999999999999999AAAAAAAAAAAAAAAA0001001C80000004123456788000080610203040506080000606AABBCCDDEEFF0001000801000000");
}
