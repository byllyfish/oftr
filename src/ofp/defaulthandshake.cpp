//  ===== ---- ofp/defaulthandshake.cpp --------------------*- C++ -*- =====  //
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
/// \brief Implements DefaultHandshake class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/defaulthandshake.h"
#include "ofp/sys/connection.h"
#include "ofp/hello.h"
#include "ofp/message.h"
#include "ofp/headeronly.h"
#include "ofp/featuresreply.h"
#include "ofp/error.h"
#include "ofp/log.h"
#include "ofp/constants.h"
#include "ofp/exception.h"
#include "ofp/defaultauxiliarylistener.h"

using namespace ofp;
using sys::Connection;

DefaultHandshake::DefaultHandshake(Connection *channel, Driver::Role role,
                                   ProtocolVersions versions,
                                   Factory listenerFactory)
    : channel_{channel}, versions_{versions}, listenerFactory_{listenerFactory},
      role_{role} {
  assert(listenerFactory_ != nullptr);
}

void DefaultHandshake::onChannelUp(Channel *channel) {
  assert(channel == channel_);

  channel->setStartingXid(startingXid_);

  ProtocolVersions useVersion = versions_;
  if (startingVersion_ == OFP_VERSION_1) {
    // If the starting version is set to 1, we'll use this version on the
    // reconnect.
    useVersion = ProtocolVersions{startingVersion_};
  }

  HelloBuilder msg{useVersion};
  msg.send(channel_);
}

void DefaultHandshake::onChannelDown(Channel *channel) {
  log::info("DefaultHandshake: Channel down before controller handshake could "
            "complete.");
}

void DefaultHandshake::onMessage(const Message *message) {
  switch (message->type()) {
  case Hello::type() :
    onHello(message);
    break;

  //case FeaturesRequest::type() :
  //  onFeaturesRequest(message);
  //  break;

  case FeaturesReply::type() :
    onFeaturesReply(message);
    break;

  case Error::type() :
    onError(message);
    break;

  default:
    log::info("DefaultHandshake ignored message type", message->type());
    break;
  }
}

void DefaultHandshake::onException(const Exception *exception) {
  log::info("DefaultHandshake: protocol exception", *exception);
}

void DefaultHandshake::onHello(const Message *message) {
  const Hello *msg = Hello::cast(message);
  if (!msg)
    return;

  ProtocolVersions versions = msg->protocolVersions();
  versions = versions.intersection(versions_);

  if (versions.empty()) {
    replyError(OFPET_HELLO_FAILED, OFPHFC_INCOMPATIBLE, message);

  } else if (role_ == Driver::Controller) {
    channel_->setVersion(versions.highestVersion());
    FeaturesRequestBuilder reply{};
    reply.send(channel_);

  } else {
    assert(role_ == Driver::Bridge || role_ == Driver::Agent || role_ == Driver::Auxiliary);

    channel_->setVersion(versions.highestVersion());
    installNewChannelListener(nullptr);
  }

  // TODO handle case where we reconnected with a startingVersion of 1 but
  // the other end supports a higher version number.
}

#if 0
void DefaultHandshake::onFeaturesRequest(const Message *message) {
  // A controller shouuld not receive a features request message.
  if (role_ == Driver::Controller) {
    return; // FIXME error
  }

  const FeaturesRequest *request = FeaturesRequest::cast(message);
  if (!request)
    return; // FIXME log

  FeaturesReplyBuilder reply{message->xid()};
  //reply.setFeatures(channel_->features());
  reply.send(channel_);

  installNewChannelListener(nullptr);
}
#endif //0

void DefaultHandshake::onFeaturesReply(const Message *message) {
  // Only a controller should be receiving a features reply message.
  if (role_ != Driver::Controller) {
    return;
  }

  const FeaturesReply *msg = FeaturesReply::cast(message);
  if (!msg)
    return; // FIXME log

  // Registering the connection allows us to attach auxiliary connections to
  // their main connections.

  channel_->postDatapathId(msg->datapathId(), msg->auxiliaryId());

  if (role_ == Driver::Controller && channel_->mainConnection() != channel_) {
    installAuxiliaryChannelListener(message);
  } else {
    installNewChannelListener(message);
  }
}

void DefaultHandshake::onError(const Message *message) {
  // FIXME log it
}

void DefaultHandshake::replyError(UInt16 type, UInt16 code,
                                  const Message *message) {
  ErrorBuilder error{type, code};
  error.setErrorData(message);
  error.send(channel_);
  channel_->shutdown();
}

void DefaultHandshake::installNewChannelListener(const Message *message) {
  assert(channel_->channelListener() == this);
  assert(listenerFactory_ != nullptr);

  ChannelListener *newListener = listenerFactory_();
  channel_->setChannelListener(newListener);
  newListener->onChannelUp(channel_);

  if (message)
    newListener->onMessage(message);
}

void DefaultHandshake::installAuxiliaryChannelListener(const Message *message) {
  assert(channel_->channelListener() == this);
  assert(message);

  ChannelListener *newListener = new DefaultAuxiliaryListener;
  channel_->setChannelListener(newListener);
  newListener->onChannelUp(channel_);
  newListener->onMessage(message);
}
