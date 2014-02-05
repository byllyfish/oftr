//  ===== ---- ofp/actionrange.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the ActionRange class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ACTIONRANGE_H_
#define OFP_ACTIONRANGE_H_

#include "ofp/byterange.h"
#include "ofp/writable.h"
#include "ofp/actioniterator.h"

namespace ofp {

class ActionList;

class ActionRange {
 public:
  ActionRange() = default;
  /* implicit NOLINT */ ActionRange(const ByteRange &range) : range_{range} {}
  /* implicit NOLINT */ ActionRange(const ActionList &list);

  size_t itemCount() const { return ActionIterator::distance(begin(), end()); }

  ActionIterator begin() const {
    return ActionIterator{data()};
  }
  ActionIterator end() const {
    return ActionIterator{data() + size()};
  }

  const UInt8 *data() const { return range_.data(); }
  size_t size() const { return range_.size(); }
  ByteRange toByteRange() const { return range_; }

  /// \returns Size of action list when written to channel using the specified
  /// protocol version.
  size_t writeSize(Writable *channel);

  /// \brief Writes action list to the channel using the specified protocol
  /// version.
  void write(Writable *channel);

 private:
  ByteRange range_;

  static unsigned writeSizeMinusSetFieldV1(ActionIterator iter);
  static void writeSetFieldV1(ActionIterator iter, Writable *channel);
};

}  // namespace ofp

#endif  // OFP_ACTIONRANGE_H_
