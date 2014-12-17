// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef TOOLS_OFPX_OFPX_H_
#define TOOLS_OFPX_OFPX_H_

#include <vector>
#include <string>
#include "llvm/Support/CommandLine.h"
#include "ofp/ofp.h"

namespace ofpx {

namespace cl = llvm::cl;

class Subprogram {
 public:
  virtual ~Subprogram() {}
  virtual int run(int argc, const char *const *argv) = 0;

 protected:
  static const int MinExitStatus = 10;
};

using RunSubprogram = int (*)(int argc, const char *const *argv);

template <class Type>
inline int Run(int argc, const char *const *argv) {
  Type t;
  return t.run(argc, argv);
}

struct IPv6EndpointParser : public cl::parser<ofp::IPv6Endpoint> {
 public:
  // parse - Return true on error.
  bool parse(cl::Option &O, llvm::StringRef ArgName, llvm::StringRef ArgValue,
             ofp::IPv6Endpoint &Val) {
    if (Val.parse(ArgValue)) return false;
    return O.error("Unexpected endpoint format '" + ArgValue + "'!");
  }
};

}  // namespace ofpx

#endif  // TOOLS_OFPX_OFPX_H_
