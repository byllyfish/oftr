// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/standardmatch.h"
#include "ofp/originalmatch.h"
#include "ofp/matchbuilder.h"
#include "ofp/oxmfields.h"

using namespace ofp;
using namespace deprecated;

TEST(standardmatch, constructor) {
  StandardMatch match;

  EXPECT_EQ(OFPMT_STANDARD_LENGTH, sizeof(match));
  EXPECT_EQ(OFPMT_STANDARD, match.type);
  EXPECT_EQ(OFPMT_STANDARD_LENGTH, match.length);
  EXPECT_EQ(0, match.in_port);

  const char *hex =
      "0000005800000000000003FF0000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000";
  EXPECT_HEX(hex, &match, sizeof(match));
}

TEST(standardmatch, toOXMList) {
  StandardMatch match;

  UInt32 wc = StandardMatch::OFPFW_ALL;
  match.in_port = 5;
  wc &= ~StandardMatch::OFPFW_IN_PORT;
  match.wildcards = wc;

  OXMList list = match.toOXMList();

  EXPECT_HEX("8000 0004 00000005", list.data(), list.size());
}

TEST(standardmatch, fromOXMList) {
  std::string s = HexToRawData("8000000400000005");
  OXMRange range{s.data(), s.size()};

  StandardMatch match{range};

  EXPECT_EQ(OFPMT_STANDARD_LENGTH, sizeof(match));
  EXPECT_EQ(OFPMT_STANDARD, match.type);
  EXPECT_EQ(OFPMT_STANDARD_LENGTH, match.length);
  EXPECT_EQ(StandardMatch::OFPFW_ALL & ~StandardMatch::OFPFW_IN_PORT,
            match.wildcards);
  EXPECT_EQ(5, match.in_port);

  EXPECT_TRUE(IsMemFilled(&match.dl_src, sizeof(match) - 12, '\0'));

  OXMList list = match.toOXMList();
  EXPECT_HEX("8000000400000005", list.data(), list.size());
}

TEST(standardmatch, fromOriginalMatch) {
  OriginalMatch origMatch;

  auto data = HexToRawData(
      "0010001F0000000000000000000000000000000000000000000"
      "00000000000000000000000000000");
  EXPECT_EQ(sizeof(origMatch), data.length());
  std::memcpy(&origMatch, data.data(), data.length());

  StandardMatch stdMatch{origMatch};
  EXPECT_HEX(
      "00000058000000000000030F00000000000000000000000000000000000000000"
      "0000000000000000000000000000000FFFFFFFF00000000FFFFFFFF0000000000"
      "0000000000000000000000000000000000000000000000",
      &stdMatch, sizeof(stdMatch));
}

TEST(standardmatch, oxm2) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{0xCCCCCCCC});
  match.add(OFB_IPV4_DST{IPv4Address{"192.168.1.1"}});

  StandardMatch stdMatch{match.toRange()};
  EXPECT_HEX(
      "00000058CCCCCCCC000003F600000000000000000000000000000000000000000"
      "000000000000000080000000000000000000000C0A80101FFFFFFFF0000000000"
      "0000000000000000000000000000000000000000000000",
      &stdMatch, sizeof(stdMatch));
}

TEST(standardmatch, vlan_vid) {
  // Test interpretation of vlan_vid field when converting from oxmrange.

  // Packets with *or* without any vlan tag.
  {
    MatchBuilder match;

    StandardMatch stdMatch{match.toRange()};
    EXPECT_EQ(0, stdMatch.dl_vlan);
    EXPECT_EQ(1023, stdMatch.wildcards);

    OXMList oxm = stdMatch.toOXMList();
    EXPECT_EQ(0, oxm.size());
    EXPECT_EQ(match.toRange(), oxm.toRange());
  }

  // Only packets without a vlan tag.
  {
    MatchBuilder match;
    match.add(OFB_VLAN_VID{OFPVID_NONE});

    StandardMatch stdMatch{match.toRange()};
    EXPECT_EQ(0xffff, stdMatch.dl_vlan);
    EXPECT_EQ(1021, stdMatch.wildcards);

    OXMList oxm = stdMatch.toOXMList();
    EXPECT_EQ(match.toRange(), oxm.toRange());
  }

  // Packets with any vlan tag present
  {
    MatchBuilder match;
    match.add(OFB_VLAN_VID{OFPVID_PRESENT}, OFB_VLAN_VID{OFPVID_PRESENT});

    StandardMatch stdMatch{match.toRange()};
    EXPECT_EQ(0xfffe, stdMatch.dl_vlan);
    EXPECT_EQ(1021, stdMatch.wildcards);

    OXMList oxm = stdMatch.toOXMList();
    EXPECT_EQ(match.toRange(), oxm.toRange());
  }

  // Packets with a specific vlan tag present
  {
    MatchBuilder match;
    match.add(OFB_VLAN_VID{1 | OFPVID_PRESENT});

    StandardMatch stdMatch{match.toRange()};
    EXPECT_EQ(1, stdMatch.dl_vlan);
    EXPECT_EQ(1021, stdMatch.wildcards);

    OXMList oxm = stdMatch.toOXMList();
    EXPECT_EQ(match.toRange(), oxm.toRange());
  }
}
