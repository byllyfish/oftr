// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_BYTELIST_H_
#define OFP_BYTELIST_H_

#include "ofp/smallbuffer.h"
#include "ofp/byterange.h"

namespace ofp {

/// \brief Concrete class representing a mutable byte buffer.
///
/// This class manages its own memory. The memory underlying a ByteList is
/// relocatable; methods that change the size of the buffer can cause pointers
/// into the ByteList to become invalid.
///
/// When you copy or assign a ByteList, it produces a deep copy.
///
/// @see ByteData
/// @see ByteRange
class ByteList {
 public:
  /// \brief Construct an empty byte buffer.
  ByteList() = default;

  /// \brief Construct byte buffer copying the specified data.
  explicit ByteList(const void *data, size_t length) : buf_{data, length} {}

  /// \brief Construct byte buffer copying data in range.
  /* implicit NOLINT */ ByteList(const ByteRange &range)
      : ByteList{range.data(), range.size()} {}

  explicit ByteList(const std::string &s) : ByteList{s.data(), s.size()} {}

  /// \returns pointer to beginning of constant byte buffer.
  const UInt8 *begin() const { return buf_.begin(); }

  /// \returns pointer to end of constant byte buffer (i.e. one past last byte).
  const UInt8 *end() const { return buf_.end(); }

  /// \returns pointer to beginning of constant byte buffer.
  const UInt8 *data() const { return buf_.begin(); }

  /// \returns pointer to beginning of mutable byte buffer.
  UInt8 *mutableData() { return buf_.begin(); }

  /// \returns size of buffer in bytes.
  size_t size() const { return buf_.size(); }

  /// \returns true if buffer is empty.
  bool empty() const { return buf_.empty(); }

  /// \returns offset of pointer relative to start of the byte buffer.
  ptrdiff_t offset(const UInt8 *pos) const {
    assertInRange(pos);
    return pos - data();
  }

  /// \brief Set contents of the entire byte buffer.
  void set(const void *data, size_t length);

  /// \brief Append data to the end of the byte buffer.
  void add(const void *data, size_t length) { buf_.add(data, length); }

  /// \brief Append data to the end of the byte buffer applying a mask.
  void addMasked(const void *data, const void *mask, size_t length);

  /// \brief Insert data at the specified position in the byte buffer.
  void insert(const UInt8 *pos, const void *data, size_t length);

  /// \brief Replace existing data in the byte buffer with new values.
  void replace(const UInt8 *pos, const UInt8 *posEnd, const void *data,
               size_t length);

  /// \brief Append uninitialized bytes to the end of the byte buffer.
  void addUninitialized(size_t length);

  /// \brief Insert uninitialized bytes at the specified position in the byte
  /// buffer.
  void insertUninitialized(const UInt8 *pos, size_t length);

  /// \brief Replace existing data in the byte buffer with uninitialized values.
  void replaceUninitialized(const UInt8 *pos, const UInt8 *posEnd,
                            size_t length);

  /// \brief Add zero bytes to the end of the byte buffer.
  void addZeros(size_t length);

  /// \brief Insert zero bytes at the specified position in the byte buffer.
  void insertZeros(const UInt8 *pos, size_t length);

  /// \brief Remove specified range from the byte buffer.
  void remove(const UInt8 *pos, size_t length);

  /// \brief Resize buffer to specified length.
  void resize(size_t length);

  /// \brief Clear contents of the buffer.
  void clear();

  bool operator==(const ByteList &rhs) const { return buf_ == rhs.buf_; }
  bool operator!=(const ByteList &rhs) const { return !operator==(rhs); }

  ByteRange toRange() const;
  ByteList &operator=(const ByteRange &range);

  static ByteList iota(size_t length);

 private:
  SmallBuffer buf_;

  /// \brief Helper function to verify pointer is valid for byte buffer.
  void assertInRange(const UInt8 *pos) const {
    assert((pos != nullptr) && "position is null");
    assert((pos >= begin() && pos <= end()) && "position out of range");
  }
};

static_assert(IsConvertible<ByteRange, ByteList>(), "Expected conversion.");

/// \brief Write buffer to stream in hexadecimal format.
std::ostream &operator<<(std::ostream &os, const ByteList &value);

inline std::ostream &operator<<(std::ostream &os, const ByteList &value) {
  return os << "[ByteList size=" << value.size()
            << " data=" << RawDataToHex(value.data(), value.size()) << "]";
}

}  // namespace ofp

#endif  // OFP_BYTELIST_H_
