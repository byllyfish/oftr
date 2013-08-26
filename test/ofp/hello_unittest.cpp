#include "ofp/unittest.h"
#include "ofp/hello.h"

using namespace ofp;

TEST(hello, HelloBuilder1)
{
    HelloBuilder msg{ProtocolVersions{1, 2, 3, 4}};

    auto buf = MockChannel::serialize(msg, 99);

    EXPECT_EQ(16, buf.size());
    EXPECT_HEX("0400-0010-00000001 00010008-0000001E", buf.data(), buf.size());
}

TEST(hello, HelloBuilder2)
{
    HelloBuilder msg{ProtocolVersions{1, 4}};

    auto buf = MockChannel::serialize(msg, 1);

    EXPECT_EQ(16, buf.size());
    EXPECT_HEX("0400-0010-00000001 00010008-00000012", buf.data(), buf.size());
}

TEST(hello, HelloBuilder3)
{
    HelloBuilder msg{ProtocolVersions{1}};

    auto buf = MockChannel::serialize(msg, 4);

    EXPECT_EQ(8, buf.size());
    EXPECT_HEX("0100-0008-00000001", buf.data(), buf.size());
}
