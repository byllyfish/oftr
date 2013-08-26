#ifndef OFP_IMPL_TCP_SERVER_H
#define OFP_IMPL_TCP_SERVER_H

#include "ofp/types.h"
#include "ofp/sys/boost_asio.h"
#include "ofp/sys/tcp_connection.h"

namespace ofp {  // <namespace ofp>
namespace sys { // <namespace sys>

class Engine;

OFP_BEGIN_IGNORE_PADDING
class TCP_Server {
public:

    TCP_Server(Engine *engine, Driver::Role role, const tcp::endpoint &endpt, ProtocolVersions versions, ChannelListener::Factory listenerFactory);
    ~TCP_Server();

private:
    log::Lifetime lifetime_{"TCP_Server"};
	Engine *engine_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    Driver::Role role_;
    ProtocolVersions versions_;
    ChannelListener::Factory factory_;
    
    void asyncAccept();
};
OFP_END_IGNORE_PADDING

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_IMPL_TCP_SERVER_H
