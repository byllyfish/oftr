// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_DEFAULTHANDSHAKE_H_
#define OFP_SYS_DEFAULTHANDSHAKE_H_

#include "ofp/channellistener.h"
#include "ofp/driver.h"
#include "ofp/protocolversions.h"
#include "ofp/bytelist.h"
#include "ofp/portrange.h"

namespace ofp {

class FeaturesReply;

namespace sys {
class Connection;
}  // namespace sys

OFP_BEGIN_IGNORE_PADDING

class DefaultHandshake : public ChannelListener {
 public:
  DefaultHandshake(sys::Connection *channel, ChannelOptions options,
                   ProtocolVersions versions, Factory listenerFactory);

  void onChannelUp(Channel *channel) override;
  void onChannelDown(Channel *channel) override;
  void onMessage(Message *message) override;
  bool onTickle(Channel *channel, TimePoint now) override;

  ChannelOptions options() const { return options_; }
  ProtocolVersions versions() const { return versions_; }

  void setStartingXid(UInt32 xid) { startingXid_ = xid; }
  void setConnection(sys::Connection *channel) { channel_ = channel; }

  const FeaturesReply *featuresReply() const { 
    if (featuresReply_.empty()) return nullptr;
    return reinterpret_cast<const FeaturesReply *>(featuresReply_.data());
  }

 private:

  enum HandshakeState {
    kHandshakeInit,
    kSentHello,
    kSentFeaturesRequest,
    kSentPortRequest
  };

  sys::Connection *channel_;
  ProtocolVersions versions_;
  Factory listenerFactory_;
  ChannelOptions options_;
  UInt32 startingXid_ = 0;
  HandshakeState state_ = kHandshakeInit;
  TimePoint timeStarted_;
  ByteList featuresReply_;

  void onHello(const Message *message);
  void onFeaturesReply(Message *message);
  void onPortDescReply(Message *message);
  void onError(const Message *message);

  void installNewChannelListener();
  void clearChannelListener();

  bool wantFeatures() const {
    return (options_ & ChannelOptions::FEATURES_REQ) != 0;
  }

  void sendHello();
  void sendFeaturesRequest();
  void sendPortDescRequest();
  void appendPortsToFeaturesReply(PortRange ports);
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_SYS_DEFAULTHANDSHAKE_H_
