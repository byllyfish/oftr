//  ===== ---- ofp/bytelist.cpp ----------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements ByteList class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/bytelist.h"

using namespace ofp;

/// Construct ByteList by making a copy of a ByteRange.
ByteList::ByteList(const ByteRange &range) { add(range.data(), range.size()); }

/// \brief Construct byte buffer copying the specified data.
ByteList::ByteList(const void *data, size_t length) { add(data, length); }

/// \brief Set contents of the entire byte buffer.
void ByteList::set(const void *data, size_t length) {
  assert(data != nullptr || length == 0);
  clear();
  add(data, length);
}

/// \brief Append data to the end of the byte buffer.
void ByteList::add(const void *data, size_t length) {
  assert(data != nullptr || length == 0);
  auto bp = BytePtr(data);
  buf_.insert(buf_.end(), bp, bp + length);
}

/// \brief Insert data at the specified position in the byte buffer.
void ByteList::insert(const UInt8 *pos, const void *data, size_t length) {
  assertInRange(pos);
  assert(data != nullptr || length == 0);
  auto bp = BytePtr(data);
  buf_.insert(buf_.begin() + offset(pos), bp, bp + length);
}

/// \brief Replace existing data in the byte buffer with new values.
void ByteList::replace(const UInt8 *pos, const UInt8 *posEnd, const void *data,
                       size_t length) {
  assert(data != nullptr || length == 0);
  auto idx = offset(pos);
  replaceUninitialized(pos, posEnd, length);
  // N.B. Memory might have moved.
  auto bp = BytePtr(data);
  std::copy(bp, bp + length, buf_.begin() + idx);
}

/// \brief Append uninitialized bytes to the end of the byte buffer.
void ByteList::addUninitialized(size_t length) {
  buf_.insert(buf_.end(), length, 0);
}

/// \brief Insert uninitialized bytes at the specified position in the byte
/// buffer.
void ByteList::insertUninitialized(const UInt8 *pos, size_t length) {
  assertInRange(pos);
  buf_.insert(buf_.begin() + offset(pos), length, 0);
}

/// \brief Replace existing data in the byte buffer with uninitialized values.
void ByteList::replaceUninitialized(const UInt8 *pos, const UInt8 *posEnd,
                                    size_t length) {
  assert(pos <= posEnd);
  assertInRange(pos);
  assertInRange(posEnd);

  size_t oldlen = Unsigned_cast(posEnd - pos);
  if (length > oldlen) {
    buf_.insert(buf_.begin() + offset(posEnd), length - oldlen, 0);
  } else if (length < oldlen) {
    auto iter = buf_.begin() + offset(pos);
    buf_.erase(iter, iter + Signed_cast(oldlen - length));
  }
}

/// \brief Remove specified range from the byte buffer.
void ByteList::remove(const UInt8 *pos, size_t length) {
  assertInRange(pos);
  auto iter = buf_.begin() + offset(pos);
  buf_.erase(iter, iter + Signed_cast(length));
}

/// \brief Resize buffer to specified length.
void ByteList::resize(size_t length) { buf_.resize(length); }

/// \brief Clear contents of the buffer.
void ByteList::clear() { buf_.clear(); }

/// Return range of bytes.
ByteRange ByteList::toRange() const {
  return ByteRange{data(), size()};
}

void ByteList::operator=(const ByteRange &range) {
  set(range.data(), range.size());
}
