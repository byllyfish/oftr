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

#ifndef OFP_SYS_TCP_CONNECTION_H_
#define OFP_SYS_TCP_CONNECTION_H_

#include "ofp/types.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/engine.h"
#include "ofp/message.h"
#include "ofp/sys/connection.h"
#include "ofp/driver.h"
#include "ofp/deferred.h"
#include "ofp/sys/buffered.h"
#include "ofp/sys/handler_allocator.h"

namespace ofp {
namespace sys {

OFP_BEGIN_IGNORE_PADDING

template <class SocketType>
class TCP_Connection : public std::enable_shared_from_this<TCP_Connection<SocketType>>,
                       public Connection {
public:
  TCP_Connection(Engine *engine, ChannelMode mode, ProtocolVersions versions,
                 ChannelListener::Factory factory);
  TCP_Connection(Engine *engine, tcp::socket socket, ChannelMode mode,
                 ProtocolVersions versions, ChannelListener::Factory factory);
  ~TCP_Connection();

  void asyncConnect(const IPv6Endpoint &remoteEndpt, std::function<void(Channel*,std::error_code)> resultHandler);
  void asyncAccept();

  IPv6Endpoint remoteEndpoint() const override;
  IPv6Endpoint localEndpoint() const override;
  
  void write(const void *data, size_t length) override {
    socket_.buf_write(data, length);
  }

  void flush() override;
  void shutdown() override;

  Transport transport() const override { return Transport::TCP; }

private:
  Message message_;
  Buffered<SocketType> socket_;
  std::chrono::steady_clock::time_point latestActivity_;
  handler_allocator allocator_;
  bool isChannelUp_ = false;

  void channelUp();
  void channelDown();

  void asyncReadHeader();
  void asyncReadMessage(size_t length);
  void asyncWrite();
  void asyncHandshake(bool isClient);

  void updateLatestActivity() {
    latestActivity_ = std::chrono::steady_clock::now();
  }
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#include "ofp/sys/tcp_connection.ipp"

#endif  // OFP_SYS_TCP_CONNECTION_H_
