// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PROTOCOLRANGE_H_
#define OFP_PROTOCOLRANGE_H_

#include <algorithm>

#include "ofp/log.h"
#include "ofp/protocoliterator.h"

namespace ofp {

template <class IteratorType>
class ProtocolRange {
 public:
  using Iterator = IteratorType;
  using Element = typename IteratorType::Element;

  ProtocolRange() = default;
  /* implicit NOLINT */ ProtocolRange(const ByteRange &range) : range_{range} {
    assert(range.begin() <= range.end());
  }

  Iterator begin() const { return Iterator(range_.begin()); }
  Iterator end() const { return Iterator(range_.end()); }

  const UInt8 *data() const { return range_.data(); }
  size_t size() const { return range_.size(); }
  bool empty() const { return range_.empty(); }
  ByteRange toByteRange() const { return range_; }

  size_t itemCount() const { return Iterator::itemCount(range_); }

  bool validateInput(Validation *context) const {
    if (!Iterator::isValid(range_, context))
      return false;
    for (const Element &elem : *this) {
      if (!elem.validateInput(context))
        return false;
    }
    return true;
  }

  bool operator==(const ProtocolRange &rhs) const {
    return range_ == rhs.range_;
  }

 private:
  ByteRange range_;
};

}  // namespace ofp

#endif  // OFP_PROTOCOLRANGE_H_
