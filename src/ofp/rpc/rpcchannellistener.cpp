// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcchannellistener.h"
#include "ofp/rpc/rpcserver.h"

using namespace ofp;
using namespace ofp::rpc;

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
