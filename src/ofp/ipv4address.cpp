#include "ofp/ipv4address.h"
#include <arpa/inet.h>


ofp::IPv4Address::IPv4Address(const ArrayType &a) : addr_(a)
{
}


ofp::IPv4Address::IPv4Address(const std::string &s)
{
	// TODO this function should handle / notation. ie. "/24" should give you a
	// network mask 255.255.255.0. and a.b.c.d/24 should give you a.b.c.0

	if (!parse(s)) {
		addr_.fill(0);
	}
}


bool ofp::IPv4Address::parse(const std::string &s)
{
	int result = inet_pton(AF_INET, s.c_str(), addr_.data());
	return (result > 0);
}


std::string ofp::IPv4Address::toString() const
{
	char ipv4str[INET_ADDRSTRLEN] = {};
	const char *result = inet_ntop(AF_INET, addr_.data(), ipv4str, sizeof(ipv4str));
	return result ? ipv4str : "<inet_ntop_error4>";
}

