#ifndef OFP_OXM_VALUE_H
#define OFP_OXM_VALUE_H

#include "ofp/byteorder.h"
#include "ofp/oxmtype.h"
#include "ofp/oxmrange.h"

namespace ofp { // <namespace ofp>

template <
	UInt16 Class,
	UInt8 Field,
	class ValueType,
	UInt16 Bits,
	bool Mask,
	const OXMRange *Prereqs = nullptr
>
class OXMValue {
public:
	
	using NativeType = typename NativeTypeOf<ValueType>::type;

	constexpr static OXMType type() { return OXMType{Class, Field, Bits}; }
	constexpr static OXMType typeWithMask() { return type().withMask(); }
	constexpr static UInt16	bits() { return Bits; }
	constexpr static bool maskSupported() { return Mask; }
	static inline const OXMRange *prerequisites() { return Prereqs; }
	
	/* implicit */ OXMValue(NativeType value) : value_{value} {}
	
	explicit OXMValue(const UInt8 *data, size_t) 
	{
		// FIXME length ignored. Same as OXMType.
		std::memcpy(&value_, data, sizeof(value_));
	}
	
	NativeType value() const { return value_; }
	operator NativeType() const { return value_; }
	void operator=(NativeType value) { value_ = value; }

private:
	ValueType value_;
	
	static_assert(8*sizeof(ValueType) >= Bits, "Unexpected oxm_value size.");
	static_assert(Bits <= 8*127, "Unexpected size in bits.");
};

} // </namespace ofp>

#endif // OFP_OXM_VALUE_H
