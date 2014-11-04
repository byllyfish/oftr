// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/ofp.h"
#include "ofpx_decode.h"
#include "ofpx_encode.h"
#include "ofpx_ping.h"
#include "ofpx_jsonrpc.h"
#include <iostream>

using namespace llvm;

struct SubprogramEntry {
  const char *name;
  ofpx::RunSubprogram run;
};

static SubprogramEntry programs[] = {{"ping", ofpx::Run<ofpx::Ping>},
                                     {"decode", ofpx::Run<ofpx::Decode>},
                                     {"encode", ofpx::Run<ofpx::Encode>},
                                     {"jsonrpc", ofpx::Run<ofpx::JsonRpc>}};

//-----------------------//
// p r i n t _ u s a g e //
//-----------------------//

static void print_usage(std::ostream &out) {
  out << "Usage: ofpx <command> [ <options> ]\n\n";
  out << "Commands:\n";
  for (size_t i = 0; i < ofp::ArrayLength(programs); ++i) {
    out << "  " << programs[i].name << '\n';
  }
  out << '\n';
}

//---------//
// m a i n //
//---------//

int main(int argc, char **argv) {
  ofp::log::setOutputStream(&std::cerr);
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
