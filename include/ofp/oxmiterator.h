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

		template <class ValueType>
		ValueType mask() {
			// FIXME needs to work with primitive types.
			return ValueType{position_ + sizeof(OXMType) + sizeof(ValueType), 0};
		}

		OXMIterator position() const { return OXMIterator{position_}; }
		
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
		position_ += size();
	}
		
	bool operator==(const OXMIterator &rhs) const {
		return position_ == rhs.position_;
	}
	
	bool operator!=(const OXMIterator &rhs) const {
		return !(*this == rhs);
	}

	bool operator<=(const OXMIterator &rhs) const {
		return position_ <= rhs.position_;
	}
	
	constexpr const UInt8 *data() const { return position_; }
	size_t size() const { return sizeof(OXMType) + position_[3]; }

private:
	const UInt8 *position_;
	
	constexpr explicit OXMIterator(const void *pos)
		: position_{static_cast<const UInt8 *>(pos)} {}
		
	friend class OXMRange;
	friend class OXMList;
};

} // </namespace ofp>

#endif // OFP_OXMITERATOR_H
