//  ===== ---- ofp/byterange.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the ByteRange class.
//  ===== ------------------------------------------------------------ =====  //

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
  /* implicit NOLINT */ ByteRange(const ByteList &data);

  constexpr const UInt8 *begin() const { return begin_; }
  constexpr const UInt8 *end() const { return end_; }

  constexpr const UInt8 *data() const { return begin_; }
  constexpr size_t size() const { return Unsigned_cast(end_ - begin_); }

  bool operator==(const ByteRange &rhs) const {
    return size() == rhs.size() && std::equal(begin_, end_, rhs.data());
  }
  bool operator!=(const ByteRange &rhs) const { return !(*this == rhs); }

private:
  const UInt8 *begin_;
  const UInt8 *end_;
};

static_assert(IsConvertible<ByteList,ByteRange>(), "Expected conversion.");

}  // namespace ofp

#endif  // OFP_BYTERANGE_H_
