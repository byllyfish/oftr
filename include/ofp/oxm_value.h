#ifndef OFP_OXM_VALUE_H
#define OFP_OXM_VALUE_H

#include "ofp/byteorder.h"

namespace ofp { // <namespace ofp>

namespace details { // <namespace ofp>

inline constexpr 
UInt32 CalcLength(UInt32 bits) {
	return (bits >> 3) + ((bits & 0x07) != 0);
}

inline constexpr 
UInt32 MakeID(UInt16 oxm_class, UInt8 oxm_field, bool oxm_hasmask, UInt16 oxm_bits)
{
	return (UInt32_cast(oxm_class) << 16) | 
			(UInt32_cast(oxm_field) << 9) | 
			(oxm_hasmask ? 0x0100U : 0U) | 
			 CalcLength(oxm_bits);
}

} // </namespace details>


template <
	UInt16 Class,
	UInt8 Field,
	class ValueType,
	UInt16 Bits,
	bool HasMask
>
class oxm_value {
public:
	using NativeType = typename BigEndianTraits<ValueType>::NativeType;
	
	enum : UInt32 {
		id = details::MakeID(Class, Field, HasMask, Bits),
	};
	
	enum {
		bits = Bits,
		hasMask = HasMask,
	};

	static_assert(sizeof(ValueType) == (id & 0x0FFUL), "Unexpected oxm_length.");
	
	oxm_value(NativeType value) : value_{value} {}
	operator NativeType() const { return value_; }
	void operator=(NativeType value) { value_ = value; }

private:
	ValueType value_;
};

} // </namespace ofp>

#endif // OFP_OXM_VALUE_H
