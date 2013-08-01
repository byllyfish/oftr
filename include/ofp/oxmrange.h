#ifndef OFP_OXMRANGE_H
#define OFP_OXMRANGE_H

#include "ofp/oxmiterator.h"


namespace ofp { // <namespace ofp>

class OXMRange {
public:
	constexpr OXMRange(const void *begin, const void *end)
		: begin_{static_cast<const UInt8*>(begin)}, 
		  end_{static_cast<const UInt8*>(end)} {}

	constexpr OXMRange(const void *data, size_t length)
		: begin_{static_cast<const UInt8*>(data)}, 
		  end_{static_cast<const UInt8*>(data) + length} {}

	// Backwards consructor used in compiling prereqs.
	constexpr OXMRange(size_t length, const UInt8 data[])
		: begin_{data}, end_{data + length} {}
	

	// Construct from string, vector, etc.
	template <class Type>
	OXMRange(const Type *s) 
		: OXMRange{&(*s)[0], s->size()} {}
	
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
	
	bool isSubsetOf(const OXMRange &rhs) const; // TODO
	
private:
	const UInt8 *begin_;
	const UInt8 *end_;
};

} // </namespace ofp>

#endif // OFP_OXMRANGE_H
