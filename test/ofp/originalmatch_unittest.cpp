// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/originalmatch.h"
#include "ofp/matchbuilder.h"
#include "ofp/oxmfields.h"
#include "ofp/unittest.h"

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

  EXPECT_EQ(
      "in_port: 52428 \ndl_src: 00:00:00:00:00:00*\ndl_dst: "
      "00:00:00:00:00:00*\ndl_vlan: 0*\ndl_vlan_pcp: 0*\ndl_type: 2048 "
      "\nnw_tos: 0*\nnw_proto: 0*\nnw_src: 0.0.0.0/0.0.0.0\nnw_dst: "
      "192.168.1.1/255.255.255.255\ntp_src: 0*\ntp_dst: 0*\n",
      match.toString());
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

TEST(originalmatch, masks) {
  IPv4Address dst{"192.168.1.1"};
  IPv4Address src{"192.168.1.2"};
  IPv4Address mask{"255.255.255.0"};

  MatchBuilder oxmMatch;
  oxmMatch.add(OFB_IN_PORT{0xCCCCCCCC});
  oxmMatch.add(OFB_IPV4_DST{dst}, OFB_IPV4_DST{mask});
  oxmMatch.add(OFB_IPV4_SRC{src}, OFB_IPV4_SRC{mask});
  oxmMatch.add(OFB_UDP_SRC{80});
  oxmMatch.add(OFB_UDP_DST{81});

  OriginalMatch match{oxmMatch.toRange()};

  EXPECT_HEX(
      "0032080ECCCC00000000000000000000000000000000080000110000C0A80100C0A80100"
      "00500051",
      &match, sizeof(match));
  EXPECT_EQ(
      "in_port: 52428 \ndl_src: 00:00:00:00:00:00*\ndl_dst: "
      "00:00:00:00:00:00*\ndl_vlan: 0*\ndl_vlan_pcp: 0*\ndl_type: 2048 "
      "\nnw_tos: 0*\nnw_proto: 17 \nnw_src: 192.168.1.0/255.255.255.0\nnw_dst: "
      "192.168.1.0/255.255.255.0\ntp_src: 80 \ntp_dst: 81 \n",
      match.toString());
}


TEST(originalmatch, constructor) {
  OriginalMatch match1;
  EXPECT_HEX("00000000000000000000000000000000000000000000000000000000000000000000000000000000", &match1, sizeof(match1));

  OXMRange oxm;
  OriginalMatch match2{oxm};
  EXPECT_HEX("003820FF000000000000000000000000000000000000000000000000000000000000000000000000", &match2, sizeof(match2));
}
