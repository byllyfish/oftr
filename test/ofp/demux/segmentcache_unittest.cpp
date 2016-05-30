// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/segmentcache.h"
#include "ofp/unittest.h"

using namespace ofp;
using namespace ofp::demux;

TEST(segmentcache, testEmpty) {
  // Storing an empty segment [0,0) should leave the cache empty.
  SegmentCache cache;
  cache.store(0, {});
  EXPECT_EQ(nullptr, cache.current());
  EXPECT_EQ(0, cache.segmentCount());
}

TEST(segmentcache, test1Segment) {
  // Storing one segment [0,1) should leave one segment.
  SegmentCache cache;
  ByteList data{HexToRawData("55")};

  cache.store(1, data.toRange());
  EXPECT_EQ(1, cache.segmentCount());

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("55", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(1, seg->end());
  EXPECT_FALSE(seg->final());
}

TEST(segmentcache, testAdjacentSegments) {
  // Storing two adjacent segments [0,4) and [4, 8) should combine them
  // into one segment.
  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(4, data.toRange());
  EXPECT_EQ(1, cache.segmentCount());

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(4, seg->end());
  EXPECT_FALSE(seg->final());

  cache.store(8, data.toRange());
  EXPECT_EQ(1, cache.segmentCount());

  seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("0011223300112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(8, seg->end());
  EXPECT_FALSE(seg->final());
}

TEST(segmentcache, testNonAdjacentSegments) {
  // Storing two non-adjacent segments: [0,4) and [6, 10) should create two
  // segments in the cache. Later, storing [4, 6) should cause all segments
  // to be combined into one segment.
  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(4, data.toRange());
  EXPECT_EQ(1, cache.segmentCount());

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(4, seg->end());

  cache.store(10, data.toRange());
  EXPECT_EQ(2, cache.segmentCount());

  seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(4, seg->end());

  cache.store(6, ByteRange{"  ", 2});
  EXPECT_EQ(1, cache.segmentCount());

  seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00112233202000112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(10, seg->end());
}

TEST(segmentcache, testAdjacentSegments2) {
  // Storing two adjacent segments [0xfffffffb,0xffffffff) and [0xffffffff, 3)
  // should combine them into one segment.
  const UInt32 kBegin = 0xFFFFFFFF - 4;

  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(0xFFFFFFFF, data.toRange());
  EXPECT_EQ(1, cache.segmentCount());

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(kBegin, seg->begin());
  EXPECT_EQ(kBegin + 4, seg->end());

  cache.store(3, data.toRange());
  EXPECT_EQ(1, cache.segmentCount());

  seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("0011223300112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(kBegin, seg->begin());
  EXPECT_EQ(kBegin + 8, seg->end());
}

TEST(segmentcache, testDrainAll) {
  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(4, data.toRange());
  EXPECT_EQ(1, cache.segmentCount());

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);
  EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(4, seg->end());

  cache.consume(4);
  EXPECT_EQ(0, cache.segmentCount());
  EXPECT_EQ(nullptr, cache.current());
}

TEST(segmentcache, testDrainPartial) {
  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(4, data.toRange());
  EXPECT_EQ(1, cache.segmentCount());

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);
  EXPECT_EQ(data.toRange(), seg->data());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(4, seg->end());

  cache.consume(2);
  EXPECT_EQ(1, cache.segmentCount());

  seg = cache.current();
  ASSERT_NE(nullptr, seg);
  EXPECT_HEX("2233", seg->data().data(), seg->data().size());
  EXPECT_EQ(2, seg->begin());
  EXPECT_EQ(4, seg->end());
}

TEST(segmentcache, testFinal) {
  // Storing a final, empty segment [0,0) should leave cache with one segment.
  SegmentCache cache;
  cache.store(0, {}, true);
  EXPECT_EQ(1, cache.segmentCount());

  auto seg = cache.current();
  ASSERT_NE(nullptr, cache.current());
  EXPECT_TRUE(seg->final());
  EXPECT_EQ(0, seg->size());
}

TEST(segmentcache, testAdjSegmentsSeg2Final) {
  // Storing two adjacent segments [0,4) and [4, 8) should combine them
  // into one segment. The second segment is final.
  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(4, data.toRange());
  EXPECT_EQ(1, cache.segmentCount());

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(4, seg->end());
  EXPECT_FALSE(seg->final());

  cache.store(8, data.toRange(), true);
  EXPECT_EQ(1, cache.segmentCount());

  seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("0011223300112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(8, seg->end());
  EXPECT_TRUE(seg->final());
}

TEST(segmentcache, testAdjSegmentsSeg1Final) {
  // Storing two adjacent segments [0,4) and [4, 8) should ignore the second
  // segment when the first segment is final.
  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(4, data.toRange(), true);
  EXPECT_EQ(1, cache.segmentCount());

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(4, seg->end());
  EXPECT_TRUE(seg->final());

  cache.store(8, data.toRange(), false);
  EXPECT_EQ(1, cache.segmentCount());

  seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(0, seg->begin());
  EXPECT_EQ(4, seg->end());
  EXPECT_TRUE(seg->final());
}

TEST(segmentcache, lessThan) {
  // Segment::lessThan() should do the right thing mod 2^32.
  EXPECT_TRUE(Segment::lessThan(0, 1));
  EXPECT_TRUE(Segment::lessThan(1, 2));
  EXPECT_TRUE(Segment::lessThan(0xFFFFFFFF - 1, 0xFFFFFFFF));
  EXPECT_TRUE(Segment::lessThan(0xFFFFFFFF, 1));
  EXPECT_FALSE(Segment::lessThan(0x7FFFFFFF, 0));
  EXPECT_FALSE(Segment::lessThan(1, 0));
}

TEST(segmentcache, segmentMove) {
  // Moving a segment should just work with no asserts.
  Segment seg1{2, {"1234", 4}, true};

  EXPECT_EQ(2, seg1.end());
  EXPECT_EQ(0xfffffffe, seg1.begin());
  EXPECT_EQ(4, seg1.size());
  EXPECT_TRUE(seg1.final());

  Segment seg2{0, {"", 0UL}, false};
  seg2 = std::move(seg1);
  EXPECT_EQ(0, seg1.size());

  EXPECT_EQ(2, seg2.end());
  EXPECT_EQ(0xfffffffe, seg2.begin());
  EXPECT_EQ(4, seg2.size());
  EXPECT_TRUE(seg2.final());

  seg1 = std::move(seg2);
  EXPECT_EQ(0, seg2.size());

  EXPECT_EQ(2, seg1.end());
  EXPECT_EQ(0xfffffffe, seg1.begin());
  EXPECT_EQ(4, seg1.size());
  EXPECT_TRUE(seg1.final());
}

TEST(segmentcache, overlapping1) {
  // Storing two overlapping segments [4, 8) and [6, 10) should merge the 
  // segments.

  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(8, data.toRange(), false);
  cache.store(10, data.toRange(), false);

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("001122332233", seg->data().data(), seg->data().size());
  EXPECT_EQ(4, seg->begin());
  EXPECT_EQ(10, seg->end());
}

TEST(segmentcache, overlapping2) {
  // Storing two overlapping segments [6, 10) and [4, 8) should merge the 
  // segments. (same as overlapping1 except different order).

  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(10, data.toRange(), false);
  cache.store(8, data.toRange(), false);

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("001100112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(4, seg->begin());
  EXPECT_EQ(10, seg->end());
}

TEST(segmentcache, overlapping3) {
  // Storing two overlapping segments [4, 8) and [5, 6) should have no effect.
  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(8, data.toRange(), false);
  cache.store(6, ByteRange{"x", 1}, false);

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
  EXPECT_EQ(4, seg->begin());
  EXPECT_EQ(8, seg->end());
}

TEST(segmentcache, overlapping4) {
  // Storing two overlapping segments [5, 6) and [4, 8) should leave [4, 8),
  // but the "x" byte from the original segment should remain at [5,6).
  SegmentCache cache;
  ByteList data{HexToRawData("00112233")};

  cache.store(6, ByteRange{"x", 1}, false);
  cache.store(8, data.toRange(), false);

  auto seg = cache.current();
  ASSERT_NE(nullptr, seg);

  EXPECT_HEX("00782233", seg->data().data(), seg->data().size());
  EXPECT_EQ(4, seg->begin());
  EXPECT_EQ(8, seg->end());
}
