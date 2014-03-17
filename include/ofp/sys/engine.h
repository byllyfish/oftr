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
// class TCP_Server;
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

  std::error_code listen(Driver::Role role, const IPv6Endpoint &localEndpoint,
                         ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory);

  Deferred<std::error_code> connect(Driver::Role role,
                                    const IPv6Endpoint &remoteEndpoint,
                                    ProtocolVersions versions,
                                    ChannelListener::Factory listenerFactory);

  void reconnect(DefaultHandshake *handshake,
                 const IPv6Endpoint &remoteEndpoint, Milliseconds delay);

  void run();
  void stop(Milliseconds timeout = 0_ms);
  bool isRunning() const { return isRunning_; }
  void installSignalHandlers();

  void openAuxChannel(UInt8 auxID, Channel::Transport transport,
                      Connection *mainConnection);

  asio::io_service &io() { return io_; }

  asio::ssl::context &context() { return context_; }

  Driver *driver() const { return driver_; }
  bool isTLSDesired() const { return isTLSDesired_; }

  void postDatapathID(Connection *channel);
  void releaseDatapathID(Connection *channel);

  void registerServer(Server *server);
  void releaseServer(Server *server);

private:
  // Pointer to driver object that owns engine.
  Driver *driver_;

  using DatapathMap = std::map<DatapathID, Connection *>;
  using ServerList = std::vector<Server *>;

  DatapathMap dpidMap_;
  ServerList serverList_;

  // The io_service must be one of the first objects to be destroyed when
  // engine destructor runs. Connections may need to bookkeeping objects.
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
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_ENGINE_H_
