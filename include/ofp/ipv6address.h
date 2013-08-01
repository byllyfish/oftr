#ifndef OFP_IPV6ADDRESS_H
#define OFP_IPV6ADDRESS_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class IPv6Address {
public:
	enum { Length = 16 };
	
	IPv6Address() = default;
	
private:
	UInt8 addr_[Length];
};

} // </namespace ofp>

#endif //OFP_IPV6ADDRESS_H
