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
  setMaxOpenFiles();

  ofp::Driver driver;
  ofp::api::ApiServer server{&driver, STDIN, STDOUT};
  driver.run();

  return 0;
}

//-------------------------------//
// s e t M a x O p e n F i l e s //
//-------------------------------//

void JsonRpc::setMaxOpenFiles() {
  struct rlimit rlp{};

  if (::getrlimit(RLIMIT_NOFILE, &rlp) < 0) {
    ofp::log::error("getrlimit failed:", errno);
    return;
  }

  rlp.rlim_cur = 10000;

  if (::setrlimit(RLIMIT_NOFILE, &rlp) < 0) {
    ofp::log::error("setrlimit failed:", rlp.rlim_cur, errno);
    return;
  }

  ofp::log::info("Changed open file limit to", rlp.rlim_cur);
}
