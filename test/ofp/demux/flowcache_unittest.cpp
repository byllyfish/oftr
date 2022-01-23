// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/flowcache.h"

#include "ofp/unittest.h"

using namespace ofp;
using namespace ofp::demux;

TEST(flowcache, initialSegment) {
  FlowCache cache;

  Timestamp ts = Timestamp::now();
  IPv6Endpoint src{"127.0.0.1:8000"};
  IPv6Endpoint dst{"127.0.0.1:8001"};

  // Initial empty packet [1, 1) to set up sequence numbers (ISN=0)
  auto data = cache.receive(ts, src, dst, 0, {}, TCP_SYN);
  EXPECT_EQ(0, data.size());
  EXPECT_EQ(1, cache.size());
  data.consume(0);

  // Check the flow state.
  auto state = cache.lookup(src, dst);
  ASSERT_NE(nullptr, state);
  EXPECT_EQ(1, state->end());
  EXPECT_TRUE(state->empty());

  // Check the cache entry's timestamps and sessionID.
  auto entry = cache.findEntry(src, dst);
  ASSERT_NE(nullptr, entry);
  EXPECT_EQ(ts, entry->firstSeen);
  EXPECT_EQ(ts, entry->lastSeen);
  EXPECT_EQ(1, entry->sessionID);
}

TEST(flowcache, twoDataSegments) {
  FlowCache cache;

  Timestamp ts = Timestamp::now();
  IPv6Endpoint src{"127.0.0.1:8000"};
  IPv6Endpoint dst{"127.0.0.1:8001"};
  ByteList buf{HexToRawData("00112233")};

  {  // Initial empty packet [1, 1) to set up sequence numbers (ISN=0)
    auto data = cache.receive(ts, src, dst, 0, {}, TCP_SYN);
    EXPECT_EQ(0, data.size());
  }

  EXPECT_EQ(1, cache.size());

  {  // Segment [1, 5) from src to dst.
    auto data = cache.receive(ts, src, dst, 1, buf.toRange());
    EXPECT_EQ(4, data.size());
    EXPECT_HEX("00112233", data.data(), data.size());
    data.consume(4);

    auto state = cache.lookup(src, dst);
    ASSERT_NE(nullptr, state);
    EXPECT_EQ(5, state->end());
    EXPECT_TRUE(state->empty());
  }

  {  // Segment [5, 9) from src to dst.
    auto data = cache.receive(ts, src, dst, 5, buf.toRange());
    EXPECT_EQ(4, data.size());
    EXPECT_HEX("00112233", data.data(), data.size());
    data.consume(4);

    auto state = cache.lookup(src, dst);
    ASSERT_NE(nullptr, state);
    EXPECT_EQ(9, state->end());
    EXPECT_TRUE(state->empty());
  }
}

TEST(flowcache, partialSegments) {
  FlowCache cache;

  Timestamp ts = Timestamp::now();
  IPv6Endpoint src{"127.0.0.1:8000"};
  IPv6Endpoint dst{"127.0.0.1:8001"};
  ByteList buf{HexToRawData("00112233")};

  {  // Initial empty packet [1, 1) to set up sequence numbers (ISN=0)
    auto data = cache.receive(ts, src, dst, 0, {}, TCP_SYN);
    EXPECT_EQ(0, data.size());
  }

  {  // Segment [1,5) from src to dst.
    auto data = cache.receive(ts, src, dst, 1, buf.toRange());
    ASSERT_EQ(4, data.size());
    EXPECT_HEX("00112233", data.data(), data.size());

    // Only 3 bytes used. Make sure that the leftover byte is cached.
    data.consume(3);

    auto state = cache.lookup(src, dst);
    ASSERT_NE(nullptr, state);
    EXPECT_EQ(4, state->end());
    EXPECT_FALSE(state->empty());
  }

  {  // Segment [5, 9) from src to dst.
    auto data = cache.receive(ts, src, dst, 5, buf.toRange());
    ASSERT_EQ(5, data.size());
    EXPECT_HEX("3300112233", data.data(), data.size());

    // Consume all 5 bytes.
    data.consume(5);

    auto state = cache.lookup(src, dst);
    ASSERT_NE(nullptr, state);
    EXPECT_EQ(9, state->end());
    EXPECT_TRUE(state->empty());
  }
}

TEST(flowcache, firstSegmentPriority) {
  // The first segment of a flow determines the initial sequence number.
  // Segments from earlier in the flow that arrive after the initial segment
  // are always ignored.

  FlowCache cache;

  Timestamp ts = Timestamp::now();
  IPv6Endpoint src{"127.0.0.1:8000"};
  IPv6Endpoint dst{"127.0.0.1:8001"};
  ByteList buf{HexToRawData("00112233")};

  {  // Segment [4, 8) from src to dst.
    auto data = cache.receive(ts, src, dst, 4, buf.toRange(), TCP_ACK);
    ASSERT_EQ(4, data.size());
    data.consume(0);
  }

  {  // Segment [0, 4) from src to dst. Should be ignored.
    auto data = cache.receive(ts, src, dst, 0, buf.toRange(), TCP_ACK);
    ASSERT_EQ(0, data.size());
    data.consume(0);
  }

  {  // Segment [8, 12) from src to dst.
    auto data = cache.receive(ts, src, dst, 8, buf.toRange(), TCP_ACK);
    ASSERT_EQ(8, data.size());
    data.consume(8);
  }

  auto state = cache.lookup(src, dst);
  ASSERT_NE(nullptr, state);
  EXPECT_EQ(12, state->end());
  EXPECT_TRUE(state->empty());
}

TEST(flowcache, lateSegmentFinishedFlow) {
  // When a segment arrives after both bidirectional flows have sent FIN's, it
  // should be ignored. Late segments that arrive 120 seconds or more after
  // closure should begin a brand new session.
  FlowCache cache;

  Timestamp ts = Timestamp::now();
  IPv6Endpoint src{"[::1]:8000"};
  IPv6Endpoint dst{"[::2]:7999"};
  ByteList buf{HexToRawData("00112233")};

  {  // SYN from src to dst with SEQ=1 --> ISN=2.
    auto data = cache.receive(ts, src, dst, 1, {}, TCP_SYN);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  {  // SYN-ACK from dst to src with SEQ=1000 --> ISN=1001.
    auto data = cache.receive(ts, dst, src, 1000, {}, TCP_SYNACK);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  {  // Segment [2, 6) from src to dst.
    auto data = cache.receive(ts, src, dst, 2, buf.toRange(), TCP_ACK);
    ASSERT_EQ(4, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
    data.consume(4);
  }

  {  // Empty segment [1001, 1001) from dst to src. Empty ACK should return a
    // sessionID of 0.
    auto data = cache.receive(ts, dst, src, 1001, {}, TCP_ACK);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(0, data.sessionID());
  }

  {  // FIN from src to dst with SEQ=6.
    auto data = cache.receive(ts, src, dst, 6, {}, TCP_FIN);
    ASSERT_EQ(0, data.size());
    EXPECT_TRUE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  {  // Segment [1001,1005) from dst to src.
    auto data = cache.receive(ts, dst, src, 1001, buf.toRange(), TCP_ACK);
    ASSERT_EQ(4, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
    data.consume(4);
  }

  {  // FIN from dst to src with SEQ=1005.
    auto data = cache.receive(ts, dst, src, 1005, {}, TCP_FIN);
    ASSERT_EQ(0, data.size());
    EXPECT_TRUE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  ts.addSeconds(10);

  {  // Segment [2, 6) from src to dst 10 seconds later. This should not open
    // a new session.
    auto data = cache.receive(ts, src, dst, 2, buf.toRange(), TCP_ACK);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  ts.addSeconds(110);

  {  // Segment [2, 6) from src to dst 110 seconds later (120 seconds total).
    // This should open a new session even though there is no SYN or SYN-ACK.
    auto data = cache.receive(ts, src, dst, 2, buf.toRange(), TCP_ACK);
    ASSERT_EQ(4, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(2, data.sessionID());

    data.consume(4);
  }
}

TEST(flowcache, lateSegmentHalfFinishedFlow) {
  // When a segment arrives after one side has sent a FIN, but not the other,
  // the segment should be ignored. There is currently no timeout.

  // This code is similar to `lateSegmentFinishedFlow` except one FIN is
  // commented out. There are additional tests at the end; the only way to
  // create a new session when there is an unfinished session is a SYN pkt.

  FlowCache cache;

  Timestamp ts = Timestamp::now();
  IPv6Endpoint src{"[::1]:8000"};
  IPv6Endpoint dst{"[::2]:7999"};
  ByteList buf{HexToRawData("00112233")};

  {  // SYN from src to dst with SEQ=1 --> ISN=2.
    auto data = cache.receive(ts, src, dst, 1, {}, TCP_SYN);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  {  // SYN-ACK from dst to src with SEQ=1000 --> ISN=1001.
    auto data = cache.receive(ts, dst, src, 1000, {}, TCP_SYNACK);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  {  // Segment [2, 6) from src to dst.
    auto data = cache.receive(ts, src, dst, 2, buf.toRange(), TCP_ACK);
    ASSERT_EQ(4, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
    data.consume(4);
  }

  {  // Empty segment [1001, 1001) from dst to src. Empty ACK should return a
    // sessionID of 0.
    auto data = cache.receive(ts, dst, src, 1001, {}, TCP_ACK);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(0, data.sessionID());
  }

  {  // FIN from src to dst with SEQ=6.
    auto data = cache.receive(ts, src, dst, 6, {}, TCP_FIN);
    ASSERT_EQ(0, data.size());
    EXPECT_TRUE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  {  // Segment [1001,1005) from dst to src.
    auto data = cache.receive(ts, dst, src, 1001, buf.toRange(), TCP_ACK);
    ASSERT_EQ(4, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
    data.consume(4);
  }

  /**********************
  // This FIN is commented out intentionally to imagine this segment was lost.
    { // FIN from dst to src with SEQ=1005.
      auto data = cache.receive(ts, dst, src, 1005, {}, TCP_FIN);
      ASSERT_EQ(0, data.size());
      EXPECT_TRUE(data.final());
      EXPECT_EQ(1, data.sessionID());
    }
  ***********************/

  ts.addSeconds(10);

  {  // Segment [2, 6) from src to dst 10 seconds later. This should not open
    // a new session.
    auto data = cache.receive(ts, src, dst, 2, buf.toRange(), TCP_ACK);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  ts.addSeconds(110);

  {  // Segment [2, 6) from src to dst 110 seconds later (120 seconds total).
    // Still, this should not open a new session (current one is half-finished).
    auto data = cache.receive(ts, src, dst, 2, buf.toRange(), TCP_ACK);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  ts.addSeconds(1000);

  {  // Segment [2, 6) from src to dst 110 seconds later (120 seconds total).
    // Still, this should not open a new session (current one is half-finished).
    auto data = cache.receive(ts, src, dst, 2, buf.toRange(), TCP_ACK);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(1, data.sessionID());
  }

  {  // Segment [2, 6) from src to dst with invalid SYN flag. SYN packets are
     // not
    // allowed to carry data. This should not open a new session.
    auto data = cache.receive(ts, src, dst, 2, buf.toRange(), TCP_SYN);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(0, data.sessionID());
  }

  {  // Segment with SEQ=10. ISN=11.
    auto data = cache.receive(ts, src, dst, 10, {}, TCP_SYN);
    ASSERT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(2, data.sessionID());
  }
}
