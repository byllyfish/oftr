//  ===== ---- ofp/bucketrange.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the BucketRange class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_BUCKETRANGE_H
#define OFP_BUCKETRANGE_H

#include "ofp/byterange.h"
#include "ofp/bucketiterator.h"

namespace ofp { // <namespace ofp>

class BucketRange {
public:
  BucketRange() = default;
  explicit BucketRange(const ByteRange &range) : range_{range} {}

  size_t itemCount() const { return BucketIterator::distance(begin(), end()); }

  BucketIterator begin() const {
    return BucketIterator{range_.begin()};
  }
  BucketIterator end() const {
    return BucketIterator{range_.end()};
  }

private:
  ByteRange range_;
};

} // </namespace ofp>

#endif // OFP_BUCKETRANGE_H
