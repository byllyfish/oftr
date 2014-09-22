//  ===== ---- ofp/sys/engine.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the sys::Engine class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_ENGINE_H_
#define OFP_SYS_ENGINE_H_

#include "ofp/driver.h"
#include "ofp/sys/asio_utils.h"
#include "ofp/defaulthandshake.h"
#include "ofp/channel.h"
#include "ofp/datapathid.h"
#include <map>

namespace ofp {
namespace sys {

class Server;
class TCP_Server;
class Connection; // FIXME can I use Connection here?

OFP_BEGIN_IGNORE_PADDING

class Engine {
public:
  explicit Engine(Driver *driver);
  ~Engine();

  std::error_code configureTLS(const std::string &privateKeyFile,
                               const std::string &certificateFile,
                               const std::string &certificateAuthorityFile,
                               const char *privateKeyPassword);

  UInt64 listen(ChannelMode mode, const IPv6Endpoint &localEndpoint,
                         ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory, std::error_code &error);

  UInt64 connect(ChannelMode mode, const IPv6Endpoint &remoteEndpoint,
                        ProtocolVersions versions, ChannelListener::Factory listenerFactory,
                        std::function<void(Channel*,std::error_code)> resultHandler);

  void run();
  void stop(Milliseconds timeout = 0_ms);
  bool isRunning() const { return isRunning_; }
  void installSignalHandlers();

  asio::io_service &io() { return io_; }

  asio::ssl::context &context() { return context_; }

  Driver *driver() const { return driver_; }
  bool isTLSDesired() const { return isTLSDesired_; }

  void registerDatapath(Connection *channel);
  void releaseDatapath(Connection *channel);

  UInt64 registerServer(TCP_Server *server);
  void releaseServer(TCP_Server *server);

  UInt64 registerConnection(Connection *connection);
  void releaseConnection(Connection *connection);

  size_t connectionCount() const { return connList_.size(); }
  size_t serverCount() const { return serverList_.size(); }

  template <class UnaryFunc>
  void forEachConnection(UnaryFunc func) {
    std::for_each(connList_.begin(), connList_.end(), func);
  }

  template <class UnaryFunc>
  void forEachServer(UnaryFunc func) {
    std::for_each(serverList_.begin(), serverList_.end(), func);
  }

  template <class UnaryPredicate>
  Connection *findConnection(UnaryPredicate func) const {
    auto iter = std::find_if(connList_.begin(), connList_.end(), func);
    return iter != connList_.end() ? *iter : nullptr;
  }

  template <class UnaryPredicate>
  TCP_Server *findServer(UnaryPredicate func) const {
    auto iter = std::find_if(serverList_.begin(), serverList_.end(), func);
    return iter != serverList_.end() ? *iter : nullptr;
  }

  Connection *findDatapath(const DatapathID &dpid, UInt64 connId) const;

private:
  // Pointer to driver object that owns engine.
  Driver *driver_;

  using DatapathMap = std::map<DatapathID, Connection *>;
  using ServerList = std::vector<TCP_Server *>;
  using ConnectionList = std::vector<Connection *>;

  ConnectionList connList_;
  ServerList serverList_;
  DatapathMap dpidMap_;
  UInt64 lastConnId_ = 0;
  
  // The io_service must be one of the first objects to be destroyed when
  // engine destructor runs. Connections may need to update bookkeeping objects.
  asio::io_service io_{1};
  bool isRunning_ = false;

  // SSL context
  asio::ssl::context context_;
  bool isTLSDesired_ = false;

  // Sets up signal handlers to shut down runloop.
  bool isSignalsInited_ = false;
  asio::signal_set signals_;

  // Timer that can be used to stop the engine.
  asio::steady_timer stopTimer_;

  UInt64 assignConnId();
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_ENGINE_H_
