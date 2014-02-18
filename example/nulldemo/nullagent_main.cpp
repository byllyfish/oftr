#include "ofp/ofp.h"
#include "nullagent.h"
#include <iostream>
#include <vector>

using namespace ofp;

int main(int argc, const char **argv) {
  std::vector<std::string> args{argv + 1, argv + argc};

  IPv6Address addr{};
  ProtocolVersions version{};
  if (!args.empty()) {
    addr = IPv6Address{args[0]};
    if (args.size() == 2) {
      int num = std::stoi(args[1]);
      version = ProtocolVersions{UInt8_narrow_cast(num)};
    }
  }

  ofp::log::setOutputStream(&std::cerr);

  Driver driver;

  if (addr.valid()) {
    auto result = driver.connect(Driver::Agent,
                                 IPv6Endpoint{addr, OFP_DEFAULT_PORT}, version,
                                 NullAgent::Factory);
    result.done([](const std::error_code &err) {
      // This will not be called, unless `addr` is invalid; agent will keep
      // retrying the connection.
      std::cout << "Result: " << err << '\n';
    });

  } else {
    auto err =
        driver.listen(Driver::Agent, IPv6Endpoint{OFP_DEFAULT_PORT},
                      version, NullAgent::Factory);

    std::cout << "Result: " << err << '\n';
  }

  driver.run();
}
