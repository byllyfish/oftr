// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_OXMRANGE_H_
#define OFP_OXMRANGE_H_

#include "ofp/byterange.h"
#include "ofp/oxmiterator.h"

namespace ofp {

class OXMRange {
 public:
  constexpr OXMRange() : begin_{nullptr}, end_{nullptr} {}

  constexpr OXMRange(const ByteRange &range)
      : begin_{range.begin()}, end_{range.end()} {}

  constexpr OXMRange(OXMIterator begin, OXMIterator end)
      : begin_{begin.data()}, end_{end.data()} {}

  constexpr OXMRange(const void *data, size_t length)
      : begin_{static_cast<const UInt8 *>(data)},
        end_{static_cast<const UInt8 *>(data) + length} {}

  // Backwards consructor used in compiling prereqs.
  constexpr OXMRange(size_t length, const UInt8 data[])
      : begin_{data}, end_{data + length} {}

  template <class Value>
  typename Value::NativeType get() const;

  template <class Value>
  bool exists() const;

  OXMIterator begin() const { return OXMIterator{begin_}; }
  OXMIterator end() const { return OXMIterator{end_}; }

  const UInt8 *data() const { return begin_; }
  size_t size() const { return Unsigned_cast(end_ - begin_); }

  bool operator==(const OXMRange &rhs) const {
    return size() == rhs.size() && 0 == std::memcmp(data(), rhs.data(), size());
  }

  bool operator!=(const OXMRange &rhs) const { return !(*this == rhs); }

  bool validateInput() const {
    assert(begin_ <= end_);
    const UInt8 *pos = begin_;
    size_t left = static_cast<size_t>(end_ - begin_);
    while (pos < end_) {
      if (left < 4)
        return false;
      size_t len = sizeof(OXMType) + pos[3];
      if (left < len)
        return false;
      pos += len;
      left -= len;
    }
    assert(pos == end_);
    assert(left == 0);
    return true;
  }

 private:
  const UInt8 *begin_;
  const UInt8 *end_;
};

template <class Value>
typename Value::NativeType OXMRange::get() const {
  using NativeType = typename Value::NativeType;

  for (auto &item : *this) {
    if (item.type() == Value::type()) {
      return item.value<Value>();
    }
  }

  return NativeType{};
}

template <class Value>
bool OXMRange::exists() const {
  for (auto &item : *this) {
    if (item.type() == Value::type()) {
      return true;
    }
  }
  return false;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                     const OXMRange &range) {
  return os << "[OXMRange size=" << range.size()
            << " data=" << RawDataToHex(range.data(), range.size()) << ']';
}

}  // namespace ofp

#endif  // OFP_OXMRANGE_H_
