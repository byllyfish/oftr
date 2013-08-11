#ifndef OFP_BOOST_IMPL_H
#define OFP_BOOST_IMPL_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>

namespace ofp { // <namespace ofp>
namespace impl { // <namespace impl>

using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;
using io_service = boost::asio::io_service;
using error_code = boost::system::error_code;

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_BOOST_IMPL_H
