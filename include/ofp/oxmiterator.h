//  ===== ---- ofp/oxmiterator.h ---------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the OXMIterator class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_OXMITERATOR_H
#define OFP_OXMITERATOR_H

#include "ofp/oxmtype.h"

namespace ofp { // <namespace ofp>

class OXMIterator {
public:

	class Item {
	public:
		
		Item(const Item &) = delete;
		Item &operator=(const Item &) = delete;

		OXMType type() const {
			return OXMType::fromBytes(BytePtr(this));
		}
	
		template <class ValueType>
		ValueType value() const {
			return ValueType::fromBytes(BytePtr(this) + sizeof(OXMType));
		}

		template <class ValueType>
		ValueType mask() const {
			return ValueType::fromBytes(BytePtr(this) + sizeof(OXMType) + sizeof(ValueType));
		}

		OXMIterator position() const { return OXMIterator{BytePtr(this)}; }
		
	private:
		Item() = default;
	};

	const Item &operator*() const 
	{
		return *reinterpret_cast<const Item *>(position_);
	}
	
	OXMType type() const {
		return OXMType::fromBytes(position_);
	}
	
	// No operator -> (FIXME?)
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

	/// \returns Number of OXM elements between begin and end.
	static size_t distance(OXMIterator begin, OXMIterator end)
	{
		assert(begin <= end);
		
		size_t dist = 0;
		while (begin != end) {
			++dist;
			++begin;
		}
		return dist;
	}

private:
	const UInt8 *position_;
	
	constexpr explicit OXMIterator(const void *pos)
		: position_{static_cast<const UInt8 *>(pos)} {}
		
	friend class OXMRange;
	friend class OXMList;
	friend class ActionIterator;
};


} // </namespace ofp>

#endif // OFP_OXMITERATOR_H
