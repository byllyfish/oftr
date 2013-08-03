#ifndef OFP_OXMITERATOR_H
#define OFP_OXMITERATOR_H

#include "ofp/oxmtype.h"
//#include "ofp/oxmvalue.h"

namespace ofp { // <namespace ofp>

class OXMIterator {
public:

	class Item {
	public:
		explicit Item(const UInt8 *pos) : position_{pos} {}
		
		OXMType type() const {
			return OXMType(position_, 0);
		}
	
		template <class ValueType>
		ValueType value() {
			// FIXME needs to work with primitive types.
			return ValueType{position_ + sizeof(OXMType), 0};
		}
		
	private:
		const UInt8 *position_;
	};

	Item operator*() const 
	{
		return Item{position_};
	}
	
	OXMType type() const {
		return OXMType(position_, 0);
	}
	
	// No operator ->
	// No postfix ++
	
	void operator++() 
	{
		position_ += sizeof(OXMType) + position_[3];
	}
		
	bool operator==(const OXMIterator &rhs) const {
		return position_ == rhs.position_;
	}
	
	bool operator!=(const OXMIterator &rhs) const {
		return !(*this == rhs);
	}
	
	const UInt8 *data() const { return position_; }
	
private:
	const UInt8 *position_;
	
	explicit OXMIterator(const void *pos)
		: position_{static_cast<const UInt8 *>(pos)} {}
		
	friend class OXMRange;
	friend class OXMList;
};

} // </namespace ofp>

#endif // OFP_OXMITERATOR_H
