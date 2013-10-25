//  ===== ---- ofp/bucketiterator.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the BucketIterator class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_BUCKETITERATOR_H
#define OFP_BUCKETITERATOR_H

namespace ofp { // <namespace ofp>

class BucketIterator {
public:
  using Item = Bucket;

  explicit BucketIterator(const UInt8 *pos) : position_{pos} {}

  const Item &operator*() const {
    return *reinterpret_cast<const Item *>(position_);
  }

  const UInt8 *data() const { return position_; }
  UInt16 size() const { return *reinterpret_cast<const Big16 *>(data()); }

  // No operator -> (FIXME?)
  // No postfix ++

  void operator++() {
    auto sz = size();
    assert(sz >= 4);
    position_ += sz;
  }

  bool operator==(const BucketIterator &rhs) const {
    return position_ == rhs.position_;
  }

  bool operator!=(const BucketIterator &rhs) const { return !(*this == rhs); }

  bool operator<=(const BucketIterator &rhs) const {
    return position_ <= rhs.position_;
  }

  bool operator<(const BucketIterator &rhs) const {
    return position_ < rhs.position_;
  }

  /// \returns Number of instructions between begin and end.
  static size_t distance(BucketIterator begin, BucketIterator end) {
    assert(begin <= end);

    size_t dist = 0;
    while (begin < end) {
      ++dist;
      ++begin;
    }
    return dist;
  }

private:
  const UInt8 *position_;
};

} // </namespace ofp>

#endif // OFP_BUCKETITERATOR_H
