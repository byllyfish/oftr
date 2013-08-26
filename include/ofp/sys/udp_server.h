#ifndef OFP_IMPL_UDP_SERVER_H
#define OFP_IMPL_UDP_SERVER_H

#include "ofp/types.h"
#include "ofp/sys/boost_asio.h"
#include "ofp/ipv6address.h"
#include "ofp/message.h"
#include "ofp/driver.h"
#include <unordered_map>

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

class Engine;
class UDP_Connection;

OFP_BEGIN_IGNORE_PADDING
class UDP_Server {
public:

	enum { MaxDatagramLength = 2000 };  // FIXME?

	UDP_Server(Engine *engine, Driver::Role role, const udp::endpoint &endpt, ProtocolVersions versions);

	// Used by UDP_Connections to manage their lifetimes.
	void add(UDP_Connection *conn);
	void remove(UDP_Connection *conn);

	void write(const void *data, size_t length);
	void flush(udp::endpoint endpt);
	
	Engine *engine() const { return engine_; }

private:
	using ConnectionMap = std::unordered_map<udp::endpoint,UDP_Connection*,HashEndpoint>;

	Engine *engine_;
	Driver::Role role_;
	ProtocolVersions versions_;
	udp::socket socket_;
	udp::endpoint sender_;
	Message message_;
	ConnectionMap connMap_;
	bool shuttingDown_ = false;
	log::Lifetime lifetime_{"UDP_Server"};

	void asyncReceive();
	void asyncSend();

	void dispatchMessage();
};
OFP_END_IGNORE_PADDING

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_IMPL_UDP_SERVER_H
