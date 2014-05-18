//  ===== ---- ofp/oxmrange.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the OXMRange class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_OXMRANGE_H_
#define OFP_OXMRANGE_H_

#include "ofp/oxmiterator.h"
#include "ofp/byterange.h"

namespace ofp {

class OXMRange {
public:
  constexpr OXMRange() : begin_{nullptr}, end_{nullptr} {}

  constexpr OXMRange(const ByteRange &range) : begin_{range.begin()}, end_{range.end()} {}

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

  OXMIterator begin() const {
    return OXMIterator{begin_};
  }
  OXMIterator end() const {
    return OXMIterator{end_};
  }

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

std::ostream &operator<<(std::ostream &stream, const OXMRange &range);

inline std::ostream &operator<<(std::ostream &stream, const OXMRange &range) {
  return stream << "[OXMRange size=" << range.size()
                << " data=" << RawDataToHex(range.data(), range.size()) << ']';
}

}  // namespace ofp

#endif  // OFP_OXMRANGE_H_
