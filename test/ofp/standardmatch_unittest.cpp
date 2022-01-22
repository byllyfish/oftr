// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/standardmatch.h"

#include "ofp/matchbuilder.h"
#include "ofp/originalmatch.h"
#include "ofp/oxmfields.h"
#include "ofp/unittest.h"

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

static void setupMatch(StandardMatch &match, UInt16 dl_type, UInt8 nw_proto) {
  match.wildcards = 0;
  match.in_port = 11111112;
  match.dl_src = MacAddress{"00:00:00:00:00:01"};
  match.dl_src_mask = MacAddress{"FF:FF:FF:FF:FF:FF"};
  match.dl_dst = MacAddress{"00:00:00:00:00:02"};
  match.dl_dst_mask = MacAddress{"FF:FF:FF:FF:FF:FF"};
  match.dl_vlan = 2223;
  match.dl_vlan_pcp = 34;
  match.dl_type = dl_type;
  match.nw_tos = 56;
  match.nw_proto = nw_proto;
  match.nw_src = IPv4Address{"77.77.77.78"};
  match.nw_src_mask = IPv4Address{"255.255.255.255"};
  match.nw_dst = IPv4Address{"88.88.88.89"};
  match.nw_dst_mask = IPv4Address{"255.255.255.255"};
  match.tp_src = 9990;
  match.tp_dst = 8880;
  match.mpls_label = 77777777;
  match.mpls_tc = 99;
  match.metadata = 1234567890;
  match.metadata_mask = 0xFFFFFFFFFFFFFFFF;
}

TEST(standardmatch, toString) {
  StandardMatch match;
  setupMatch(match, 4445, 67);
  EXPECT_EQ(
      "in_port: 11111112 \ndl_src: "
      "00:00:00:00:00:01/ff:ff:ff:ff:ff:ff\ndl_dst: "
      "00:00:00:00:00:02/ff:ff:ff:ff:ff:ff\ndl_vlan: 2223 \ndl_vlan_pcp: 34 "
      "\ndl_type: 4445 \nnw_tos: 56 \nnw_proto: 67 \nnw_src: "
      "77.77.77.78/255.255.255.255\nnw_dst: "
      "88.88.88.89/255.255.255.255\ntp_src: 9990 \ntp_dst: 8880 \nmpls_label: "
      "77777777 \nmpls_tc: 99 \nmetadata: 1234567890/18446744073709551615\n",
      match.toString());
}

TEST(standardmatch, roundtrip_arp) {
  StandardMatch match;
  setupMatch(match, DATALINK_ARP, 1);

  OXMList oxm = match.toOXMList();
  EXPECT_HEX(
      "8000000400A98AC88000051000000000499602D2FFFFFFFFFFFFFFFF8000080600000000"
      "00018000060600000000000280000C0218AF80000E012280000A02080680002A02000180"
      "002C044D4D4D4E80002E0458585859",
      oxm.data(), oxm.size());

  StandardMatch newMatch{oxm.toRange()};
  EXPECT_EQ(
      "in_port: 11111112 \ndl_src: "
      "00:00:00:00:00:01/ff:ff:ff:ff:ff:ff\ndl_dst: "
      "00:00:00:00:00:02/ff:ff:ff:ff:ff:ff\ndl_vlan: 2223 \ndl_vlan_pcp: 34 "
      "\ndl_type: 2054 \nnw_tos: 0*\nnw_proto: 1 \nnw_src: "
      "77.77.77.78/255.255.255.255\nnw_dst: "
      "88.88.88.89/255.255.255.255\ntp_src: 0*\ntp_dst: 0*\nmpls_label: "
      "0*\nmpls_tc: 0*\nmetadata: 1234567890/18446744073709551615\n",
      newMatch.toString());
}

TEST(standardmatch, roundtrip_tcp) {
  StandardMatch match;
  setupMatch(match, DATALINK_IPV4, PROTOCOL_TCP);

  OXMList oxm = match.toOXMList();
  EXPECT_HEX(
      "8000000400A98AC88000051000000000499602D2FFFFFFFFFFFFFFFF8000080600000000"
      "00018000060600000000000280000C0218AF80000E012280000A02080080001001388000"
      "140106800016044D4D4D4E800018045858585980001A02270680001C0222B08000440404"
      "A2CB718000460163",
      oxm.data(), oxm.size());

  StandardMatch newMatch{oxm.toRange()};
  EXPECT_EQ(
      "in_port: 11111112 \ndl_src: "
      "00:00:00:00:00:01/ff:ff:ff:ff:ff:ff\ndl_dst: "
      "00:00:00:00:00:02/ff:ff:ff:ff:ff:ff\ndl_vlan: 2223 \ndl_vlan_pcp: 34 "
      "\ndl_type: 2048 \nnw_tos: 56 \nnw_proto: 6 \nnw_src: "
      "77.77.77.78/255.255.255.255\nnw_dst: "
      "88.88.88.89/255.255.255.255\ntp_src: 9990 \ntp_dst: 8880 \nmpls_label: "
      "77777777 \nmpls_tc: 99 \nmetadata: 1234567890/18446744073709551615\n",
      newMatch.toString());
}
