// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PROTOCOLLIST_H_
#define OFP_PROTOCOLLIST_H_

#include "ofp/bytelist.h"
#include "ofp/protocolrange.h"

namespace ofp {

template <class RangeType>
class ProtocolList {
 public:
  using Iterator = typename RangeType::Iterator;
  using Element = typename RangeType::Element;

  ProtocolList() = default;
  /* implicit NOLINT */ ProtocolList(const ProtocolList &list) = default;
  /* implicit NOLINT */ ProtocolList(const RangeType &range)
      : buf_{range.toByteRange()} {}
  ProtocolList &operator=(const ProtocolList &list) = default;

  Iterator begin() const { return Iterator(buf_.begin()); }
  Iterator end() const { return Iterator(buf_.end()); }

  const UInt8 *data() const { return buf_.data(); }
  size_t size() const { return buf_.size(); }
  bool empty() const { return buf_.empty(); }

  RangeType toRange() const { return buf_.toRange(); }

  void assign(const RangeType &range) { buf_ = range.toByteRange(); }
  void clear() { buf_.clear(); }

 protected:
  ByteList buf_;
};

}  // namespace ofp

#endif  // OFP_PROTOCOLLIST_H_
