// Copyright 2014-present Bill Fisher. All rights reserved.

#include <iostream>
#include "ofp/ofp.h"
#include "./simplechannellistener.h"

using namespace ofp;

int main(int argc, char **argv) {
  log::setOutputStream(&std::clog);

  Driver driver;
  driver.installSignalHandlers();

  std::error_code err;
  (void)driver.listen(ChannelMode::Controller, 0,
                      IPv6Endpoint{OFPGetDefaultPort()}, ProtocolVersions::All,
                      []() { return new controller::SimpleChannelListener; },
                      err);

  driver.run();

  if (err) {
    log::error("Error running controller:", err);
    return 1;
  }

  return 0;
}
