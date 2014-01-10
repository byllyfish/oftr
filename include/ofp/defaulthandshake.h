//  ===== ---- ofp/defaulthandshake.h ----------------------*- C++ -*- =====  //
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
/// \brief Defines the DefaultHandshake class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_DEFAULTHANDSHAKE_H
#define OFP_DEFAULTHANDSHAKE_H

#include "ofp/channellistener.h"
#include "ofp/protocolversions.h"
#include "ofp/driver.h"

namespace ofp { // <namespace ofp>

namespace sys { // <namespace sys>
class Connection;
} // </namespace sys>

OFP_BEGIN_IGNORE_PADDING
class DefaultHandshake : public ChannelListener {
public:
  DefaultHandshake(sys::Connection *channel, Driver::Role role,
                   ProtocolVersions versions, Factory listenerFactory);

  void onChannelUp(Channel *channel) override;
  void onChannelDown(Channel *channel) override;
  void onMessage(const Message *message) override;
  void onException(const Exception *exception) override;
  void onTimer(UInt32 /* timerID */) override {}

  Driver::Role role() const { return role_; }
  ProtocolVersions versions() const { return versions_; }

  void setStartingVersion(UInt8 version) { startingVersion_ = version; }
  void setStartingXid(UInt32 xid) { startingXid_ = xid; }
  void setConnection(sys::Connection *channel) { channel_ = channel; }

private:
  sys::Connection *channel_;
  ProtocolVersions versions_;
  Factory listenerFactory_;
  Driver::Role role_;
  UInt32 startingXid_ = 0;
  UInt8 startingVersion_ = 0;

  void onHello(const Message *message);
  //void onFeaturesRequest(const Message *message);
  void onFeaturesReply(const Message *message);
  void onError(const Message *message);

  void replyError(UInt16 type, UInt16 code, const Message *message);
  void installNewChannelListener(const Message *message);
  void installAuxiliaryChannelListener(const Message *message);
};
OFP_END_IGNORE_PADDING

} // </namespace ofp>

#endif // OFP_DEFAULTHANDSHAKE_H
