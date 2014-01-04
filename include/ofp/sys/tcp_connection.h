//  ===== ---- ofp/sys/tcp_connection.h --------------------*- C++ -*- =====  //
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
/// \brief Defines the sys::TCP_Connection class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_TCP_CONNECTION_H
#define OFP_SYS_TCP_CONNECTION_H

#include "ofp/types.h"
#include "ofp/sys/boost_asio.h"
#include "ofp/sys/engine.h"
#include "ofp/message.h"
#include "ofp/sys/connection.h"
#include "ofp/driver.h"
#include "ofp/deferred.h"
#include "ofp/exception.h"
#include "ofp/sys/plaintext.h"

OFP_BEGIN_IGNORE_PADDING

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

class TCP_Server;

class TCP_Connection : public std::enable_shared_from_this<TCP_Connection>,
                       public Connection {
public:
  TCP_Connection(Engine *engine, Driver::Role role, ProtocolVersions versions,
                 ChannelListener::Factory factory);
  TCP_Connection(Engine *engine, tcp::socket socket, Driver::Role role,
                 ProtocolVersions versions, ChannelListener::Factory factory);
  TCP_Connection(Engine *engine, DefaultHandshake *handshake);
  ~TCP_Connection();

  Deferred<Exception> asyncConnect(const tcp::endpoint &endpt,
                                   milliseconds delay = 0_ms);
  void asyncAccept();

  void channelUp();
  void channelException(const Exception &exc);
  void channelDown();

  bool isOutgoing() const { return endpoint_.port() != 0; }

  bool wantsReconnect() const {
    return handshake()->role() == Driver::Agent && isOutgoing();
  }

  tcp::endpoint endpoint() const { return endpoint_; }

  IPv6Address remoteAddress() const override;
  UInt16 remotePort() const override;

  void write(const void *data, size_t length) override;
  void flush() override;
  void shutdown() override;

  Transport transport() const { return Transport::TCP; }

  void openAuxChannel(UInt8 auxID, Transport transport) override {
    engine()->openAuxChannel(auxID, transport, this);
  }

  Channel *findAuxChannel(UInt8 auxID) const override { return nullptr; }

private:
  Message message_;
  Plaintext<tcp::socket> socket_;
  tcp::endpoint endpoint_;
  DeferredResultPtr<Exception> deferredExc_ = nullptr;
  steady_timer idleTimer_;
  std::chrono::steady_clock::time_point latestActivity_;

  // Use a two buffer strategy for async-writes. We queue up data in one
  // buffer while we're in the process of writing the other buffer.
  ByteList outgoing_[2];
  int outgoingIdx_ = 0;
  bool writing_ = false;
  //std::function<void()> flushCallback_ = nullptr;

  log::Lifetime lifetime_{"TCP_Connection"};

  void asyncReadHeader();
  void asyncReadMessage(size_t length);
  void asyncWrite();
  void asyncConnect();
  void asyncDelayConnect(milliseconds delay);
  void asyncIdleCheck();

  void reconnect();
  void updateLatestActivity();
};

} // </namespace sys>
} // </namespace ofp>

OFP_END_IGNORE_PADDING

#endif // OFP_SYS_TCP_CONNECTION_H
