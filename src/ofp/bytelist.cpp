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
#include <numeric>      // for iota

using namespace ofp;

/// \brief Set contents of the entire byte buffer.
void ByteList::set(const void *data, size_t length) {
  buf_.assign(data, length);
}

/// \brief Insert data at the specified position in the byte buffer.
void ByteList::insert(const UInt8 *pos, const void *data, size_t length) {
  buf_.insert(RemoveConst_cast(pos), data, length);
}

/// \brief Replace existing data in the byte buffer with new values.
void ByteList::replace(const UInt8 *pos, const UInt8 *posEnd, const void *data,
                       size_t length) {
  buf_.replace(RemoveConst_cast(pos), RemoveConst_cast(posEnd), data, length);
}

/// \brief Append uninitialized bytes to the end of the byte buffer.
void ByteList::addUninitialized(size_t length) {
  buf_.addUninitialized(length);
}

/// \brief Insert uninitialized bytes at the specified position in the byte
/// buffer.
void ByteList::insertUninitialized(const UInt8 *pos, size_t length) {
  buf_.insertUninitialized(RemoveConst_cast(pos), length);
}

/// \brief Replace existing data in the byte buffer with uninitialized values.
void ByteList::replaceUninitialized(const UInt8 *pos, const UInt8 *posEnd,
                                    size_t length) {
  buf_.replaceUninitialized(RemoveConst_cast(pos), RemoveConst_cast(posEnd), length);
}

/// \brief Insert zero bytes at the specified position in the byte buffer.
void ByteList::insertZeros(const UInt8 *pos, size_t length) {
  buf_.insertZeros(RemoveConst_cast(pos), length);
}

/// \brief Remove specified range from the byte buffer.
void ByteList::remove(const UInt8 *pos, size_t length) {
  buf_.remove(RemoveConst_cast(pos), RemoveConst_cast(pos) + length);
}

/// \brief Resize buffer to specified length.
void ByteList::resize(size_t length) { buf_.resize(length); }

/// \brief Clear contents of the buffer.
void ByteList::clear() { buf_.clear(); }

/// Return range of bytes.
ByteRange ByteList::toRange() const {
  return ByteRange{data(), size()};
}

ByteList &ByteList::operator=(const ByteRange &range) {
  buf_.assign(range.data(), range.size());
  return *this;
}

ByteList ByteList::iota(size_t length) {
  ByteList result;
  result.resize(length);
  std::iota(result.mutableData(), result.mutableData() + length, 0);
  return result;
}

