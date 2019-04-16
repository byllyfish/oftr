// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/driver.h"
#include "ofp/sys/engine.h"

using namespace ofp;

Driver::Driver() : engine_{new sys::Engine{this}} {}

Driver::~Driver() {
  delete engine_;
}

UInt64 Driver::listen(ChannelOptions options, UInt64 securityId,
                      const IPv6Endpoint &localEndpoint,
                      ProtocolVersions versions,
                      ChannelListener::Factory listenerFactory,
                      std::error_code &error) {
  return engine_->listen(options, securityId, localEndpoint, versions,
                         listenerFactory, error);
}

UInt64 Driver::connect(
    ChannelOptions options, UInt64 securityId,
    const IPv6Endpoint &remoteEndpoint, ProtocolVersions versions,
    ChannelListener::Factory listenerFactory,
    std::function<void(Channel *, std::error_code)> resultHandler) {
  return engine_->connect(options, securityId, remoteEndpoint, versions,
                          listenerFactory, resultHandler);
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

#if LIBOFP_ENABLE_OPENSSL

UInt64 Driver::addIdentity(const std::string &certData,
                           const std::string &privKey,
                           const std::string &verifier,
                           const std::string &version,
                           const std::string &ciphers,
                           const std::string &keyLogFile,
                           std::error_code &error) {
  return engine_->addIdentity(certData, privKey, verifier, version, ciphers,
                              keyLogFile, error);
}

#endif  // LIBOFP_ENABLE_OPENSSL
