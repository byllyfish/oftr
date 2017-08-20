// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchbuilder.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(matchbuilder, OFB_IN_PORT) {
  MatchBuilder match;

  match.add(OFB_IN_PORT{7});

  EXPECT_HEX("8000 0004 00000007", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  // Following line should not compile; OFB_IN_PORT doesn't support a mask.
  // match.add(OFB_IN_PORT{9}, OFB_IN_PORT{0xFF});
}

TEST(matchbuilder, OFB_IN_PHY_PORT) {
  MatchBuilder match;

  match.add(OFB_IN_PORT{7});
  match.add(OFB_IN_PHY_PORT{9});

  EXPECT_HEX("8000 0004 00000007 8000-0204-00000009", match.data(),
             match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_IN_PHY_PORT_alt1) {
  MatchBuilder match;

  match.add(OFB_IN_PHY_PORT{9});

  // We should have the prereq placeholder for OFB_IN_PORT added.
  EXPECT_HEX("7FF1-0100 8000-0108-00000000-00000000 8000-0204-00000009",
             match.data(), match.size());
  EXPECT_FALSE(match.validate());

  match.add(OFB_IN_PORT{7});

  // The prereq for OFB_IN_PHY_PORT should replace the placeholder.
  EXPECT_HEX("8000-0004-00000007 8000-0204-00000009", match.data(),
             match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_VLAN_VID) {
  MatchBuilder match;

  // N.B. We do not set OFPVID_PRESENT.
  match.add(OFB_VLAN_VID{15});
  EXPECT_HEX("8000-0C02-000F", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_VLAN_PCP{1});

  // The current VLAN_ID does not satisfy the prerequisite; it is missing the
  // OFPVID_PRESENT bit. Therefore, we should see the prerequisite fail to
  // match the OFB_VLAN_VID oxm -- there should be a placeholder for the preq.

  EXPECT_HEX("8000-0C02-000F 7FF1-0100 8000-0D04-10001000 8000-0E01-01",
             match.data(), match.size());
  EXPECT_FALSE(match.validate());
}

TEST(matchbuilder, OFB_VLAN_VID_alt1) {
  MatchBuilder match;

  match.add(OFB_VLAN_VID{15 | OFPVID_PRESENT});
  EXPECT_HEX("8000-0C02-100F", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_VLAN_PCP{1});
  EXPECT_HEX("8000-0C02-100F 8000-0E01-01", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_VLAN_VID_alt2) {
  MatchBuilder match;

  match.add(OFB_VLAN_PCP{2});
  EXPECT_HEX("7FF1-0100 8000-0D04-10001000 8000-0E01-02", match.data(),
             match.size());
  EXPECT_FALSE(match.validate());

  match.add(OFB_VLAN_VID{15 | OFPVID_PRESENT});
  EXPECT_HEX("8000-0C02-100F 8000-0E01-02", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_VLAN_VID_alt3) {
  MatchBuilder match;

  match.add(OFB_VLAN_PCP{2});
  EXPECT_HEX("7FF1-0100 8000-0D04-10001000 8000-0E01-02", match.data(),
             match.size());
  EXPECT_FALSE(match.validate());

  match.add(OFB_VLAN_VID{15});

  // As in the original test for OFB_VLAN_VID, the VLAN_VID field does not
  // satisfy the prerequisite, so the placeholder is appended.

  EXPECT_HEX("7FF1-0100 8000-0D04-10001000 8000-0E01-02 8000-0C02-000F",
             match.data(), match.size());
  EXPECT_FALSE(match.validate());

  match.add(OFB_VLAN_VID{5 | OFPVID_PRESENT});

  EXPECT_HEX("8000-0C02-1005 8000-0E01-02 7FF3-0100 8000-0C02-000F",
             match.data(), match.size());
  EXPECT_FALSE(match.validate());
}

TEST(matchbuilder, OFB_IP_DSCP) {
  MatchBuilder match;

  match.add(OFB_IP_DSCP{1});
  EXPECT_HEX("7FF2-0100 8000-0A02-0800 8000-0A02-86DD 8000-1001-01",
             match.data(), match.size());
  EXPECT_FALSE(match.validate());

  match.add(OFB_ETH_TYPE{0x0800});

  EXPECT_HEX("8000-0A02-0800 8000-1001-01", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_IP_DSCP_alt1) {
  MatchBuilder match;

  match.add(OFB_ETH_TYPE{0x0800});
  EXPECT_HEX("8000-0A02-0800", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_IP_DSCP{1});
  EXPECT_HEX("8000-0A02-0800 8000-1001-01", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_IP_DSCP_alt2) {
  MatchBuilder match;

  match.add(OFB_ETH_TYPE{0x9000});
  EXPECT_HEX("8000-0A02-9000", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_IP_DSCP{1});
  EXPECT_HEX(
      "8000-0A02-9000 7FF2-0100 8000-0A02-0800 8000-0A02-86DD 8000-1001-01",
      match.data(), match.size());
  EXPECT_FALSE(match.validate());
}

TEST(matchbuilder, OFB_TCP_SRC) {
  MatchBuilder match;

  match.add(OFB_ETH_TYPE{0x0800});
  EXPECT_HEX("[8000-0A02-0800]", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_IP_PROTO{6});
  EXPECT_HEX("8000-0A02-0800 [8000-1401-06]", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_TCP_SRC{80});
  EXPECT_HEX("8000-0A02-0800 8000-1401-06 [8000-1A02-0050]", match.data(),
             match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_UDP_SRC{80});
  // The UDP_SRC field conflicts with the TCP_SRC field.
  EXPECT_HEX(
      "8000-0A02-0800 8000-1401-06 8000-1A02-0050 [7FF3-0100 8000-1E02-0050]",
      match.data(), match.size());
  EXPECT_FALSE(match.validate());

  match.add(OFB_IP_PROTO{17});
  // Attempting to specify the UDP protocol for IP_PROTO now poisons the first
  // IP_PROTO field.
  EXPECT_HEX(
      "8000-0A02-0800 [7FF3-0100] 8000-1401-06 8000-1A02-0050 7FF3-0100 "
      "8000-1E02-0050",
      match.data(), match.size());
  EXPECT_FALSE(match.validate());
}

TEST(matchbuilder, OFB_TCP_SRC_alt1) {
  MatchBuilder match;

  match.add(OFB_TCP_SRC{80});
  EXPECT_HEX(
      "[(7FF2-0100 8000-0A02-0800 8000-0A02-86DD) 8000-1401-06 8000-1A02-0050]",
      match.data(), match.size());
  EXPECT_FALSE(match.validate());

  match.add(OFB_UDP_SRC{80});
  EXPECT_HEX(
      "(7FF2-0100 8000-0A02-0800 8000-0A02-86DD) 8000-1401-06 "
      "8000-1A02-0050 [7FF3-01-00] 8000-1E02-0050",
      match.data(), match.size());
  EXPECT_FALSE(match.validate());

  match.add(OFB_ETH_TYPE{0x0800});
  EXPECT_HEX(
      "[8000-0A02-0800] 8000-1401-06 8000-1A02-0050 7FF3-01-00 8000-1E02-0050",
      match.data(), match.size());
  EXPECT_FALSE(match.validate());

  match.add(OFB_IP_PROTO{6});
  EXPECT_HEX(
      "8000-0A02-0800 8000-1401-06 8000-1A02-0050 7FF3-01-00 8000-1E02-0050",
      match.data(), match.size());
  EXPECT_FALSE(match.validate());
}

TEST(matchbuilder, OFB_TCP_SRC_alt2) {
  MatchBuilder match;

  match.add(OFB_ETH_TYPE{0x86DD});
  EXPECT_HEX("[8000-0A02-86DD]", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_TCP_SRC{80});
  EXPECT_HEX("8000-0A02-86DD [8000-1401-06 8000-1A02-0050]", match.data(),
             match.size());
  EXPECT_TRUE(match.validate());

  // Adding IP_PROTO{TCP} field shouldn't change anything.
  match.add(OFB_IP_PROTO{6});
  EXPECT_HEX("8000-0A02-86DD 8000-1401-06 8000-1A02-0050", match.data(),
             match.size());
  EXPECT_TRUE(match.validate());

  // Adding IP_PROTO{UDP} field should now insert poison.
  match.add(OFB_IP_PROTO{17});
  EXPECT_HEX("8000-0A02-86DD [7FF3-0100] 8000-1401-06 8000-1A02-0050",
             match.data(), match.size());
  EXPECT_FALSE(match.validate());
}

TEST(matchbuilder, OFB_IN_PORT_alt1) {
  MatchBuilder match;

  match.add(OFB_IN_PORT{7});
  EXPECT_HEX("[8000-0004-0000-0007]", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  // Adding it again shouldn't change anything.
  match.add(OFB_IN_PORT{7});
  EXPECT_HEX("8000-0004-0000-0007", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  // Adding a different value should insert poison.
  match.add(OFB_IN_PORT{9});
  EXPECT_HEX("[7FF3-0100] 8000-0004-0000-0007", match.data(), match.size());
  EXPECT_FALSE(match.validate());
}

TEST(matchbuilder, OFB_UDP_SRC_masked) {
  MatchBuilder match;
  match.add(OFB_ETH_TYPE{0x0800});
  match.add(OFB_UDP_SRC{1024}, OFB_UDP_SRC{1024});
  EXPECT_HEX("80000A02:0800 80001401:11 80001F04:0400:0400", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFB_ETH_SRC) {
  MatchBuilder match;

  match.add(OFB_ETH_DST{MacAddress{"00:11:22:33:44:55"}});
  EXPECT_HEX("80000606001122334455", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFP_ETH_SRC_mask) {
  MatchBuilder match;

  match.add(OFB_ETH_DST{MacAddress{"00:11:22:33:44:55"}},
            OFB_ETH_DST{MacAddress{"ff:ff:ff:00:00:00"}});
  EXPECT_HEX("8000070C001122000000FFFFFF000000", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, test_placeholders_1) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  EXPECT_HEX("800000040000001B", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_TCP_SRC{80});

  EXPECT_HEX(
      "800000040000001B7FF2010080000A02080080000A0286DD800014010680001A020050",
      match.data(), match.size());
  EXPECT_TRUE(!match.validate());

  match.add(OFB_ETH_TYPE{0x0800});
  EXPECT_HEX("800000040000001B80000A020800800014010680001A020050", match.data(),
             match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, test_placeholders_2) {
  MatchBuilder match;
  match.add(OFB_TCP_SRC{80});

  EXPECT_HEX("7FF2010080000A02080080000A0286DD800014010680001A020050",
             match.data(), match.size());
  EXPECT_TRUE(!match.validate());

  match.add(OFB_ETH_TYPE{0x0800});
  EXPECT_HEX("80000A020800800014010680001A020050", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_IN_PORT{27});

  EXPECT_HEX("80000A020800800014010680001A020050800000040000001B", match.data(),
             match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, addOrdered) {
  MatchBuilder match;

  match.addOrderedUnchecked(OFB_TCP_SRC{80});
  match.addOrderedUnchecked(OFB_ETH_TYPE{0x0800});
  match.addOrderedUnchecked(OFB_IN_PORT{27});

  EXPECT_HEX("800000040000001B 80000A020800 80001A020050", match.data(),
             match.size());

  match.clear();

  match.addOrderedUnchecked(OFB_ETH_TYPE{0x0800});
  match.addOrderedUnchecked(OFB_IN_PORT{27});
  match.addOrderedUnchecked(OFB_TCP_SRC{80});

  EXPECT_HEX("800000040000001B 80000A020800 80001A020050", match.data(),
             match.size());
}

TEST(matchbuilder, icmpv6Type) {
  MatchBuilder match;

  match.add(OFB_ICMPV6_TYPE{143});

  EXPECT_HEX("80000A0286DD 800014013A 80003A018F", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, outOfOrder) {
  MatchBuilder match;

  match.add(OFB_TCP_DST{80});
  match.add(OFB_ETH_TYPE{0x0800});

  EXPECT_HEX("80000A020800800014010680001C020050", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, experimenter) {
  MatchBuilder match;

  match.add(X_LLDP_CHASSIS_ID{ByteRange{"lldp", 4}});

  EXPECT_HEX(
      "FFFF024400FFFFFF046C6C64700000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000000000000000000"
      "0",
      match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, randomOrder) {
  // OpenFlow Spec 1.3.5:  7.2.3.6 Flow Match Field Prerequisite
  // "An OXM TLV that has prerequisite restrictions must appear after the OXM
  // TLVs for its prerequisites. Ordering of OXM TLVs within an OpenFlow match
  // is not otherwise constrained."

  // OpenFlow Spec 1.5.1:  7.2.3.6 Flow Match Field Prerequisite
  // "An OXM TLV that has prerequisite restrictions must appear after the OXM
  // TLVs for its prerequisites. Ordering of OXM TLVs within an OpenFlow match
  // is not otherwise constrained (apart from the Packet Type Match Field)."

  MatchBuilder match;

  match.add(OFB_ICMPV4_TYPE{0});
  match.add(OFB_ETH_DST{MacAddress{"0e:33:09:3b:f8:93"}});
  match.add(OFB_IPV4_DST{IPv4Address{"10.0.0.2"}});
  match.add(OFB_ICMPV4_CODE{0});
  match.add(OFB_IPV4_SRC{IPv4Address{"10.10.10.1"}});
  match.add(OFB_IP_PROTO{1});
  match.add(OFB_ETH_SRC{MacAddress{"0e:00:00:00:00:01"}});
  match.add(OFB_ETH_TYPE{2048});

  EXPECT_HEX(
      "80000A02:0800 80001401:01 80002601:00 80000606:0E33093BF893 "
      "80001804:0A000002 80002801:00 80001604:0A0A0A01 80000806:0E0000000001",
      match.data(), match.size());
  EXPECT_TRUE(match.validate());
}
