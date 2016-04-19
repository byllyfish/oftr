#include "ofp/unittest.h"
#include "ofp/demux/flowcache.h"

using namespace ofp;
using namespace ofp::demux;


TEST(flowcache, initialSegment) {
    FlowCache cache;

    Timestamp ts = Timestamp::now();
    IPv6Endpoint src{"127.0.0.1:8000"};
    IPv6Endpoint dst{"127.0.0.1:8001"};

    auto data = cache.receive(ts, src, dst, 0, {}, TCP_SYN);
    EXPECT_EQ(0, data.size());
    EXPECT_EQ(1, cache.size());
    data.consume(0);

    auto state = cache.lookup(src, dst);
    ASSERT_NE(nullptr, state);
    EXPECT_EQ(0, state->end());
    EXPECT_EQ(ts, state->first());
    EXPECT_TRUE(state->empty());
}


TEST(flowcache, twoDataSegments) {
    FlowCache cache;

    Timestamp ts = Timestamp::now();
    IPv6Endpoint src{"127.0.0.1:8000"};
    IPv6Endpoint dst{"127.0.0.1:8001"};
    ByteList buf{ HexToRawData("00112233")};

    {   // Initial empty packet [0, 0) to set up sequence numbers.
        auto data = cache.receive(ts, src, dst, 0, {}, TCP_SYN);
        EXPECT_EQ(0, data.size());
    }

    EXPECT_EQ(1, cache.size());

    {   // Segment [0, 4) from src to dst.
        auto data = cache.receive(ts, src, dst, 4, buf.toRange());
        EXPECT_EQ(4, data.size());
        EXPECT_HEX("00112233", data.data(), data.size());
        data.consume(4);

        auto state = cache.lookup(src, dst);
        ASSERT_NE(nullptr, state);
        EXPECT_EQ(4, state->end());
        EXPECT_TRUE(state->empty());
    }

    {   // Segment [4, 8) from src to dst.
        auto data = cache.receive(ts, src, dst, 8, buf.toRange());
        EXPECT_EQ(4, data.size());
        EXPECT_HEX("00112233", data.data(), data.size());
        data.consume(4);

        auto state = cache.lookup(src, dst);
        ASSERT_NE(nullptr, state);
        EXPECT_EQ(8, state->end());
        EXPECT_TRUE(state->empty());
    }
}

TEST(flowcache, partialSegments) {
    FlowCache cache;

    Timestamp ts = Timestamp::now();
    IPv6Endpoint src{"127.0.0.1:8000"};
    IPv6Endpoint dst{"127.0.0.1:8001"};
    ByteList buf{ HexToRawData("00112233")};

    {   // Initial packet to set up sequence numbers.
        auto data = cache.receive(ts, src, dst, 0, {}, TCP_SYN);
        EXPECT_EQ(0, data.size());
    }

    {   // Segment [0,4) from src to dst.
        auto data = cache.receive(ts, src, dst, 4, buf.toRange());
        ASSERT_EQ(4, data.size());
        EXPECT_HEX("00112233", data.data(), data.size());

        // Only 3 bytes used. Make sure that the leftover byte is cached.
        data.consume(3);

        auto state = cache.lookup(src, dst);
        ASSERT_NE(nullptr, state);
        EXPECT_EQ(3, state->end());
        EXPECT_EQ(ts, state->first());
        EXPECT_FALSE(state->empty());
    }

    {   // Segment [4, 8) from src to dst.
        auto data = cache.receive(ts, src, dst, 8, buf.toRange());
        ASSERT_EQ(5, data.size());
        EXPECT_HEX("3300112233", data.data(), data.size());

        // Consume all 5 bytes.
        data.consume(5);
        
        auto state = cache.lookup(src, dst);
        ASSERT_NE(nullptr, state);
        EXPECT_EQ(8, state->end());
        EXPECT_TRUE(state->empty());
    }
}


