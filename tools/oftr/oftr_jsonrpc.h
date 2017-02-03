// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef TOOLS_LIBOFP_OFPX_JSONRPC_H_
#define TOOLS_LIBOFP_OFPX_JSONRPC_H_

#include "./oftr.h"

namespace ofpx {

// oftr jsonrpc [options]
//
// Run a JSON-RPC server. By default, the control connection comes from stdio.
//
// Usage:
//
// To run JSON-RPC server over stdin and stdout:
//
//   oftr jsonrpc
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
