#ifndef OFP_IPV4ADDRESS_H
#define OFP_IPV4ADDRESS_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class IPv4Address {
public:
	enum { Length = 4 };
	
	IPv4Address() = default;
	explicit IPv4Address(const std::string &s);

	bool valid() const {
		return !IsMemFilled(addr_, sizeof(addr_), '\0');
	}

	std::string toString() const;

	bool operator==(const IPv4Address &rhs) const {
		return std::memcmp(addr_, rhs.addr_, Length) == 0;
	}
	
	bool operator!=(const IPv4Address &rhs) const {
		return !(*this == rhs);
	}

private:
	UInt8 addr_[Length];
};

} // </namespace ofp>

#endif //OFP_IPV4ADDRESS_H
