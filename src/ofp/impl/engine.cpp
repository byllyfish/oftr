#include "ofp/impl/engine.h"
#include "ofp/log.h"
#include "ofp/impl/tcp_server.h"

using namespace boost::asio;

ofp::impl::Engine::Engine(DriverOptions *options)
{
	log::debug(__PRETTY_FUNCTION__);
}

void ofp::impl::Engine::listen(Driver::Role role, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);

	tcp::endpoint endpt;
	if (!localAddress.valid()) {
		endpt = tcp::endpoint{tcp::v6(), localPort};
	} else {
		ip::address_v6 addr{localAddress.toBytes()};
		endpt = tcp::endpoint{addr, localPort};
	}

	(void) new TCP_Server{this, role, endpt, versions, listenerFactory};
}


ofp::Deferred<ofp::Exception> ofp::impl::Engine::connect(Driver::Role role, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);

	tcp::endpoint endpt;
	if (remoteAddress.isV4Mapped()) {
		ip::address_v4 addr{ remoteAddress.toV4().toBytes() };
		endpt = tcp::endpoint{addr, remotePort};
	} else {
		ip::address_v6 addr{remoteAddress.toBytes()};
		endpt = tcp::endpoint{addr, remotePort};
	}

	log::debug("connect endpt: ", endpt);

	return std::make_shared<TCP_Connection>(this, role, versions, listenerFactory)->asyncConnect(endpt);


#if 0
	TCP_ new TCP_Connection{this, role, versions, listenerFactory}
	socket.async_connect([this,socket,role,versions,listenerFactory](error_code err) {
        if (!err) {
            std::make_shared<TCP_Connection>(this, std::move(socket_), role_,
                                             versions_, factory_)->start();
        } else {
        	// FIXME handle error.
        }
    });

	TCP

	tcp::endpoint endpt;
	if (!localAddress.valid()) {
		endpt = tcp::endpoint{tcp::v6(), localPort};
	} else {
		ip::address_v6 addr{localAddress.toBytes()};
		endpt = tcp::endpoint{addr, localPort};
	}

	TCP_Connection *conn = new TCP_Connection{this, role, versions, listenerFactory};
	conn->connect(endpt);
#endif
}

void ofp::impl::Engine::run()
{
	log::debug(__PRETTY_FUNCTION__);

	try {
		io_.run();
	} catch (const std::exception &ex) {
		std::cout << ex.what() << '\n';
	}
}
