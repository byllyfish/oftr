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

#ifndef OFP_SYS_UDP_CONNECTION_H_
#define OFP_SYS_UDP_CONNECTION_H_

#include "ofp/sys/connection.h"
#include "ofp/driver.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/protocolversions.h"

namespace ofp {
namespace sys {

class UDP_Server;

OFP_BEGIN_IGNORE_PADDING

class UDP_Connection : public Connection {
public:
    UDP_Connection(UDP_Server *server, ChannelMode mode, ProtocolVersions versions, ChannelListener::Factory factory);
    ~UDP_Connection();

    void connect(const udp::endpoint &remoteEndpt);
    void accept(const udp::endpoint &remoteEndpt);

    void write(const void *data, size_t length) override;
	void flush() override;
	void shutdown() override;
	
    ChannelTransport transport() const override { return ChannelTransport::UDP_Plaintext; }
    IPv6Endpoint remoteEndpoint() const override;
    IPv6Endpoint localEndpoint() const override;

private:
    UDP_Server *server_;
    udp::endpoint remoteEndpt_;

    void channelUp();
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_UDP_CONNECTION_H_
