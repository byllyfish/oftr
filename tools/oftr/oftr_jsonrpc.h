// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef TOOLS_OFTR_OFTR_JSONRPC_H_
#define TOOLS_OFTR_OFTR_JSONRPC_H_

#include "./oftr.h"

namespace ofpx {

// oftr jsonrpc [options]
//
// Run a JSON-RPC server. By default, the control connection comes from stdio.
//
//   --binary-protocol       Use the binary framed protocol
//   --rpc-socket=<path>     Control connection runs over a unix domain socket.
//                           If <path> is an integer, use inherited descriptor number.
//                           Otherwise, listen on <path> for first connection.
//   --metric-interval=0     Log RPC metrics at specified interval (msec)
//
// Usage:
//
// To run JSON-RPC server over stdin and stdout:
//
//   oftr jsonrpc
//

class JsonRpc : public Subprogram {
 public:
  enum class ExitStatus { Success = 0, ListenFailed = MinExitStatus };

  int run(int argc, const char *const *argv) override;

 private:
  // --- Command-line Arguments ---
  cl::opt<bool> binaryProtocol_{"binary-protocol",
                                cl::desc("Use binary frame protocol")};
  cl::opt<std::string> rpcSocket_{"rpc-socket",
                                  cl::desc("Listen on unix domain socket")};
  cl::opt<unsigned> metricInterval_{
      "metric-interval",
      cl::desc("Log RPC metrics at specified interval (msec)"),
      cl::ValueRequired};

  void setMaxOpenFiles();

  int runStdio();
  int runSocket(int socketFD);
  int runSocketServer(const std::string &path);

  int getSocketFD() const;
};

}  // namespace ofpx

#endif  // TOOLS_OFTR_OFTR_JSONRPC_H_
