#include "ofp/sys/engine.h"
#include "ofp/log.h"
#include "ofp/sys/tcp_server.h"
#include "ofp/sys/udp_server.h"

using namespace boost::asio;

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>


Engine::Engine(Driver *driver, DriverOptions *options) : driver_{driver}
{
	log::debug(__PRETTY_FUNCTION__);


}

Deferred<Exception> Engine::listen(Driver::Role role, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
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


Deferred<Exception> Engine::connect(Driver::Role role, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);

	tcp::endpoint endpt = makeTCPEndpoint(remoteAddress, remotePort);

	return std::make_shared<TCP_Connection>(this, role, versions, listenerFactory)->asyncConnect(endpt);
}


void Engine::reconnect(DefaultHandshake *handshake, const IPv6Address &remoteAddress, UInt16 remotePort)
{
	tcp::endpoint endpt = makeTCPEndpoint(remoteAddress, remotePort);

	(void) std::make_shared<TCP_Connection>(this, handshake)->asyncConnect(endpt);
}


Exception Engine::run()
{
	Exception result;

	try {
		io_.run();

	} catch (const boost::system::system_error &ex) {
		log::debug("System error caught in Engine::run(): ", ex.code());
		result = makeException(ex.code());

	} catch (std::exception &ex) {
		log::debug("Unexpected exception caught in Engine::run(): ", ex.what());
		// FIXME - set result here?
	}

	return result;
}


} // </namespace sys>
} // </namespace ofp>

