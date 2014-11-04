// Copyright 2014-present Bill Fisher. All rights reserved.

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
