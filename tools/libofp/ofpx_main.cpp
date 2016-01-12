// Copyright 2014-present Bill Fisher. All rights reserved.

#include <openssl/ssl.h>  // For OPENSSL_VERSION_NUMBER
#include <asio/version.hpp>
#include "ofp/ofp.h"
#include "./ofpx_decode.h"
#include "./ofpx_encode.h"
#if LIBOFP_ENABLE_JSONRPC
#include "./ofpx_jsonrpc.h"
#endif  // LIBOFP_ENABLE_JSONRPC
#include "./ofpx_help.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Host.h"
#include "./libofp.h"

using namespace llvm;

struct SubprogramEntry {
  const char *name;
  ofpx::RunSubprogram run;
};

static SubprogramEntry programs[] = {{"encode", ofpx::Run<ofpx::Encode>},
                                     {"decode", ofpx::Run<ofpx::Decode>},
#if LIBOFP_ENABLE_JSONRPC
                                     {"jsonrpc", ofpx::Run<ofpx::JsonRpc>},
#endif  // LIBOFP_ENABLE_JSONRPC
                                     {"help", ofpx::Run<ofpx::Help>}};

static void print_usage(std::ostream &out);
static void print_version();
static void force_link_api();

int main(int argc, const char *const *argv) {
  if (argc < 2) {
    if (argc == 0)
      force_link_api();
    print_usage(std::cerr);
    return 1;
  }

  std::string name = argv[1];
  if (name == "--help" || name == "-h") {
    print_usage(std::cout);
    return 0;
  }

  if (name == "version" || name == "-v" || name == "-version" ||
      name == "--version") {
    print_version();
    return 0;
  }

  cl::SetVersionPrinter(print_version);

  for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
    if (name == programs[i].name) {
      return (*programs[i].run)(argc - 1, argv + 1);
    }
  }

  std::cerr << "libofp: '" << name
            << "' is not a libofp command. See 'libofp --help'.\n";

  return 1;
}

void print_usage(std::ostream &out) {
  out << "Usage: libofp <command> [ <options> ]\n\n";
  out << "Commands:\n";
  for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
    out << "  " << programs[i].name << '\n';
  }
  out << '\n';
}

void print_version() {
  raw_ostream &os = outs();

  std::string libofpCommit{LIBOFP_GIT_COMMIT_LIBOFP};
  os << "libofp " << LIBOFP_VERSION_STRING << " (" << libofpCommit.substr(0, 7)
     << ")";

  os << "  <" << LIBOFP_GITHUB_URL << ">\n";

#ifndef __OPTIMIZE__
  os << "  DEBUG build";
#else
  os << "  Optimized build";
#endif
#ifndef NDEBUG
  os << " with assertions";
#endif
  os << '\n';

#if LIBOFP_ENABLE_JSONRPC
  unsigned asioMajor = ASIO_VERSION / 100000;
  unsigned asioMinor = ASIO_VERSION / 100 % 1000;
  unsigned asioPatch = ASIO_VERSION % 100;
  std::string asioCommit{LIBOFP_GIT_COMMIT_ASIO};

  os << "  ASIO " << asioMajor << '.' << asioMinor << '.' << asioPatch << " ("
     << asioCommit.substr(0, 7) << ")\n";

  unsigned sslMajor = (OPENSSL_VERSION_NUMBER >> 28) & 0x0F;
  unsigned sslMinor = (OPENSSL_VERSION_NUMBER >> 20) & 0xFF;
  unsigned sslPatch = (OPENSSL_VERSION_NUMBER >> 12) & 0xFF;
  std::string sslCommit{LIBOFP_GIT_COMMIT_BORINGSSL};

  os << "  BoringSSL " << sslMajor << '.' << sslMinor << '.' << sslPatch << " ("
     << sslCommit.substr(0, 7) << ")\n";
#endif  // LIBOFP_ENABLE_JSONRPC
}

void force_link_api() {
  libofp_buffer buf;
  libofp_version(&buf);
  libofp_buffer_free(&buf);

  libofp_buffer empty = {nullptr, 0};
  libofp_encode(&buf, &empty, 0);
  libofp_buffer_free(&buf);

  libofp_decode(&buf, &empty, 0);
  libofp_buffer_free(&buf);
}
