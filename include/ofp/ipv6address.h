#ifndef OFP_IPV6ADDRESS_H
#define OFP_IPV6ADDRESS_H

#include "ofp/types.h"
#include <array>

namespace ofp { // <namespace ofp>

class IPv6Address {
public:
	enum { Length = 16 };
	
	IPv6Address() = default;
	explicit IPv6Address(const std::string &s);

	bool valid() const {
		return !IsMemFilled(addr_, sizeof(addr_), '\0');
	}

	std::string toString() const;

	std::array<UInt8,Length> toBytes() const {
		std::array<UInt8,Length> result;
		std::copy(addr_, addr_ + Length, result.begin());
		return result;
	}

	bool operator==(const IPv6Address &rhs) const {
		return std::memcmp(addr_, rhs.addr_, Length) == 0;
	}
	
	bool operator!=(const IPv6Address &rhs) const {
		return !(*this == rhs);
	}

private:
	UInt8 addr_[Length];
};

} // </namespace ofp>

#endif //OFP_IPV6ADDRESS_H
