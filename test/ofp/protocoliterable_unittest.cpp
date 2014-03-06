#include "ofp/unittest.h"
#include "ofp/protocoliterable.h"

using namespace ofp;
using namespace ofp::detail;

TEST(protocoliterable, valid_empty) {
  ByteRange r1;
  EXPECT_TRUE(IsProtocolIteratorValid(r1));

  UInt64 d2 = 0;
  ByteRange r2{&d2, 0};
  EXPECT_TRUE(IsProtocolIteratorValid(r2));
}

TEST(protocoliterable, invalid_empty) {
  UInt8 d1[2];
  ByteRange r1{&d1[1], 0};
  EXPECT_FALSE(IsProtocolIteratorValid(r1));
}

TEST(protocoliterable, invalid_short) {
  Big16 d1 = 0xff01;
  Big64 d2 = 0xffff0001ffffffff;
  Big64 d3 = 0xffff0000ffffffff;

  ByteRange r1{&d1, 2};
  ByteRange r2{&d2, 8};
  ByteRange r3{&d3, 8};

  EXPECT_FALSE(IsProtocolIteratorValid(r1));
  EXPECT_FALSE(IsProtocolIteratorValid(r2));
  EXPECT_FALSE(IsProtocolIteratorValid(r3));
}

TEST(protocoliterable, valid_short) {
  Big64 d1 = 0xffff0004ffffffff;
  Big64 d2 = 0xffff0005ffffffff;
  Big64 d3 = 0xffff0007ffffffff;

  ByteRange r1{&d1, 8};
  ByteRange r2{&d2, 8};
  ByteRange r3{&d3, 8};

  EXPECT_TRUE(IsProtocolIteratorValid(r1));
  EXPECT_TRUE(IsProtocolIteratorValid(r2));
  EXPECT_TRUE(IsProtocolIteratorValid(r3));
}

TEST(protocoliterable, valid_exact) {
  Big64 d1 = 0xffff0008ffffffff;
  ByteRange r1{&d1, 8};

  EXPECT_TRUE(IsProtocolIteratorValid(r1));
}

TEST(protocoliterable, invalid1) {
  // Need to convert to ByteList for alignment; string data not necessarily
  // aligned to an 8-byte boundary.

  ByteList r1{HexToRawData("FFFFFFFFFFFFFFFFFF")};
  ByteList r2{HexToRawData("FFFF0009FFFFFFFF")};
  ByteList r3{HexToRawData("FFFFFFFFFFFFFFFF")};

  EXPECT_FALSE(IsProtocolIteratorValid(r1));
  EXPECT_FALSE(IsProtocolIteratorValid(r2));
  EXPECT_FALSE(IsProtocolIteratorValid(r3));
}

TEST(protocoliterable, invalid2) {
  ByteList r1{HexToRawData("ffff0008ffffffffff")};
  ByteList r2{HexToRawData("ffff0006ffffffffffffffffffffffff")};

  EXPECT_FALSE(IsProtocolIteratorValid(r1));
  EXPECT_FALSE(IsProtocolIteratorValid(r2));
}

TEST(protocoliterable, iteration) {
  Big64 data[] = {0xffff000800000001, 0xffff000800000002,
                  0xffff000800000003, 0xffff000800000004};

  struct Item {
    Big16 type;
    Big16 len;
    Big32 value;
  };

  ByteRange r1{data, sizeof(data)};
  EXPECT_TRUE(IsProtocolIteratorValid(r1));

  ProtocolIterable<Item> iterable{r1};
  EXPECT_TRUE(iterable.validateInput(""));
  EXPECT_EQ(4, iterable.itemCount());
  
  unsigned sum = 0;

  // Three different ways to iterate.
  // (1) Make a copy.
  for (auto elem : iterable) {
    sum += elem.value;
    elem.value = elem.value + 1; // Assignment to local copy (no effect).
  }
  EXPECT_EQ(10, sum);

  // (2) Use a const reference.
  for (auto &elem : iterable) {
    sum += elem.value;
  }
  EXPECT_EQ(20, sum);

  // (3) Iterate using pointers.
  for (auto iter = iterable.begin(); iter != iterable.end(); ++iter) {
    sum += iter->value;
    sum += (*iter).value;
  }
  EXPECT_EQ(40, sum);
}
