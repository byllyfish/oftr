// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_UDP_SERVER_H_
#define OFP_SYS_UDP_SERVER_H_

#include <deque>
#include <unordered_map>
#include "ofp/driver.h"
#include "ofp/ipv6address.h"
#include "ofp/message.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/datagram.h"
#include "ofp/types.h"

namespace ofp {
namespace sys {

class Engine;

OFP_BEGIN_IGNORE_PADDING

class UDP_Server : public std::enable_shared_from_this<UDP_Server> {
  // Private token parameter used to keep constructor private with make_shared.
  class PrivateToken {};

 public:
  static std::shared_ptr<UDP_Server> create(
      Engine *engine, ChannelOptions options, UInt64 securityId,
      const IPv6Endpoint &localEndpt, ProtocolVersions versions, UInt64 connId,
      std::error_code &error);
  static std::shared_ptr<UDP_Server> create(Engine *engine,
                                            std::error_code &error);

  UDP_Server(PrivateToken t, Engine *engine, ChannelOptions options,
             UInt64 securityId, ProtocolVersions versions, UInt64 connId);
  UDP_Server(PrivateToken t, Engine *engine);
  ~UDP_Server();

  // Make an outgoing connection.
  UInt64 connect(const IPv6Endpoint &remoteEndpt, UInt64 securityId,
                 ChannelListener::Factory factory, std::error_code &error);

  UInt64 connectionId() const { return connId_; }
  IPv6Endpoint localEndpoint() const;
  void shutdown();

  // Used by UDP_Connections to manage their lifetimes.
  void add(Connection *conn);
  void remove(Connection *conn);
  Connection *findConnection(const IPv6Endpoint &endpt);

  void send(udp::endpoint endpt, UInt64 connId, const void *data,
            size_t length);

  // void write(const void *data, size_t length);
  // void flush(udp::endpoint endpt, UInt64 connId);

  Engine *engine() const { return engine_; }

 private:
  enum { MaxDatagramLength = 4000 };  // FIXME?

  Engine *engine_;
  ChannelOptions options_;
  ProtocolVersions versions_;
  udp::socket socket_;
  udp::endpoint sender_;
  udp protocol_ = udp::v6();
  ByteList buffer_;
  std::unordered_map<IPv6Endpoint, Connection *> connMap_;
  UInt64 connId_ = 0;
  UInt64 securityId_ = 0;
  std::deque<Datagram> datagrams_;

  void asyncListen(const IPv6Endpoint &localEndpt, std::error_code &error);
  void listen(const IPv6Endpoint &localEndpt, std::error_code &error);
  void asyncReceive();
  void asyncSend();
  Connection *accept();

  void datagramReceived();
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_UDP_SERVER_H_
