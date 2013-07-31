#ifndef OFP_OXM_VALUE_H
#define OFP_OXM_VALUE_H

#include "ofp/byteorder.h"
#include "ofp/oxm_type.h"

namespace ofp { // <namespace ofp>

template <
	UInt16 Class,
	UInt8 Field,
	class ValueType,
	UInt16 Bits,
	bool Mask
>
class oxm_value {
public:
	using NativeType = typename ValueType::NativeType;

	constexpr static inline oxm_type type() { return oxm_type{Class, Field, Bits}; }
	constexpr static inline UInt16	bits() { return Bits; }
	constexpr static inline bool maskSupported() { return Mask; }
	
	oxm_value(NativeType value) : value_{value} {}
	operator NativeType() const { return value_; }
	void operator=(NativeType value) { value_ = value; }

	
private:
	ValueType value_;
	
	static_assert(8*sizeof(ValueType) >= Bits, "Unexpected oxm_value size.");
};

} // </namespace ofp>

#endif // OFP_OXM_VALUE_H
