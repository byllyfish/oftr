#include "ofp/unittest.h"
#include "ofp/protocoliterator.h"

using namespace ofp;


TEST(protocoliterator, ProtocolRangeItemCount) {
    UInt64 buffer;
    ByteRange data{&buffer, 0UL};
    const size_t unused = ~0UL;

    EXPECT_EQ(0, detail::ProtocolRangeItemCount(8, data, PROTOCOL_ITERATOR_SIZE_FIXED));

    for (size_t offset = 0; offset < 32; offset += 2) {
        EXPECT_EQ(0, detail::ProtocolRangeItemCount(unused, data, offset));
    }
}
