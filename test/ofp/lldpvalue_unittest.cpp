#include "ofp/unittest.h"
#include "ofp/lldpvalue.h"
#include <numeric>

using namespace ofp;

TEST(lldpvalue, constructor) {
    LLDPValue value1;
    EXPECT_HEX("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", &value1, sizeof(value1));
    EXPECT_EQ(0, value1.size());
    EXPECT_EQ(value1.maxSize(), sizeof(value1) - 1);

    LLDPValue value2{{"abcde", 5}};
    EXPECT_HEX("05616263646500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", &value2, sizeof(value2));
    EXPECT_EQ(5, value2.size());

    UInt8 buf[LLDPValue::maxSize() + 32];
    std::iota(buf, buf + sizeof(buf), 0);

    LLDPValue value3{{buf, sizeof(buf)}};
    EXPECT_HEX("3F000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2F303132333435363738393A3B3C3D3E", &value3, sizeof(value3));
    EXPECT_EQ(63, value3.size());
    EXPECT_EQ(LLDPValue::maxSize(), value3.size());
}




