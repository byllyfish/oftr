// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./simplechannellistener.h"

using namespace controller;

void SimpleChannelListener::onChannelUp(Channel *channel) {
  UInt32 xid;

  log_debug("SimpleChannelListener UP");

  SetConfigBuilder config;
  config.setMissSendLen(14);  // ethernet header only
  config.send(channel);
  // No Reply expected.

  FlowModBuilder flowMod;
  if (channel->version() > OFP_VERSION_1) {
    // Version 1 doesn't support non-zero tableId field.
    flowMod.setTableId(OFPTT_ALL);
  }
  flowMod.setCommand(OFPFC_DELETE);
  flowMod.setPriority(0x8000);
  flowMod.setBufferId(OFP_NO_BUFFER);
  flowMod.setOutPort(OFPP_ANY);
  flowMod.setOutGroup(OFPG_ANY);
  (void)flowMod.send(channel);
  // No reply expected.

  BarrierRequestBuilder barrier;
  xid = barrier.send(channel);
  trackReply(xid, &SimpleChannelListener::onBarrierReply);
}

void SimpleChannelListener::onMessage(Message *message) {
  switch (message->type()) {
    case PacketIn::type():
      if (auto msg = PacketIn::cast(message))
        controller_.onPacketIn(message->source(), msg);
      break;

    case PortStatus::type():
      if (auto msg = PortStatus::cast(message))
        controller_.onPortStatus(message->source(), msg);
      break;

    case Error::type():
      if (auto msg = Error::cast(message))
        controller_.onError(message->source(), msg);
      break;

    default:
      onReply(message);
      break;
  }
}

void SimpleChannelListener::onGetAsyncReply(const GetAsyncReply *msg) {
  log_debug("GetAsyncReply");
}

void SimpleChannelListener::onGetConfigReply(const GetConfigReply *msg) {
  log_debug("GetConfigReply");
}

void SimpleChannelListener::onBarrierReply(const BarrierReply *msg) {
  log_debug("BarrierReply");
}

void SimpleChannelListener::onReply(const Message *message) {
  auto xid = message->xid();
  auto iter = tracker_.find(xid);
  if (iter != tracker_.end()) {
    auto &callback = iter->second;
    callback(message);
    tracker_.erase(iter);
  } else {
    log_debug("Untracked XID:", xid);
  }
}
