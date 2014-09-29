#include "ofp/ofp.h"
#include "nullagent.h"
#include <iostream>
#include <vector>

using namespace ofp;

int main(int argc, const char **argv) {
  std::vector<std::string> args{argv + 1, argv + argc};

  IPv6Address addr{};
  ProtocolVersions version{ProtocolVersions::All};
  if (!args.empty()) {
    addr = IPv6Address{args[0]};
    if (args.size() == 2) {
      int num = std::stoi(args[1]);
      version = ProtocolVersions{UInt8_narrow_cast(num)};
    }
  }

  ofp::log::setOutputStream(&std::cerr);

  Driver driver;
  driver.installSignalHandlers();
  
  if (addr.valid()) {
    (void)driver.connect(ChannelMode::Raw, ChannelTransport::TCP_Plaintext,
                                 IPv6Endpoint{addr, OFP_DEFAULT_PORT}, version,
                                 NullAgent::Factory, [](Channel *channel, std::error_code err) {
                                  std::cout << "Result: connId=" << channel->connectionId() << ", " << err << '\n';
                                 });

  } else {
    std::error_code err;
    (void)driver.listen(ChannelMode::Raw, IPv6Endpoint{OFP_DEFAULT_PORT},
                      version, NullAgent::Factory, err);

    std::cout << "Result: " << err << '\n';
  }

  driver.run();

  return 0;
}
