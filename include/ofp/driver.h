// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DRIVER_H_
#define OFP_DRIVER_H_

#include "ofp/channellistener.h"
#include "ofp/ipv6endpoint.h"
#include "ofp/protocolversions.h"
#include "ofp/channeloptions.h"
#include "ofp/channeltransport.h"

namespace ofp {

namespace sys {
class Engine;
}  // namespace sys

class Driver {
 public:
  Driver();
  ~Driver();

  UInt64 listen(ChannelOptions options, UInt64 securityId,
                const IPv6Endpoint &localEndpoint, ProtocolVersions versions,
                ChannelListener::Factory listenerFactory,
                std::error_code &error);

  UInt64 connect(ChannelOptions options, UInt64 securityId,
                 const IPv6Endpoint &remoteEndpoint, ProtocolVersions versions,
                 ChannelListener::Factory listenerFactory,
                 std::function<void(Channel *, std::error_code)> resultHandler);

  void run();

  /// \brief Tells the driver to stop running.
  void stop(Milliseconds timeout = 0_ms);

  sys::Engine *engine() { return engine_; }

  void installSignalHandlers(std::function<void()> callback = nullptr);

 private:
  sys::Engine *engine_;
};

}  // namespace ofp

#endif  // OFP_DRIVER_H_
