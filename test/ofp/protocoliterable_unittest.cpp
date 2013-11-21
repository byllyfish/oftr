#include "ofp/unittest.h"
#include "ofp/protocoliterable.h"

using namespace ofp;
using namespace ofp::detail;

TEST(protocoliterable, valid_empty) {
  ByteRange r1;
  EXPECT_TRUE(IsProtocolIterableValid(r1));

  UInt64 d2 = 0;
  ByteRange r2{&d2, 0};
  EXPECT_TRUE(IsProtocolIterableValid(r2));
}

TEST(protocoliterable, invalid_empty) {
  UInt8 d1[2];
  ByteRange r1{&d1[1], 0};
  EXPECT_FALSE(IsProtocolIterableValid(r1));
}

TEST(protocoliterable, invalid_short) {
  Big16 d1 = 0xff01;
  Big64 d2 = 0xffff0001ffffffff;
  Big64 d3 = 0xffff0000ffffffff;

  ByteRange r1{&d1, 2};
  ByteRange r2{&d2, 8};
  ByteRange r3{&d3, 8};

  EXPECT_FALSE(IsProtocolIterableValid(r1));
  EXPECT_FALSE(IsProtocolIterableValid(r2));
  EXPECT_FALSE(IsProtocolIterableValid(r3));
}

TEST(protocoliterable, valid_short) {
  Big64 d1 = 0xffff0004ffffffff;
  Big64 d2 = 0xffff0005ffffffff;
  Big64 d3 = 0xffff0007ffffffff;

  ByteRange r1{&d1, 8};
  ByteRange r2{&d2, 8};
  ByteRange r3{&d3, 8};

  EXPECT_TRUE(IsProtocolIterableValid(r1));
  EXPECT_TRUE(IsProtocolIterableValid(r2));
  EXPECT_TRUE(IsProtocolIterableValid(r3));
}

TEST(protocoliterable, valid_exact) {
  Big64 d1 = 0xffff0008ffffffff;
  ByteRange r1{&d1, 8};

  EXPECT_TRUE(IsProtocolIterableValid(r1));
}

TEST(protocoliterable, invalid1) {
  auto d1 = ByteRange{HexToRawData("FFFFFFFFFFFFFFFFFF")};
  auto d2 = ByteRange{HexToRawData("FFFF0009FFFFFFFF")};
  auto d3 = ByteRange{HexToRawData("FFFFFFFFFFFFFFFF")};

  // Need to convert to ByteList for alignment; string data not necessarily
  // aligned to an 8-byte boundary.

  ByteList r1{d1};
  ByteList r2{d2};
  ByteList r3{d3};

  EXPECT_FALSE(IsProtocolIterableValid(r1));
  EXPECT_FALSE(IsProtocolIterableValid(r2));
  EXPECT_FALSE(IsProtocolIterableValid(r3));
}

TEST(protocoliterable, invalid2) {
  auto d1 = ByteRange{HexToRawData("ffff0008ffffffffff")};
  auto d2 = ByteRange{HexToRawData("ffff0006ffffffffffffffffffffffff")};

  ByteList r1{d1};
  ByteList r2{d2};

  EXPECT_FALSE(IsProtocolIterableValid(r1));
  EXPECT_FALSE(IsProtocolIterableValid(r2));
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
  EXPECT_TRUE(IsProtocolIterableValid(r1));

  ProtocolIterable<Item> iterable{r1};
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
