#include "ofp/protocoliterator.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(protocoliterator, ProtocolRangeItemCount) {
  UInt64 buffer;
  ByteRange data{&buffer, 0UL};
  const size_t unused = ~0UL;

  EXPECT_EQ(
      0, detail::ProtocolRangeItemCount(8, data, PROTOCOL_ITERATOR_SIZE_FIXED));

  for (size_t offset = 0; offset < 32; offset += 2) {
    EXPECT_EQ(0, detail::ProtocolRangeItemCount(unused, data, offset));
  }
}

TEST(protocoliterator, ProtocolRangeSplitOffset) {
  UInt64 buffer = 0;
  ByteRange data{&buffer, 0UL};
  const size_t unused = ~0UL;

  EXPECT_EQ(0, detail::ProtocolRangeSplitOffset(8, unused, data, 0));

  *Big16_cast(&buffer) = 8;
  ByteRange data8{&buffer, sizeof(buffer)};
  EXPECT_EQ(8, detail::ProtocolRangeSplitOffset(8, unused, data8, 0));
}
