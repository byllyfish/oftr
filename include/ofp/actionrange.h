#ifndef OFP_ACTIONRANGE_H
#define OFP_ACTIONRANGE_H

#include "ofp/byterange.h"

namespace ofp { // <namespace ofp>

class ActionRange {
public:
	ActionRange() = default;
	ActionRange(const ByteRange &range) : range_{range} {}

	const UInt8 *data() const { return range_.data(); }
	size_t size() const { return range_.size(); }
	
private:
	ByteRange range_;
};

} // </namespace ofp>

#endif // OFP_ACTIONRANGE_H
