// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/smallcstring.h"
#include "llvm/Support/ConvertUTF.h"  // isLegalUTF8Sequence, getNumBytesForUTF8

using namespace ofp;

// Find the next invalid utf-8 sequence in the given range [begin, end).
//
// A utf-8 sequence may be invalid if:
//
// 1) The first byte is invalid (10xxxxxx, 11111xxx).
// 2) There range ends before the utf-8 sequence finishes.
// 3) Subsequent bytes are invalid (not 10xxxxxx).
// 4) Sequence is not in shortest form.
// 5) The sequence represents U+D800 - U+DFFF, or > U+10FFFF.

static const UInt8 *findIllegalUtf8(const UInt8 *begin, const UInt8 *end) {
  assert(begin != nullptr);
  assert(begin <= end);

  const UInt8 *pos = begin;

  while (true) {
    assert(pos <= end);

    // Find first byte that has the high bit set.
    pos = std::find_if(pos, end, [](UInt8 b) { return (b & 0x80) != 0; });
    if (pos == end) {
      break;
    }

    if (!llvm::isLegalUTF8Sequence(pos, end)) {
      return pos;
    }

    pos += llvm::getNumBytesForUTF8(*pos);
  }

  return end;
}

// Replace bytes that are part of invalid utf-8 sequences with '?'. For example,
// the invalid sequence "\xC1\x9C" is replaced by "??". The value
// "\x41\xC2\x3E\x42" is replaced by "\x41?\x3E\x42"
// This transformation will never change the number of bytes in the string.

std::string ofp::detail::validUtf8String(const char *b, const char *e) {
  assert(b != nullptr);
  assert(b <= e);

  const UInt8 *pos = reinterpret_cast<const UInt8 *>(b);
  const UInt8 *end = reinterpret_cast<const UInt8 *>(e);

  std::string result;

  while (true) {
    const UInt8 *next = findIllegalUtf8(pos, end);
    if (next == end) {
      result.append(pos, end);
      break;
    }

    result.append(pos, next);
    result.push_back('?');

    pos = next + 1;
  }

  return result;
}
