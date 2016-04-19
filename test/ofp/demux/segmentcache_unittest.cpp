#include "ofp/unittest.h"
#include "ofp/demux/segmentcache.h"

using namespace ofp;
using namespace ofp::demux;

TEST(segmentcache, testEmpty) {
    // Storing an empty segment [0,0) should leave the cache empty.
    SegmentCache cache;
    cache.store(0, {});
    EXPECT_EQ(nullptr, cache.current());
    EXPECT_EQ(0, cache.size());
}

TEST(segmentcache, test1Segment) {
    // Storing one segment [0,1) should leave one segment.
    SegmentCache cache;
    ByteList data{HexToRawData("55")};

    cache.store(1, data.toRange());
    EXPECT_EQ(1, cache.size());

    auto seg = cache.current();
    ASSERT_NE(nullptr, seg);
    
    EXPECT_HEX("55", seg->data().data(), seg->data().size());
    EXPECT_EQ(0, seg->begin());
    EXPECT_EQ(1, seg->end());
}

TEST(segmentcache, testAdjacentSegments) {
    // Storing two adjacent segments [0,4) and [4, 8) should combine them
    // into one segment.
    SegmentCache cache;
    ByteList data{HexToRawData("00112233")};

    cache.store(4, data.toRange());
    EXPECT_EQ(1, cache.size());

    auto seg = cache.current();
    ASSERT_NE(nullptr, seg);

    EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
    EXPECT_EQ(0, seg->begin());
    EXPECT_EQ(4, seg->end());

    cache.store(8, data.toRange());
    EXPECT_EQ(1, cache.size());

    seg = cache.current();
    ASSERT_NE(nullptr, seg);

    EXPECT_HEX("0011223300112233", seg->data().data(), seg->data().size());
    EXPECT_EQ(0, seg->begin());
    EXPECT_EQ(8, seg->end());
}

TEST(segmentcache, testNonAdjacentSegments) {
    // Storing two non-adjacent segments: [0,4) and [6, 10) should create two
    // segments in the cache. Later, storing [4, 6) should cause all segments
    // to be combined into one segment.
    SegmentCache cache;
    ByteList data{HexToRawData("00112233")};

    cache.store(4, data.toRange());
    EXPECT_EQ(1, cache.size());

    auto seg = cache.current();
    ASSERT_NE(nullptr, seg);

    EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
    EXPECT_EQ(0, seg->begin());
    EXPECT_EQ(4, seg->end());

    cache.store(10, data.toRange());
    EXPECT_EQ(2, cache.size());

    seg = cache.current();
    ASSERT_NE(nullptr, seg);

    EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
    EXPECT_EQ(0, seg->begin());
    EXPECT_EQ(4, seg->end());

    cache.store(6, ByteRange{"  ", 2});
    EXPECT_EQ(1, cache.size());

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
    EXPECT_EQ(1, cache.size());

    auto seg = cache.current();
    ASSERT_NE(nullptr, seg);

    EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
    EXPECT_EQ(kBegin, seg->begin());
    EXPECT_EQ(kBegin + 4, seg->end());

    cache.store(3, data.toRange());
    EXPECT_EQ(1, cache.size());

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
    EXPECT_EQ(1, cache.size());

    auto seg = cache.current();
    ASSERT_NE(nullptr, seg);
    EXPECT_HEX("00112233", seg->data().data(), seg->data().size());
    EXPECT_EQ(0, seg->begin());
    EXPECT_EQ(4, seg->end());

    cache.consume(4);
    EXPECT_EQ(0, cache.size());
    EXPECT_EQ(nullptr, cache.current());
}


TEST(segmentcache, testDrainPartial) {
    SegmentCache cache;
    ByteList data{HexToRawData("00112233")};

    cache.store(4, data.toRange());
    EXPECT_EQ(1, cache.size());

    auto seg = cache.current();
    ASSERT_NE(nullptr, seg);
    EXPECT_EQ(data.toRange(), seg->data());
    EXPECT_EQ(0, seg->begin());
    EXPECT_EQ(4, seg->end());

    cache.consume(2);
    EXPECT_EQ(1, cache.size());

    seg = cache.current();
    ASSERT_NE(nullptr, seg);
    EXPECT_HEX("2233", seg->data().data(), seg->data().size());
    EXPECT_EQ(2, seg->begin());
    EXPECT_EQ(4, seg->end());
}


TEST(segmentcache, lessThan) {
    EXPECT_TRUE(Segment::lessThan(0, 1));
    EXPECT_TRUE(Segment::lessThan(1, 2));
    EXPECT_TRUE(Segment::lessThan(0xFFFFFFFF - 1, 0xFFFFFFFF));
    EXPECT_TRUE(Segment::lessThan(0xFFFFFFFF, 1));
    EXPECT_FALSE(Segment::lessThan(0x7FFFFFFF, 0));
    EXPECT_FALSE(Segment::lessThan(1, 0));
}
