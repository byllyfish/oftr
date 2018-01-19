// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchpacketbuilder.h"
#include "ofp/matchpacket.h"  // for roundtrip testing
#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"
#include "ofp/pkt.h"  // for pkt::Checksum
#include "ofp/unittest.h"

using namespace ofp;

TEST(matchpacketbuilder, empty) {
  OXMList oxm;
  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {"a1b2", 4});

  EXPECT_HEX(
      "000000000000000000000000000061316232000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000",
      data.data(), data.size());
}

TEST(matchpacketbuilder, arp) {
  MacAddress ethSrc{"00:00:00:00:00:01"};
  MacAddress ethDst{"00:00:00:00:00:02"};
  IPv4Address src{"192.168.1.1"};
  IPv4Address dst{"192.168.1.2"};

  OXMList oxm;
  oxm.add(OFB_ETH_DST{ethDst});
  oxm.add(OFB_ETH_SRC{ethSrc});
  oxm.add(OFB_ETH_TYPE{DATALINK_ARP});
  oxm.add(OFB_ARP_OP{1});
  oxm.add(OFB_ARP_SPA{src});
  oxm.add(OFB_ARP_TPA{dst});
  oxm.add(OFB_ARP_SHA{ethSrc});
  oxm.add(OFB_ARP_THA{ethDst});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {});

  EXPECT_HEX(
      "00000000000200000000000108060001080006040001000000000001C0A8010100000000"
      "0002C0A80102000000000000000000000000000000000000",
      data.data(), data.size());

  // N.B. This test depends on field ordering being the same.
  MatchPacket decode{data};
  EXPECT_EQ(oxm.toRange(), decode.toRange());
}

TEST(matchpacketbuilder, vlan_arp) {
  MacAddress ethSrc{"00:00:00:00:00:01"};
  MacAddress ethDst{"00:00:00:00:00:02"};
  IPv4Address src{"192.168.1.1"};
  IPv4Address dst{"192.168.1.2"};

  OXMList oxm;
  oxm.add(OFB_ETH_DST{ethDst});
  oxm.add(OFB_ETH_SRC{ethSrc});
  oxm.add(OFB_VLAN_VID{OFPVID_PRESENT | 0x0001});
  oxm.add(OFB_VLAN_PCP{0x7});
  oxm.add(OFB_ETH_TYPE{DATALINK_ARP});
  oxm.add(OFB_ARP_OP{1});
  oxm.add(OFB_ARP_SPA{src});
  oxm.add(OFB_ARP_TPA{dst});
  oxm.add(OFB_ARP_SHA{ethSrc});
  oxm.add(OFB_ARP_THA{ethDst});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {});

  EXPECT_HEX(
      "0000000000020000000000018100E00108060001080006040001000000000001C0A80101"
      "000000000002C0A801020000000000000000000000000000",
      data.data(), data.size());

  // N.B. This test depends on field ordering being the same.
  MatchPacket decode{data};
  EXPECT_EQ(oxm.toRange(), decode.toRange());
}

TEST(matchpacketbuilder, lldp) {
  MacAddress ethSrc{"00:00:00:00:00:01"};
  MacAddress ethDst{"00:00:00:00:00:02"};

  OXMList oxm;
  oxm.add(OFB_ETH_DST{ethDst});
  oxm.add(OFB_ETH_SRC{ethSrc});
  oxm.add(OFB_ETH_TYPE{DATALINK_LLDP});

  LLDPValue<LLDPType::ChassisID> chassisID;
  EXPECT_TRUE(chassisID.parse("chassis 001122334455"));
  oxm.add(X_LLDP_CHASSIS_ID{chassisID});

  LLDPValue<LLDPType::PortID> portID;
  EXPECT_TRUE(portID.parse("port 0102"));
  oxm.add(X_LLDP_PORT_ID{portID});

  oxm.add(X_LLDP_TTL{0x45});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {});

  EXPECT_HEX(
      "00000000000200000000000188CC02070100112233445504030201020602004500000000"
      "000000000000000000000000000000000000000000000000",
      data.data(), data.size());

  MatchPacket decode{data};
  OXMRange range = decode.toRange();
  EXPECT_HEX(
      "800006060000000000028000080600000000000180000A0288CCFFFF024400FFFFFF0701"
      "001122334455000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000FFFF044400FFFFFF0302"
      "010200000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000FFFF060600FFFFFF0045"
      "FFFF080600FFFFFF0022",
      range.data(), range.size());
}

TEST(matchpacketbuilder, minimal_arp) {
  OXMList oxm;
  oxm.add(OFB_ETH_TYPE{DATALINK_ARP});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {});

  EXPECT_HEX(
      "000000000000000000000000080600010800060400000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000",
      data.data(), data.size());

  MatchPacket decode{data};
  OXMRange range = decode.toRange();
  EXPECT_HEX(
      "800006060000000000008000080600000000000080000A02080680002A02000080002C04"
      "0000000080002E04000000008000300600000000000080003206000000000000",
      range.data(), range.size());
}

TEST(matchpacketbuilder, minimal_lldp) {
  OXMList oxm;
  oxm.add(OFB_ETH_TYPE{DATALINK_LLDP});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {});

  EXPECT_HEX(
      "00000000000000000000000088CC02000400060200000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000",
      data.data(), data.size());

  MatchPacket decode{data};
  OXMRange range = decode.toRange();
  EXPECT_HEX(
      "800006060000000000008000080600000000000080000A0288CCFFFF024400FFFFFF0000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000FFFF044400FFFFFF0000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000FFFF060600FFFFFF0000"
      "FFFF080600FFFFFF0018",
      range.data(), range.size());
}

TEST(matchpacketbuilder, icmpv4) {
  MacAddress ethSrc{"00:00:00:00:00:01"};
  MacAddress ethDst{"00:00:00:00:00:02"};
  IPv4Address src{"192.168.1.1"};
  IPv4Address dst{"192.168.1.2"};

  OXMList oxm;
  oxm.add(OFB_ETH_DST{ethDst});
  oxm.add(OFB_ETH_SRC{ethSrc});
  oxm.add(OFB_ETH_TYPE{DATALINK_IPV4});
  oxm.add(OFB_IPV4_SRC{src});
  oxm.add(OFB_IPV4_DST{dst});
  oxm.add(OFB_IP_PROTO{PROTOCOL_ICMP});
  oxm.add(OFB_ICMPV4_TYPE{0});
  oxm.add(OFB_ICMPV4_CODE{0});

  ByteList payload{HexToRawData("0102030405")};

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, payload.toRange());

  EXPECT_HEX(
      "00000000000200000000000108004500001D000000004001F78CC0A80101C0A801020000"
      "F6F901020304050000000000000000000000000000000000",
      data.data(), data.size());

  ByteRange icmp = SafeByteRange(data.data(), data.size(), 34);
  EXPECT_EQ(0, pkt::Checksum(icmp));

  MatchPacket decode{data};
  OXMRange range = decode.toRange();
  EXPECT_HEX(
      "800006060000000000028000080600000000000180000A020800800014010180001604C0"
      "A8010180001804C0A8010200013A014080002601008000280100FFFF080600FFFFFF002"
      "6",
      range.data(), range.size());
}

TEST(matchpacketbuilder, icmpv4_2) {
  MacAddress ethSrc{"00:00:00:00:00:01"};
  MacAddress ethDst{"00:00:00:00:00:02"};
  IPv4Address src{"192.168.0.3"};
  IPv4Address dst{"192.168.0.1"};

  OXMList oxm;
  oxm.add(OFB_ETH_DST{ethDst});
  oxm.add(OFB_ETH_SRC{ethSrc});
  oxm.add(OFB_ETH_TYPE{DATALINK_IPV4});
  oxm.add(OFB_IPV4_SRC{src});
  oxm.add(OFB_IPV4_DST{dst});
  oxm.add(OFB_IP_PROTO{PROTOCOL_ICMP});
  oxm.add(OFB_ICMPV4_TYPE{8});
  oxm.add(OFB_ICMPV4_CODE{0});

  ByteList payload{HexToRawData("7832 0000 00")};

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, payload.toRange());

  EXPECT_HEX(
      "00000000000200000000000108004500001D000000004001F98BC0A80003C0A800010800"
      "7FCD78320000000000000000000000000000000000000000",
      data.data(), data.size());

  ByteRange icmp = SafeByteRange(data.data(), data.size(), 34);
  EXPECT_EQ(0, pkt::Checksum(icmp));
}

TEST(matchpacketbuilder, icmpv6_nd_solicit) {
  MacAddress ethSrc{"00:00:00:00:00:01"};
  MacAddress ethDst{"00:00:00:00:00:02"};
  IPv6Address src{"2000::1"};
  IPv6Address dst{"2000::2"};

  OXMList oxm;
  oxm.add(OFB_ETH_DST{ethDst});
  oxm.add(OFB_ETH_SRC{ethSrc});
  oxm.add(OFB_ETH_TYPE{DATALINK_IPV6});
  oxm.add(OFB_IPV6_SRC{src});
  oxm.add(OFB_IPV6_DST{dst});
  oxm.add(OFB_IP_PROTO{PROTOCOL_ICMPV6});
  oxm.add(OFB_ICMPV6_TYPE{ICMPV6_TYPE_NEIGHBOR_SOLICIT});
  oxm.add(OFB_ICMPV6_CODE{0});
  oxm.add(OFB_IPV6_ND_TARGET{dst});
  oxm.add(OFB_IPV6_ND_SLL{ethSrc});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {});

  EXPECT_HEX(
      "00000000000200000000000186DD6000000000203A402000000000000000000000000000"
      "0001200000000000000000000000000000028700179E0000000020000000000000000000"
      "0000000000020101000000000001",
      data.data(), data.size());

  MatchPacket decode{data};
  OXMRange range = decode.toRange();
  EXPECT_HEX(
      "800006060000000000028000080600000000000180000A0286DD00013A0140800014013A"
      "800034102000000000000000000000000000000180003610200000000000000000000000"
      "0000000280003A018780003C010080003E10200000000000000000000000000000028000"
      "400600000000000180004E020001",
      range.data(), range.size());
}

TEST(matchpacketbuilder, icmpv6_nd_advertise) {
  MacAddress ethSrc{"00:00:00:00:00:01"};
  MacAddress ethDst{"00:00:00:00:00:02"};
  IPv6Address src{"2000::1"};
  IPv6Address dst{"2000::2"};

  OXMList oxm;
  oxm.add(OFB_ETH_DST{ethDst});
  oxm.add(OFB_ETH_SRC{ethSrc});
  oxm.add(OFB_ETH_TYPE{DATALINK_IPV6});
  oxm.add(OFB_IPV6_SRC{src});
  oxm.add(OFB_IPV6_DST{dst});
  oxm.add(OFB_IP_PROTO{PROTOCOL_ICMPV6});
  oxm.add(OFB_ICMPV6_TYPE{ICMPV6_TYPE_NEIGHBOR_ADVERTISE});
  oxm.add(OFB_ICMPV6_CODE{0});
  oxm.add(OFB_IPV6_ND_TARGET{dst});
  oxm.add(OFB_IPV6_ND_TLL{ethDst});
  oxm.add(X_IPV6_ND_RES{0x70010203});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {});

  EXPECT_HEX(
      "00000000000200000000000186DD6000000000203A402000000000000000000000000000"
      "0001200000000000000000000000000000028800A3987001020320000000000000000000"
      "0000000000020201000000000002",
      data.data(), data.size());

  MatchPacket decode{data};
  OXMRange range = decode.toRange();
  EXPECT_HEX(
      "800006060000000000028000080600000000000180000A0286DD00013A0140800014013A"
      "800034102000000000000000000000000000000180003610200000000000000000000000"
      "0000000280003A018880003C0100FFFF0A0800FFFFFF7001020380003E10200000000000"
      "000000000000000000028000420600000000000280004E020001",
      range.data(), range.size());
}

TEST(matchpacketbuilder, icmpv6_nd_solicit2) {
  MacAddress ethSrc{"2e:29:6e:ee:d1:44"};
  MacAddress ethDst{"33:33:ff:10:00:01"};
  IPv6Address src{"fc00::1"};
  IPv6Address dst{"ff02::1:ff10:1"};
  IPv6Address target{"fc00::10:10:10:1"};

  OXMList oxm;
  oxm.add(OFB_ETH_DST{ethDst});
  oxm.add(OFB_ETH_SRC{ethSrc});
  oxm.add(OFB_ETH_TYPE{DATALINK_IPV6});
  oxm.add(OFB_IPV6_SRC{src});
  oxm.add(OFB_IPV6_DST{dst});
  oxm.add(OFB_IP_PROTO{PROTOCOL_ICMPV6});
  oxm.add(NXM_NX_IP_TTL{255});
  oxm.add(OFB_ICMPV6_TYPE{ICMPV6_TYPE_NEIGHBOR_SOLICIT});
  oxm.add(OFB_ICMPV6_CODE{0});
  oxm.add(OFB_IPV6_ND_TARGET{target});
  oxm.add(OFB_IPV6_ND_SLL{ethSrc});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {});

  EXPECT_HEX(
      "3333ff100001 2e296eeed144 86dd 6000 0000 0020 3aff fc00 0000 0000 0000 "
      "0000 0000 0000 0001 ff02 0000 0000 0000 0000 0001 ff10 0001 8700 12fe "
      "0000 0000 fc00 0000 0000 0000 0010 0010 0010 0001 0101 2e29 6eee d144",
      data.data(), data.size());
}

TEST(matchpacketbuilder, icmpv6_nd_router_advert) {
  MacAddress ethSrc{"2e:29:6e:ee:d1:44"};
  MacAddress ethDst{"33:33:ff:10:00:01"};
  IPv6Address src{"fc00::1"};
  IPv6Address dst{"ff02::1:ff10:1"};
  IPv6Address target{"fc00::10:10:10:1"};

  OXMList oxm;
  oxm.add(OFB_ETH_DST{ethDst});
  oxm.add(OFB_ETH_SRC{ethSrc});
  oxm.add(OFB_ETH_TYPE{DATALINK_IPV6});
  oxm.add(OFB_IPV6_SRC{src});
  oxm.add(OFB_IPV6_DST{dst});
  oxm.add(OFB_IP_PROTO{PROTOCOL_ICMPV6});
  oxm.add(NXM_NX_IP_TTL{255});
  oxm.add(OFB_ICMPV6_TYPE{0x85});
  oxm.add(OFB_ICMPV6_CODE{0});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {"\x01\x02\x03\x04", 4});

  EXPECT_HEX(
      "3333FF1000012E296EEED14486DD6000000000083AFFFC00000000000000000000000000"
      "0001FF0200000000000000000001FF10000185007C9F01020304",
      data.data(), data.size());
}
