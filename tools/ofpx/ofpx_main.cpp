
#include "ofp.h"
#include "ofpx.h"
#include <iostream>

using namespace llvm;

typedef int (*Program)(int argc, char **argv);

struct ProgramEntry {
  const char *name;
  Program prog;
};

static ProgramEntry programs[] = {{"ping", ofpx_ping}};

int main(int argc, char **argv) {

  ofp::log::set(&std::cerr);
  
  if (argc >= 2) {
    char *name = argv[1];
    for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
      if (std::strcmp(name, programs[i].name) == 0) {
        return (*programs[i].prog)(argc - 1, argv + 1);
      }
    }
  }

  std::cerr << "Invalid arguments." << '\n';

  return -2;
}
