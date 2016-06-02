// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/flowstate.h"
#include "ofp/unittest.h"

using namespace ofp;
using namespace ofp::demux;

TEST(flowstate, basicTest) {
  FlowState state;
  ByteList buf{HexToRawData("00112233")};
  Timestamp ts = Timestamp::now();
  const UInt64 kSessionID = 1;
  bool kNotFinal = false;

  {  // Receive segment [0, 0). Consume 0 bytes.
    Timestamp lastSeen;
    auto data = state.receive(ts, 0, {}, kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(0, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_EQ(ts, lastSeen);
    data.consume(0);
  }

  {  // Receive segment [0, 4). Consume 4 bytes.
    Timestamp lastSeen;
    auto data =
        state.receive(ts, 4, buf.toRange(), kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(4, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_HEX("00112233", data.data(), data.size());
    EXPECT_EQ(ts, lastSeen);
    data.consume(4);
  }

  {  // Receive segment [4, 8). Consume only 3 bytes.
    Timestamp lastSeen;
    auto data =
        state.receive(ts, 8, buf.toRange(), kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(4, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_HEX("00112233", data.data(), data.size());
    EXPECT_EQ(ts, lastSeen);
    data.consume(3);
  }

  {  // Receive segment [8, 12). Consume all 5 bytes. Final segment.
    Timestamp lastSeen;
    auto data =
        state.receive(ts, 12, buf.toRange(), kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(5, data.size());
    EXPECT_FALSE(data.final());
    EXPECT_HEX("3300112233", data.data(), data.size());
    EXPECT_EQ(ts, lastSeen);
    data.consume(5);
  }

  {  // Receive segment [12, 12). Final segment.
    Timestamp lastSeen;
    auto data = state.receive(ts, 12, {}, kSessionID, true, &lastSeen);
    EXPECT_EQ(0, data.size());
    EXPECT_TRUE(data.final());
    EXPECT_EQ(ts, lastSeen);
    data.consume(0);
  }
}

TEST(flowstate, outOfOrderTest) {
  FlowState state;
  ByteList buf{HexToRawData("00112233")};
  Timestamp ts = Timestamp::now();
  const UInt64 kSessionID = 1;
  bool kNotFinal = false;

  {  // Receive segment [4, 4). Consume 0 bytes.
    Timestamp lastSeen;
    auto data = state.receive(ts, 4, {}, kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(0, data.size());
    EXPECT_EQ(ts, lastSeen);
    data.consume(0);
  }

  EXPECT_TRUE(state.empty());

  {  // Receive segment [8, 12). Consume 0 bytes.
    Timestamp lastSeen;
    auto data =
        state.receive(ts, 12, buf.toRange(), kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(0, data.size());
    EXPECT_EQ(ts, lastSeen);
    data.consume(0);
  }

  {  // Receive segment [0, 4). Consume 0 bytes. (should be discarded)
    Timestamp lastSeen;
    auto data =
        state.receive(ts, 4, buf.toRange(), kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(0, data.size());
    EXPECT_FALSE(lastSeen.valid());
    data.consume(0);
  }

  {  // Receive segment [12, 16). Consume 0 bytes.
    Timestamp lastSeen;
    auto data =
        state.receive(ts, 16, buf.toRange(), kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(0, data.size());
    EXPECT_EQ(ts, lastSeen);
    data.consume(0);
  }

  {  // Receive segment [4, 8). Consume 8 bytes.
    Timestamp lastSeen;
    auto data =
        state.receive(ts, 8, buf.toRange(), kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(12, data.size());
    EXPECT_HEX("001122330011223300112233", data.data(), data.size());
    EXPECT_EQ(ts, lastSeen);
    data.consume(8);
  }

  // Turn back a second.
  ts.addSeconds(-1);

  {  // Receive segment [16, 17). Consume 5 bytes.
    Timestamp lastSeen;
    ByteRange seg{" ", 1};
    auto data = state.receive(ts, 17, seg, kSessionID, kNotFinal, &lastSeen);
    EXPECT_EQ(5, data.size());
    EXPECT_HEX("0011223320", data.data(), data.size());
    // Should still return timestamp of last seen segment (not the max).
    EXPECT_EQ(ts, lastSeen);
    data.consume(5);
  }

  EXPECT_TRUE(state.empty());
}
