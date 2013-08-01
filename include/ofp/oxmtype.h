#ifndef OFP_OXM_TYPE_H
#define OFP_OXM_TYPE_H

#include "ofp/byteorder.h"

namespace ofp { // <namespace ofp>

class OXMType {
public:

	constexpr OXMType(UInt16 oxmClass, UInt8 oxmField, UInt16 oxmBits)
		: value32_{make(oxmClass, oxmField, oxmBits)} {}
	
	explicit OXMType(const UInt8 *data, size_t) {
		// FIXME length ignored. Use RawMem interface?
		std::memcpy(&value32_, data, sizeof(value32_));
	}
	
	// Return `opaque` identifier. Value depends on host's byte order.
	constexpr operator UInt32() const { return value32_; }
	
	constexpr size_t length() const { return (value32_ & End8Bits) >> End8RightShift; }
	constexpr bool hasMask() const { return value32_ & MaskBits; }
	
	// When we add the mask, double the length.
	constexpr OXMType withMask() const { 
		return hasMask() ? *this : OXMType((value32_ & ~End8Bits) | MaskBits | ((value32_ & End7Bits) << 1)); 
	}
	
	constexpr OXMType withoutMask() const { 
		return hasMask() ? OXMType((value32_ & ~End8Bits & ~MaskBits) | ((value32_ & End8Bits) >> 1)) : *this; 
	}
	
	constexpr UInt16 oxmClass() const  { return oxmNative() >> 16; }
	constexpr UInt8  oxmField() const { return (oxmNative() >> 9) & 0x07FU; }
	constexpr UInt32 oxmNative() const { return BigEndianToNative(value32_); }
		
private:
	UInt32 value32_;
	
	enum : UInt32 {
		MaskBits = BigEndianFromNative(0x0100U),
		End7Bits = BigEndianFromNative(0x007FU),
		End8Bits = BigEndianFromNative(0x00FFU),
		End8RightShift = (End8Bits == 0x00FFU ? 0 : 24)
	};
	
	constexpr explicit OXMType(const UInt32 &value) : value32_{value} {}
	
	constexpr static
	UInt32 make(UInt16 oxmClass, UInt8 oxmField, UInt16 oxmBits)
	{
		return BigEndianFromNative((UInt32_cast(oxmClass) << 16) | 
							(UInt32_cast(oxmField & 0x7F) << 9) | 
				 			((oxmBits >> 3) + ((oxmBits & 0x07) != 0)));
	}
};

static_assert(std::is_literal_type<OXMType>::value, "Literal type expected.");


} // </namespace ofp>

#endif //OFP_OXM_TYPE_H

