// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_BYTERANGE_H_
#define OFP_BYTERANGE_H_

#include "ofp/types.h"

namespace ofp {

class ByteList;

class ByteRange {
 public:
  constexpr ByteRange() : begin_{nullptr}, end_{nullptr} {}
  constexpr ByteRange(const void *data, size_t length)
      : begin_{BytePtr(data)}, end_{BytePtr(data) + length} {}
  constexpr ByteRange(const void *data, const void *end)
      : begin_{BytePtr(data)}, end_{BytePtr(end)} {}
  /* implicit NOLINT */ ByteRange(const ByteList &data);

  constexpr const UInt8 *begin() const { return begin_; }
  constexpr const UInt8 *end() const { return end_; }

  constexpr const UInt8 *data() const { return begin_; }
  constexpr size_t size() const { return Unsigned_cast(end_ - begin_); }

  bool operator==(const ByteRange &rhs) const {
    return size() == rhs.size() && std::equal(begin_, end_, rhs.data());
  }
  bool operator!=(const ByteRange &rhs) const { return !(*this == rhs); }

  bool isPrintable() const;
  
 private:
  const UInt8 *begin_;
  const UInt8 *end_;
};

static_assert(IsConvertible<ByteList, ByteRange>(), "Expected conversion.");

/// \brief Write buffer to stream in hexadecimal format.
std::ostream &operator<<(std::ostream &os, const ByteRange &value);

inline std::ostream &operator<<(std::ostream &os, const ByteRange &value) {
  return os << "[ByteRange size=" << value.size()
            << " data=" << RawDataToHex(value.data(), value.size()) << "]";
}

}  // namespace ofp

#endif  // OFP_BYTERANGE_H_
