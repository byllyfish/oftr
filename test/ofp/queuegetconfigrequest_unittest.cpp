#include "ofp/unittest.h"
#include "ofp/queuegetconfigrequest.h"

using namespace ofp;

TEST(queuegetconfigrequest, builderV4) {
    QueueGetConfigRequestBuilder req;

    req.setPort(0x22222222);

    MemoryChannel channel{OFP_VERSION_4};
    req.send(&channel);

    EXPECT_EQ(0x10, channel.size());
    EXPECT_HEX("04160010000000012222222200000000", channel.data(), channel.size());
}
