// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_HASH_H_
#define OFP_HASH_H_

#include "ofp/types.h"

// This file contains source code adapted from MurmurHash3:
// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp
//
// "MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code."

namespace ofp {
namespace hash {
namespace detail {

inline UInt32 RotateLeft32(UInt32 x, int n) {
  return (x << n) | (x >> (32 - n));
}

inline UInt32 FinalMix32(UInt32 x) {
  x ^= x >> 16;
  x *= 0x85ebca6b;
  x ^= x >> 13;
  x *= 0xc2b2ae35;
  x ^= x >> 16;
  return x;
}

}  // namespace detail

// MurmurHash32 is adapted from the C source code of `MurmurHash3_x86_32`:
// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp

template <class Type, unsigned Size = sizeof(Type)>
size_t MurmurHash32(const Type *key) {
  const UInt32 seed = 29;
  const UInt32 c1 = 0xcc9e2d51;
  const UInt32 c2 = 0x1b873593;

  UInt32 h1 = seed;
  const UInt8 *ptr = BytePtr(key);
  unsigned len = Size;

  while (len >= sizeof(UInt32)) {
    UInt32 k1 = UInt32_unaligned(ptr);
    ptr += sizeof(UInt32);
    len -= sizeof(UInt32);

    k1 *= c1;
    k1 = detail::RotateLeft32(k1, 15);
    k1 *= c2;

    h1 ^= k1;
    h1 = detail::RotateLeft32(h1, 13);
    h1 = h1 * 5 + 0xe6546b64;
  }

  UInt32 k1 = 0;
  if (Size & 3) {
    assert((Size & 3) < sizeof(k1));
    memcpy(&k1, ptr, Size & 3);
  }

  k1 *= c1;
  k1 = detail::RotateLeft32(k1, 15);
  k1 *= c2;

  h1 ^= k1;
  h1 ^= Size;

  return detail::FinalMix32(h1);
}

/// Utility function to combine hash values.
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3876.pdf
template <typename T>
void HashCombine(size_t &seed, const T &val) {
  seed ^= std::hash<T>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

}  // namespace hash
}  // namespace ofp

#endif  // OFP_HASH_H_
