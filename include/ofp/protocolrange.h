#ifndef OFP_PROTOCOLITERABLE_H_
#define OFP_PROTOCOLITERABLE_H_

#include "ofp/protocoliterator.h"
#include "ofp/log.h"

namespace ofp {

template <class IteratorType>
class ProtocolRange {
public:
  using Iterator = IteratorType;
  using Element = typename IteratorType::Element;

  ProtocolRange() = default;
  /* implicit */ ProtocolRange(const ByteRange &range) : range_{range} {}

  Iterator begin() const { return Iterator(range_.begin()); }
  Iterator end() const { return Iterator(range_.end()); }

  const UInt8 *data() const { return range_.data(); }
  size_t size() const { return range_.size(); }
  ByteRange toByteRange() const { return range_; }

  size_t itemCount() const { return Iterator::rangeItemCount(range_); }

  bool validateInput(const char *context) const {
    return Iterator::isRangeValid(range_, context);
  }

private:
  ByteRange range_;
};

}  // namespace ofp

#endif  // OFP_PROTOCOLITERABLE_H_
