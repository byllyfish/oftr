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
#include "ofp/channelmode.h"
#include "ofp/channeltransport.h"

namespace ofp {

namespace sys {
class Engine;
}  // namespace sys

class Driver {
public:
  Driver();
  ~Driver();

  UInt64 listen(ChannelMode mode, UInt64 securityId, const IPv6Endpoint &localEndpoint,
                         ProtocolVersions versions,
                         ChannelListener::Factory listenerFactory, std::error_code &error);

  UInt64 connect(ChannelMode mode, UInt64 securityId, const IPv6Endpoint &remoteEndpoint, 
    ProtocolVersions versions, ChannelListener::Factory listenerFactory, 
    std::function<void(Channel*,std::error_code)> resultHandler);

  UInt64 connectUDP(ChannelMode mode, UInt64 securityId, const IPv6Endpoint &remoteEndpoint, 
    ProtocolVersions versions, ChannelListener::Factory listenerFactory, 
    std::error_code &error);

  void run();

  /// \brief Tells the driver to stop running.
  void stop(Milliseconds timeout = 0_ms);

  sys::Engine *engine() { return engine_; }

  void installSignalHandlers();
  
private:
  sys::Engine *engine_;
};

}  // namespace ofp

#endif  // OFP_DRIVER_H_
