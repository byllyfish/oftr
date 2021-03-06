// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_TCP_SERVER_H_
#define OFP_SYS_TCP_SERVER_H_

#include "ofp/driver.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/types.h"

namespace ofp {
namespace sys {

class Engine;
class UDP_Server;

OFP_BEGIN_IGNORE_PADDING

class TCP_Server : public std::enable_shared_from_this<TCP_Server> {
  // Private token parameter used to keep constructor private with make_shared.
  class PrivateToken {};

 public:
  static std::shared_ptr<TCP_Server> create(
      Engine *engine, ChannelOptions options, UInt64 securityId,
      const IPv6Endpoint &localEndpt, ProtocolVersions versions,
      ChannelListener::Factory listenerFactory, std::error_code &error);

  TCP_Server(PrivateToken t, Engine *engine, ChannelOptions options,
             UInt64 securityId, const IPv6Endpoint &localEndpt,
             ProtocolVersions versions,
             ChannelListener::Factory listenerFactory);
  ~TCP_Server();

  IPv6Endpoint localEndpoint() const;
  UInt64 connectionId() const { return connId_; }
  void shutdown();

 private:
  Engine *engine_;
  tcp::acceptor acceptor_;
  ChannelOptions options_;
  ProtocolVersions versions_;
  ChannelListener::Factory factory_;
  UInt64 connId_ = 0;
  UInt64 securityId_;
  std::shared_ptr<UDP_Server> udpServer_;

  void asyncListen(const IPv6Endpoint &localEndpt, std::error_code &error);
  void listen(const IPv6Endpoint &localEndpt, std::error_code &error);
  void asyncAccept();
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_TCP_SERVER_H_
