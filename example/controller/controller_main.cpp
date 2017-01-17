// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./simplechannellistener.h"
#include "ofp/ofp.h"

using namespace ofp;

int main(int argc, char **argv) {
  log::configure(log::Level::Debug);

  Driver driver;
  driver.installSignalHandlers();

  std::error_code err;
  (void)driver.listen(ChannelOptions::FEATURES_REQ, 0,
                      IPv6Endpoint{OFPGetDefaultPort()}, ProtocolVersions::All,
                      []() { return new controller::SimpleChannelListener; },
                      err);

  driver.run();

  if (err) {
    log_error("Error running controller:", err);
    return 1;
  }

  return 0;
}
