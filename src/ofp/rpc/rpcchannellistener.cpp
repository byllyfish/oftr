// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/rpc/rpcchannellistener.h"
#include "ofp/rpc/rpcserver.h"

using ofp::rpc::RpcChannelListener;

void RpcChannelListener::onChannelUp(Channel *channel) {
  assert(channel_ == nullptr);

  channel_ = channel;
  server_->onChannelUp(channel_);
}

void RpcChannelListener::onChannelDown(Channel *channel) {
  assert(channel == channel_);
  server_->onChannelDown(channel_);
}

void RpcChannelListener::onMessage(const Message *message) {
  server_->onMessage(channel_, message);
}
