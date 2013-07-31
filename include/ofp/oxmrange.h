#ifndef OFP_OXMRANGE_H
#define OFP_OXMRANGE_H

#include "oxmiterator.h"

namespace ofp { // <namespace ofp>

class OXMRange {
public:
	OXMRange(OXMIterator begin, OXMIterator end)
		: begin_{begin}, end_{end} {}
	
	OXMRange(const UInt8 *begin, const UInt8 *end)
		: begin_{begin}, end_{end} {}

	OXMRange(const void *data, size_t length)
		: begin_{static_cast<const UInt8*>(data)}, 
		  end_{static_cast<const UInt8*>(data) + length} {}
		
	OXMIterator begin() const { return begin_; }
	OXMIterator end() const { return end_; }
	
private:
	const OXMIterator begin_;
	const OXMIterator end_;
};

} // </namespace ofp>

#endif // OFP_OXMRANGE_H
