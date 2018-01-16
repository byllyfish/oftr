// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/bytelist.h"
#include <numeric>  // for iota

using namespace ofp;

/// \brief Set contents of the entire byte buffer.
void ByteList::set(const void *data, size_t length) {
  buf_.assign(data, length);
}

/// \brief Append data to the end of the byte buffer applying a mask.
void ByteList::addMasked(const void *data, const void *mask, size_t length) {
  buf_.addUninitialized(length);
  UInt8 *dest = buf_.end() - length;
  MemCopyMasked(dest, data, mask, length);
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
  buf_.replaceUninitialized(RemoveConst_cast(pos), RemoveConst_cast(posEnd),
                            length);
}

/// \brief Add zero bytes to the end of the byte buffer.
void ByteList::addZeros(size_t length) {
  buf_.addZeros(length);
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
void ByteList::resize(size_t length) {
  buf_.resize(length);
}

/// \brief Clear contents of the buffer.
void ByteList::clear() {
  buf_.clear();
}

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
