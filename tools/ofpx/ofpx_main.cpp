// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/ofp.h"
#include "./ofpx_decode.h"
#include "./ofpx_encode.h"
#include "./ofpx_jsonrpc.h"
#include "./ofpx_help.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Host.h"
#include <openssl/ssl.h> // For SSLeay_version

using namespace llvm;

struct SubprogramEntry {
  const char *name;
  ofpx::RunSubprogram run;
};

static SubprogramEntry programs[] = {{"encode", ofpx::Run<ofpx::Encode>},
                                     {"decode", ofpx::Run<ofpx::Decode>},
                                     {"jsonrpc", ofpx::Run<ofpx::JsonRpc>},
                                     {"help", ofpx::Run<ofpx::Help>}};

static int run_xpc_service();
static void print_usage(std::ostream &out);
static void print_version();

int main(int argc, const char *const *argv) {
  // If the OFPX_JSONRPC_XPC_SERVICE environment variable is set to 1,
  // immediately run the JSON-RPC XPC service (Mac OS X only).

  if (const char *env = getenv("OFPX_JSONRPC_XPC_SERVICE")) {
    if (strcmp(env, "1") == 0) {
      return run_xpc_service();
    }
  }

  if (argc < 2) {
    print_usage(std::cerr);
    return 1;
  }

  std::string name = argv[1];
  if (name == "--help" || name == "-h") {
    print_usage(std::cout);
    return 0;
  }

  if (name == "version" || name == "-v" || name == "-version" || name == "--version") {
    print_version();
    return 0;
  }

  cl::SetVersionPrinter(print_version);

  for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
    if (name == programs[i].name) {
      return (*programs[i].run)(argc - 1, argv + 1);
    }
  }

  std::cerr << "ofpx: '" << name
            << "' is not a ofpx command. See 'ofpx --help'.\n";

  return 1;
}

int run_xpc_service() {
  const char *args[] = {"jsonrpc", "--xpc"};
  return ofpx::Run<ofpx::JsonRpc>(2, args);
}

void print_usage(std::ostream &out) {
  out << "Usage: ofpx <command> [ <options> ]\n\n";
  out << "Commands:\n";
  for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
    out << "  " << programs[i].name << '\n';
  }
  out << "  version\n";
  out << '\n';
}

void print_version() {
  raw_ostream &os = outs();

  os << "libofp " << LIBOFP_VERSION_MAJOR << '.' << LIBOFP_VERSION_MINOR << " (" << LIBOFP_DOWNLOAD_URL << ")\n";
#ifndef __OPTIMIZE__
  os << "  DEBUG build";
#else
  os << "  Optimized build";
#endif
#ifndef NDEBUG
  os << " with assertions";
#endif
  std::string CPU = llvm::sys::getHostCPUName();
  if (CPU == "generic")
    CPU = "(unknown)";
  os << ".\n"
     << "  Default target: " << llvm::sys::getDefaultTargetTriple() << '\n'
     << "  Host CPU: " << CPU << '\n';

  unsigned major = (OPENSSL_VERSION_NUMBER >> 28) & 0x0F;
  unsigned minor = (OPENSSL_VERSION_NUMBER >> 20) & 0xFF;
  unsigned subminor = (OPENSSL_VERSION_NUMBER >> 12) & 0xFF;
  os << "  BoringSSL " << major << '.' << minor << '.' << subminor << '\n';
}
