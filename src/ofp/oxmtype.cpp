#include "ofp/oxmtype.h"
#include "ofp/log.h"

// These globals are declared in the generated header `ofp/oxmfields.h`.
// Unfortunately, we cannot #include that file, because doing so will create a 
// circular make dependency.

namespace ofp {
	extern const OXMTypeInfo OXMTypeInfoArray[];
	extern const size_t OXMTypeInfoArraySize;
};


const ofp::OXMTypeInfo *ofp::OXMType::lookupInfo()
{
	// Get unmasked value before we search for it.
	UInt32 value32 = hasMask() ? withoutMask() : value32_;

	// TODO: More efficient lookup.
	for (size_t i = 0; i < ofp::OXMTypeInfoArraySize; ++i) {
		if (value32 == ofp::OXMTypeInfoArray[i].value32) {
			return &ofp::OXMTypeInfoArray[i];
		}
	}

	return nullptr;
}