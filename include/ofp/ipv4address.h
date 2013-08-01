#ifndef OFP_IPV4ADDRESS_H
#define OFP_IPV4ADDRESS_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class IPv4Address {
public:
	enum { Length = 4 };
	
	IPv4Address() = default;
	
private:
	UInt8 addr_[Length];
};

} // </namespace ofp>

#endif //OFP_IPV4ADDRESS_H
