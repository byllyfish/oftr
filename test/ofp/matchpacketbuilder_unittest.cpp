// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchpacketbuilder.h"
#include "ofp/matchpacket.h"  // for roundtrip testing
#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"
#include "ofp/pkt.h"  // for pkt::Checksum
#include "ofp/unittest.h"

using namespace ofp;

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
      "0002C0A80102",
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
      "0000000000020000000000018100 E0010806 "
      "0001080006040001000000000001C0A80101000000000002C0A80102",
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
      "00000000000200000000000188CC 020701001122334455 0403020102 06020045",
      data.data(), data.size());

  // N.B. This test depends on field ordering being the same.
  MatchPacket decode{data};
  EXPECT_EQ(oxm.toRange(), decode.toRange());
}

TEST(matchpacketbuilder, minimal_arp) {
  OXMList oxm;
  oxm.add(OFB_ETH_TYPE{DATALINK_ARP});

  ByteList data;
  MatchPacketBuilder packet{oxm.toRange()};
  packet.build(&data, {});

  EXPECT_HEX(
      "000000000000000000000000080600010800060400000000000000000000000000000000"
      "000000000000",
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

  EXPECT_HEX("00000000000000000000000088CC0200040006020000", data.data(),
             data.size());

  MatchPacket decode{data};
  OXMRange range = decode.toRange();
  EXPECT_HEX(
      "800006060000000000008000080600000000000080000A0288CCFFFF024400FFFFFF0000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000FFFF044400FFFFFF0000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000FFFF060600FFFFFF000"
      "0",
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
      "F6F90102030405",
      data.data(), data.size());

  ByteRange icmp = SafeByteRange(data.data(), data.size(), 34);
  EXPECT_EQ(0, pkt::Checksum(icmp));

  MatchPacket decode{data};
  OXMRange range = decode.toRange();
  EXPECT_HEX(
      "800006060000000000028000080600000000000180000A02080080001001008000120100"
      "800014010180001604C0A8010180001804C0A8010200013A014080002601008000280100"
      "FFFF080600FFFFFF0026",
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
      "0000 0000 0002 0000 0000 0001 0800 4500"
      "001d 0000 0000 4001 f98b c0a8 0003 c0a8"
      "0001 0800 7fcd 7832 0000 00",
      data.data(), data.size());

  ByteRange icmp = SafeByteRange(data.data(), data.size(), 34);
  EXPECT_EQ(0, pkt::Checksum(icmp));
}
