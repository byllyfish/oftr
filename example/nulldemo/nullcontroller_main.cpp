// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include <iostream>

#include "ofp/ofp.h"

using namespace ofp;

class NullController : public ChannelListener {
 public:
  void onChannelUp(Channel *channel) override {
    log_debug(__PRETTY_FUNCTION__);
  }

  void onMessage(Message *message) override {}

  static ChannelListener *Factory() { return new NullController; }
};

int main(int argc, char **argv) {
  log::configure(log::Level::Debug);

  std::vector<std::string> args{argv + 1, argv + argc};

  IPv6Address addr;
  if (!args.empty()) {
    addr = IPv6Address{args[0]};
  }

  Driver driver;
  driver.installSignalHandlers();

  if (addr.valid()) {
    (void)driver.connect(
        ChannelOptions::FEATURES_REQ, 0,
        IPv6Endpoint{addr, OFPGetDefaultPort()}, ProtocolVersions::All,
        NullController::Factory, [](Channel *channel, std::error_code err) {
          std::cout << "Result: connId=" << channel->connectionId()
                    << " err=" << err << '\n';
        });

  } else {
    std::error_code err;
    (void)driver.listen(ChannelOptions::FEATURES_REQ, 0,
                        IPv6Endpoint{OFPGetDefaultPort()},
                        ProtocolVersions::All, NullController::Factory, err);
  }

  driver.run();
}
