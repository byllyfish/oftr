// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/smallbuffer.h"
#include <numeric>
#include "ofp/unittest.h"

using namespace ofp;

static void setAll(SmallBuffer *buf, UInt8 val, size_t length) {
  buf->resize(length);
  std::memset(buf->begin(), val, length);
}

TEST(smallbuffer, small) {
  SmallBuffer buf;

  EXPECT_TRUE(IsPtrAligned(buf.begin(), 8));
  EXPECT_EQ(0, buf.size());
  EXPECT_EQ(64, buf.capacity());
  EXPECT_TRUE(buf.begin() != nullptr);
  EXPECT_EQ(buf.begin(), buf.end());
  EXPECT_EQ(0, std::accumulate(buf.begin(), buf.end(), 0));

  buf.add("123456789", 10);
  EXPECT_EQ(10, buf.size());
  EXPECT_HEX("31323334353637383900", buf.begin(), buf.size());
  EXPECT_EQ(477, std::accumulate(buf.begin(), buf.end(), 0));
}

TEST(smallbuffer, increaseCapacity) {
  SmallBuffer buf;

  for (int i = 0; i < 20; ++i) {
    buf.add("123456789", 10);
  }

  EXPECT_TRUE(IsPtrAligned(buf.begin(), 8));
  EXPECT_EQ(200, buf.size());
  EXPECT_HEX(
      "313233343536373839003132333435363738390031323334353637383900313233343536"
      "373839003132333435363738390031323334353637383900313233343536373839003132"
      "333435363738390031323334353637383900313233343536373839003132333435363738"
      "390031323334353637383900313233343536373839003132333435363738390031323334"
      "353637383900313233343536373839003132333435363738390031323334353637383900"
      "3132333435363738390031323334353637383900",
      buf.begin(), buf.size());
  EXPECT_EQ(9540, std::accumulate(buf.begin(), buf.end(), 0));

  for (int i = 0; i < 2000; ++i) {
    buf.add("123456789", 10);
  }

  EXPECT_TRUE(IsPtrAligned(buf.begin(), 8));
  EXPECT_EQ(20200, buf.size());
  EXPECT_EQ(963540, std::accumulate(buf.begin(), buf.end(), 0));
}

TEST(smallbuffer, copyConstruct) {
  SmallBuffer buf{"1234567890", 11};
  EXPECT_EQ(525, std::accumulate(buf.begin(), buf.end(), 0));

  SmallBuffer buf2(buf);
  EXPECT_EQ(11, buf2.size());
  EXPECT_EQ(525, std::accumulate(buf2.begin(), buf2.end(), 0));
  EXPECT_EQ(buf, buf2);
}

TEST(smallbuffer, copyAssign) {
  SmallBuffer buf{"1234567890", 11};
  SmallBuffer buf2{"1", 1};

  buf2 = buf;
  EXPECT_EQ(11, buf2.size());
  EXPECT_EQ(525, std::accumulate(buf.begin(), buf.end(), 0));
  EXPECT_EQ(buf, buf2);
}

TEST(smallbuffer, moveConstruct) {
  {
    SmallBuffer buf{"1234567890", 11};
    const UInt8 *bufBegin = buf.begin();

    SmallBuffer buf2{std::move(buf)};

    EXPECT_NE(bufBegin, buf2.begin());
    EXPECT_EQ(525, std::accumulate(buf2.begin(), buf2.end(), 0));
    EXPECT_EQ(11, buf2.size());
    EXPECT_EQ(0, buf.size());
    EXPECT_EQ(0, std::accumulate(buf.begin(), buf.end(), 0));
  }

  {
    SmallBuffer buf;
    setAll(&buf, 1, 800);
    const UInt8 *bufBegin = buf.begin();

    SmallBuffer buf2{std::move(buf)};
    EXPECT_EQ(bufBegin, buf2.begin());
    EXPECT_EQ(800, buf2.size());
    EXPECT_EQ(800, std::accumulate(buf2.begin(), buf2.end(), 0));
    EXPECT_EQ(0, buf.size());
  }
}

TEST(smallbuffer, resize) {
  SmallBuffer buf;

  buf.resize(8);
  EXPECT_EQ(8, buf.size());

  buf.resize(0);
  EXPECT_EQ(0, buf.size());

  buf.resize(10000);
  EXPECT_EQ(10000, buf.size());
}

TEST(smallbuffer, replace) {
  SmallBuffer buf;

  buf.add("XXXXXXXXXX", 10);
  EXPECT_EQ(64, buf.capacity());

  for (int i = 0; i < 10; ++i) {
    UInt8 *pos = buf.begin();
    buf.replace(pos, pos + 2, "1234567890", 10);
  }

  EXPECT_EQ(90, buf.size());
  EXPECT_EQ(1024, buf.capacity());
  EXPECT_HEX(
      "313233343536373839303334353637383930333435363738393033343536373839303334"
      "353637383930333435363738393033343536373839303334353637383930333435363738"
      "393033343536373839305858585858585858",
      buf.begin(), buf.size());

  buf.replace(buf.begin() + 1, buf.begin() + 75, "x", 1);

  EXPECT_EQ(17, buf.size());
  EXPECT_HEX("3178343536373839305858585858585858", buf.begin(), buf.size());
}

TEST(smallbuffer, moveAssignSmall) {
  SmallBuffer buf1;
  buf1.add("XXXXXXXXXX", 10);

  EXPECT_EQ(10, buf1.size());
  EXPECT_EQ(64, buf1.capacity());

  SmallBuffer original = buf1;
  SmallBuffer buf2;
  buf2 = std::move(buf1);

  EXPECT_EQ(10, buf2.size());
  EXPECT_EQ(64, buf2.capacity());

  EXPECT_EQ(0, buf1.size());
  EXPECT_EQ(64, buf1.capacity());

  EXPECT_EQ(original, buf2);

  // Make sure we can move *into* a buffer that has previously been moved from.
  buf1 = std::move(buf2);

  EXPECT_EQ(10, buf1.size());
  EXPECT_EQ(64, buf1.capacity());

  EXPECT_EQ(0, buf2.size());
  EXPECT_EQ(64, buf2.capacity());

  EXPECT_EQ(original, buf1);
}

TEST(smallbuffer, moveAssignLarge) {
  SmallBuffer buf1;

  for (int i = 0; i < 10; ++i) {
    buf1.add("XXXXXXXXXX", 10);
  }

  EXPECT_EQ(100, buf1.size());
  EXPECT_EQ(1024, buf1.capacity());

  SmallBuffer original = buf1;
  SmallBuffer buf2;
  buf2 = std::move(buf1);

  EXPECT_EQ(100, buf2.size());
  EXPECT_EQ(1024, buf2.capacity());

  EXPECT_EQ(0, buf1.size());
  EXPECT_EQ(64, buf1.capacity());

  EXPECT_EQ(original, buf2);

  // Make sure we can move *into* a buffer that has previously been moved from.
  buf1 = std::move(buf2);

  EXPECT_EQ(100, buf1.size());
  EXPECT_EQ(1024, buf1.capacity());

  EXPECT_EQ(0, buf2.size());
  EXPECT_EQ(64, buf2.capacity());

  EXPECT_EQ(original, buf1);
}

TEST(smallbuffer, reset) {
  SmallBuffer buf;

  buf.reset(10);
  EXPECT_EQ(10, buf.size());
  EXPECT_LE(10, buf.capacity());
  EXPECT_GT(500, buf.capacity());

  buf.reset(500);
  EXPECT_EQ(500, buf.size());
  EXPECT_LE(500, buf.capacity());

  buf.reset(10);
  EXPECT_EQ(10, buf.size());
  EXPECT_LE(10, buf.size());
  EXPECT_GT(500, buf.capacity());
}

TEST(smallbuffer, replaceUninitialized) {
  SmallBuffer buf;

  buf.add("0123456789", 10);
  EXPECT_EQ(10, buf.size());

  buf.replaceUninitialized(buf.begin(), buf.begin() + 1, 100);
  EXPECT_EQ(109, buf.size());
  EXPECT_HEX("313233343536373839", buf.begin() + 100, 9);
}
