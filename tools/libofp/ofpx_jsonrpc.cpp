// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./ofpx_jsonrpc.h"
#include <sys/resource.h>  // for getrlimit, setrlimit
#include "ofp/rpc/rpcserver.h"

using namespace ofpx;
using namespace ofp;

using ExitStatus = JsonRpc::ExitStatus;

const int STDIN = 0;
const int STDOUT = 1;

int JsonRpc::run(int argc, const char *const *argv) {
  parseCommandLineOptions(argc, argv, "Run a JSON-RPC server\n");
  setMaxOpenFiles();
  runStdio();

  return 0;
}

void JsonRpc::setMaxOpenFiles() {
  struct rlimit rlp;

  if (::getrlimit(RLIMIT_NOFILE, &rlp) < 0) {
    std::error_code err{errno, std::generic_category()};
    log::error("getrlimit failed for RLIMIT_NOFILE:", err);
    return;
  }

  log::debug("Open file limit: rlim_cur", rlp.rlim_cur, "rlim_max", rlp.rlim_max);

#ifdef __APPLE__
  // Set soft limit to lower of OPEN_MAX and rlim_max.
  rlp.rlim_cur = std::min<UInt64>(OPEN_MAX, rlp.rlim_max);
#else
  rlp.rlim_cur = rlp.rlim_max;
#endif

  if (::setrlimit(RLIMIT_NOFILE, &rlp) < 0) {
    std::error_code err{errno, std::generic_category()};
    log::error("setrlimit failed for RLIMIT_NOFILE:", rlp.rlim_cur, err);
    return;
  }

  log::info("Open file limit: rlim_cur", rlp.rlim_cur, "rlim_max", rlp.rlim_max);
}

void JsonRpc::runStdio() {
  Driver driver;
  rpc::RpcServer server{&driver, STDIN, STDOUT};
  driver.installSignalHandlers([&server]() { server.close(); });
  driver.run();
}
