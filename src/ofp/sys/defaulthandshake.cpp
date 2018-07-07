// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/defaulthandshake.h"
#include <sstream>
#include "ofp/constants.h"
#include "ofp/error.h"
#include "ofp/featuresreply.h"
#include "ofp/headeronly.h"
#include "ofp/hello.h"
#include "ofp/multipartrequest.h"
#include "ofp/multipartreply.h"
#include "ofp/log.h"
#include "ofp/message.h"
#include "ofp/sys/connection.h"
#include "ofp/sys/engine.h"

using namespace ofp;
using sys::Connection;

const Milliseconds kControllerKeepAliveTimeout = 10000_ms;
const Milliseconds kRawKeepAliveTimeout = 6000_ms;
const Milliseconds kHandshakeTimeout = 5000_ms;

DefaultHandshake::DefaultHandshake(Connection *channel, ChannelOptions options,
                                   ProtocolVersions versions,
                                   Factory listenerFactory)
    : channel_{channel},
      versions_{versions},
      listenerFactory_{listenerFactory},
      options_{options} {}

void DefaultHandshake::onChannelUp(Channel *channel) {
  assert(channel == channel_);
  assert(state_ == kHandshakeInit);

  channel->setStartingXid(startingXid_);
  sendHello();
}

void DefaultHandshake::onChannelDown(Channel *channel) {
  log_warning("DefaultHandshake: Channel down before handshake could complete",
              std::make_pair("connid", channel->connectionId()));
}

bool DefaultHandshake::onTickle(Channel *channel, TimePoint now) {
  assert(channel == channel_);

  auto age = now - timeStarted_;
  if (age >= kHandshakeTimeout) {
    auto msec = std::chrono::duration_cast<Milliseconds>(age).count();
    log_warning("DefaultHandshake: Timed out; age is", msec,
                std::make_pair("connid", channel_->connectionId()));
    channel_->shutdown();
  }

  return true;
}

void DefaultHandshake::onMessage(Message *message) {
  switch (message->type()) {
    case Hello::type():
      onHello(message);
      break;

    case FeaturesReply::type():
      onFeaturesReply(message);
      break;

    case Error::type():
      onError(message);
      break;

    case MultipartReply::type():
      if (message->subtype() == OFPMP_PORT_DESC) {
        onPortDescReply(message);
      }
      break;

    default:
      log_warning("DefaultHandshake: Ignored message type", message->type(),
                  std::make_pair("connid", message->source()->connectionId()));
      break;
  }
}

void DefaultHandshake::onHello(const Message *message) {
  if (state_ > kSentHello) {
    log_warning("DefaultHandshake: Extra Hello message");
    return;
  }

  const Hello *msg = Hello::cast(message);
  if (!msg) {
    log_warning("DefaultHandshake: Invalid Hello message");
    return;
  }

  const Header *header = msg->msgHeader();
  UInt8 msgVersion = header->version();
  ProtocolVersions msgVersionSet = msg->protocolVersions();
  UInt8 version = versions_.negotiateVersion(msgVersion, msgVersionSet);

  if (version == 0) {
    // If there are no versions in common, send an error and terminate the
    // connection.
    channel_->setVersion(versions_.highestVersion());

    std::stringstream sstr;
    sstr << "Incompatible OpenFlow version: ";
    sstr << static_cast<int>(msgVersion) << ' ' << msgVersionSet.toString();
    sstr << " Supported versions: " << versions_.toString();
    auto explanation = sstr.str();

    log_warning(explanation,
                std::make_pair("connid", channel_->connectionId()));
    channel_->engine()->alert(channel_, explanation, {header, sizeof(*header)});

    if (!wantFeatures()) {
      // If we're not a controller, send an error back.
      message->replyError(OFPHFC_INCOMPATIBLE, explanation);
    }
    channel_->shutdown();
    return;
  }

  log_debug("Negotiated version is", static_cast<int>(version));

  channel_->setVersion(version);

  log_info("OpenFlow version:", static_cast<int>(msgVersion),
           "Peer versions:", msg->protocolVersions().toString(),
           std::make_pair("connid", channel_->connectionId()));

  if (wantFeatures()) {
    channel_->setKeepAliveTimeout(kControllerKeepAliveTimeout);
    sendFeaturesRequest();

  } else {
    channel_->setKeepAliveTimeout(kRawKeepAliveTimeout);
    installNewChannelListener();
  }
}

void DefaultHandshake::onFeaturesReply(Message *message) {
  if (state_ != kSentFeaturesRequest) {
    log_warning("DefaultHandshake: Odd FeaturesReply message");
    return;
  }

  const FeaturesReply *msg = FeaturesReply::cast(message);
  if (!msg) {
    log_warning("DefaultHandshake: Invalid FeaturesReply message");
    return;
  }

  // Registering the connection allows us to attach auxiliary connections to
  // their main connections. A main connection (auxiliary_id == 0) cannot use
  // a UDP transport.

  if (channel_->postDatapath(msg->datapathId(), msg->auxiliaryId())) {
    if ((options_ & ChannelOptions::AUXILIARY) != 0 &&
        channel_->mainConnection() != channel_) {
      assert(msg->auxiliaryId() != 0);
      // This is an auxiliary connection, clear its channel listener. Note
      // that we do not pass the (auxiliary) FeaturesReply message to the
      // channel.
      clearChannelListener();

    } else {
      featuresReply_.set(msg, msg->msgLength());
      if (channel_->version() > OFP_VERSION_1) {
        // Still need to request port information.
        sendPortDescRequest();
      } else {
        // V1 FeaturesReply includes port information.
        installNewChannelListener();
      }
    }

  } else {
    log_debug("DefaultHandshake::onFeaturesReply: postDatapath failed");
    channel_->shutdown();
  }
}

void DefaultHandshake::onPortDescReply(Message *message) {
  if (state_ != kSentPortRequest) {
    log_warning("DefaultHandshake: Odd PortDesc message");
    return;
  }

  const MultipartReply *msg = MultipartReply::cast(message);
  if (!msg) {
    log_warning("DefaultHandshake: Invalid FeaturesReply message");
    return;
  }

  PortRange ports = ByteRange{msg->replyBody(), msg->replyBodySize()};
  appendPortsToFeaturesReply(ports);
  installNewChannelListener();
}

void DefaultHandshake::onError(const Message *message) {
  log_warning("DefaultHandshake: Received error message");
}

void DefaultHandshake::installNewChannelListener() {
  assert(channel_->channelListener() == this);

  if (listenerFactory_) {
    ChannelListener *newListener = listenerFactory_();

    // Invoke onChannelUp on new listener before replacing channel
    // listener. New listener can access DefaultHandshake data using
    // channelListener() accessor.
    newListener->onChannelUp(channel_);
    channel_->setChannelListener(newListener);
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

void DefaultHandshake::sendHello() {
  HelloBuilder msg{versions_};
  msg.send(channel_);
  state_ = kSentHello;
  timeStarted_ = TimeClock::now();
}

void DefaultHandshake::sendFeaturesRequest() {
  FeaturesRequestBuilder reply{};
  reply.send(channel_);
  state_ = kSentFeaturesRequest;
  timeStarted_ = TimeClock::now();
}

void DefaultHandshake::sendPortDescRequest() {
  MultipartRequestBuilder msg;
  msg.setRequestType(OFPMP_PORT_DESC);
  msg.send(channel_);
  state_ = kSentPortRequest;
  timeStarted_ = TimeClock::now();
}

void DefaultHandshake::appendPortsToFeaturesReply(PortRange ports) {
  assert(featuresReply_.size() > 0);
  
  if (ports.empty())
    return;

  Header *header = reinterpret_cast<Header *>(featuresReply_.mutableData());
  assert((header->length() % 8) == 0);

  size_t newLength = header->length() + ports.size();
  header->setLength(newLength);

  featuresReply_.add(ports.data(), ports.size());
}
