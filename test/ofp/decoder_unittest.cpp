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

TEST(decoder, flowmodv4) 
{
    testDecodeEncode("040E006800000001000000000000000000000000000000000000000000000000000000000000000000000000000000000001001A800000040000000D80000A02080080001804C0A8010100000000000000040018000000000019001080001804C0A8020100000000", 
        "---\ntype:            OFPT_FLOW_MOD\nxid:             0x00000001\nversion:         4\nmsg:             \n  cookie:          0x0000000000000000\n  cookie_mask:     0x0000000000000000\n  table_id:        0\n  command:         0\n  idle_timeout:    0x0000\n  hard_timeout:    0x0000\n  priority:        0x0000\n  buffer_id:       0x00000000\n  out_port:        0x00000000\n  out_group:       0x00000000\n  flags:           0x0000\n  match:           \n    - type:            OFB_IN_PORT\n      value:           13\n    - type:            OFB_ETH_TYPE\n      value:           2048\n    - type:            OFB_IPV4_DST\n      value:           192.168.1.1\n  instructions:    \n    - type:            OFPIT_APPLY_ACTIONS\n      value:           \n        - action:          OFPAT_SET_FIELD\n          type:            OFB_IPV4_DST\n          value:           192.168.2.1\n...\n");
}

TEST(decoder, flowmodv4_2)
{
    testDecodeEncode("040E0068000000011111111111111111222222222222222233445555666677778888888899999999AAAAAAAABBBB00000001001A80000004CCCCCCCC80000A02080080001804C0A8010100000000000000040018000000000019001080001804C0A8020100000000", "---\ntype:            OFPT_FLOW_MOD\nxid:             0x00000001\nversion:         4\nmsg:             \n  cookie:          0x1111111111111111\n  cookie_mask:     0x2222222222222222\n  table_id:        51\n  command:         68\n  idle_timeout:    0x5555\n  hard_timeout:    0x6666\n  priority:        0x7777\n  buffer_id:       0x88888888\n  out_port:        0x99999999\n  out_group:       0xAAAAAAAA\n  flags:           0xBBBB\n  match:           \n    - type:            OFB_IN_PORT\n      value:           3435973836\n    - type:            OFB_ETH_TYPE\n      value:           2048\n    - type:            OFB_IPV4_DST\n      value:           192.168.1.1\n  instructions:    \n    - type:            OFPIT_APPLY_ACTIONS\n      value:           \n        - action:          OFPAT_SET_FIELD\n          type:            OFB_IPV4_DST\n          value:           192.168.2.1\n...\n");
}

TEST(decoder, flowmodv1)
{
    const char *hex = "010E00500000000100303FEECCCC0000000000000000000000000000000008000000000000000000C0A801010000000011111111111111110044555566667777888888889999BBBB00070008C0A80201";
    const char *yaml = "---\ntype:            OFPT_FLOW_MOD\nxid:             0x00000001\nversion:         1\nmsg:             \n  cookie:          0x1111111111111111\n  cookie_mask:     0xFFFFFFFFFFFFFFFF\n  table_id:        0\n  command:         68\n  idle_timeout:    0x5555\n  hard_timeout:    0x6666\n  priority:        0x7777\n  buffer_id:       0x88888888\n  out_port:        0x00009999\n  out_group:       0x00000000\n  flags:           0xBBBB\n  match:           \n    - type:            OFB_IN_PORT\n      value:           52428\n    - type:            OFB_ETH_TYPE\n      value:           2048\n    - type:            OFB_IPV4_DST\n      value:           192.168.1.1\n  instructions:    \n    - type:            OFPIT_APPLY_ACTIONS\n      value:           \n        - action:          OFPAT_SET_FIELD\n          type:            OFB_IPV4_DST\n          value:           192.168.2.1\n...\n";
    
    auto s = HexToRawData(hex);

    Message msg{s.data(), s.size()};
    msg.transmogrify();
    EXPECT_EQ(0xA0, msg.size());
    EXPECT_HEX("010E00A0000000011111111111111111FFFFFFFFFFFFFFFF0044555566667777888888880000999900000000BBBB0000000000580000CCCC000003F600000000000000000000000000000000000000000000000000000000080000000000000000000000C0A80101FFFFFFFF0000000000000000000000000000000000000000000000000000000000040018000000000019001080001804C0A8020100000000", msg.data(), msg.size());

    Decoder decoder{&msg};

    EXPECT_EQ("", decoder.error());
    EXPECT_EQ(yaml, decoder.result());

    Encoder encoder{decoder.result()};

    EXPECT_EQ("", encoder.error());
    EXPECT_HEX(hex, encoder.data(), encoder.size());
}

TEST(decoder, flowmod1_2)
{
    testDecodeEncode("010E00580000000100303F8ECCCC0000000000000000000000000000000008000001000000000000C0A8010100DD000011111111111111110044555566667777888888889999BBBB000A0008EE000000000B000800000000", "---\ntype:            OFPT_FLOW_MOD\nxid:             0x00000001\nversion:         1\nmsg:             \n  cookie:          0x1111111111111111\n  cookie_mask:     0xFFFFFFFFFFFFFFFF\n  table_id:        0\n  command:         68\n  idle_timeout:    0x5555\n  hard_timeout:    0x6666\n  priority:        0x7777\n  buffer_id:       0x88888888\n  out_port:        0x00009999\n  out_group:       0x00000000\n  flags:           0xBBBB\n  match:           \n    - type:            OFB_IN_PORT\n      value:           52428\n    - type:            OFB_ETH_TYPE\n      value:           2048\n    - type:            OFB_IP_PROTO\n      value:           1\n    - type:            OFB_IPV4_DST\n      value:           192.168.1.1\n    - type:            OFB_ICMPV4_TYPE\n      value:           221\n  instructions:    \n    - type:            OFPIT_APPLY_ACTIONS\n      value:           \n        - action:          OFPAT_SET_FIELD\n          type:            OFB_ICMPV4_CODE\n          value:           238\n        - action:          OFPAT_COPY_TTL_OUT\n...\n");
}

TEST(decoder, packetinv4)
{
    testDecodeEncode("040A0064000000013333333344440188999999999999999900010020 800000045555555580000204666666668000040877777777777777770000FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002", "---\ntype:            OFPT_PACKET_IN\nxid:             0x00000001\nversion:         4\nmsg:             \n  buffer_id:       858993459\n  total_len:       17476\n  in_port:         1431655765\n  in_phy_port:     1717986918\n  metadata:        8608480567731124087\n  reason:          OFPR_ACTION\n  table_id:        136\n  cookie:          11068046444225730969\n  enet_frame:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002\n...\n");
}

TEST(decoder, packetinv1)
{
    testDecodeEncode("010A003C0000000233333333444455550100FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002", "---\ntype:            OFPT_PACKET_IN\nxid:             0x00000002\nversion:         1\nmsg:             \n  buffer_id:       858993459\n  total_len:       17476\n  in_port:         21845\n  in_phy_port:     0\n  metadata:        0\n  reason:          OFPR_ACTION\n  table_id:        0\n  cookie:          0\n  enet_frame:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002\n...\n");
}

TEST(decoder, packetoutv4) 
{
    #if 0
    const char *hex = "040D00620000000133333333444444440020000000000000000000100000000500140000000000000019001080001804C0A8010100000000FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002";
    const char *yaml = "---\ntype:            OFPT_PACKET_OUT\nxid:             0x00000001\nversion:         4\nmsg:             \n  buffer_id:       858993459\n  in_port:         1145324612\n  actions:         \n    - action:          OFPAT_OUTPUT\n      port:            5\n      maxlen:          20\n    - action:          OFPAT_SET_FIELD\n      type:            OFB_IPV4_DST\n      value:           192.168.1.1\n  enet_frame:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002\n...\n";

    auto s = HexToRawData(hex);

    Message msg{s.data(), s.size()};
    msg.transmogrify();

    Decoder decoder{&msg};

    EXPECT_EQ("", decoder.error());
    EXPECT_EQ(yaml, decoder.result());

    Encoder encoder{decoder.result()};

    EXPECT_EQ("", encoder.error());
    EXPECT_HEX(hex, encoder.data(), encoder.size());
#endif
    testDecodeEncode("040D00620000000133333333444444440020000000000000000000100000000500140000000000000019001080001804C0A8010100000000FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002", "---\ntype:            OFPT_PACKET_OUT\nxid:             0x00000001\nversion:         4\nmsg:             \n  buffer_id:       858993459\n  in_port:         1145324612\n  actions:         \n    - action:          OFPAT_OUTPUT\n      port:            5\n      maxlen:          20\n    - action:          OFPAT_SET_FIELD\n      type:            OFB_IPV4_DST\n      value:           192.168.1.1\n  enet_frame:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002\n...\n");
}

TEST(decoder, packetoutv1)
{
   const char *hex = "010D004A000000013333333344440010000000080005001400070008C0A80101FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002";
    const char *yaml = "---\ntype:            OFPT_PACKET_OUT\nxid:             0x00000001\nversion:         1\nmsg:             \n  buffer_id:       858993459\n  in_port:         17476\n  actions:         \n    - action:          OFPAT_OUTPUT\n      port:            5\n      maxlen:          20\n    - action:          OFPAT_SET_FIELD\n      type:            OFB_IPV4_DST\n      value:           192.168.1.1\n  enet_frame:      FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002\n...\n";

    auto s = HexToRawData(hex);

    Message msg{s.data(), s.size()};
    msg.transmogrify();
    EXPECT_HEX("010D00620000000133333333000044440020000000000000000000100000000500140000000000000019001080001804C0A8010100000000FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002", msg.data(), msg.size());

    Decoder decoder{&msg};

    EXPECT_EQ("", decoder.error());
    EXPECT_EQ(yaml, decoder.result());

    Encoder encoder{decoder.result()};

    EXPECT_EQ("", encoder.error());
    EXPECT_HEX(hex, encoder.data(), encoder.size());
    //testDecodeEncode("010D004A000000013333333344440010000000080005001400070008C0A80101FFFFFFFFFFFF000000000001080600010800060400010000000000010A0000010000000000000A000002", "");
}


TEST(decoder, setconfigv4) 
{
    testDecodeEncode("0409000C11111111AAAABBBB", "---\ntype:            OFPT_SET_CONFIG\nxid:             0x11111111\nversion:         4\nmsg:             \n  flags:           0xAAAA\n  miss_send_len:   0xBBBB\n...\n");
}
