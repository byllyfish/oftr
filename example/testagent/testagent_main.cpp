// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/ofp.h"
#include "testagent.h"
#include <iostream>

using namespace testagent;

int main(int argc, char **argv) {
  std::vector<std::string> args{argv + 1, argv + argc};

  IPv6Endpoint remoteEndpoint;
  if (!args.empty()) {
    (void)remoteEndpoint.parse(args[0]);
    if (remoteEndpoint.port() == 0) {
      remoteEndpoint.setPort(OFP_DEFAULT_PORT);
    }
  }

  std::error_code error;
  Driver driver;
  driver.installSignalHandlers();

  if (remoteEndpoint.valid()) {
    (void)driver.connect(ChannelMode::Raw, 0, remoteEndpoint, {OFP_VERSION_1},
                         TestAgent::Factory,
                         [&error](Channel *channel, std::error_code err) {
      std::cerr << "Error connecting: connId=" << channel->connectionId()
                << " err=" << err.message() << '\n';
      error = err;
    });

  } else {
    (void)driver.listen(ChannelMode::Raw, 0, IPv6Endpoint{OFP_DEFAULT_PORT},
                        {OFP_VERSION_1}, TestAgent::Factory, error);
  }

  driver.run();

  if (error) {
    std::cerr << "Error starting agent: " << error.message() << '\n';
    return 1;
  }

  return 0;
}
