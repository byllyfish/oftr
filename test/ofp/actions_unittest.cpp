// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/actions.h"
#include "ofp/actionlist.h"
#include "ofp/oxmfields.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(actions, AT_COPY_TTL_OUT) {
  AT_COPY_TTL_OUT act;
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_COPY_TTL_OUT::type().length());

  auto expected = HexToRawData("000B 0008 0000 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_COPY_TTL_IN) {
  AT_COPY_TTL_IN act;
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_COPY_TTL_IN::type().length());

  auto expected = HexToRawData("000C 0008 0000 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_DEC_MPLS_TTL) {
  AT_DEC_MPLS_TTL act;
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_DEC_MPLS_TTL::type().length());

  auto expected = HexToRawData("0010 0008 0000 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_POP_VLAN) {
  AT_POP_VLAN act;
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_POP_VLAN::type().length());

  auto expected = HexToRawData("0012 0008 0000 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_POP_MPLS) {
  AT_POP_MPLS act{5};
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_POP_MPLS::type().length());

  auto expected = HexToRawData("0014 0008 0005 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_DEC_NW_TTL) {
  AT_DEC_NW_TTL act;
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_DEC_NW_TTL::type().length());

  auto expected = HexToRawData("0018 0008 0000 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_POP_PBB) {
  AT_POP_PBB act;
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_POP_PBB::type().length());

  auto expected = HexToRawData("001B 0008 0000 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_OUTPUT) {
  AT_OUTPUT act{5, 9};
  EXPECT_EQ(5, act.port());
  EXPECT_EQ(9, act.maxlen());
  EXPECT_EQ(16, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_OUTPUT::type().length());

  EXPECT_HEX("0000 0010 0000 0005 0009 000000000000", &act, sizeof(act));

  AT_OUTPUT act2{OFPP_CONTROLLER, 9};
  EXPECT_EQ(OFPP_CONTROLLER, act2.port());
  EXPECT_EQ(9, act2.maxlen());
  EXPECT_EQ(16, sizeof(act2));
  EXPECT_EQ(sizeof(act2), AT_OUTPUT::type().length());

  EXPECT_HEX("0000 0010 FFFFFFFD 0009 000000000000", &act2, sizeof(act2));
}

TEST(actions, AT_SET_MPLS_TTL) {
  AT_SET_MPLS_TTL act{5};
  EXPECT_EQ(5, act.ttl());
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_SET_MPLS_TTL::type().length());

  auto expected = HexToRawData("000F 0008 0005 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_PUSH_VLAN) {
  AT_PUSH_VLAN act{5};
  EXPECT_EQ(5, act.ethertype());
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_PUSH_VLAN::type().length());

  auto expected = HexToRawData("0011 0008 0005 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_PUSH_MPLS) {
  AT_PUSH_MPLS act{5};
  EXPECT_EQ(5, act.ethertype());
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_PUSH_MPLS::type().length());

  auto expected = HexToRawData("0013 0008 0005 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_SET_QUEUE) {
  AT_SET_QUEUE act{5};
  EXPECT_EQ(5, act.queue());
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_SET_QUEUE::type().length());

  auto expected = HexToRawData("0015 0008 0000 0005");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_GROUP) {
  AT_GROUP act{5};
  EXPECT_EQ(5, act.group());
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_GROUP::type().length());

  auto expected = HexToRawData("0016 0008 0000 0005");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_SET_NW_TTL) {
  AT_SET_NW_TTL act{5};
  EXPECT_EQ(5, act.ttl());
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_SET_NW_TTL::type().length());

  auto expected = HexToRawData("0017 0008 0500 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_PUSH_PBB) {
  AT_PUSH_PBB act{5};
  EXPECT_EQ(5, act.ethertype());
  EXPECT_EQ(8, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_PUSH_PBB::type().length());

  auto expected = HexToRawData("001A 0008 0005 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_EXPERIMENTER) {
  ByteList value{HexToRawData("11223344")};
  AT_EXPERIMENTER act{5, value};

  ActionList actions;
  actions.add(act);

  EXPECT_HEX("FFFF001000000005 1122334400000000", actions.data(),
             actions.size());
}

TEST(actions, AT_SET_FIELD_8bit) {
  AT_SET_FIELD<OFB_IP_PROTO> act{5};
  EXPECT_EQ(5, act.value());
  EXPECT_EQ(16, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_SET_FIELD<OFB_IP_PROTO>::type().length());

  auto expected = HexToRawData("0019 0010 8000 1401 05 00000000000000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_SET_FIELD_16bit) {
  AT_SET_FIELD<OFB_UDP_DST> act{5};

  EXPECT_EQ(5, act.value());
  EXPECT_EQ(16, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_SET_FIELD<OFB_UDP_DST>::type().length());

  auto expected = HexToRawData("0019 0010 8000 2002 0005 000000000000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_SET_FIELD_32bit) {
  PortNumber port{5};
  AT_SET_FIELD<OFB_IN_PORT> act{port};

  EXPECT_EQ(port, act.value());
  EXPECT_EQ(16, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_SET_FIELD<OFB_IN_PORT>::type().length());

  auto expected = HexToRawData("0019 0010 8000 0004 0000 0005 00000000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_SET_FIELD_48bit) {
  MacAddress addr{"01-02-03-04-05-06"};
  AT_SET_FIELD<OFB_ETH_DST> act{addr};

  EXPECT_EQ(addr, act.value());
  EXPECT_EQ(16, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_SET_FIELD<OFB_ETH_DST>::type().length());

  auto expected = HexToRawData("0019 0010 8000 0606 010203040506 0000");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_SET_FIELD_64bit) {
  AT_SET_FIELD<OFB_METADATA> act{5};

  EXPECT_EQ(5, act.value());
  EXPECT_EQ(16, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_SET_FIELD<OFB_METADATA>::type().length());

  auto expected = HexToRawData("0019 0010 8000 0408 0000000000000005");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}

TEST(actions, AT_SET_FIELD_128bit) {
  IPv6Address addr{"::1"};
  AT_SET_FIELD<OFB_IPV6_SRC> act{addr};

  EXPECT_EQ(addr, act.value());
  EXPECT_EQ(24, sizeof(act));
  EXPECT_EQ(sizeof(act), AT_SET_FIELD<OFB_IPV6_SRC>::type().length());

  auto expected =
      HexToRawData("0019 0018 8000 3410 00000000000000000000000000000001");
  EXPECT_EQ(0, std::memcmp(expected.data(), &act, sizeof(act)));
}
