#include "ofp/ipv6address.h"
#include <arpa/inet.h>


ofp::IPv6Address::IPv6Address(const std::string &s)
{
	int result = inet_pton(AF_INET6, s.c_str(), addr_);
	if (result <= 0) {
		std::memset(addr_, 0, sizeof(addr_));
	}
}


std::string ofp::IPv6Address::toString() const
{
	char ipv6str[INET6_ADDRSTRLEN] = {};
	const char *result = inet_ntop(AF_INET6, addr_, ipv6str, sizeof(ipv6str));
	return result ? ipv6str : "<inet_ntop_error6>";
}