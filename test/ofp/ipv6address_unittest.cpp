#include "ofp/unittest.h"
#include "ofp/ipv6address.h"

using namespace ofp;

TEST(ipv6address, basic) {
  IPv6Address addr{"::1"};

  EXPECT_HEX("0000 0000 0000 0000 0000 0000 0000 0001", &addr, sizeof(addr));
  EXPECT_TRUE(addr.valid());
  EXPECT_EQ("::1", addr.toString());
}

TEST(ipv6address, v4) {
  IPv6Address addr{"192.168.1.1"};

  EXPECT_HEX("0000 0000 0000 0000 0000 00FF C0A8 0101", &addr, sizeof(addr));
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

  EXPECT_HEX("0000 0000 0000 0000 0000 00FF C0A8 0101", &v6, sizeof(v6));
  EXPECT_EQ("192.168.1.1", v6.toString());
}

TEST(ipv6address, valid) {
  IPv6Address addr;

  EXPECT_FALSE(addr.valid());
}


TEST(ipv6address, zones) {
  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(0, a.zone());
    EXPECT_HEX("FE80 0000 0000 0000 1122 33FF fe44 5566", &a, sizeof(a));
  }

  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566%1"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(1, a.zone());
    EXPECT_HEX("FE80 0001 0000 0000 1122 33FF fe44 5566", &a, sizeof(a));
  }

  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566%65535"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(65535, a.zone());
    EXPECT_HEX("FE80 FFFF 0000 0000 1122 33FF fe44 5566", &a, sizeof(a));
  }

  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566%65536"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(65536, a.zone());
    EXPECT_HEX("FE80 0000 0001 0000 1122 33FF fe44 5566", &a, sizeof(a));
  }

  {
    IPv6Address a{"fe80::1122:33ff:fe44:5566%4294967294"};
    EXPECT_TRUE(a.valid());
    EXPECT_TRUE(a.isLinkLocal());
    EXPECT_EQ(4294967294, a.zone());
    EXPECT_HEX("FE80 FFFE FFFF 0000 1122 33FF fe44 5566", &a, sizeof(a));
  }
}
