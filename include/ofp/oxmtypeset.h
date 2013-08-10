#ifndef OFP_OXMTYPESET_H
#define OFP_OXMTYPESET_H

#include "ofp/oxmtype.h"
#include <bitset>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"

namespace ofp { // <namespace ofp>

class OXMTypeSet {
public:

	OXMTypeSet() {}

	bool find(OXMType type);
	bool add(OXMType type);

private:
	
	enum { MaxOXMClasses = 10 };

	struct ClassEntry {
		std::bitset<128> fields;
		UInt16 oxmClass;
	};

	// TODO replace with std::array?
	ClassEntry classes_[MaxOXMClasses];
	unsigned classCount_ = 0;
};

} // </namespace ofp>

#pragma clang diagnostic pop // ignored "-Wpadded"

#endif // OFP_OXMTYPESET_H
