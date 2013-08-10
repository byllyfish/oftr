#ifndef OFP_DATAPATHID_H
#define OFP_DATAPATHID_H

#include "ofp/byteorder.h"


namespace ofp { // <namespace ofp>

class DatapathID {
public:

	explicit DatapathID(UInt64 dpid = 0) : dpid_{dpid} {}

	std::string toString() const;
	
private:
	Big64 dpid_;
};

} // </namespace ofp>

#endif // OFP_DATAPATHID_H
