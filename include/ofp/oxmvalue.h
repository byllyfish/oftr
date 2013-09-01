//  ===== ---- ofp/oxmvalue.h ------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the OXMValue template class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_OXM_VALUE_H
#define OFP_OXM_VALUE_H

#include "ofp/byteorder.h"
#include "ofp/oxmtype.h"
#include "ofp/oxmrange.h"

namespace ofp { // <namespace ofp>

enum class OXMInternalID : UInt16;

template <
	OXMInternalID ID,
	UInt16 Class,
	UInt8 Field,
	class ValueType,
	UInt16 Size,
	bool Mask,
	const OXMRange *Prereqs = nullptr
>
class OXMValue {
public:
	
	using NativeType = typename NativeTypeOf<ValueType>::type;

	constexpr static OXMInternalID internalId() { return ID; }
	constexpr static OXMType type() { return OXMType{Class, Field, Size}; }
	constexpr static OXMType typeWithMask() { return type().withMask(); }
	constexpr static bool maskSupported() { return Mask; }
	static inline const OXMRange *prerequisites() { return Prereqs; }
	
	/* implicit */ OXMValue(NativeType value) : value_{value} {}
	
	NativeType value() const { return value_; }
	operator NativeType() const { return value_; }
	void operator=(NativeType value) { value_ = value; }

	static OXMValue fromBytes(const UInt8 *data) {
		OXMValue result{};
		std::memcpy(&result.value_, data, sizeof(value_));
		return result;
	}

private:
	ValueType value_;
	
	// Used by fromBytes().
	OXMValue() = default;
};

} // </namespace ofp>

#endif // OFP_OXM_VALUE_H
