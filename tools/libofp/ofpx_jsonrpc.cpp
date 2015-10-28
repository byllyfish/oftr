// Copyright 2014-present Bill Fisher. All rights reserved.

#include "./ofpx_jsonrpc.h"
#include <sys/resource.h>  // for getrlimit, setrlimit
#include "./ofpx_xpc.h"
#include "ofp/rpc/rpcserver.h"

using namespace ofpx;
using ExitStatus = JsonRpc::ExitStatus;

const int STDIN = 0;
const int STDOUT = 1;

int JsonRpc::run(int argc, const char *const *argv) {
  parseCommandLineOptions(argc, argv, "Run a JSON-RPC server\n");
  setMaxOpenFiles();

  if (xpc_) {
    runXpc();
  } else {
    runStdio();
  }

  return 0;
}

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

  ofp::log::debug("Changed open file limit to", rlp.rlim_cur);
}

void JsonRpc::runStdio() {
  ofp::Driver driver;
  ofp::rpc::RpcServer server{&driver, STDIN, STDOUT};
  driver.installSignalHandlers([&server](){
    server.close();
  });
  driver.run();
}

void JsonRpc::runXpc() {
#if LIBOFP_TARGET_DARWIN
  // Use ASL on Darwin.
  ofp::log::setOutputStream(static_cast<aslclient>(nullptr));
  ofp::log::setOutputLevelFilter(ofp::log::Level::Info);
  run_xpc_main();
#else
  ofp::log::fatal("XPC service is only available on Darwin/MacOS/IOS.");
#endif
}
