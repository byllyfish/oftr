//  ===== ---- ofp/channel.h -------------------------------*- C++ -*- =====  //
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
/// \brief Defines the abstract Channel class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_CHANNEL_H_
#define OFP_CHANNEL_H_

#include "ofp/writable.h"
#include "ofp/ipv6endpoint.h"
#include "ofp/datapathid.h"
#include "ofp/channeltransport.h"

namespace ofp {

class Driver;
class ChannelListener;
class DatapathID;

class Channel : public Writable {
 public:
  virtual ~Channel() {}

  virtual Driver *driver() const = 0;
  virtual UInt64 connectionId() const = 0;
  virtual DatapathID datapathId() const = 0;
  virtual UInt8 auxiliaryId() const = 0;
  virtual ChannelTransport transport() const = 0;
  virtual IPv6Endpoint remoteEndpoint() const = 0;
  virtual IPv6Endpoint localEndpoint() const = 0;
  virtual void shutdown() = 0;

  virtual ChannelListener *channelListener() const = 0;
  virtual void setChannelListener(ChannelListener *listener) = 0;

  // The following methods are provided for OpenFlow agents.
  // The failure to open an auxiliary channel will be reported to the main
  // listener via onException.

  virtual void setStartingXid(UInt32 xid) = 0;
};

std::ostream &operator<<(std::ostream &os, Channel *channel);

inline std::ostream &operator<<(std::ostream &os, Channel *channel) {
  return os << "[Channel to=" << channel->remoteEndpoint() << ']';
}

}  // namespace ofp

#endif  // OFP_CHANNEL_H_
