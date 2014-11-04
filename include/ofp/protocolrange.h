// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PROTOCOLRANGE_H_
#define OFP_PROTOCOLRANGE_H_

#include <algorithm>
#include "ofp/protocoliterator.h"
#include "ofp/log.h"

namespace ofp {

template <class IteratorType>
class ProtocolRange {
 public:
  using Iterator = IteratorType;
  using Element = typename IteratorType::Element;

  ProtocolRange() = default;
  /* implicit */ ProtocolRange(const ByteRange &range) : range_{range} {
    assert(begin() <= end());
  }

  Iterator begin() const { return Iterator(range_.begin()); }
  Iterator end() const { return Iterator(range_.end()); }

  const UInt8 *data() const { return range_.data(); }
  size_t size() const { return range_.size(); }
  ByteRange toByteRange() const { return range_; }

  size_t itemCount() const { return Iterator::itemCount(range_); }

  bool validateInput(Validation *context) const {
    return Iterator::isValid(range_, context);
  }

  bool operator==(const ProtocolRange &rhs) const {
    return range_ == rhs.range_;
  }

 private:
  ByteRange range_;
};

}  // namespace ofp

#endif  // OFP_PROTOCOLRANGE_H_
