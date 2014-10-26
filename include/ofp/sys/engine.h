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
#include "ofp/sys/tcp_server.h"
#include "ofp/sys/identity.h"
#include <map>

namespace ofp {
namespace sys {

OFP_BEGIN_IGNORE_PADDING

// RAII Utility class to prevent modification while iterating. (Not thread-safe)
class IterLock {
public: 
  explicit IterLock(bool &ref, bool val=true) : val_{ref}, ref_{ref} { ref_ = val; }
  ~IterLock() { ref_ = val_; }

private:
  bool val_;
  bool &ref_; 
};

class Connection;

class Engine {
public:
  explicit Engine(Driver *driver);
  ~Engine();

  UInt64 listen(ChannelMode mode, UInt64 securityId, const IPv6Endpoint &localEndpoint,
                         ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory, std::error_code &error);

  UInt64 connect(ChannelMode mode, UInt64 securityId, const IPv6Endpoint &remoteEndpoint,
                        ProtocolVersions versions, ChannelListener::Factory listenerFactory,
                        std::function<void(Channel*,std::error_code)> resultHandler);

  UInt64 connectUDP(ChannelMode mode, UInt64 securityId, const IPv6Endpoint &remoteEndpoint,
                        ProtocolVersions versions, ChannelListener::Factory listenerFactory,
                        std::error_code &error);

  size_t close(UInt64 connId);

  UInt64 addIdentity(const std::string &certFile, const std::string &password, const std::string &verifier, std::error_code &error);

  void run();
  void stop(Milliseconds timeout = 0_ms);
  bool isRunning() const { return isRunning_; }
  void installSignalHandlers();

  asio::io_service &io() { return io_; }
  asio::ssl::context *securityContext(UInt64 securityId);

  Driver *driver() const { return driver_; }

  bool registerDatapath(Connection *channel);
  void releaseDatapath(Connection *channel);

  UInt64 registerServer(TCP_Server *server);
  void releaseServer(TCP_Server *server);

  UInt64 registerConnection(Connection *connection);
  void releaseConnection(Connection *connection);

  template <class UnaryFunc>
  void forEachConnection(UnaryFunc func) {
    IterLock lock{connListLock_};
    std::for_each(connList_.begin(), connList_.end(), func);
  }

  template <class UnaryFunc>
  void forEachTCPServer(UnaryFunc func) {
    IterLock lock{serverListLock_};
    std::for_each(serverList_.begin(), serverList_.end(), func);
  }

  template <class UnaryFunc>
  void forEachUDPServer(UnaryFunc func) {
    if (udpConnect_) {
      func(udpConnect_.get());
    }
  }

  template <class UnaryPredicate>
  Connection *findConnection(UnaryPredicate func) const {
    IterLock lock{connListLock_};
    auto iter = std::find_if(connList_.begin(), connList_.end(), func);
    return iter != connList_.end() ? *iter : nullptr;
  }

  template <class UnaryPredicate>
  TCP_Server *findTCPServer(UnaryPredicate func) const {
    IterLock lock{serverListLock_};
    auto iter = std::find_if(serverList_.begin(), serverList_.end(), func);
    return iter != serverList_.end() ? *iter : nullptr;
  }

  Connection *findDatapath(const DatapathID &dpid, UInt64 connId) const;

  UInt64 assignConnectionId();
  UInt64 assignSecurityId();

private:
  // Pointer to driver object that owns engine.
  Driver *driver_;

  using DatapathMap = std::map<DatapathID, Connection *>;
  using ServerList = std::vector<TCP_Server *>;
  using ConnectionList = std::vector<Connection *>;

  // The identities_ vector must be the last object destroyed, since io_service
  // objects may depend on it.

  std::vector<std::unique_ptr<Identity>> identities_;

  ConnectionList connList_;
  ServerList serverList_;
  DatapathMap dpidMap_;
  std::shared_ptr<UDP_Server> udpConnect_;
  UInt64 lastConnId_ = 0;
  UInt64 lastSecurityId_ = 0;
  
  // The io_service must be one of the first objects to be destroyed when
  // engine destructor runs. Connections may need to update bookkeeping objects.
  asio::io_service io_{1};
  bool isRunning_ = false;

  // Sets up signal handlers to shut down runloop.
  bool isSignalsInited_ = false;
  asio::signal_set signals_;

  // Timer that can be used to stop the engine.
  asio::steady_timer stopTimer_;


  mutable bool connListLock_ = false;
  mutable bool serverListLock_ = false;
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_ENGINE_H_
