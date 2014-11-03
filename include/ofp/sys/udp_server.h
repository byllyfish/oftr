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

#ifndef OFP_SYS_UDP_SERVER_H_
#define OFP_SYS_UDP_SERVER_H_

#include "ofp/types.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/ipv6address.h"
#include "ofp/message.h"
#include "ofp/driver.h"
#include "ofp/sys/datagram.h"
#include <unordered_map>
#include <deque>

namespace ofp {
namespace sys {

class Engine;
class UDP_Connection;

OFP_BEGIN_IGNORE_PADDING

class UDP_Server : public std::enable_shared_from_this<UDP_Server> {
  // Private token parameter used to keep constructor private with make_shared.
  class PrivateToken {};

 public:
  static std::shared_ptr<UDP_Server> create(Engine *engine, ChannelMode mode,
                                            const IPv6Endpoint &localEndpt,
                                            ProtocolVersions versions,
                                            UInt64 connId,
                                            std::error_code &error);
  static std::shared_ptr<UDP_Server> create(Engine *engine,
                                            std::error_code &error);

  UDP_Server(PrivateToken t, Engine *engine, ChannelMode mode,
             ProtocolVersions versions, UInt64 connId);
  UDP_Server(PrivateToken t, Engine *engine);
  ~UDP_Server();

  // Make an outgoing connection.
  UInt64 connect(const IPv6Endpoint &remoteEndpt,
                 ChannelListener::Factory factory, std::error_code &error);

  UInt64 connectionId() const { return connId_; }
  IPv6Endpoint localEndpoint() const;
  void shutdown();

  // Used by UDP_Connections to manage their lifetimes.
  void add(UDP_Connection *conn);
  void remove(UDP_Connection *conn);

  void write(const void *data, size_t length);
  void flush(udp::endpoint endpt, UInt64 connId);

  Engine *engine() const { return engine_; }

 private:
  using ConnectionMap = std::unordered_map<IPv6Endpoint, UDP_Connection *>;
  enum { MaxDatagramLength = 2000 };  // FIXME?

  Engine *engine_;
  ChannelMode mode_;
  ProtocolVersions versions_;
  udp::socket socket_;
  udp::endpoint sender_;
  udp protocol_ = udp::v6();
  Message message_;
  ConnectionMap connMap_;
  UInt64 connId_ = 0;
  std::deque<Datagram> datagrams_;

  void asyncListen(const IPv6Endpoint &localEndpt, std::error_code &error);
  void listen(const IPv6Endpoint &localEndpt, std::error_code &error);
  void asyncReceive();
  void asyncSend();

  void dispatchMessage();
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_UDP_SERVER_H_
