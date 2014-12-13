// Copyright 2014-present Bill Fisher. All rights reserved.

#include <sys/resource.h>  // for getrlimit, setrlimit
#include "ofpx_jsonrpc.h"
#include "ofpx_xpc.h"
#include "ofp/api/apiserver.h"

using namespace ofpx;
using ExitStatus = JsonRpc::ExitStatus;

const int STDIN = 0;
const int STDOUT = 1;

//-------//
// r u n //
//-------//

int JsonRpc::run(int argc, const char *const *argv) {
  cl::ParseCommandLineOptions(argc, argv);
  setMaxOpenFiles();

  ofp::log::setOutputLevelFilter(ofp::log::Level::Debug);

  if (xpc_) {
    runXpc();
  } else {
    runStdio();
  }

  return 0;
}

//-------------------------------//
// s e t M a x O p e n F i l e s //
//-------------------------------//

void JsonRpc::setMaxOpenFiles() {
  struct rlimit rlp;

  if (::getrlimit(RLIMIT_NOFILE, &rlp) < 0) {
    std::error_code err{errno, std::generic_category()};
    ofp::log::error("getrlimit failed for RLIMIT_NOFILE:", err);
    return;
  }

  rlp.rlim_cur = 1000;

  if (::setrlimit(RLIMIT_NOFILE, &rlp) < 0) {
    std::error_code err{errno, std::generic_category()};
    ofp::log::error("setrlimit failed for RLIMIT_NOFILE:", rlp.rlim_cur, err);
    return;
  }

  ofp::log::info("Changed open file limit to", rlp.rlim_cur);
}

//-----------------//
// r u n S t d i o //
//-----------------//

void JsonRpc::runStdio() {
  ofp::Driver driver;
  ofp::api::ApiServer server{&driver, STDIN, STDOUT};
  driver.run();
}

//-------------//
// r u n X p c //
//-------------//

void JsonRpc::runXpc() {
#if LIBOFP_TARGET_DARWIN
  // Use ASL on Darwin.
  ofp::log::setOutputStream(static_cast<aslclient>(nullptr));
  ofp::log::setOutputLevelFilter(ofp::log::Level::Info);
  run_xpc_main();
#else
  log::fatal("XPC service is only available on Darwin/MacOS/IOS.");
#endif
}
