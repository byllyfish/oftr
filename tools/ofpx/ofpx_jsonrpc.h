// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFPX_JSONRPC_H_
#define OFPX_JSONRPC_H_

#include "ofpx.h"

namespace ofpx {

// ofpx jsonrpc [options]
//
// Run a JSON-RPC server. By default, the control connection comes from stdio.
//
//   --xpc              Run as a XPC service (Mac OS X only)
//
// Usage:
//
// To run JSON-RPC server over stdin and stdout:
//
//   ofpx jsonrpc
//

class JsonRpc : public Subprogram {
 public:
  enum class ExitStatus { Success = 0 };

  int run(int argc, const char *const *argv) override;

 private:
  // --- Command-line Arguments ---
  cl::opt<bool> xpc_{"xpc", cl::desc("Run as a XPC service (Mac OS X only)")};

  void setMaxOpenFiles();
  void runStdio();

  [[noreturn]] void runXpc();
};

}  // namespace ofpx

#endif  // OFPX_JSONRPC_H_
