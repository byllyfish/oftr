// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/byterange.h"
#include "ofp/bytelist.h"

using namespace ofp;

TEST(byterange, test) {
  const char *s = "abc";
  ByteRange r{s, 3};

  EXPECT_EQ(BytePtr(s), r.data());
  EXPECT_EQ(3, r.size());

  ByteList list{"abc", 3};
  EXPECT_NE(r.data(), list.data());
  EXPECT_EQ(r, list.toRange());
}

TEST(byterange, isPrintable) {
  ByteRange a{" \n\t\v", 4};
  EXPECT_TRUE(a.isPrintable());

  ByteRange b{"\0", 1};
  EXPECT_FALSE(b.isPrintable());

  ByteRange c{"hello", 5};
  EXPECT_TRUE(c.isPrintable());

  ByteRange d{"\x7F", 1};
  EXPECT_FALSE(d.isPrintable());

  ByteRange e{"\xA0", 1};
  EXPECT_FALSE(e.isPrintable());
}
