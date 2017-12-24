// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./oftr_decode.h"
#include "./oftr_encode.h"
#include "ofp/ofp.h"
#if LIBOFP_ENABLE_OPENSSL
#include <openssl/ssl.h>  // For OPENSSL_VERSION_NUMBER
#endif                    // LIBOFP_ENABLE_OPENSSL
#if LIBOFP_ENABLE_JSONRPC
#include <asio/version.hpp>
#include "./oftr_jsonrpc.h"
#endif  // LIBOFP_ENABLE_JSONRPC
#if HAVE_LIBPCAP
#include <pcap/pcap.h>
#endif
#include "./oftr_help.h"
#include "llvm/Support/Host.h"

using namespace llvm;

struct SubprogramEntry {
  const char *name;
  ofpx::RunSubprogram run;
};

static const SubprogramEntry programs[] = {
    {"encode", ofpx::Run<ofpx::Encode>},
    {"decode", ofpx::Run<ofpx::Decode>},
#if LIBOFP_ENABLE_JSONRPC
    {"jsonrpc", ofpx::Run<ofpx::JsonRpc>},
#endif  // LIBOFP_ENABLE_JSONRPC
    {"help", ofpx::Run<ofpx::Help>}};

static void print_usage(llvm::raw_ostream &out);
static void print_version(llvm::raw_ostream &out);

int main(int argc, const char *const *argv) {
  if (argc < 2) {
    print_usage(llvm::errs());
    return 1;
  }

  std::string name = argv[1];
  if (name == "--help" || name == "-h") {
    print_usage(llvm::outs());
    return 0;
  }

  if (name == "version" || name == "-v" || name == "-version" ||
      name == "--version") {
    print_version(llvm::outs());
    return 0;
  }

  cl::SetVersionPrinter(print_version);

  for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
    if (name == programs[i].name) {
      return (*programs[i].run)(argc - 1, argv + 1);
    }
  }

  llvm::errs() << "oftr: '" << name
               << "' is not a oftr command. See 'oftr --help'.\n";

  return 1;
}

void print_usage(llvm::raw_ostream &out) {
  out << "Usage: oftr <command> [ <options> ]\n\n";
  out << "Commands:\n";
  for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
    out << "  " << programs[i].name << '\n';
  }
  out << '\n';
}

void print_version(llvm::raw_ostream &out) {
  std::string libofpCommit{LIBOFP_GIT_COMMIT_LIBOFP};
  out << "oftr " << LIBOFP_VERSION_STRING << " (" << libofpCommit.substr(0, 7)
      << ")";

  out << "  <" << LIBOFP_GITHUB_URL << ">\n";

  const char *bits = "? bit";
  if (sizeof(void *) == 8) {
    bits = "64 bit";
  } else if (sizeof(void *) == 4) {
    bits = "32 bit";
  }

#ifndef __OPTIMIZE__
  out << "  DEBUG ";
#else
  out << "  Optimized ";
#endif
  out << bits << " build";
#ifndef NDEBUG
  out << " with assertions";
#endif
  out << " (" << LIBOFP_COMPILER_SPEC << ")\n";

#if LIBOFP_ENABLE_JSONRPC
  unsigned asioMajor = ASIO_VERSION / 100000;
  unsigned asioMinor = ASIO_VERSION / 100 % 1000;
  unsigned asioPatch = ASIO_VERSION % 100;
  std::string asioCommit{LIBOFP_GIT_COMMIT_ASIO};

  out << "  ASIO " << asioMajor << '.' << asioMinor << '.' << asioPatch << " ("
      << asioCommit.substr(0, 7) << ")\n";
#endif  // LIBOFP_ENABLE_JSONRPC

#if LIBOFP_ENABLE_OPENSSL
  unsigned sslMajor = (OPENSSL_VERSION_NUMBER >> 28) & 0x0F;
  unsigned sslMinor = (OPENSSL_VERSION_NUMBER >> 20) & 0xFF;
  unsigned sslPatch = (OPENSSL_VERSION_NUMBER >> 12) & 0xFF;
  std::string sslCommit{LIBOFP_GIT_COMMIT_BORINGSSL};

  out << "  BoringSSL " << sslMajor << '.' << sslMinor << '.' << sslPatch
      << " (" << sslCommit.substr(0, 7) << ")\n";
#endif  // LIBOFP_ENABLE_OPENSSL

#if HAVE_LIBPCAP
  // Print libpcap version.
  out << "  Using: " << pcap_lib_version() << '\n';
#endif  // HAVE_LIBPCAP
}
