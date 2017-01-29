#include "ofp/unittest.h"
#include "ofp/pkt.h"

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
    ByteList buf{HexToRawData("010203040506 0001 112233445566 0A0B0C0D 010203040506 0708090A")};
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
    ByteList buf{HexToRawData("01020304 0102 03 04 000102030405060708090A0B0C0D0E0F 00102030405060708090A0B0C0D0E0F0")};
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

