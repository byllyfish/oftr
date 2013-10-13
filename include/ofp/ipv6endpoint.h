#ifndef OFP_IPV6ENDPOINT_H
#define OFP_IPV6ENDPOINT_H

#include "ofp/ipv6address.h"
#include <istream>
#include "ofp/log.h"

namespace ofp { // <namespace ofp>

class IPv6Endpoint {
public:
	IPv6Endpoint() = default;
	IPv6Endpoint(const IPv6Address &addr, UInt16 port) : addr_{addr}, port_{port} {}
	IPv6Endpoint(const std::string &addr, UInt16 port) : addr_{addr}, port_{port} {}
	explicit IPv6Endpoint(UInt16 port) : addr_{}, port_{port} {}

	bool parse(const std::string &s);
	void clear();
	
	bool valid() const { return port_ != 0; }
	const IPv6Address &address() const { return addr_; }
	UInt16 port() const { return port_; }

	void setAddress(const IPv6Address &addr) { addr_ = addr; }
	void setPort(UInt16 port) { port_ = port; }

	std::string toString() const;
	
private:
	IPv6Address addr_;
	UInt16 port_ = 0;
};

std::istream &operator>>(std::istream &is, IPv6Endpoint &value);
std::ostream &operator<<(std::ostream &os, const IPv6Endpoint &value);


inline std::istream &operator>>(std::istream &is, IPv6Endpoint &value)
{
	std::string str;
	is >> str;
	if (!value.parse(str)) {
		is.setstate(std::ios::failbit);
	}
	return is;
}

inline std::ostream &operator<<(std::ostream &os, const IPv6Endpoint &value)
{
	return os << value.toString();
}

} // </namespace ofp>

#endif // OFP_IPV6ENDPOINT_H
