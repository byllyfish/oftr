
// The following two definitions are required by llvm/Support/DataTypes.h
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1

#include "bridgelistener.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>

using namespace bridge;
using namespace llvm;

using IPv6Pair = std::pair<IPv6Endpoint, IPv6Endpoint>;

namespace std {
istream &operator>>(istream &is, IPv6Pair &pair);
ostream &operator<<(ostream &os, const IPv6Pair &pair);
}

int main(int argc, char **argv) {
  cl::opt<IPv6Pair> argEndpoints{
      "L", cl::desc("Listen and bridge to remote endpoint"), cl::Required};
  cl::ParseCommandLineOptions(argc, argv);

  log::set(&std::cerr);

  Driver driver;
  auto exc = driver.listen(
      Driver::Bridge, argEndpoints.first, ProtocolVersions::All,
      [argEndpoints]() { return new BridgeListener(argEndpoints.second); });

  int exitCode = 0;
  exc.done([&exitCode](const std::error_code &err) {
    if (err) {
      std::cerr << "ERROR: " << err << '\n';
      exitCode = 2;
    }
  });

  driver.run();

  return exitCode;
}

namespace std {
istream &operator>>(istream &is, IPv6Pair &pair) {
  return is >> skipws >> pair.first >> pair.second;
}

ostream &operator<<(ostream &os, const IPv6Pair &pair) {
  return os << pair.first << ' ' << pair.second;
}
}
