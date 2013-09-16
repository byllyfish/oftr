#include "ofp/sys/engine.h"
#include "ofp/log.h"
#include "ofp/sys/tcp_server.h"
#include "ofp/sys/udp_server.h"
#include "ofp/defaultauxiliarylistener.h"

using namespace boost::asio;

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

namespace detail { // <namespace detail>

OFP_BEGIN_IGNORE_PADDING
/// \brief Protects a variable's value using RAII. When the scope of exits, the
/// protected variable is restored to its value at entry.
template <class Type>
class VarProtect {
public:
	explicit VarProtect(Type *ptr) : ptr_{ptr}, val_{*ptr} {}
	~VarProtect() { *ptr_ = val_; }

private:
	Type *ptr_;
	Type val_;
};
OFP_END_IGNORE_PADDING

} // </namespace detail>

Engine::Engine(Driver *driver, DriverOptions *options) : driver_{driver}
{
	log::debug(__PRETTY_FUNCTION__);
}

Engine::~Engine()
{
	// Copy the serverlist into a temporary and clear the original list to help
	// speed things up; servers attempt to remove themselves from the server
	// list when they are destroyed.

	ServerList servers;
	std::swap(servers, serverList_);
	assert(serverList_.empty());

	for (auto svr : servers) {
		delete svr;
	}
}

Deferred<Exception> Engine::listen(Driver::Role role, const Features *features, const IPv6Address &localAddress, UInt16 localPort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
	auto tcpEndpt = makeTCPEndpoint(localAddress, localPort);
	auto udpEndpt = makeUDPEndpoint(localAddress, localPort);
	auto result = Deferred<Exception>::makeResult();

	try {
		auto tcpsvr = std::unique_ptr<TCP_Server>{new TCP_Server{this, role, features, tcpEndpt, versions, listenerFactory}};
		auto udpsvr = std::unique_ptr<UDP_Server>{new UDP_Server{this, role, features, udpEndpt, versions}};

		(void) tcpsvr.release();
		(void) udpsvr.release();

		// Pass back success.
		result->done(Exception{});

	} catch (const boost::system::system_error &ex) {
		log::debug("System error caught in Engine::listen: ", ex.code());
		result->done(makeException(ex.code()));
	}

	return result;
}


Deferred<Exception> Engine::connect(Driver::Role role, const Features *features, const IPv6Address &remoteAddress, UInt16 remotePort, ProtocolVersions versions, ChannelListener::Factory listenerFactory)
{
	log::debug(__PRETTY_FUNCTION__);

	tcp::endpoint endpt = makeTCPEndpoint(remoteAddress, remotePort);

	auto connPtr = std::make_shared<TCP_Connection>(this, role, versions, listenerFactory);
	if (features != nullptr) {
		connPtr->setFeatures(*features);
	}
	return connPtr->asyncConnect(endpt);
}


void Engine::reconnect(DefaultHandshake *handshake, const Features *features, const IPv6Address &remoteAddress, UInt16 remotePort, std::chrono::milliseconds delay)
{
	tcp::endpoint endpt = makeTCPEndpoint(remoteAddress, remotePort);

	auto connPtr = std::make_shared<TCP_Connection>(this, handshake);
	if (features != nullptr) {
		connPtr->setFeatures(*features);
	}
	(void) connPtr->asyncConnect(endpt, delay);
}


void Engine::run()
{
	if (!isRunning_) {
		detail::VarProtect<bool> protect{&isRunning_};
		isRunning_ = true;

		// Set isRunning_ to true when we are in io.run(). This guards against
		// re-entry and provides a flag to test when shutting down.

		try {
			io_.run();

		} catch (std::exception &ex) {
			log::debug("Unexpected exception caught in Engine::run(): ", ex.what());
		}

		isRunning_ = false;
	}
}


void Engine::quit()
{	
	io_.stop();
}


void Engine::openAuxChannel(UInt8 auxID, Channel::Transport transport, TCP_Connection *mainConnection)
{
	// Find the localport of the mainConnection.
	// Look up the apppropriate server object if necessary (for UDP).
	// Create connection using AuxChannelListener and set up connection to main
	// connection. Note that deferred from connection attempt must be sent to
	// mainConnection.
	
	if (transport == Channel::Transport::TCP) {
		log::debug("openAuxChannel", auxID);

		tcp::endpoint endpt = mainConnection->endpoint();
		DefaultHandshake *hs = mainConnection->handshake();
		ProtocolVersions versions = hs->versions();

		ChannelListener::Factory listenerFactory = [](){
			return new DefaultAuxiliaryListener;
		};

		// FIXME should Auxiliary connections use a null listenerFactory? (Use defaultauxiliarylistener by default?)
		auto connPtr = std::make_shared<TCP_Connection>(this, Driver::Auxiliary, versions, listenerFactory);
		
		Features features = mainConnection->features();
		features.setAuxiliaryId(auxID);
		connPtr->setFeatures(features);
		connPtr->setMainConnection(mainConnection);

		Deferred<Exception> result = connPtr->asyncConnect(endpt);

		// FIXME where does the exception go?
		//result.done([mainConnection](Exception exc){
		//	mainConnection
		//});
	}
}


void Engine::postDatapathID(Connection *channel)
{
	DatapathID dpid = channel->datapathId();
	UInt8 auxID = channel->auxiliaryId();

	if (auxID == 0) {
		// Insert main connection's datapathID into the dpidMap, if not present
		// already.
		auto pair = dpidMap_.insert({dpid, channel});
		if (!pair.second) {
			// DatapathID already exists in map. If the existing channel is 
			// different, close it and replace it with the new one.
			auto item = pair.first;
			if (item->second != channel) {
				log::info("Engine.postDatapathID: Conflict between main connections detected. Closing old.", dpid);
				Connection *old = item->second;
				item->second = channel;
				old->shutdown();

			} else {
				log::info("Engine.postDatapathID: DatapathID is already registered.", dpid);
			}
		}
		
	} else {
		assert(auxID != 0);

		// Look up main connection and assign auxiliary connection to it. If we
		// don't find a main connection, close the auxiliary channel.
		auto item = dpidMap_.find(dpid);
		if (item != dpidMap_.end()) {
			channel->setMainConnection(item->second);

		} else {
			log::info("Engine.postDatapathID: Main connection not found.", dpid);
			channel->shutdown();
		}
	}
}

void Engine::releaseDatapathID(Connection *channel)
{
	DatapathID dpid = channel->datapathId();
	UInt8 auxID = channel->auxiliaryId();

	if (auxID == 0) {
		// When releasing the datapathID for a main connection, we need to
		// verify that the datapathID _is_ registered to this connection.
		auto item = dpidMap_.find(dpid);
		if (item != dpidMap_.end()) {
		 	if (item->second == channel)
				dpidMap_.erase(item);
		} else if (isRunning_) {
			log::debug("Engine.releaseDatapathID: Unable to find datapathID.", dpid);
		}
	} else {
		log::debug("Engine.releaseDatapathID called for auxID", auxID);
	}
}

void Engine::registerServer(Server *server)
{
	serverList_.push_back(server);
}


void Engine::releaseServer(Server *server)
{
	auto iter = std::find(serverList_.begin(), serverList_.end(), server);
	if (iter != serverList_.end()) {
		serverList_.erase(iter);
	}
}


} // </namespace sys>
} // </namespace ofp>
