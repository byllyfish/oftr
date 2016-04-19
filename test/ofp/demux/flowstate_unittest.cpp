#include "ofp/unittest.h"
#include "ofp/demux/flowstate.h"

using namespace ofp;
using namespace ofp::demux;

TEST(flowstate, basicTest) {
    FlowState state;
    ByteList buf{HexToRawData("00112233")};
    Timestamp ts = Timestamp::now();
    const UInt64 kSessionID = 1;

    {   // Receive segment [0, 0). Consume 0 bytes.
        auto data = state.receive(ts, 0, {}, kSessionID);
        EXPECT_EQ(0, data.size());
        data.consume(0);
    }

    {   // Receive segment [0, 4). Consume 4 bytes.
        auto data = state.receive(ts, 4, buf.toRange(), kSessionID);
        EXPECT_EQ(4, data.size());
        EXPECT_HEX("00112233", data.data(), data.size());
        data.consume(4);
    }

    {   // Receive segment [4, 8). Consume only 3 bytes.
        auto data = state.receive(ts, 8, buf.toRange(), kSessionID);
        EXPECT_EQ(4, data.size());
        EXPECT_HEX("00112233", data.data(), data.size());
        data.consume(3);
    }

    {   // Receive segment [8, 12). Consume all 5 bytes.
        auto data = state.receive(ts, 12, buf.toRange(), kSessionID);
        EXPECT_EQ(5, data.size());
        EXPECT_HEX("3300112233", data.data(), data.size());
        data.consume(5);
    }
}

TEST(flowstate, outOfOrderTest) {
    FlowState state;
    ByteList buf{HexToRawData("00112233")};
    Timestamp ts = Timestamp::now();
    const UInt64 kSessionID = 1;

    {   // Receive segment [4, 4). Consume 0 bytes.
        auto data = state.receive(ts, 4, {}, kSessionID);
        EXPECT_EQ(0, data.size());
        data.consume(0);
    }

    EXPECT_TRUE(state.empty());

    {   // Receive segment [8, 12). Consume 0 bytes.
        auto data = state.receive(ts, 12, buf.toRange(), kSessionID);
        EXPECT_EQ(0, data.size());
        data.consume(0);
    }

    {   // Receive segment [0, 4). Consume 0 bytes. (should be discarded)
        auto data = state.receive(ts, 4, buf.toRange(), kSessionID);
        EXPECT_EQ(0, data.size());
        data.consume(0);
    }

    {   // Receive segment [12, 16). Consume 0 bytes.
        auto data = state.receive(ts, 16, buf.toRange(), kSessionID);
        EXPECT_EQ(0, data.size());
        data.consume(0);
    }

    {   // Receive segment [4, 8). Consume 8 bytes.
        auto data = state.receive(ts, 8, buf.toRange(), kSessionID);
        EXPECT_EQ(12, data.size());
        EXPECT_HEX("001122330011223300112233", data.data(), data.size());
        data.consume(8);        
    }

    {   // Receive segment [16, 17). Consume 5 bytes.
        ByteRange seg{" ", 1};
        auto data = state.receive(ts, 17, seg, kSessionID);
        EXPECT_EQ(5, data.size());
        EXPECT_HEX("0011223320", data.data(), data.size());
        data.consume(5);
    }

    EXPECT_TRUE(state.empty());
}