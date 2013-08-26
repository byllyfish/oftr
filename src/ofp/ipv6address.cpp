#include "ofp/ipv6address.h"
#include "ofp/sys/boost_asio.h"
#include <arpa/inet.h>
#include "ofp/log.h"


ofp::IPv6Address::IPv6Address(const IPv4Address &addr)
{
	std::memcpy(&addr_[12], &addr, 4);
	addr_[11] = 0xFF;
	std::memset(addr_.data(), 0, 11);
}

ofp::IPv6Address::IPv6Address(const ArrayType &a) : addr_(a)
{
}

ofp::IPv6Address::IPv6Address(const std::string &s)
{
	if (!parse(s))
		addr_.fill(0);
}

bool ofp::IPv6Address::parse(const std::string &s)
{
	using namespace boost::asio;
	boost::system::error_code err;

	ip::address_v6 addr6 = ip::address_v6::from_string(s, err);
	if (!err) {
		addr_ = addr6.to_bytes();
		return true;
	}

	ip::address_v4 addr4 = ip::address_v4::from_string(s, err);
	if (!err) {
		auto a = addr4.to_bytes();
		std::copy(a.data(), a.data()+4, &addr_[12]);
		addr_[11] = 0xFF;
		std::memset(addr_.data(), 0, 11);
		return true;
	}

	return false;
}


std::string ofp::IPv6Address::toString() const
{
	if (isV4Mapped()) {
		IPv4Address v4 = toV4();
		return v4.toString();
	}

	char ipv6str[INET6_ADDRSTRLEN] = {};
	const char *result = inet_ntop(AF_INET6, addr_.data(), ipv6str, sizeof(ipv6str));
	return result ? ipv6str : "<inet_ntop_error6>";
}