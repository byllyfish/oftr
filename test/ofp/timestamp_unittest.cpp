// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/timestamp.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(timestamp, testInvalid) {
  Timestamp t;
  EXPECT_FALSE(t.valid());
  EXPECT_EQ("0.000000000", t.toString());
}

TEST(timestamp, testValid) {
  Timestamp t{123456789, 123456789};
  EXPECT_TRUE(t.valid());
  EXPECT_EQ("123456789.123456789", t.toString());
}

TEST(timestamp, parseValid) {
  Timestamp t;

  EXPECT_TRUE(t.parse("123456789.123456789"));
  EXPECT_EQ(Timestamp(123456789, 123456789), t);
  EXPECT_EQ("123456789.123456789", t.toString());

  EXPECT_TRUE(t.parse("12.21"));
  EXPECT_EQ(Timestamp(12, 210000000), t);
  EXPECT_EQ("12.210000000", t.toString());

  EXPECT_TRUE(t.parse("0.0"));
  EXPECT_EQ(Timestamp(0, 0), t);
  EXPECT_EQ("0.000000000", t.toString());

  EXPECT_TRUE(t.parse("1.001"));
  EXPECT_EQ(Timestamp(1, 1000000), t);
  EXPECT_EQ("1.001000000", t.toString());

  EXPECT_TRUE(t.parse("1.001000"));
  EXPECT_EQ(Timestamp(1, 1000000), t);
  EXPECT_EQ("1.001000000", t.toString());

  EXPECT_TRUE(t.parse("1.0000010"));
  EXPECT_EQ(Timestamp(1, 1000), t);
  EXPECT_EQ("1.000001000", t.toString());

  EXPECT_TRUE(t.parse("1.000000001"));
  EXPECT_EQ(Timestamp(1, 1), t);
  EXPECT_EQ("1.000000001", t.toString());

  EXPECT_TRUE(t.parse("1.000000000"));
  EXPECT_EQ(Timestamp(1, 0), t);
  EXPECT_EQ("1.000000000", t.toString());

  EXPECT_TRUE(t.parse("1.100000000"));
  EXPECT_EQ(Timestamp(1, 100000000), t);
  EXPECT_EQ("1.100000000", t.toString());

  EXPECT_TRUE(t.parse("0"));
  EXPECT_EQ(Timestamp(0, 0), t);
  EXPECT_EQ("0.000000000", t.toString());

  EXPECT_TRUE(t.parse("1"));
  EXPECT_EQ(Timestamp(1, 0), t);
  EXPECT_EQ("1.000000000", t.toString());

  EXPECT_TRUE(t.parse("2.0"));
  EXPECT_EQ(Timestamp(2, 0), t);
  EXPECT_EQ("2.000000000", t.toString());

  EXPECT_TRUE(t.parse("123"));
  EXPECT_EQ(Timestamp(123, 0), t);
  EXPECT_EQ("123.000000000", t.toString());
}

TEST(timestamp, parseInvalid) {
  Timestamp t;

  EXPECT_FALSE(t.parse(""));
  EXPECT_FALSE(t.parse(".0"));
  EXPECT_FALSE(t.parse("0."));
  EXPECT_FALSE(t.parse(" 0.0"));
  EXPECT_FALSE(t.parse("0.0 "));
  EXPECT_FALSE(t.parse("0.0t"));
  EXPECT_FALSE(t.parse("0. 0"));
  EXPECT_FALSE(t.parse("-10.1"));
  EXPECT_FALSE(t.parse("10.-1"));

  // too many digits after decimal pt
  EXPECT_FALSE(t.parse("1.0000000001"));
  EXPECT_FALSE(t.parse("1.0000000000"));
}

TEST(timestamp, relational) {
  Timestamp a{1, 1};
  Timestamp b{2, 1};
  Timestamp c{1, 2};

  EXPECT_EQ(a, a);
  EXPECT_EQ(c, c);

  EXPECT_NE(a, b);
  EXPECT_NE(a, c);

  EXPECT_GE(c, a);
  EXPECT_GE(b, a);
  EXPECT_GE(c, c);
}

TEST(timestamp, now) {
  log_info("Timestamp::now", Timestamp::now().toStringUTC());
  log_info("Timestamp::now", Timestamp::now().toStringUTC());
}

TEST(timestamp, secondsSince) {
  Timestamp a{1, 1000000};
  Timestamp b{1, 0};
  Timestamp c{0, 9000000};
  Timestamp d{0, 1000000};

  EXPECT_DOUBLE_EQ(0.001, a.secondsSince(b));
  EXPECT_DOUBLE_EQ(-0.001, b.secondsSince(a));

  EXPECT_DOUBLE_EQ(0.991, b.secondsSince(c));
  EXPECT_DOUBLE_EQ(-0.991, c.secondsSince(b));

  EXPECT_DOUBLE_EQ(0.008, c.secondsSince(d));
  EXPECT_DOUBLE_EQ(-0.008, d.secondsSince(c));
}

TEST(timestamp, units) {
  Timestamp a{1, 1000000};

  EXPECT_EQ(1000000, a.nanoseconds());
  EXPECT_EQ(1000, a.microseconds());
  EXPECT_EQ(1, a.milliseconds());
}

TEST(timestamp, toStringUTC) {
  Timestamp a{123456789, 123456789};
  EXPECT_EQ("1973-11-29T21:33:09.123456789Z", a.toStringUTC());

  Timestamp b{2, 1000000};
  EXPECT_EQ("1970-01-01T00:00:02.001000000Z", b.toStringUTC());
}

TEST(timestamp, stream) {
  std::string buf;
  llvm::raw_string_ostream oss{buf};

  Timestamp a{123456789101112, 3456789};
  oss << a;
  EXPECT_EQ("123456789101112.003456789", oss.str());
}

TEST(timestamp, addSeconds) {
  Timestamp a{123456789, 123456789};

  a.addSeconds(20);
  EXPECT_EQ(123456789 + 20, a.seconds());
  EXPECT_EQ(123456789, a.nanoseconds());

  a.addSeconds(-40);
  EXPECT_EQ(123456789 - 20, a.seconds());
  EXPECT_EQ(123456789, a.nanoseconds());
}

TEST(timestamp, unix_time) {
  Timestamp a{123456789101112, 3456789};

  if (sizeof(time_t) > 4) {
    EXPECT_EQ(123456789101112, a.unix_time());
  } else {
    EXPECT_EQ(123456789101112 & 0xffffffff, Unsigned_cast(a.unix_time()));
  }
}

TEST(timestamp, plus_interval) {
  Timestamp a{123456789, 123456789};
  TimeInterval x{20, 0};

  auto b = a + x;
  EXPECT_EQ(123456789 + 20, b.seconds());
  EXPECT_EQ(123456789, b.nanoseconds());

  TimeInterval y{20, 20};
  auto c = a + y;
  EXPECT_EQ(123456789 + 20, c.seconds());
  EXPECT_EQ(123456789 + 20, c.nanoseconds());
}

TEST(timestamp, plus_double) {
  Timestamp a{123456789, 123456789};

  auto b = a + TimeInterval{20.0};
  EXPECT_EQ(123456789 + 20, b.seconds());
  EXPECT_EQ(123456789, b.nanoseconds());
}

TEST(timestamp, from_double) {
  Timestamp a{3.141592653};
  EXPECT_EQ(3, a.seconds());
  EXPECT_EQ(141592653, a.nanoseconds());
}
