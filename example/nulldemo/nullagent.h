// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef EXAMPLE_NULLDEMO_NULLAGENT_H_
#define EXAMPLE_NULLDEMO_NULLAGENT_H_

#include "ofp/ofp.h"
#include "ofp/yaml/yflowmod.h"

using ofp::UInt16;
using ofp::UInt32;
using ofp::Message;
using ofp::Channel;
using ofp::Milliseconds;

class NullAgent : public ofp::ChannelListener {
 public:
  static NullAgent *Factory() { return new NullAgent; }

  void onChannelUp(Channel *channel) override {
    ofp::log::debug("NullAgent channel up.");

    // FIXME support binding of related connections.
    // channel->openAuxChannel(1, Channel::Transport::TCP);
  }

  void onChannelDown(Channel *channel) override {
    ofp::log::debug("NullAgent channel down.");
  }

  void onMessage(const Message *message) override;

 private:
  void onSetConfig(const Message *message);
  void onFlowMod(const Message *message);
  void onGetAsyncRequest(const Message *message);
  void onGetConfigRequest(const Message *message);
  void onBarrierRequest(const Message *message);

  void sendError(ofp::OFPErrorCode err, const Message *message);
};

void NullAgent::onMessage(const Message *message) {
  switch (message->type()) {
    case ofp::SetConfig::type():
      onSetConfig(message);
      break;

    case ofp::FlowMod::type():
      onFlowMod(message);
      break;

    case ofp::GetAsyncRequest::type():
      onGetAsyncRequest(message);
      break;

    case ofp::GetConfigRequest::type():
      onGetConfigRequest(message);
      break;

    case ofp::BarrierRequest::type():
      onBarrierRequest(message);
      break;

    default:
      ofp::log::debug("Unknown message type",
                      static_cast<int>(message->type()));
      sendError(ofp::OFPBRC_BAD_TYPE, message);
      break;
  }
}

void NullAgent::onSetConfig(const Message *message) {
  auto setConfig = ofp::SetConfig::cast(message);
  if (!setConfig) {
    sendError(ofp::OFPBRC_BAD_TYPE, message);
    return;
  }

  // ofp::log::debug(ofp::yaml::write(setConfig));
}

void NullAgent::onFlowMod(const Message *message) {
  auto flowMod = ofp::FlowMod::cast(message);
  if (!flowMod) {
    sendError(ofp::OFPBRC_BAD_TYPE, message);
    return;
  }

  ofp::log::debug("FlowMod: ",
                  ofp::RawDataToHex(message->data(), message->size()));
}

void NullAgent::onGetAsyncRequest(const Message *message) {
  auto getAsyncReq = ofp::GetAsyncRequest::cast(message);
  if (!getAsyncReq) {
    sendError(ofp::OFPBRC_BAD_TYPE, message);
    return;
  }

  ofp::log::debug("GetAsyncRequest");
}

void NullAgent::onGetConfigRequest(const Message *message) {
  auto getConfigReq = ofp::GetConfigRequest::cast(message);
  if (!getConfigReq) {
    sendError(ofp::OFPBRC_BAD_TYPE, message);
    return;
  }

  ofp::log::debug("GetConfigRequest");
}

void NullAgent::onBarrierRequest(const Message *message) {
  ofp::log::debug("BarrierRequest");

  auto barrierReq = ofp::BarrierRequest::cast(message);
  if (!barrierReq) {
    sendError(ofp::OFPBRC_BAD_TYPE, message);
    return;
  }

  ofp::BarrierReplyBuilder reply{message};
  reply.send(message->source());
}

void NullAgent::sendError(ofp::OFPErrorCode err, const Message *message) {
  ofp::ErrorBuilder msg{message->xid()};
  msg.setErrorCode(err);
  msg.setErrorData(message);
  msg.send(message->source());
}

#endif  // EXAMPLE_NULLDEMO_NULLAGENT_H_
