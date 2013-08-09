#include "ofp/unittest.h"
#include "ofp/bytedata.h"

using namespace ofp;

TEST(bytedata, basics)
{
    ByteData b{"abc", 3};

    EXPECT_EQ(3, b.size());
    EXPECT_NE(nullptr, b.data());

    ByteData b2{std::move(b)};
    EXPECT_EQ(3, b2.size());

    // Check moved from object.
    EXPECT_EQ(nullptr, b.data());
    EXPECT_EQ(0, b.size());
}
