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

  log::setOutputStream(&std::cerr);

  Driver driver;

  if (addr.valid()) {
    auto result =
        driver.connect(Driver::Controller, IPv6Endpoint{addr, OFP_DEFAULT_PORT},
                       ProtocolVersions{}, NullController::Factory);

    result.done([](const std::error_code &err) {
      std::cout << "Result " << err << '\n';
    });

  } else {
    driver.listen(Driver::Controller, IPv6Endpoint{OFP_DEFAULT_PORT},
                  ProtocolVersions{}, NullController::Factory);
  }

  driver.run();
}
