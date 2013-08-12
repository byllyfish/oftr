#include "ofp/ipv6address.h"
#include "ofp/impl/boost_asio.h"
#include <arpa/inet.h>
#include "ofp/log.h"

ofp::IPv6Address::IPv6Address(const std::string &s)
{
	using namespace boost::asio;

	boost::system::error_code err;

	log::debug("string is ", s);

	std::memset(addr_, 0, sizeof(addr_));

	ip::address_v6 addr6 = ip::address_v6::from_string(s, err);
	if (!err) {
		auto iter = addr6.to_bytes().begin();
		std::copy(iter, iter+16, addr_);
	} else {
		log::debug("IPv6Address ", err);

		ip::address_v4 addr4 = ip::address_v4::from_string(s, err);
		if (!err) {
			auto iter = addr4.to_bytes().begin();
			std::copy(iter, iter+4, &addr_[12]);
			addr_[11] = 0xFF;
		} else {
			log::debug("IPv4Address ", err);
		}
	}

#if 0	
	int result = inet_pton(AF_INET6, s.c_str(), addr_);
	if (result <= 0) {
		std::memset(addr_, 0, sizeof(addr_));
	}
#endif
}


std::string ofp::IPv6Address::toString() const
{
	char ipv6str[INET6_ADDRSTRLEN] = {};
	const char *result = inet_ntop(AF_INET6, addr_, ipv6str, sizeof(ipv6str));
	return result ? ipv6str : "<inet_ntop_error6>";
}