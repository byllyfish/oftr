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

	auto tcpEndpt = makeTCPEndpoint(localAddress, localPort);
	auto udpEndpt = makeUDPEndpoint(localAddress, localPort);
	auto result = Deferred<Exception>::makeResult();

	try {
		auto tcpsvr = std::unique_ptr<TCP_Server>{new TCP_Server{this, role, tcpEndpt, versions, listenerFactory}};
		auto udpsvr = std::unique_ptr<UDP_Server>{new UDP_Server{this, role, udpEndpt, versions}};

		(void) tcpsvr.release();
		(void) udpsvr.release();

	} catch (const boost::system::system_error &ex) {
		log::debug("System error caught in Engine::listen: ", ex.code());
		result->done(makeException(ex.code()));
	}

	return result;
}


ofp::Deferred<ofp::Exception> ofp::impl::Engine::connect(Driver::Role role, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);

	tcp::endpoint endpt = makeTCPEndpoint(remoteAddress, remotePort);
	log::debug("connect endpt: ", endpt);

	return std::make_shared<TCP_Connection>(this, role, versions, listenerFactory)->asyncConnect(endpt);
}

ofp::Exception ofp::impl::Engine::run()
{
	log::debug(__PRETTY_FUNCTION__);

	Exception result;

	try {
		io_.run();

	} catch (const boost::system::system_error &ex) {
		log::debug("System error caught in Engine::run(): ", ex.code());
		result = makeException(ex.code());

	} catch (const std::exception &ex) {
		log::debug("Unexpected exception caught in Engine::run(): ", ex.what());
	}

	return result;
}
