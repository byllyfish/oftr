// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./oftr_jsonrpc.h"

#include <sys/resource.h>  // for getrlimit, setrlimit
#include <unistd.h>        // for STDIN_FILENO, STDOUT_FILENO

#include "ofp/rpc/rpcserver.h"

using namespace ofpx;
using namespace ofp;

using ExitStatus = JsonRpc::ExitStatus;

int JsonRpc::run(int argc, const char *const *argv) {
  parseCommandLineOptions(argc, argv, "Run a JSON-RPC server\n");
  setMaxOpenFiles();

  if (rpcSocket_.empty()) {
    // Communicate over STDIN/STDOUT.
    return runStdio();

  } else {
    int existingFD = getSocketFD();
    if (existingFD >= 0) {
      // Communicate over specified file descriptor.
      return runSocket(existingFD);
    }

    // Communicate over unix domain socket.
    return runSocketServer(rpcSocket_);
  }
}

void JsonRpc::setMaxOpenFiles() {
  struct rlimit rlp;

  if (::getrlimit(RLIMIT_NOFILE, &rlp) < 0) {
    std::error_code err{errno, std::generic_category()};
    log_error("getrlimit failed for RLIMIT_NOFILE:", err);
    return;
  }

  log_debug("Open file limit: rlim_cur", rlp.rlim_cur, "rlim_max",
            rlp.rlim_max);

#ifdef __APPLE__
  // Set soft limit to lower of OPEN_MAX and rlim_max.
  rlp.rlim_cur = std::min<UInt64>(OPEN_MAX, rlp.rlim_max);
#else
  rlp.rlim_cur = rlp.rlim_max;
#endif

  if (::setrlimit(RLIMIT_NOFILE, &rlp) < 0) {
    std::error_code err{errno, std::generic_category()};
    log_error("setrlimit failed for RLIMIT_NOFILE:", rlp.rlim_cur, err);
    return;
  }

  log_info("Open file limit: rlim_cur", rlp.rlim_cur, "rlim_max", rlp.rlim_max);
}

int JsonRpc::runStdio() {
  const Milliseconds metricInterval{metricInterval_};

  rpc::RpcServer server{binaryProtocol_, metricInterval};
  server.bind(::dup(STDIN_FILENO), ::dup(STDOUT_FILENO));
  server.run();

  return 0;
}

int JsonRpc::runSocket(int socketFD) {
  const Milliseconds metricInterval{metricInterval_};

  rpc::RpcServer server{binaryProtocol_, metricInterval};
  auto err = server.bind(socketFD);
  if (err) {
    log_error("Unix domain socket error:", err);
    return static_cast<int>(ExitStatus::ListenFailed);
  }

  server.run();

  return 0;
}

int JsonRpc::runSocketServer(const std::string &path) {
  const Milliseconds metricInterval{metricInterval_};

  rpc::RpcServer server{binaryProtocol_, metricInterval};
  auto err = server.bind(path);
  if (err) {
    log_error("Unix domain socket error:", path, err);
    return static_cast<int>(ExitStatus::ListenFailed);
  }

  server.run();

  return 0;
}

/// Return integer value of RPC socket path.
/// Return -1 if value is not a positive integer.
int JsonRpc::getSocketFD() const {
  llvm::StringRef path{rpcSocket_};
  int result = 0;

  if (path.getAsInteger(10, result) || (result < 0)) {
    return -1;
  }

  return result;
}
