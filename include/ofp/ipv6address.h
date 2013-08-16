#ifndef OFP_IPV6ADDRESS_H
#define OFP_IPV6ADDRESS_H

#include "ofp/types.h"
#include "ofp/ipv4address.h"
#include <array>

namespace ofp { // <namespace ofp>

class IPv6Address {
public:
	enum { Length = 16 };
	
	using ArrayType = std::array<UInt8,Length>;

	IPv6Address() = default;
	IPv6Address(const IPv4Address &addr);
	explicit IPv6Address(const ArrayType &a);
	explicit IPv6Address(const std::string &s);

	bool valid() const {
		return !IsMemFilled(addr_.data(), sizeof(addr_), '\0');
	}

	bool isV4Mapped() const {
		return IsMemFilled(addr_.data(), 11, '\0') && (addr_[11] == 0xFF);
	}

	IPv4Address toV4() const {
		assert(isV4Mapped());
		IPv4Address v4;
		std::memcpy(&v4, &addr_[12], sizeof(v4));
		return v4;
	}

	std::string toString() const;

	ArrayType toArray() const {
		return addr_;
	}

	bool operator==(const IPv6Address &rhs) const {
		return addr_ == rhs.addr_;
	}
	
	bool operator!=(const IPv6Address &rhs) const {
		return !(*this == rhs);
	}

private:
	ArrayType addr_;
};


std::ostream &operator<<(std::ostream &os, const IPv6Address &value);

} // </namespace ofp>


inline std::ostream &ofp::operator<<(std::ostream &os, const ofp::IPv6Address &value)
{
	return os << value.toString();
}

#endif //OFP_IPV6ADDRESS_H
