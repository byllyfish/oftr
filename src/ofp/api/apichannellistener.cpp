// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/api/apichannellistener.h"
#include "ofp/api/apiserver.h"

using ofp::api::ApiChannelListener;

void ApiChannelListener::onChannelUp(Channel *channel) {
  assert(channel_ == nullptr);

  channel_ = channel;
  server_->onChannelUp(channel_);
}

void ApiChannelListener::onChannelDown(Channel *channel) {
  assert(channel == channel_);
  server_->onChannelDown(channel_);
}

void ApiChannelListener::onMessage(const Message *message) {
  server_->onMessage(channel_, message);
}
