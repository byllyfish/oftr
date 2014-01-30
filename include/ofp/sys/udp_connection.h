//  ===== ---- ofp/sys/udp_connection.h --------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the sys::UDP_Connection class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_UDP_CONNECTION_H
#define OFP_SYS_UDP_CONNECTION_H

#include "ofp/sys/connection.h"
#include "ofp/driver.h"
#include "ofp/sys/boost_asio.h"
#include "ofp/protocolversions.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

class UDP_Server;

OFP_BEGIN_IGNORE_PADDING
class UDP_Connection : public Connection {
public:
    UDP_Connection(UDP_Server *server, Driver::Role role, ProtocolVersions versions, udp::endpoint remoteEndpt);
    ~UDP_Connection();

    void write(const void *data, size_t length) override;
	void flush() override;
	void shutdown() override;
	
    IPv6Endpoint remoteEndpoint() const override {
        return convertEndpoint<udp>(remoteEndpt_);
    }

    Transport transport() const { return Transport::UDP; }
    void openAuxChannel(UInt8 auxID, Transport transport) override {}       // should not be called
    Channel *findAuxChannel(UInt8 auxID) const override { return 0; }

private:
    UDP_Server *server_;
    udp::endpoint remoteEndpt_;
    log::Lifetime lifetime_{"UDP_Connection"};
};
OFP_END_IGNORE_PADDING

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_UDP_CONNECTION_H
