#ifndef OFP_SYS_BOOST_ASIO_H
#define OFP_SYS_BOOST_ASIO_H

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

#include "ofp/exception.h"
#include "ofp/log.h"
#include "ofp/ipv6address.h"

namespace ofp {  // <namespace ofp>
namespace sys { // <namespace sys>

using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;
using io_service = boost::asio::io_service;
using error_code = boost::system::error_code;
using steady_timer = boost::asio::steady_timer;


inline bool isAsioEOF(const error_code &error)
{
    using namespace boost::asio::error;
    return error.value() == eof && error.category() == misc_category;
}

inline bool isAsioCanceled(const error_code &error)
{
    using namespace boost::asio::error;
    return error.value() == ECANCELED && error.category() == system_category;
}

inline Exception makeException(const boost::system::error_code &error)
{
    Exception::Category category{{'*', '*', '*', '*'}};

    const char *name = error.category().name();
    if (name != nullptr) {
        size_t len = std::strlen(name);
        if (len >= 9 && name[4] == '.') {
            // Skip "asio."
            std::copy(name + 5, name + 9, category.begin());
        } else if (len >= 4) {

            std::copy(name, name + 4, category.begin());
        }
    }

    return Exception{category, error.value()};
}

struct HashEndpoint {
    size_t operator()(const udp::endpoint &endpt) const;
};

inline IPv6Address makeIPv6Address(const boost::asio::ip::address &addr)
{
	if (addr.is_v6()) {
		return IPv6Address{addr.to_v6().to_bytes()};
	} else {
		assert(addr.is_v4());
		IPv4Address v4{addr.to_v4().to_bytes()};
		return IPv6Address{v4};
	}
}

inline tcp::endpoint makeTCPEndpoint(const IPv6Address &addr, UInt16 port)
{
	using namespace boost::asio;

    if (!addr.valid()) {
        return tcp::endpoint{tcp::v6(), port};
    } else if (addr.isV4Mapped()) {
        ip::address_v4 v4{addr.toV4().toArray()};
        return tcp::endpoint{v4, port};
    } else {
        ip::address_v6 v6{addr.toArray()};
        return tcp::endpoint{v6, port};
    }
}

inline udp::endpoint makeUDPEndpoint(const IPv6Address &addr, UInt16 port)
{
	using namespace boost::asio;

    if (!addr.valid()) {
        return udp::endpoint{udp::v6(), port};
    } else if (addr.isV4Mapped()) {
        ip::address_v4 v4{addr.toV4().toArray()};
        return udp::endpoint{v4, port};
    } else {
        ip::address_v6 v6{addr.toArray()};
        return udp::endpoint{v6, port};
    }
}

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_BOOST_ASIO_H
