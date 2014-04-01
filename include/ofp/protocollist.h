#ifndef OFP_PROTOCOLLIST_H_
#define OFP_PROTOCOLLIST_H_

#include "ofp/protocoliterable.h"
#include "ofp/bytelist.h"

namespace ofp {

template <class RangeType>
class ProtocolList {
public:
    using Iterator = typename RangeType::Iterator;
    using Element = typename RangeType::Element;

    Iterator begin() const { return Iterator(buf_.begin()); }
    Iterator end() const { return Iterator(buf_.end()); }

    const UInt8 *data() const { return buf_.data(); }
    size_t size() const { return buf_.size(); }

    RangeType toRange() const { return buf_.toRange(); }

    void assign(const RangeType &range) { buf_ = range.toByteRange(); }

protected:
    ByteList buf_;
};

}  // namespace ofp

#endif // OFP_PROTOCOLLIST_H_
