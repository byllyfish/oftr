#ifndef OFP_OXMRANGE_H
#define OFP_OXMRANGE_H

#include "ofp/oxmiterator.h"
#include <ostream>

namespace ofp { // <namespace ofp>

class OXMRange {
public:

	constexpr OXMRange(OXMIterator begin, OXMIterator end)
		: begin_{begin.data()}, end_{end.data()} {}

	//constexpr OXMRange(const void *begin, const void *end)
	//	: begin_{static_cast<const UInt8*>(begin)}, 
	//	  end_{static_cast<const UInt8*>(end)} {}

	constexpr OXMRange(const void *data, size_t length)
		: begin_{static_cast<const UInt8*>(data)}, 
		  end_{static_cast<const UInt8*>(data) + length} {}

	// Backwards consructor used in compiling prereqs.
	constexpr OXMRange(size_t length, const UInt8 data[])
		: begin_{data}, end_{data + length} {}
	

	// Construct from vector, etc.
	//OXMRange(const std::vector<UInt8> *v) 
	//	: OXMRange{&v->[0], v->size()} {}
	
	OXMIterator begin() const { return OXMIterator{begin_}; }
	OXMIterator end() const { return OXMIterator{end_}; }
	
	const UInt8 *data() const { return begin_; }
	size_t size() const { return Unsigned_cast(end_ - begin_); }
	
	bool operator==(const OXMRange &rhs) const {
		return size() == rhs.size() &&
				0 == std::memcmp(data(), rhs.data(), size());
	}

	bool operator!=(const OXMRange &rhs) const {
		return !(*this == rhs);
	}
	
	///bool validate() const;
	
private:
	const UInt8 *begin_;
	const UInt8 *end_;
};


std::ostream &operator<<(std::ostream &stream, const OXMRange &range);


} // </namespace ofp>


inline
std::ostream &ofp::operator<<(std::ostream &stream, const OXMRange &)
{
	return stream << "<OXMRange>";
}


#endif // OFP_OXMRANGE_H
