// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/datapathid.h"

using namespace ofp;

TEST(datapathid, test) {
  DatapathID a;
  EXPECT_HEX("0000 0000 0000 0000", &a, sizeof(a));
  EXPECT_EQ(0, a.implementerDefined());
  EXPECT_EQ("00:00:00:00:00:00:00:00", a.toString());

  DatapathID b{0x1234, EnetAddress{"00-01-02-03-04-05"}};
  EXPECT_EQ(0x1234, b.implementerDefined());
  EXPECT_EQ(EnetAddress{"00-01-02-03-04-05"}, b.macAddress());
  EXPECT_HEX("1234 00 01 02 03 04 05", &b, sizeof(b));
  EXPECT_EQ("12:34:00:01:02:03:04:05", b.toString());

  EXPECT_LT(a, b);
  EXPECT_NE(a, b);

  DatapathID c{b};
  EXPECT_EQ(b, c);

  DatapathID d{0xffff, EnetAddress{"ff:ff:ff:ff:ff:ff"}};
  EXPECT_EQ(0xffff, d.implementerDefined());
  EXPECT_EQ(EnetAddress{"ff-ff-ff-ff-ff-ff"}, d.macAddress());
  EXPECT_HEX("ffff ff ff ff ff ff ff", &d, sizeof(d));
  EXPECT_EQ("ff:ff:ff:ff:ff:ff:ff:ff", d.toString());
}
