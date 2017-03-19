// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/pkt.h"

using namespace ofp;

/// Fold 32bit sum into 16 bits.
inline UInt16 fold(UInt32 n) {
  n = (n & 0xffff) + (n >> 16);
  n += (n >> 16);
  return UInt16_narrow_cast(n);
}

/// Convert byte into 16bits.
inline UInt16 to16(UInt8 b, unsigned offset) {
  assert(offset <= 1);
  UInt16 val = 0;
  MutableBytePtr(&val)[offset] = b;
  return val;
}

/// Compute 16-bit ones complement sum.
static UInt16 sum1c(UInt16 accum, const UInt8 *ptr, size_t size, bool odd) {
  assert(size <= 2 * 65537);  // min size at which 32-bit overflow occurs

  UInt32 sum = 0;

  if (size > 0 && !IsPtrAligned(ptr, 2)) {
    sum = to16(*ptr, 1);
    ++ptr;
    --size;
    odd = !odd;
  }

  while (size > 1) {
    sum += *reinterpret_cast<const UInt16 *>(ptr);
    ptr += 2;
    size -= 2;
  }

  if (size > 0) {
    sum += to16(*ptr, 0);
  }

  UInt16 result = fold(sum);
  if (odd) {
    result = detail::SwapByteOrder(result);
  }

  return accum == 0 ? result : fold(accum + result);
}

Big16 pkt::Checksum(ByteRange d1) {
  return Big16(~sum1c(0, d1.data(), d1.size(), false), true);
}

Big16 pkt::Checksum(ByteRange d1, ByteRange d2) {
  return Big16(~sum1c(sum1c(0, d1.data(), d1.size(), false), d2.data(),
                      d2.size(), (d1.size() % 2) != 0),
               true);
}

Big16 pkt::Checksum(ByteRange d1, ByteRange d2, ByteRange d3) {
  const size_t d12size = d1.size() + d2.size();
  return Big16(~sum1c(sum1c(sum1c(0, d1.data(), d1.size(), false), d2.data(), d2.size(), (d1.size() % 2) != 0), d3.data(), d3.size(), (d12size % 2) != 0), true);
}
