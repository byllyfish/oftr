// Copyright (c) 2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include <vector>
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/MemoryBuffer.h"

// The "yamlio_parse" program runs a memory buffer through the YAML
// parser. It dumps out the tokens that are parsed.
//
// Usage:
//
//    yamlio_parse [-s] [filename...]
//
// A filename of "-" specifies stdin.
//
// The "-s" option uses the "scanTokens" function which does not
// emit any output. Use this for profiling the parser.

int main(int argc, char **argv) {
  std::vector<llvm::StringRef> args{argv + 1, argv + argc};
  if (args.empty()) {
    args.push_back("-");
  }

  bool scanOnly = false;
  if (args[0] == "-s") {
    scanOnly = true;
    args.erase(args.begin());
  }

  for (const auto filename: args) {
    auto result = llvm::MemoryBuffer::getFileOrSTDIN(filename);
    if (!result) {
    	llvm::errs() << filename << ": Failed to open\n";
    	return 1;
    }

    llvm::StringRef buffer = (*result)->getBuffer();

    if (scanOnly) {
      if (!llvm::yaml::scanTokens(buffer)) {
        llvm::errs() << filename << ": Parse error\n";
        return 1;
      }
    } else if (!llvm::yaml::dumpTokens(buffer, llvm::outs())) {
    	llvm::errs() << filename << ": Parse error\n";
    	return 1;
    }
  }

  return 0;
}
