#include "ofp/unittest.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using namespace yaml;

static void testDecodeEncode(const char *hex, const char *yaml)
{
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

TEST(decoder, hellov1)
{
    testDecodeEncode(
        "0100000800000001",
        "---\ntype:            OFPT_HELLO\nxid:             "
        "0x00000001\nversion:         1\nmsg:             \n  versions:  "
        "      [ 1 ]\n...\n");
}

TEST(decoder, hellov4)
{
    testDecodeEncode(
        "04000010000000010001000800000012",
        "---\ntype:            OFPT_HELLO\nxid:             "
        "0x00000001\nversion:         4\nmsg:             \n  versions:  "
        "      [ 1, 4 ]\n...\n");
}

TEST(decoder, errorv1)
{
    testDecodeEncode(
        "010100130000006200010001FFFF1234567890",
        "---\ntype:            OFPT_ERROR\nxid:             "
        "0x00000062\nversion:         1\nmsg:             \n  type:      "
        "      1\n  code:            1\n  data:            "
        "FFFF1234567890\n...\n");
}

TEST(decoder, echoRequest)
{
    testDecodeEncode("0102000E00000007AABBCCDDEEFF",
                     "---\ntype:            OFPT_ECHO_REQUEST\nxid:            "
                     " 0x00000007\nversion:         1\nmsg:             \n  "
                     "data:            AABBCCDDEEFF\n...\n");
}

TEST(decoder, echoReply)
{
    testDecodeEncode("0103000E00000007AABBCCDDEEFF",
                     "---\ntype:            OFPT_ECHO_REPLY\nxid:             "
                     "0x00000007\nversion:         1\nmsg:             \n  "
                     "data:            AABBCCDDEEFF\n...\n");
}

TEST(decoder, experimenterv4)
{
    testDecodeEncode("0404001800000018DEADBEEFAABBCCDDABCDEF0123456789",
                     "---\ntype:            OFPT_EXPERIMENTER\nxid:            "
                     " 0x00000018\nversion:         4\nmsg:             \n  "
                     "experimenter:    3735928559\n  exp_type:        "
                     "2864434397\n  data:            ABCDEF0123456789\n...\n");
}

TEST(decoder, experimenterv1)
{
    testDecodeEncode("010400140000001BDEADBEEFABCDEF0123456789",
                     "---\ntype:            OFPT_EXPERIMENTER\nxid:            "
                     " 0x0000001B\nversion:         1\nmsg:             \n  "
                     "experimenter:    3735928559\n  exp_type:        0\n  "
                     "data:            ABCDEF0123456789\n...\n");
}

TEST(decoder, experimenterv2)
{
    testDecodeEncode("02040018000000FFDEADBEEF00000000ABCDEF0123456789",
                     "---\ntype:            OFPT_EXPERIMENTER\nxid:            "
                     " 0x000000FF\nversion:         2\nmsg:             \n  "
                     "experimenter:    3735928559\n  exp_type:        0\n  "
                     "data:            ABCDEF0123456789\n...\n");
}

TEST(decoder, featuresrequest)
{
    testDecodeEncode("04050008000000BF", "---\ntype:            "
                                         "OFPT_FEATURES_REQUEST\nxid:          "
                                         "   0x000000BF\nversion:         "
                                         "4\nmsg:             \n...\n");
}

TEST(decoder, getconfigrequest)
{
    testDecodeEncode("04070008000000BF", "---\ntype:            "
                                         "OFPT_GET_CONFIG_REQUEST\nxid:        "
                                         "     0x000000BF\nversion:         "
                                         "4\nmsg:             \n...\n");
}

TEST(decoder, barrierrequestv4)
{
    testDecodeEncode("04140008000000BF", "---\ntype:            "
                                         "OFPT_BARRIER_REQUEST\nxid:           "
                                         "  0x000000BF\nversion:         "
                                         "4\nmsg:             \n...\n");
}

TEST(decoder, barrierrequestv1)
{
    testDecodeEncode("01120008000000BF", "---\ntype:            "
                                         "OFPT_BARRIER_REQUEST\nxid:           "
                                         "  0x000000BF\nversion:         "
                                         "1\nmsg:             \n...\n");
}

TEST(decoder, barrierreplyv4)
{
    testDecodeEncode("04150008000000BF", "---\ntype:            "
                                         "OFPT_BARRIER_REPLY\nxid:             "
                                         "0x000000BF\nversion:         4\nmsg: "
                                         "            \n...\n");
}

TEST(decoder, barrierreplyv1)
{
    testDecodeEncode("01130008000000BF", "---\ntype:            "
                                         "OFPT_BARRIER_REPLY\nxid:             "
                                         "0x000000BF\nversion:         1\nmsg: "
                                         "            \n...\n");
}

TEST(decoder, getasyncrequestv4)
{
    testDecodeEncode("041A0008000000BF", "---\ntype:            "
                                         "OFPT_GET_ASYNC_REQUEST\nxid:         "
                                         "    0x000000BF\nversion:         "
                                         "4\nmsg:             \n...\n");
}

TEST(decoder, getasyncrequestv1)
{
    auto s = HexToRawData("011A0008000000BF");

    Message msg{s.data(), s.size()};
    msg.transmogrify();

    Decoder decoder{&msg};

    EXPECT_EQ("Invalid data.", decoder.error());
    EXPECT_EQ("", decoder.result());
}

TEST(decoder, featuresreplyv1)
{
    testDecodeEncode(
        "01060020000000BF000001020304050600000100FF0000000000000000000000",
        "---\ntype:            OFPT_FEATURES_REPLY\nxid:             "
        "0x000000BF\nversion:         1\nmsg:             \n  datapath_id:     "
        "0000-0102-0304-0506\n  n_buffers:       256\n  n_tables:        255\n "
        " auxiliary_id:    0\n  capabilities:    0\n  reserved:        "
        "0\n...\n");
}

TEST(decoder, featuresreplyv4)
{
    testDecodeEncode(
        "04060020000000BF000001020304050600000100FF0000000000000000000000",
        "---\ntype:            OFPT_FEATURES_REPLY\nxid:             "
        "0x000000BF\nversion:         4\nmsg:             \n  datapath_id:     "
        "0000-0102-0304-0506\n  n_buffers:       256\n  n_tables:        255\n "
        " auxiliary_id:    0\n  capabilities:    0\n  reserved:        "
        "0\n...\n");
}

TEST(decoder, multipartrequest)
{
    testDecodeEncode("0412003C1122334400010000000000000100000000000002000000030000000000000000000000040000000000000005000100088000000412345678", "---\ntype:            OFPT_MULTIPART_REQUEST\nxid:             0x11223344\nversion:         4\nmsg:             \n  type:            OFPMP_FLOW\n  flags:           0x0000\n  body:            \n    table_id:        1\n    out_port:        2\n    out_group:       3\n    cookie:          4\n    cookie_mask:     5\n    match:           \n      - type:            OFB_IN_PORT\n        value:           305419896\n...\n");
}

TEST(decoder, multipartreply)
{
    testDecodeEncode("0413005411223344000100000000000000440100000000020000000300040005000600070000000000000000000000080000000000000009000000000000000A0001000880000004123456780001000801000000", "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             0x11223344\nversion:         4\nmsg:             \n  type:            OFPMP_FLOW\n  flags:           0x0000\n  body:            \n    - table_id:        1\n      duration_sec:    0x00000002\n      duration_nsec:   0x00000003\n      priority:        0x0004\n      idle_timeout:    0x0005\n      hard_timeout:    0x0006\n      flags:           0x0007\n      cookie:          0x0000000000000008\n      packet_count:    0x0000000000000009\n      byte_count:      0x000000000000000A\n      match:           \n        - type:            OFB_IN_PORT\n          value:           305419896\n      instructions:    \n        - type:            OFPIT_GOTO_TABLE\n          value:           \n            table_id:        1\n...\n");
}

TEST(decoder, multipartreply2)
{
    testDecodeEncode("0413006411223344000100000000000000540100000000020000000300040005000600070000000000000000000000080000000000000009000000000000000A0001000000040020000000000000001000000001FFFF0000000000000017000840000000", "---\ntype:            OFPT_MULTIPART_REPLY\nxid:             0x11223344\nversion:         4\nmsg:             \n  type:            OFPMP_FLOW\n  flags:           0x0000\n  body:            \n    - table_id:        1\n      duration_sec:    0x00000002\n      duration_nsec:   0x00000003\n      priority:        0x0004\n      idle_timeout:    0x0005\n      hard_timeout:    0x0006\n      flags:           0x0007\n      cookie:          0x0000000000000008\n      packet_count:    0x0000000000000009\n      byte_count:      0x000000000000000A\n      match:           \n      instructions:    \n        - type:            OFPIT_APPLY_ACTIONS\n          value:           \n            - action:          OFPAT_OUTPUT\n              port:            1\n              maxlen:          65535\n            - action:          OFPAT_SET_NW_TTL\n              ttl:             64\n...\n");
}
