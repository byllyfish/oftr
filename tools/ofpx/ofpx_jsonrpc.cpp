// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofpx_jsonrpc.h"
#include "ofp/api/apiserver.h"

using namespace ofpx;
using ExitStatus = JsonRpc::ExitStatus;

const int STDIN = 0;
const int STDOUT = 1;

//-------//
// r u n //
//-------//

int JsonRpc::run(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  // ofp::log::setOutputLevelFilter(ofp::log::Level::Trace);

  ofp::Driver driver;
  ofp::api::ApiServer server{&driver, STDIN, STDOUT};
  driver.run();

  return 0;
}
