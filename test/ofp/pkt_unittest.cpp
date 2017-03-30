// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/pkt.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(pkt, ethernet) {
  ByteList buf{HexToRawData("010203040506 112233445566 1122")};
  EXPECT_EQ(sizeof(pkt::Ethernet), buf.size());
  auto eth = pkt::Ethernet::cast(buf.data(), buf.size());

  EXPECT_EQ(eth->dst, MacAddress{"010203040506"});
  EXPECT_EQ(eth->src, MacAddress{"112233445566"});
  EXPECT_EQ(eth->type, 0x1122);

  auto fail = pkt::Ethernet::cast(buf.data(), buf.size() - 1);
  EXPECT_EQ(fail, nullptr);
}

TEST(pkt, vlanHdr) {
  ByteList buf{HexToRawData("01020304")};
  EXPECT_EQ(sizeof(pkt::VlanHdr), buf.size());
  auto vlan = pkt::VlanHdr::cast(buf.data(), buf.size());

  EXPECT_EQ(vlan->tci, 0x0102);
  EXPECT_EQ(vlan->ethType, 0x0304);

  auto fail = pkt::VlanHdr::cast(buf.data(), buf.size() - 1);
  EXPECT_EQ(fail, nullptr);
}

TEST(pkt, arpHdr) {
  ByteList buf{HexToRawData(
      "010203040506 0001 112233445566 0A0B0C0D 010203040506 0708090A")};
  EXPECT_EQ(sizeof(pkt::Arp), buf.size());
  auto arp = pkt::Arp::cast(buf.data(), buf.size());

  EXPECT_HEX("010203040506", arp->prefix, sizeof(arp->prefix));
  EXPECT_EQ(arp->op, 0x0001);
  EXPECT_EQ(arp->sha, MacAddress{"112233445566"});
  EXPECT_EQ(arp->spa, IPv4Address{"10.11.12.13"});
  EXPECT_EQ(arp->tha, MacAddress{"010203040506"});
  EXPECT_EQ(arp->tpa, IPv4Address{"7.8.9.10"});

  auto fail = pkt::Arp::cast(buf.data(), buf.size() - 1);
  EXPECT_EQ(fail, nullptr);
}

TEST(pkt, ipV4Hdr) {
  ByteList buf{HexToRawData("010200330044005506070008 0A0B0C0D 0708090A")};
  EXPECT_EQ(sizeof(pkt::IPv4Hdr), buf.size());
  auto ip = pkt::IPv4Hdr::cast(buf.data(), buf.size());

  EXPECT_EQ(ip->ver, 0x01);
  EXPECT_EQ(ip->tos, 0x02);
  EXPECT_EQ(ip->length, 0x0033);
  EXPECT_EQ(ip->ident, 0x0044);
  EXPECT_EQ(ip->frag, 0x0055);
  EXPECT_EQ(ip->ttl, 0x06);
  EXPECT_EQ(ip->proto, 0x07);
  EXPECT_EQ(ip->cksum, 0x0008);
  EXPECT_EQ(ip->src, IPv4Address{"10.11.12.13"});
  EXPECT_EQ(ip->dst, IPv4Address{"7.8.9.10"});

  auto fail = pkt::IPv4Hdr::cast(buf.data(), buf.size() - 1);
  EXPECT_EQ(fail, nullptr);
}

TEST(pkt, ipV6Hdr) {
  ByteList buf{
      HexToRawData("01020304 0102 03 04 000102030405060708090A0B0C0D0E0F "
                   "00102030405060708090A0B0C0D0E0F0")};
  EXPECT_EQ(sizeof(pkt::IPv6Hdr), buf.size());
  auto ip = pkt::IPv6Hdr::cast(buf.data(), buf.size());

  EXPECT_EQ(ip->verClassLabel, 0x01020304);
  EXPECT_EQ(ip->payloadLength, 0x0102);
  EXPECT_EQ(ip->nextHeader, 0x03);
  EXPECT_EQ(ip->hopLimit, 0x04);
  EXPECT_EQ(ip->src, IPv6Address{"0001:0203:0405:0607:0809:0A0B:0C0D:0E0F"});
  EXPECT_EQ(ip->dst, IPv6Address{"0010:2030:4050:6070:8090:A0B0:C0D0:E0F0"});

  auto fail = pkt::IPv6Hdr::cast(buf.data(), buf.size() - 1);
  EXPECT_EQ(fail, nullptr);
}

TEST(pkt, checksum) {
  ByteList buf{HexToRawData("00 01 02 03 04 05 00")};

  // Even starting ptr. (first byte zero)
  EXPECT_EQ(pkt::Checksum({}), 65535);
  EXPECT_EQ(pkt::Checksum({buf.data(), 1}), 65535);
  EXPECT_EQ(pkt::Checksum({buf.data(), 2}), 65534);
  EXPECT_EQ(pkt::Checksum({buf.data(), 3}), 65022);
  EXPECT_EQ(pkt::Checksum({buf.data(), 4}), 65019);
  EXPECT_EQ(pkt::Checksum({buf.data(), 5}), 63995);
  EXPECT_EQ(pkt::Checksum({buf.data(), 6}), 63990);
  EXPECT_EQ(pkt::Checksum({buf.data(), 7}), 63990);

  // Odd starting ptr. (first byte zero)
  EXPECT_EQ(pkt::Checksum({}), 65535);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 1}), 65279);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 2}), 65277);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 3}), 64509);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 4}), 64505);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 5}), 63225);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 6}), 63225);
}

TEST(pkt, checksum2) {
  ByteList buf{HexToRawData("01 02 03 04 05 00")};

  // Even starting ptr.
  EXPECT_EQ(pkt::Checksum({buf.data(), 1}), 65279);
  EXPECT_EQ(pkt::Checksum({buf.data(), 2}), 65277);
  EXPECT_EQ(pkt::Checksum({buf.data(), 3}), 64509);
  EXPECT_EQ(pkt::Checksum({buf.data(), 4}), 64505);
  EXPECT_EQ(pkt::Checksum({buf.data(), 5}), 63225);
  EXPECT_EQ(pkt::Checksum({buf.data(), 6}), 63225);

  // Odd starting ptr.
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 1}), 65023);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 2}), 65020);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 3}), 63996);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 4}), 63991);
  EXPECT_EQ(pkt::Checksum({buf.data() + 1, 5}), 63991);
}

TEST(pkt, checksum_multi2) {
  ByteList buf{HexToRawData("00 01 02 03 04 05 06 07")};

  UInt16 cksum1 = pkt::Checksum(buf.toRange());

  for (unsigned i = 0; i <= buf.size(); ++i) {
    EXPECT_EQ(pkt::Checksum({buf.data(), i}, {buf.data() + i, buf.size() - i}),
              cksum1);
  }

  UInt16 cksum2 = pkt::Checksum({buf.data() + 1, buf.size() - 1});

  for (unsigned i = 0; i <= buf.size() - 1; ++i) {
    EXPECT_EQ(pkt::Checksum({buf.data() + 1, i},
                            {buf.data() + 1 + i, buf.size() - 1 - i}),
              cksum2);
  }
}

TEST(pkt, checksum_multi3) {
  ByteList buf{HexToRawData("00 01 02 03 04 05 06 07")};

  UInt16 cksum1 = pkt::Checksum(buf.toRange());

  for (unsigned i = 0; i <= buf.size(); ++i) {
    for (unsigned j = i; j <= buf.size(); ++j) {
      EXPECT_EQ(pkt::Checksum(
        {buf.data(), i}, 
        {buf.data() + i, j - i}, 
        {buf.data() + j, buf.size() - j}),
              cksum1);
    }
  }

  UInt16 cksum2 = pkt::Checksum({buf.data() + 1, buf.size() - 1});

  for (unsigned i = 0; i <= buf.size() - 1; ++i) {
    for (unsigned j = i; j <= buf.size() - 1; ++j) {
      EXPECT_EQ(pkt::Checksum(
        {buf.data() + 1, i},
        {buf.data() + 1 + i, j - i},
        {buf.data() + 1 + j, buf.size() - 1 - j}),
              cksum2);
    }
  }
}

