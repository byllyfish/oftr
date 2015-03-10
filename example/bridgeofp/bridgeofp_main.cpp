// Copyright 2014-present Bill Fisher. All rights reserved.

// The following two definitions are required by llvm/Support/DataTypes.h
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1

#include <iostream>
#include "./bridgelistener.h"
#include "llvm/Support/CommandLine.h"

using namespace bridge;
using namespace llvm;

struct IPv6EndpointParser : public cl::parser<ofp::IPv6Endpoint> {
 public:
  // parse - Return true on error.
  bool parse(cl::Option &O, llvm::StringRef ArgName, llvm::StringRef ArgValue,
             ofp::IPv6Endpoint &Val) {
    if (Val.parse(ArgValue)) return false;
    return O.error("Unexpected endpoint format '" + ArgValue + "'!");
  }
};

int main(int argc, char **argv) {
  cl::opt<IPv6Endpoint> listen{"listen", cl::desc("Listen on local endpoint"),
                               cl::Required};
  cl::opt<IPv6Endpoint> connect{
      "connect", cl::desc("Connect to remote endpoint"), cl::Required};
  cl::ParseCommandLineOptions(argc, argv);

  log::setOutputStream(&std::clog);

  Driver driver;
  std::error_code err;
  (void)driver.listen(ChannelMode::Raw, 0, listen, ProtocolVersions::All,
                      [connect]() { return new BridgeListener(connect); }, err);

  int exitCode = 0;
  if (err) {
    log::error("ERROR", err);
    exitCode = 2;
  }

  driver.run();

  return exitCode;
}
