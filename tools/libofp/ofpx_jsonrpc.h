// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef TOOLS_LIBOFP_OFPX_JSONRPC_H_
#define TOOLS_LIBOFP_OFPX_JSONRPC_H_

#include "./ofpx.h"

namespace ofpx {

// libofp jsonrpc [options]
//
// Run a JSON-RPC server. By default, the control connection comes from stdio.
//
// Usage:
//
// To run JSON-RPC server over stdin and stdout:
//
//   libofp jsonrpc
//

class JsonRpc : public Subprogram {
 public:
  enum class ExitStatus { Success = 0 };

  int run(int argc, const char *const *argv) override;

 private:
  // --- Command-line Arguments ---
  // None

  void setMaxOpenFiles();
  void runStdio();
};

}  // namespace ofpx

#endif  // TOOLS_LIBOFP_OFPX_JSONRPC_H_
