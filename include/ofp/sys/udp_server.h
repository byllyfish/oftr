//  ===== ---- ofp/sys/udp_server.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the UDP_Server class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_UDP_SERVER_H
#define OFP_SYS_UDP_SERVER_H

#include "ofp/types.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/server.h"
#include "ofp/ipv6address.h"
#include "ofp/message.h"
#include "ofp/driver.h"
#include <unordered_map>

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

class Engine;
class UDP_Connection;

OFP_BEGIN_IGNORE_PADDING

class UDP_Server : public Server {
public:

	enum { MaxDatagramLength = 2000 };  // FIXME?

	UDP_Server(Engine *engine, Driver::Role role, const udp::endpoint &endpt, ProtocolVersions versions, std::error_code &error);
	~UDP_Server();

	// Used by UDP_Connections to manage their lifetimes.
	void add(UDP_Connection *conn);
	void remove(UDP_Connection *conn);

	void write(const void *data, size_t length);
	void flush(udp::endpoint endpt);
	
	Engine *engine() const { return engine_; }

private:
	using ConnectionMap = std::unordered_map<IPv6Endpoint,UDP_Connection*, HashEndpoint>;

	Engine *engine_;
	Driver::Role role_;
	ProtocolVersions versions_;
	udp::socket socket_;
	udp::endpoint sender_;
	Message message_;
	ConnectionMap connMap_;
	bool shuttingDown_ = false;
	log::Lifetime lifetime_{"UDP_Server"};

	void listen(const udp::endpoint &endpt, std::error_code &error);
	void asyncReceive();
	void asyncSend();

	void dispatchMessage();
};

OFP_END_IGNORE_PADDING

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_UDP_SERVER_H
