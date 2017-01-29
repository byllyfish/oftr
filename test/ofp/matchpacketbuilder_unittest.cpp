#include "ofp/matchpacketbuilder.h"
#include "ofp/matchpacket.h"  // for roundtrip testing
#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"
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
