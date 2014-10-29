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

using namespace ofp;
using sys::Connection;

DefaultHandshake::DefaultHandshake(Connection *channel, ChannelMode mode,
                                   ProtocolVersions versions,
                                   Factory listenerFactory)
    : channel_{channel}, versions_{versions}, listenerFactory_{listenerFactory},
      mode_{mode} {
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
  log::warning("DefaultHandshake: Channel down before handshake could complete", std::make_pair("connid", channel->connectionId()));
}

void DefaultHandshake::onMessage(const Message *message) {
  switch (message->type()) {
  case Hello::type() :
    onHello(message);
    break;

  case FeaturesReply::type() :
    onFeaturesReply(message);
    break;

  case Error::type() :
    onError(message);
    break;

  default:
    log::warning("DefaultHandshake: Ignored message type", message->type(), std::make_pair("connid", message->source()->connectionId()));
    break;
  }
}

void DefaultHandshake::onHello(const Message *message) {
  const Hello *msg = Hello::cast(message);
  if (!msg)
    return;

  UInt8 msgVersion = msg->msgHeader()->version();
  UInt8 version = versions_.negotiateVersion(msgVersion, msg->protocolVersions());

  if (version == 0) {
    // If there are no versions in common, send an error and terminate the
    // connection.
    channel_->setVersion(versions_.highestVersion());
    std::string explanation = "Supported Versions: ";
    explanation += versions_.toString();

    ErrorBuilder error{message->xid()};
    error.setErrorType(OFPET_HELLO_FAILED);
    error.setErrorCode(OFPHFC_INCOMPATIBLE);
    error.setErrorData(explanation.data(), explanation.size());
    error.send(channel_);
    channel_->shutdown();
    return;
  }

  channel_->setVersion(version);

  if (mode_ == ChannelMode::Controller) {
    FeaturesRequestBuilder reply{};
    reply.send(channel_);

  } else {
    installNewChannelListener(nullptr);
  }

  // TODO handle case where we reconnected with a startingVersion of 1 but
  // the other end supports a higher version number.
}

void DefaultHandshake::onFeaturesReply(const Message *message) {
  // Only a controller should be receiving a features reply message.
  if (mode_ != ChannelMode::Controller) {
    return;
  }

  const FeaturesReply *msg = FeaturesReply::cast(message);
  if (!msg)
    return; // FIXME log

  // Registering the connection allows us to attach auxiliary connections to
  // their main connections. A main connection (auxiliary_id == 0) cannot use
  // a UDP transport.

  if (channel_->postDatapath(msg->datapathId(), msg->auxiliaryId())) {

    if (mode_ == ChannelMode::Controller && channel_->mainConnection() != channel_) {
      assert(msg->auxiliaryId() != 0);
      // If this is an auxiliary connection, clear its channel listener. Note
      // that we do not pass the (auxiliary) FeaturesReply message to the channel.
      clearChannelListener();

    } else {
      installNewChannelListener(message);
    }

  } else {
    channel_->shutdown();
  }
}

void DefaultHandshake::onError(const Message *message) {
  // FIXME log it
}

void DefaultHandshake::installNewChannelListener(const Message *message) {
  assert(channel_->channelListener() == this);
  
  if (listenerFactory_) {
    ChannelListener *newListener = listenerFactory_();
    channel_->setChannelListener(newListener);
    newListener->onChannelUp(channel_);

    if (message)
      newListener->onMessage(message);

    ChannelListener::dispose(this);

  } else {
    clearChannelListener();
  }
}

void DefaultHandshake::clearChannelListener() {
  assert(channel_->channelListener() == this);

  channel_->setChannelListener(nullptr);
  ChannelListener::dispose(this);
}
