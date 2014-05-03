#include "ofp/unittest.h"
#include "ofp/protocolrange.h"

using namespace ofp;
using namespace ofp::detail;

TEST(protocolrange, valid_empty) {
  ByteRange r1;
  EXPECT_TRUE(IsProtocolRangeValid(0, r1, 2));

  UInt64 d2 = 0;
  ByteRange r2{&d2, 0};
  EXPECT_TRUE(IsProtocolRangeValid(0, r2, 2));
}

TEST(protocolrange, invalid_empty) {
  UInt8 d1[2];
  ByteRange r1{&d1[1], 0};
  EXPECT_FALSE(IsProtocolRangeValid(0, r1, 2));
}

TEST(protocolrange, invalid_short) {
  Big16 d1 = 0xff01;
  Big64 d2 = 0xffff0001ffffffff;
  Big64 d3 = 0xffff0000ffffffff;

  ByteRange r1{&d1, 2};
  ByteRange r2{&d2, 8};
  ByteRange r3{&d3, 8};

  EXPECT_FALSE(IsProtocolRangeValid(0, r1, 2));
  EXPECT_FALSE(IsProtocolRangeValid(0, r2, 2));
  EXPECT_FALSE(IsProtocolRangeValid(0, r3, 2));
}

TEST(protocolrange, valid_short) {
  Big64 d1 = 0xffff0004ffffffff;
  Big64 d2 = 0xffff0005ffffffff;
  Big64 d3 = 0xffff0007ffffffff;

  ByteRange r1{&d1, 8};
  ByteRange r2{&d2, 8};
  ByteRange r3{&d3, 8};

  EXPECT_TRUE(IsProtocolRangeValid(0, r1, 2));
  EXPECT_TRUE(IsProtocolRangeValid(0, r2, 2));
  EXPECT_TRUE(IsProtocolRangeValid(0, r3, 2));
}

TEST(protocolrange, valid_exact) {
  Big64 d1 = 0xffff0008ffffffff;
  ByteRange r1{&d1, 8};

  EXPECT_TRUE(IsProtocolRangeValid(0, r1, 2));
}

TEST(protocolrange, invalid1) {
  // Need to convert to ByteList for alignment; string data not necessarily
  // aligned to an 8-byte boundary.

  ByteList r1{HexToRawData("FFFFFFFFFFFFFFFFFF")};
  ByteList r2{HexToRawData("FFFF0009FFFFFFFF")};
  ByteList r3{HexToRawData("FFFFFFFFFFFFFFFF")};

  EXPECT_FALSE(IsProtocolRangeValid(0, r1, 2));
  EXPECT_FALSE(IsProtocolRangeValid(0, r2, 2));
  EXPECT_FALSE(IsProtocolRangeValid(0, r3, 2));
}

TEST(protocolrange, invalid2) {
  ByteList r1{HexToRawData("ffff0008ffffffffff")};
  ByteList r2{HexToRawData("ffff0006ffffffffffffffffffffffff")};

  EXPECT_FALSE(IsProtocolRangeValid(0, r1, 2));
  EXPECT_FALSE(IsProtocolRangeValid(0, r2, 2));
}

TEST(protocolrange, iteration) {
  Big64 data[] = {0xffff000800000001, 0xffff000800000002,
                  0xffff000800000003, 0xffff000800000004};

  struct Item : private NonCopyable {
    enum { ProtocolIteratorSizeOffset = 2,
           ProtocolIteratorAlignment = 8 };
    Big16 type;
    Big16 len;
    Big32 value;
  };

  ByteRange r1{data, sizeof(data)};
  EXPECT_TRUE(IsProtocolRangeValid(0, r1, 2));

  ProtocolRange<ProtocolIterator<Item>> iterable{r1};
  EXPECT_TRUE(iterable.validateInput(""));
  EXPECT_EQ(4, iterable.itemCount());
  
  unsigned sum = 10;

  // Two ways to iterate:
 
  // (1) Use a const reference.
  for (auto &elem : iterable) {
    sum += elem.value;
  }
  EXPECT_EQ(20, sum);

  // (2) Iterate using pointers.
  for (auto iter = iterable.begin(); iter != iterable.end(); ++iter) {
    sum += iter->value;
    sum += (*iter).value;
  }
  EXPECT_EQ(40, sum);

  // Wrong: Assignment in a for loop doesn't compile...
  // for (auto elem : iterable) {
  //   sum += elem.value;
  // }
}
