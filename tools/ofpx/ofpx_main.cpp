
#include "ofp/ofp.h"
#include "ofpx_decode.h"
#include "ofpx_ping.h"
#include <iostream>

using namespace llvm;

struct SubprogramEntry {
  const char *name;
  ofpx::RunSubprogram run;
};

static SubprogramEntry programs[] = {
  {"ping", ofpx::Run<ofpx::Ping>},
  {"decode", ofpx::Run<ofpx::Decode>}
};

int main(int argc, char **argv) {

  ofp::log::setOutputStream(&std::cerr);
  ofp::log::setOutputLevelFilter(ofp::log::Level::Info);

  if (argc >= 2) {
    char *name = argv[1];
    for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
      if (std::strcmp(name, programs[i].name) == 0) {
        return (*programs[i].run)(argc - 1, argv + 1);
      }
    }
  }

  std::cerr << "Invalid arguments." << '\n';

  return -2;
}
