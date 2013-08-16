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
    UDP_Connection(UDP_Server *server, Driver::Role role, ProtocolVersions versions, udp::endpoint remoteEndpt);
    ~UDP_Connection();

    void write(const void *data, size_t length) override;
	void flush() override;
	void close() override;
	
    IPv6Address remoteAddress() const override {
        return makeIPv6Address(remoteEndpoint().address());
    }

    UInt16 remotePort() const override {
        return remoteEndpoint().port();
    }

    udp::endpoint remoteEndpoint() const;

    Transport transport() const { return Transport::UDP; }
    void openAuxChannel(UInt8 auxID, Transport transport) override {}       // should not be called
    Channel *findAuxChannel(UInt8 auxID) const override { return 0; }

private:
    UDP_Server *server_;
    udp::endpoint remoteEndpt_;
    log::Lifetime lifetime_{"UDP_Connection"};
};
OFP_END_IGNORE_PADDING

} // </namespace impl>
} // </namespace ofp>


inline ofp::impl::udp::endpoint ofp::impl::UDP_Connection::remoteEndpoint() const
{
	return remoteEndpt_;
}


#endif // OFP_UDP_CONNECTION_H
