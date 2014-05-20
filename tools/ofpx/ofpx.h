#ifndef OFPX_PING_H
#define OFPX_PING_H

#include "llvm/Support/CommandLine.h"
#include <vector>
#include <string>

namespace ofpx {

namespace cl = llvm::cl;

class Subprogram {
public:
    virtual ~Subprogram() {}
    virtual int run(int argc, char **argv) = 0;
};

using RunSubprogram = int(*)(int argc, char **argv);

template <class Type>
inline int Run(int argc, char **argv) {
  Type t;
  return t.run(argc, argv);
}

}  // namespace ofpx


int ofpx_ping(int argc, char **argv);

#endif // OFPX_PING_H
