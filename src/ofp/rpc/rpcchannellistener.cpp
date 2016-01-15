// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
