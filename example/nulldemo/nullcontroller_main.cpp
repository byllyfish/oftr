// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/ofp.h"
#include <iostream>

using namespace ofp;

class NullController : public ChannelListener {
 public:
  void onChannelUp(Channel *channel) override {
    log::debug(__PRETTY_FUNCTION__);
  }

  void onMessage(const Message *message) override {}

  static ChannelListener *Factory() { return new NullController; }
};

int main(int argc, char **argv) {
  std::vector<std::string> args{argv + 1, argv + argc};

  IPv6Address addr{};
  if (!args.empty()) {
    addr = IPv6Address{args[0]};
  }

  log::setOutputStream(&std::clog);

  Driver driver;

  if (addr.valid()) {
    (void)driver.connect(ChannelMode::Controller, 0,
                         IPv6Endpoint{addr, OFP_DEFAULT_PORT},
                         ProtocolVersions::All, NullController::Factory,
                         [](Channel *channel, std::error_code err) {
      std::cout << "Result: connId=" << channel->connectionId()
                << " err=" << err << '\n';
    });

  } else {
    std::error_code err;
    (void)driver.listen(ChannelMode::Controller, 0,
                        IPv6Endpoint{OFP_DEFAULT_PORT}, ProtocolVersions::All,
                        NullController::Factory, err);
  }

  driver.run();
}
