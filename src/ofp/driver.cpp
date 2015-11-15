// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/driver.h"
#include "ofp/sys/engine.h"

using namespace ofp;

Driver::Driver() : engine_{new sys::Engine{this}} {
}

Driver::~Driver() {
  delete engine_;
}

UInt64 Driver::listen(ChannelMode mode, UInt64 securityId,
                      const IPv6Endpoint &localEndpoint,
                      ProtocolVersions versions,
                      ChannelListener::Factory listenerFactory,
                      std::error_code &error) {
  return engine_->listen(mode, securityId, localEndpoint, versions,
                         listenerFactory, error);
}

UInt64 Driver::connect(
    ChannelMode mode, UInt64 securityId, const IPv6Endpoint &remoteEndpoint,
    ProtocolVersions versions, ChannelListener::Factory listenerFactory,
    std::function<void(Channel *, std::error_code)> resultHandler) {
  return engine_->connect(mode, securityId, remoteEndpoint, versions,
                          listenerFactory, resultHandler);
}

UInt64 Driver::connectUDP(ChannelMode mode, UInt64 securityId,
                          const IPv6Endpoint &remoteEndpoint,
                          ProtocolVersions versions,
                          ChannelListener::Factory listenerFactory,
                          std::error_code &error) {
  return engine_->connectUDP(mode, securityId, remoteEndpoint, versions,
                             listenerFactory, error);
}

void Driver::run() {
  engine_->run();
}

void Driver::stop(Milliseconds timeout) {
  engine_->stop(timeout);
}

void Driver::installSignalHandlers(std::function<void()> callback) {
  engine_->installSignalHandlers(callback);
}
