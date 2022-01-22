// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/channellistener.h"

#include "ofp/log.h"

using namespace ofp;

void ChannelListener::onChannelUp(Channel *channel) {
  log_debug("ChannelListener - onChannelUp ignored:", channel);
}

void ChannelListener::onChannelDown(Channel *channel) {
  log_debug("ChannelListener - onChannelDown ignored:", channel);
}

void ChannelListener::dispose(ChannelListener *listener) {
  delete listener;
}
