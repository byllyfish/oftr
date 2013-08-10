#ifndef OFP_TCP_SERVER_IMPL_H
#define OFP_TCP_SERVER_IMPL_H

#include "ofp/types.h"
#include "ofp/boost_impl.h"
#include "ofp/tcp_connection_impl.h"

namespace ofp {  // <namespace ofp>
namespace impl { // <namespace impl>

class Driver_Impl;

class TCP_Server {
public:

    TCP_Server(Driver_Impl *driver, const tcp::endpoint &endpt);
    ~TCP_Server();

    Driver_Impl *driver() const { return driver_; }

private:
	Driver_Impl *driver_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;

    void asyncAccept();
    //void onAccept(TCP_SharedConnPtr conn, const error_code &error);
};

} // </namespace impl>
} // </namespace ofp>

#endif // OFP_TCP_SERVER_IMPL_H
