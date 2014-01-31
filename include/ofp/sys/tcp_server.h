//  ===== ---- ofp/sys/tcp_server.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the sys::TCP_Server class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_TCP_SERVER_H
#define OFP_SYS_TCP_SERVER_H

#include "ofp/types.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/server.h"
#include "ofp/driver.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

class Engine;

OFP_BEGIN_IGNORE_PADDING

class TCP_Server : public Server {
public:
  TCP_Server(Engine *engine, Driver::Role role, const tcp::endpoint &endpt,
             ProtocolVersions versions,
             ChannelListener::Factory listenerFactory, std::error_code &error);
  ~TCP_Server();

private:
  log::Lifetime lifetime_{"TCP_Server"};
  Engine *engine_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
  Driver::Role role_;
  ProtocolVersions versions_;
  ChannelListener::Factory factory_;

  void listen(const tcp::endpoint &localEndpt, std::error_code &error);
  void asyncAccept();
};

OFP_END_IGNORE_PADDING

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_TCP_SERVER_H
