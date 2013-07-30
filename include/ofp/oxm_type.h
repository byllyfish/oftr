#ifndef OFP_OXM_TYPE_H
#define OFP_OXM_TYPE_H

#include "ofp/byteorder.h"


namespace ofp { // <namespace ofp>


class oxm_type {
public:

	constexpr oxm_type(UInt16 oxmClass, UInt8 oxmField, UInt16 oxmBits)
		: value32_{make(oxmClass, oxmField, oxmBits)} {}
	
	explicit oxm_type(const UInt8 *data) : value32_{ReadMemory<UInt32>(data)} 
	{}
	
	// Return `opaque` unsigned integer. Value depends on host's byte order.
	constexpr operator UInt32() const { return value32_; }
	
	constexpr size_t length() const { return value8_[3]; }
	constexpr bool hasMask() const { return value32_ & MaskBits; }
	
	constexpr oxm_type withMask() const { return oxm_type(value32_ | MaskBits); }
	constexpr oxm_type withoutMask() const { return oxm_type(value32_ & ~MaskBits); }
	
	constexpr UInt16 oxmClass() const  { return oxmNative() >> 16; }
	constexpr UInt8  oxmField() const { return (oxmNative() >> 9) & 0x07FU; }
	constexpr UInt32 oxmNative() const { return BigEndianToNative(value32_); }
		
private:
	union {
		const UInt32 value32_;
		const UInt8  value8_[4];
	};
	
	enum : UInt32 {
		MaskBits = BigEndianFromNative(0x0100)
	};
	
	constexpr explicit oxm_type(const UInt32 &value) : value32_{value} {}
	
	constexpr static
	UInt32 make(UInt16 oxmClass, UInt8 oxmField, UInt16 oxmBits)
	{
		return BigEndianFromNative((UInt32_cast(oxmClass) << 16) | 
							(UInt32_cast(oxmField & 0x7F) << 9) | 
				 			((oxmBits >> 3) + ((oxmBits & 0x07) != 0)));
	}
};

static_assert(std::is_literal_type<oxm_type>::value, "Literal type expected.");

} // </namespace ofp>

#endif //OFP_OXM_TYPE_H

