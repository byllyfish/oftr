// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/smallbuffer.h"
#include <numeric>

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
