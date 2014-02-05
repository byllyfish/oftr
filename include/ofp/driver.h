//  ===== ---- ofp/driver.h --------------------------------*- C++ -*- =====  //
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
/// \brief Defines the Driver class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_DRIVER_H_
#define OFP_DRIVER_H_

#include "ofp/channellistener.h"
#include "ofp/ipv6endpoint.h"
#include "ofp/protocolversions.h"
#include "ofp/deferred.h"

namespace ofp {

namespace sys {
class Engine;
}  // namespace sys

class Driver {
public:
  enum Role {
    Agent = 0,
    Controller,
    Bridge,
    Auxiliary // for internal use only
  };

  Driver();
  ~Driver();

  std::error_code configureTLS(const std::string &privateKeyFile,
                               const std::string &certificateFile,
                               const std::string &certificateAuthorityFile,
                               const char *privateKeyPassword);

  std::error_code listen(Role role, const IPv6Endpoint &localEndpoint,
                         ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory);

  // TODO(bfish): this should take an array of remote endpoints...
  Deferred<std::error_code> connect(Role role,
                                    const IPv6Endpoint &remoteEndpoint,
                                    ProtocolVersions versions,
                                    ChannelListener::Factory listenerFactory);

  void run();

  /// \brief Tells the driver to stop running.
  void stop();

  sys::Engine *engine() { return engine_; }

private:
  sys::Engine *engine_;
};

}  // namespace ofp

#endif  // OFP_DRIVER_H_
