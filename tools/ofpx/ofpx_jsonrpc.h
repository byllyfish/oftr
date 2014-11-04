// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFPX_JSONRPC_H_
#define OFPX_JSONRPC_H_

#include "ofpx.h"

namespace ofpx {

// ofpx jsonrpc [options]
//
// Run a JSON-RPC server. You must specify where the control connection
// comes from.
//
//   --stdin            Take commands from standard input
//
// Usage:
//
// To run JSON-RPC server over stdin and stdout:
//
//   ofpx jsonrpc --stdin
//

class JsonRpc : public Subprogram {
 public:
  enum class ExitStatus { Success = 0 };

  int run(int argc, char **argv) override;

 private:
  // --- Command-line Arguments ---
  cl::opt<bool> stdin_{"stdin", cl::desc("Take commands from standard input")};
};

}  // namespace ofpx

#endif  // OFPX_JSONRPC_H_
