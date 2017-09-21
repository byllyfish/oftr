// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/byterange.h"
#include "ofp/bytelist.h"
#include "ofp/unittest.h"

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

TEST(byterange, equal) {
  ByteRange a{"1234", 4};
  ByteRange b{"1234", 4};
  ByteRange c{"1235", 4};
  ByteRange d{"123", 3};

  EXPECT_EQ(a, b);
  EXPECT_EQ(b, a);
  EXPECT_NE(c, a);
  EXPECT_NE(b, c);
  EXPECT_NE(c, d);
  EXPECT_NE(d, c);

  ByteRange empty1;
  ByteRange empty2;

  EXPECT_EQ(empty1, empty2);
}

TEST(byterange, stream) {
  ByteRange range{"1234", 4};

  std::string buf;
  llvm::raw_string_ostream oss{buf};

  oss << range;
  EXPECT_EQ("[ByteRange size=4 data=31323334]", oss.str());
}
