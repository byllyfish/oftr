#include "ofp/ipv4address.h"
#include <arpa/inet.h>


ofp::IPv4Address::IPv4Address(const std::string &s)
{
	int result = inet_pton(AF_INET, s.c_str(), addr_);
	if (result <= 0) {
		std::memset(addr_, 0, sizeof(addr_));
	}
}

std::string ofp::IPv4Address::toString() const
{
	char ipv4str[INET_ADDRSTRLEN] = {};
	const char *result = inet_ntop(AF_INET, addr_, ipv4str, sizeof(ipv4str));
	return result ? ipv4str : "<inet_ntop_error4>";
}

