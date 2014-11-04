// Copyright 2014-present Bill Fisher. All rights reserved.

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
