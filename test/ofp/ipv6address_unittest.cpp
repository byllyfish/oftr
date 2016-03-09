// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/ipv6address.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(ipv6address, basic) {
  IPv6Address addr{"::1"};

  EXPECT_HEX("0000 0000 0000 0000 0000 0000 0000 0001", &addr, sizeof(addr));
  EXPECT_TRUE(addr.valid());
  EXPECT_EQ("::1", addr.toString());
}

TEST(ipv6address, v4) {
  IPv6Address addr{"192.168.1.1"};

  EXPECT_TRUE(addr.valid());
  EXPECT_HEX("0000 0000 0000 0000 0000 FFFF C0A8 0101", &addr, sizeof(addr));
  EXPECT_TRUE(addr.isV4Mapped());
  EXPECT_EQ("192.168.1.1", addr.toString());
}

TEST(ipv6address, paddedv4) {
  IPv6Address addr{"127.000.090.009"};

  EXPECT_TRUE(addr.valid());
  EXPECT_HEX("0000 0000 0000 0000 0000 FFFF 7F00 5A09", &addr, sizeof(addr));
  EXPECT_TRUE(addr.isV4Mapped());
  EXPECT_EQ("127.0.90.9", addr.toString());
}

TEST(ipv6address, equals) {
  IPv6Address x{"192.168.1.1"};
  IPv6Address y{"192.168.1.1"};

  EXPECT_TRUE(x == y);

  IPv6Address z = y;

  EXPECT_TRUE(y == z);
}

TEST(ipv6address, fromv4) {
  IPv4Address addr{"192.168.1.1"};
  IPv6Address v6{addr};

  EXPECT_HEX("0000 0000 0000 0000 0000 FFFF C0A8 0101", &v6, sizeof(v6));
  EXPECT_EQ("192.168.1.1", v6.toString());
  EXPECT_TRUE(v6.isV4Mapped());
}

TEST(ipv6address, invalid) {
  IPv6Address addr;

  EXPECT_FALSE(addr.valid());
}

TEST(ipv6address, linklocal) {
  IPv6Address a{"fe80::1"};
  EXPECT_TRUE(a.valid());
  EXPECT_TRUE(a.isLinkLocal());
  EXPECT_EQ(0, a.zone());

  IPv6Address b{"2000::1"};
  EXPECT_TRUE(b.valid());
  EXPECT_FALSE(b.isLinkLocal());
  EXPECT_EQ(0, b.zone());
}

TEST(ipv6address, zones) {
  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(0, a.zone());
    EXPECT_HEX("FE80 0000 0000 0000 1122 33FF fe44 5566", &a, sizeof(a));
    EXPECT_EQ("fe80::1122:33ff:fe44:5566", a.toString());
  }

  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566%1"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(1, a.zone());
    EXPECT_HEX("FE80 0001 0000 0000 1122 33FF fe44 5566", &a, sizeof(a));
    EXPECT_EQ("fe80::1122:33ff:fe44:5566%1", a.toString());
  }

  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566%65535"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(65535, a.zone());
    EXPECT_HEX("FE80 FFFF 0000 0000 1122 33FF fe44 5566", &a, sizeof(a));
    EXPECT_EQ("fe80::1122:33ff:fe44:5566%65535", a.toString());
  }

  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566%65536"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(65536, a.zone());
    EXPECT_HEX("FE80 0000 0001 0000 1122 33FF fe44 5566", &a, sizeof(a));
    EXPECT_EQ("fe80::1122:33ff:fe44:5566%65536", a.toString());
  }

  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566%4294967294"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(4294967294, a.zone());
    EXPECT_HEX("FE80 FFFE FFFF 0000 1122 33FF fe44 5566", &a, sizeof(a));
    EXPECT_EQ("fe80::1122:33ff:fe44:5566%4294967294", a.toString());
  }
}

TEST(ipv6address, rfc5952) {
  // 4.1 Handling leading zeros in a 16-bit field.

  IPv6Address a{"2001:0db8::0001"};
  EXPECT_EQ("2001:db8::1", a.toString());

  // 4.2.1 Shorten as much as possible.

  IPv6Address b{"2001:db8:0:0:0:0:2:1"};
  EXPECT_EQ("2001:db8::2:1", b.toString());

  IPv6Address c{"2001:db8::0:2:1"};
  EXPECT_EQ("2001:db8::2:1", c.toString());

  // 4.2.2. Handling One 16-Bit 0 Field (ignored)

  // IPv6Address d{"2001:db8:0:1:1:1:1:1"};
  // EXPECT_EQ("2001:db8:0:1:1:1:1:1", d.toString());

  // IPv6Address e{"2001:db8::1:1:1:1:1"};
  // EXPECT_EQ("2001:db8:0:1:1:1:1:1", e.toString());

  // 4.2.3. Choice in Placement of "::"

  IPv6Address f{"2001:0:0:1:0:0:0:1"};
  EXPECT_EQ("2001:0:0:1::1", f.toString());

  IPv6Address g{"2001:db8:0:0:1:0:0:1"};
  EXPECT_EQ("2001:db8::1:0:0:1", g.toString());

  // 4.3. Lowercase

  IPv6Address h{"2001:0DBA::000F"};
  EXPECT_EQ("2001:dba::f", h.toString());
}

TEST(ipv6address, invalid2) {
  IPv6Address a{"127x"};
  EXPECT_FALSE(a.valid());
}

TEST(ipv6address, stream) {
  IPv6Address ip{"2000::1"};

  std::ostringstream oss;
  oss << ip;
  EXPECT_EQ("2000::1", oss.str());
}

TEST(ipv6address, relational) {
  IPv6Address a{"2001::1"};
  IPv6Address b{"2001::2"};

  EXPECT_TRUE(a < b);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(a > b);
  EXPECT_FALSE(a >= b);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);

  EXPECT_FALSE(b < a);
  EXPECT_FALSE(b <= a);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(b >= a);
  EXPECT_FALSE(b == a);
  EXPECT_TRUE(b != a);
}

TEST(ipv6address, hash) {
  IPv6Address a{"2001::1"};
  IPv6Address b{"2001::2"};

  std::hash<IPv6Address> hasher;
  EXPECT_EQ(1003530086136274462, hasher(a));
  EXPECT_EQ(2007060172272548861, hasher(b));
}
