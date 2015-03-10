// Copyright 2014-present Bill Fisher. All rights reserved.

#include <iostream>
#include "ofp/ofp.h"
#include "./ofpx_decode.h"
#include "./ofpx_encode.h"
#include "./ofpx_ping.h"
#include "./ofpx_jsonrpc.h"
#include "./ofpx_help.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

struct SubprogramEntry {
  const char *name;
  ofpx::RunSubprogram run;
};

static SubprogramEntry programs[] = {{"ping", ofpx::Run<ofpx::Ping>},
                                     {"decode", ofpx::Run<ofpx::Decode>},
                                     {"encode", ofpx::Run<ofpx::Encode>},
                                     {"jsonrpc", ofpx::Run<ofpx::JsonRpc>},
                                     {"help", ofpx::Run<ofpx::Help>}};

static int run_xpc_service();
static void print_usage(std::ostream &out);

//---------//
// m a i n //
//---------//

int main(int argc, const char *const *argv) {
  // If the OFPX_JSONRPC_XPC_SERVICE environment variable is set to 1,
  // immediately run the JSON-RPC XPC service (Mac OS X only).

  if (const char *env = getenv("OFPX_JSONRPC_XPC_SERVICE")) {
    if (strcmp(env, "1") == 0) {
      return run_xpc_service();
    }
  }

  llvm::raw_fd_ostream logfile{2, true};
  logfile.SetBufferSize(4096);
  ofp::log::setOutputStream(&logfile);
  ofp::log::setOutputLevelFilter(ofp::log::Level::Info);

  if (argc < 2) {
    print_usage(std::cerr);
    return 1;
  }

  std::string name = argv[1];
  if (name == "--help" || name == "-h") {
    print_usage(std::cout);
    return 0;
  }

  for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
    if (name == programs[i].name) {
      return (*programs[i].run)(argc - 1, argv + 1);
    }
  }

  std::cerr << "ofpx: '" << name
            << "' is not a ofpx command. See 'ofpx --help'.\n";

  return 1;
}

//-------------------------------//
// r u n _ x p c _ s e r v i c e //
//-------------------------------//

int run_xpc_service() {
  const char *args[] = {"jsonrpc", "--xpc"};
  return ofpx::Run<ofpx::JsonRpc>(2, args);
}

//-----------------------//
// p r i n t _ u s a g e //
//-----------------------//

void print_usage(std::ostream &out) {
  out << "Usage: ofpx <command> [ <options> ]\n\n";
  out << "Commands:\n";
  for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
    out << "  " << programs[i].name << '\n';
  }
  out << '\n';
}
