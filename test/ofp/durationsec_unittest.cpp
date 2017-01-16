#include "ofp/durationsec.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(durationsec, basic) {
  DurationSec zero{0, 0};
  EXPECT_EQ("0", zero.toString());

  DurationSec a{0, 1};
  EXPECT_EQ("0.000000001", a.toString());

  DurationSec b{1, 0};
  EXPECT_EQ("1.000000000", b.toString());

  DurationSec c{0xFFFFFFFF, 999999999};
  EXPECT_EQ("4294967295.999999999", c.toString());

  DurationSec d{12345};
  EXPECT_EQ("12345.000000000", d.toString());
}

TEST(durationsec, too_many_nanoseconds) {
  DurationSec a{0, 1000000000};
  EXPECT_EQ("0.x3b9aca00", a.toString());

  DurationSec b{0xFFFFFFFF, 0xFFFFFFFF};
  EXPECT_EQ("4294967295.xffffffff", b.toString());
}

TEST(durationsec, equals) {
  DurationSec a{0x12345678, 0x99AABBCC};
  EXPECT_HEX("1234567899AABBCC", &a, sizeof(a));

  DurationSec b{0x12345678, 0x99AABBCC};
  EXPECT_EQ(a, b);

  DurationSec c{2, 1};
  EXPECT_NE(a, c);
}

TEST(durationsec, parse) {
  DurationSec a;

  EXPECT_TRUE(a.parse("0"));
  EXPECT_EQ("0", a.toString());

  EXPECT_TRUE(a.parse("0.0"));
  EXPECT_EQ("0", a.toString());

  EXPECT_TRUE(a.parse("0.000000001"));
  EXPECT_EQ("0.000000001", a.toString());

  EXPECT_TRUE(a.parse("0.1"));
  EXPECT_EQ("0.100000000", a.toString());

  EXPECT_TRUE(a.parse("0.000001"));
  EXPECT_EQ("0.000001000", a.toString());

  EXPECT_TRUE(a.parse("1.000000000"));
  EXPECT_EQ("1.000000000", a.toString());

  EXPECT_TRUE(a.parse("12345"));
  EXPECT_EQ("12345.000000000", a.toString());

  EXPECT_TRUE(a.parse("4294967295.999999999"));
  EXPECT_EQ("4294967295.999999999", a.toString());

  EXPECT_TRUE(a.parse("0.x3b9aca00"));
  EXPECT_EQ("0.x3b9aca00", a.toString());

  EXPECT_TRUE(a.parse("4294967295.xffffffff"));
  EXPECT_EQ("4294967295.xffffffff", a.toString());

  EXPECT_FALSE(a.parse(""));
  EXPECT_FALSE(a.parse("0.0000000001"));
  EXPECT_FALSE(a.parse("1.1000000000"));
  EXPECT_FALSE(a.parse("4294967296.1"));
  EXPECT_FALSE(a.parse("12.x0fffffffff"));
}
