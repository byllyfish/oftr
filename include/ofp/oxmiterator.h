#ifndef OFP_OXMITERATOR_H
#define OFP_OXMITERATOR_H

#include "ofp/oxmtype.h"
#include "ofp/oxmvalue.h"

namespace ofp { // <namespace ofp>

class OXMIterator {
public:

	class Item {
	public:
		explicit Item(const UInt8 *pos) : position_{pos} {}
		
		OXMType type() const {
			return OXMType(position_);
		}
	
		template <class ValueType>
		ValueType value() {
			return ValueType{position_ + sizeof(OXMType)};
		}
		
	private:
		const UInt8 *position_;
	};
	
	explicit OXMIterator(const void *pos)
		: position_{static_cast<const UInt8 *>(pos)} {}


	Item operator*() const 
	{
		return Item{position_};
	}
	
	// No operator ->
	
	void operator++() 
	{
		position_ += sizeof(OXMType) + position_[3];
	}
		
	bool operator==(const OXMIterator &rhs) {
		return position_ == rhs.position_;
	}
	
	bool operator!=(const OXMIterator &rhs) {
		return position_ != rhs.position_;
	}
	
private:
	const UInt8 *position_;
};

} // </namespace ofp>

#endif // OFP_OXMITERATOR_H
