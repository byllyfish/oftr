#include "ofp/unittest.h"
#include "ofp/hash.h"
#include <array>

using namespace ofp;

// Test MurmurHash32 by comparing results to output of this python program:
// 
// ```
// import mmh3
//
// seed = 29
// keys = [b'\x00', b'\x01\x02\x03\x04', b'\x01\x02\x03\x04\x05', 
//   b'\x01\x02\x03\x04\x05\x06', b'\x01\x02\x03\x04\x05\x06\x07', 
//   b'\x01\x02\x03\x04\x05\x06\x07\x08']
// 
// for key in keys:
//   print(hex(mmh3.hash(key, seed) & 0xffffffff))
// ```
// 
// Output is:
// 
//   0x4289c4e0
//   0x148189dd
//   0x575a633e
//   0xf6fb78f6
//   0xd247e646
//   0xd82dd722

TEST(hash, MurmurHash32) {
    UInt8 t1 = 0;
    EXPECT_EQ(0x4289c4e0, ofp::hash::MurmurHash32(&t1));

    std::array<UInt8, 4> t4 = {{ 1, 2, 3, 4 }};
    EXPECT_EQ(0x148189dd, ofp::hash::MurmurHash32(&t4));

    std::array<UInt8, 5> t5 = {{ 1, 2, 3, 4, 5 }};
    EXPECT_EQ(0x575a633e, ofp::hash::MurmurHash32(&t5));

    std::array<UInt8, 6> t6 = {{ 1, 2, 3, 4, 5, 6 }};
    EXPECT_EQ(0xf6fb78f6, ofp::hash::MurmurHash32(&t6));

    std::array<UInt8, 7> t7 = {{ 1, 2, 3, 4, 5, 6, 7 }};
    EXPECT_EQ(0xd247e646, ofp::hash::MurmurHash32(&t7));

    std::array<UInt8, 8> t8 = {{ 1, 2, 3, 4, 5, 6, 7, 8 }};
    EXPECT_EQ(0xd82dd722, ofp::hash::MurmurHash32(&t8));
}
