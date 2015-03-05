// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/originalmatch.h"
#include "ofp/matchbuilder.h"
#include "ofp/oxmfields.h"

using namespace ofp;
using namespace deprecated;

TEST(originalmatch, oxmrange) {
  MatchBuilder oxmMatch;
  oxmMatch.add(OFB_IN_PORT{0xCCCCCCCC});
  oxmMatch.add(OFB_IPV4_DST{IPv4Address{"192.168.1.1"}});

  OriginalMatch match{oxmMatch.toRange()};

  EXPECT_HEX(
      "003020EECCCC0000000000000000000000000000000008000000000000000000C"
      "0A8010100000000",
      &match, sizeof(match));
}

TEST(originalmatch, vlan_vid) {
  // Test interpretation of vlan_vid field when converting from oxmrange.

  // Packets with *or* without any vlan tag.
  {
    MatchBuilder match;

    OriginalMatch origMatch{match.toRange()};
    EXPECT_EQ(0, origMatch.dl_vlan);
    EXPECT_EQ(0x3820ff, origMatch.wildcards);

    StandardMatch stdMatch{origMatch};
    OXMList oxm = stdMatch.toOXMList();
    EXPECT_EQ(0, oxm.size());
    EXPECT_EQ(match.toRange(), oxm.toRange());
  }

  // Only packets without a vlan tag.
  {
    MatchBuilder match;
    match.add(OFB_VLAN_VID{OFPVID_NONE});

    OriginalMatch origMatch{match.toRange()};
    EXPECT_EQ(0xffff, origMatch.dl_vlan);
    EXPECT_EQ(0x3820fd, origMatch.wildcards);

    StandardMatch stdMatch{origMatch};
    OXMList oxm = stdMatch.toOXMList();
    EXPECT_EQ(match.toRange(), oxm.toRange());
  }

  // Packets with any vlan tag present
  {
    MatchBuilder match;
    match.add(OFB_VLAN_VID{OFPVID_PRESENT}, OFB_VLAN_VID{OFPVID_PRESENT});

    OriginalMatch origMatch{match.toRange()};
    EXPECT_EQ(0xfffe, origMatch.dl_vlan);
    EXPECT_EQ(0x3820fd, origMatch.wildcards);

    StandardMatch stdMatch{origMatch};
    OXMList oxm = stdMatch.toOXMList();
    EXPECT_EQ(match.toRange(), oxm.toRange());
  }

  // Packets with a specific vlan tag present
  {
    MatchBuilder match;
    match.add(OFB_VLAN_VID{1 | OFPVID_PRESENT});

    OriginalMatch origMatch{match.toRange()};
    EXPECT_EQ(1, origMatch.dl_vlan);
    EXPECT_EQ(0x3820fd, origMatch.wildcards);

    StandardMatch stdMatch{origMatch};
    OXMList oxm = stdMatch.toOXMList();
    EXPECT_EQ(match.toRange(), oxm.toRange());
  }
}
