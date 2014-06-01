#ifndef OFP_TABLEIDRANGE_H_
#define OFP_TABLEIDRANGE_H_

#include "ofp/byterange.h"

namespace ofp {

class TableIDRange {
public:
    TableIDRange() = default;
    TableIDRange(const ByteRange &range) : range_{range} {}

    size_t size() const { return range_.size(); }
    const UInt8 *nthItem(size_t index) const { return range_.data() + index; }

    ByteRange toByteRange() const { return range_; }
    
    bool operator==(const TableIDRange &rhs) const {
        return range_ == rhs.range_;
    }

private:
    ByteRange range_;
};

}  // namespace ofp

#endif // OFP_TABLEIDRANGE_H_
