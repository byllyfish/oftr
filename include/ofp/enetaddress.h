#ifndef OFP_ENETADDRESS_H
#define OFP_ENETADDRESS_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class EnetAddress {
public:
	enum { Length = 6 };
	
	EnetAddress() = default;
	
	
private:
	UInt8  addr_[Length];
};

} // </namespace ofp>

#endif //OFP_ENETADDRESS_H
