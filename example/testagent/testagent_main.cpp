#include "ofp/ofp.h"
#include "testagent.h"
#include <iostream>

using namespace testagent;

int main(int argc, char **argv) {
  std::vector<std::string> args{argv + 1, argv + argc};

  IPv6Endpoint remoteEndpoint;
  if (!args.empty()) {
    (void) remoteEndpoint.parse(args[0]);
    if (remoteEndpoint.port() == 0) {
      remoteEndpoint.setPort(OFP_DEFAULT_PORT);
    }
  }

  Driver driver;
  std::error_code error;

  if (remoteEndpoint.valid()) {
    auto result = driver.connect(ChannelMode::Agent, remoteEndpoint, {OFP_VERSION_1},
                                 TestAgent::Factory);
    result.done([&error](const std::error_code &err) {
      // This will not be called, unless `addr` is invalid; agent will keep
      // retrying the connection.
      std::cerr << "Error connecting: " << err << '\n';
      error = err;
    });

  } else {
    error = driver.listen(ChannelMode::Agent, IPv6Endpoint{OFP_DEFAULT_PORT},
                          {OFP_VERSION_1}, TestAgent::Factory);
  }

  driver.run();

  if (error) {
    std::cerr << "Error starting agent: " << error << '\n';
    return 1;
  }

  return 0;
}
