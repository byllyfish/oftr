// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/matchbuilder.h"

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

  // FIXME: should we allow VID to be set without OFPVID_PRESENT?
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

  //"80000C021005 80000E0102 80000C02000F"
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
  EXPECT_HEX("8000-0A02-0800 [7FF3-0100] 8000-1401-06 8000-1A02-0050 7FF3-0100 "
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
  EXPECT_HEX("(7FF2-0100 8000-0A02-0800 8000-0A02-86DD) 8000-1401-06 "
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


TEST(matchbuilder, OFB_ETH_SRC) {
  MatchBuilder match;

  match.add(OFB_ETH_DST{EnetAddress{"00-11-22-33-44-55"}});
  EXPECT_HEX("80000606001122334455", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, OFP_ETH_SRC_mask) {
  MatchBuilder match;

  match.add(OFB_ETH_DST{EnetAddress{"00-11-22-33-44-55"}}, OFB_ETH_DST{EnetAddress{"ff-ff-ff-00-00-00"}});
  EXPECT_HEX("8000070C001122334455FFFFFF000000", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, test_placeholders_1) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{27});

  EXPECT_HEX("800000040000001B", match.data(), match.size());
  EXPECT_TRUE(match.validate());
  
  match.add(OFB_TCP_SRC{80});

  EXPECT_HEX("800000040000001B7FF2010080000A02080080000A0286DD800014010680001A020050", match.data(), match.size());
  EXPECT_TRUE(!match.validate());

  match.add(OFB_ETH_TYPE{0x0800});
  EXPECT_HEX("800000040000001B80000A020800800014010680001A020050", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}

TEST(matchbuilder, test_placeholders_2) {
  MatchBuilder match;
  match.add(OFB_TCP_SRC{80});

  EXPECT_HEX("7FF2010080000A02080080000A0286DD800014010680001A020050", match.data(), match.size());
  EXPECT_TRUE(!match.validate());

  match.add(OFB_ETH_TYPE{0x0800});
  EXPECT_HEX("80000A020800800014010680001A020050", match.data(), match.size());
  EXPECT_TRUE(match.validate());

  match.add(OFB_IN_PORT{27});

  EXPECT_HEX("80000A020800800014010680001A020050800000040000001B", match.data(), match.size());
  EXPECT_TRUE(match.validate());
}
