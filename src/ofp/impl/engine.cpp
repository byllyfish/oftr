#include "ofp/impl/engine.h"
#include "ofp/log.h"
#include "ofp/impl/tcp_server.h"
#include "ofp/impl/udp_server.h"

using namespace boost::asio;

ofp::impl::Engine::Engine(Driver *driver, DriverOptions *options) : driver_{driver}
{
	log::debug(__PRETTY_FUNCTION__);


}

ofp::Deferred<ofp::Exception> ofp::impl::Engine::listen(Driver::Role role, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);

	tcp::endpoint tcpEndpt;
	udp::endpoint udpEndpt;
	if (!localAddress.valid()) {
		tcpEndpt = tcp::endpoint{tcp::v6(), localPort};
		udpEndpt = udp::endpoint{udp::v6(), localPort};
	} else {
		ip::address_v6 addr{localAddress.toBytes()};
		tcpEndpt = tcp::endpoint{addr, localPort};
		udpEndpt = udp::endpoint{addr, localPort};
	}

	auto result = Deferred<Exception>::makeResult();

	TCP_Server *tcpServer = nullptr;
	try {
		tcpServer = new TCP_Server{this, role, tcpEndpt, versions, listenerFactory};

	} catch (std::exception &ex) {
		log::debug("Caught exception in engine! ");

		// FIXME
	}

	UDP_Server *udpServer = new UDP_Server{this, role, udpEndpt, versions};

	return result;
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
