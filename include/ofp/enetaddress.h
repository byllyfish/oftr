#ifndef OFP_ENETADDRESS_H
#define OFP_ENETADDRESS_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class EnetAddress {
public:
	enum { Length = 6 };
	
	EnetAddress() = default;
	explicit EnetAddress(const std::string &s);

	bool parse(const std::string &s);
	
	bool valid() const {
		return !IsMemFilled(addr_, sizeof(addr_), '\0');
	}

	void setAllOnes() {
		std::memset(addr_, 0xFF, sizeof(addr_));
	}

	std::string toString() const;

	bool operator==(const EnetAddress &rhs) const {
		return std::memcmp(addr_, rhs.addr_, sizeof(addr_)) == 0;
	}
	
	bool operator!=(const EnetAddress &rhs) const {
		return !(*this == rhs);
	}
	
private:
	UInt8  addr_[Length];
};


} // </namespace ofp>

#endif //OFP_ENETADDRESS_H
