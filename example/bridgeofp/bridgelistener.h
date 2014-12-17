// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef EXAMPLE_BRIDGEOFP_BRIDGELISTENER_H_
#define EXAMPLE_BRIDGEOFP_BRIDGELISTENER_H_

#include "ofp/ofp.h"

OFP_BEGIN_IGNORE_PADDING

namespace bridge {  // <namespace bridge>

using namespace ofp;

class BridgeListener : public ChannelListener {
 public:
  explicit BridgeListener(BridgeListener *otherBridge);
  explicit BridgeListener(const IPv6Endpoint &remoteEndpoint);
  ~BridgeListener();

  void onChannelUp(Channel *channel) override;
  // void onChannelDown(Channel *channel) override;

  void onMessage(const Message *message) override;
  // void onException(const Exception *exception) override;

  Channel *channel() const { return channel_; }

 private:
  Channel *channel_ = nullptr;
  IPv6Endpoint remoteEndpoint_;
  BridgeListener *otherBridge_ = nullptr;

  void forget(BridgeListener *other);

  static void translateAndForward(const Message *message, Channel *channel);
};

}  // </namespace bridge>

OFP_END_IGNORE_PADDING

#endif  // EXAMPLE_BRIDGEOFP_BRIDGELISTENER_H_
