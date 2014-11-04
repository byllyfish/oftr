// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_DEFAULTHANDSHAKE_H_
#define OFP_DEFAULTHANDSHAKE_H_

#include "ofp/channellistener.h"
#include "ofp/protocolversions.h"
#include "ofp/driver.h"

namespace ofp {

namespace sys {
class Connection;
}  // namespace sys

OFP_BEGIN_IGNORE_PADDING

class DefaultHandshake : public ChannelListener {
 public:
  DefaultHandshake(sys::Connection *channel, ChannelMode mode,
                   ProtocolVersions versions, Factory listenerFactory);

  void onChannelUp(Channel *channel) override;
  void onChannelDown(Channel *channel) override;
  void onMessage(const Message *message) override;

  ChannelMode mode() const { return mode_; }
  ProtocolVersions versions() const { return versions_; }

  void setStartingVersion(UInt8 version) { startingVersion_ = version; }
  void setStartingXid(UInt32 xid) { startingXid_ = xid; }
  void setConnection(sys::Connection *channel) { channel_ = channel; }

 private:
  sys::Connection *channel_;
  ProtocolVersions versions_;
  Factory listenerFactory_;
  ChannelMode mode_;
  UInt32 startingXid_ = 0;
  UInt8 startingVersion_ = 0;

  void onHello(const Message *message);
  void onFeaturesReply(const Message *message);
  void onError(const Message *message);

  void installNewChannelListener(const Message *message);
  void clearChannelListener();
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_DEFAULTHANDSHAKE_H_
