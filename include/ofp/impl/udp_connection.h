#ifndef OFP_UDP_CONNECTION_H
#define OFP_UDP_CONNECTION_H

#include "ofp/internalchannel.h"
#include "ofp/driver.h"
#include "ofp/impl/boost_asio.h"
#include "ofp/protocolversions.h"

namespace ofp { // <namespace ofp>
namespace impl { // <namespace impl>

class UDP_Server;

OFP_BEGIN_IGNORE_PADDING
class UDP_Connection : public InternalChannel {
public:
    UDP_Connection(UDP_Server *server, Driver::Role role, ProtocolVersions versions, udp::endpoint endpt);
    ~UDP_Connection();

    void write(const void *data, size_t length) override;
	void flush() override;
	void close() override;
	
    udp::endpoint endpoint() const;
    void openAuxChannel() override {}       // should not be called

private:
    UDP_Server *server_;
    udp::endpoint endpt_;
    log::Lifetime lifetime_{"UDP_Connection"};
};
OFP_END_IGNORE_PADDING

} // </namespace impl>
} // </namespace ofp>


inline ofp::impl::udp::endpoint ofp::impl::UDP_Connection::endpoint() const
{
	return endpt_;
}


#endif // OFP_UDP_CONNECTION_H
