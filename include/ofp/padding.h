#ifndef OFP_PADDING_H
#define OFP_PADDING_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>


template <size_t N>
struct Padding {
	constexpr Padding() {}
	const UInt8 pad_[N] = {};
};


constexpr size_t PadLength(size_t length)
{
	return 8 * ((length + 7) / 8);
}


} // </namespace ofp>

#endif // OFP_PADDING_H
