// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/channellistener.h"
#include "ofp/log.h"

using namespace ofp;

void ChannelListener::onChannelUp(Channel *channel) {
  log::debug("ChannelListener - onChannelUp ignored:", channel);
}

void ChannelListener::onChannelDown(Channel *channel) {
  log::debug("ChannelListener - onChannelDown ignored:", channel);
}

void ChannelListener::dispose(ChannelListener *listener) {
  delete listener;
}
