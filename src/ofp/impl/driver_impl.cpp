#include "ofp/impl/driver_impl.h"
#include "ofp/log.h"
#include "ofp/impl/tcp_server.h"

using namespace boost::asio;

ofp::impl::Driver_Impl::Driver_Impl(DriverOptions *options)
{
	log::debug(__PRETTY_FUNCTION__);
}

void ofp::impl::Driver_Impl::listen(Driver::Role role, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);

	tcp::endpoint endpt;
	if (!localAddress.valid()) {
		endpt = tcp::endpoint{tcp::v6(), localPort};
	} else {
		ip::address_v6 addr{localAddress.toBytes()};
		endpt = tcp::endpoint{addr, localPort};
	}

	TCP_Server *server = new TCP_Server{this, role, endpt, versions, listenerFactory};
	servers_.push_back(server);
}


void ofp::impl::Driver_Impl::connect(Driver::Role role, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);
}

void ofp::impl::Driver_Impl::run()
{
	log::debug(__PRETTY_FUNCTION__);

	io_.run();
}
