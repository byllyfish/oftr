#ifndef OFP_BOOST_ASIO_H
#define OFP_BOOST_ASIO_H

#include <boost/asio.hpp>
#include "ofp/exception.h"
#include "ofp/log.h"

namespace ofp { // <namespace ofp>
namespace impl { // <namespace impl>

using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;
using io_service = boost::asio::io_service;
using error_code = boost::system::error_code;


inline bool isEOF(const error_code &error) {
	using namespace boost::asio::error;
	return error.value() == eof && error.category() == misc_category;
}


inline Exception makeException(const error_code &error)
{
	Exception::Category category{ {'*', '*', '*', '*'} };

	const char *name = error.category().name();
	if (name != nullptr) {
		size_t len = std::strlen(name);
		if (len >= 9 && name[4] == '.') {
			// Skip "asio."
			std::copy(name+5, name+9, category.begin());
		} else if (len >= 4) {

			std::copy(name, name+4, category.begin());
		}
	}

	return Exception{category, error.value()};
}


struct HashEndpoint {
	size_t operator()(const udp::endpoint &endpt) const;
};


} // </namespace impl>
} // </namespace ofp>

#endif // OFP_BOOST_ASIO_H
