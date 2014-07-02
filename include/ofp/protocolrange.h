#ifndef OFP_PROTOCOLITERABLE_H_
#define OFP_PROTOCOLITERABLE_H_

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
  /* implicit */ ProtocolRange(const ByteRange &range) : range_{range} { assert(begin() <= end()); }

  Iterator begin() const { return Iterator(range_.begin()); }
  Iterator end() const { return Iterator(range_.end()); }

  const UInt8 *data() const { return range_.data(); }
  size_t size() const { return range_.size(); }
  ByteRange toByteRange() const { return range_; }

  size_t itemCount() const { return Iterator::itemCount(range_); }

  template <class UnaryPredicate>
  size_t itemCountIf(UnaryPredicate pred) const {
    return Unsigned_cast(std::count_if(begin(), end(), pred));
  }

  Iterator nthItem(size_t index) const { 
    Iterator iter = begin();
    for (size_t i = 0; i < index; ++i) {
      ++iter;
    }
    return iter;
  }

OFP_BEGIN_IGNORE_PADDING
  template <class UnaryPredicate>
  Iterator nthItemIf(size_t index, UnaryPredicate pred) const { 
    return std::find_if(begin(), end(), [pred, &index](const Element &elem){
      if (pred(elem)) {
        if (index == 0) return true;
        --index;
      }
      return false;
    });
  }
OFP_END_IGNORE_PADDING

  bool validateInput(const char *context) const {
    return Iterator::isValid(range_, context);
  }

  bool operator==(const ProtocolRange &rhs) const {
    return range_ == rhs.range_;
  }

private:
  ByteRange range_;
};

}  // namespace ofp

#endif  // OFP_PROTOCOLITERABLE_H_
