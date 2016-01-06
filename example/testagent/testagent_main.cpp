// Copyright 2014-present Bill Fisher. All rights reserved.

#include <iostream>
#include "./testagent.h"

using namespace testagent;

int main(int argc, char **argv) {
  std::vector<std::string> args{argv + 1, argv + argc};

  IPv6Endpoint remoteEndpoint;
  if (!args.empty()) {
    if (!remoteEndpoint.parse(args[0])) {
      std::cerr << "testagent: Argument 1 is not an endpoint: `" << args[0]
                << "`\n";
      return 1;
    }
  }

  std::error_code error;
  Driver driver;
  driver.installSignalHandlers();

  if (remoteEndpoint.valid()) {
    (void)driver.connect(
        ChannelMode::Raw, 0, remoteEndpoint, {OFP_VERSION_1},
        TestAgent::Factory,
        [&error, &remoteEndpoint](Channel *channel, std::error_code err) {
          if (err) {
            std::cerr << "testagent: Error connecting to `" << remoteEndpoint
                      << "`: connId=" << channel->connectionId()
                      << " err=" << err.message() << '\n';
          }
          error = err;
        });

  } else {
    (void)driver.listen(ChannelMode::Raw, 0, IPv6Endpoint{OFPGetDefaultPort()},
                        {OFP_VERSION_1}, TestAgent::Factory, error);
  }

  driver.run();

  if (error) {
    std::cerr << "testagent: Error starting agent: " << error.message() << '\n';
    return 1;
  }

  return 0;
}
