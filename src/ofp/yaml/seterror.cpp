// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ycontext.h"
#include "ofp/yaml/seterror.h"
#include "ofp/yaml/encoder.h"

static llvm::StringRef closestValue(llvm::StringRef name,
                                    const std::vector<llvm::StringRef> &vals) {
  assert(!vals.empty());

  std::string s = llvm::StringRef{name}.upper();
  unsigned minDistance = 0xffff;
  llvm::StringRef minVal;

  for (auto val : vals) {
    if (val.startswith(s))
      return val;
    unsigned dist = val.edit_distance(s, true, minDistance);
    if (dist < minDistance) {
      minDistance = dist;
      minVal = val;
    }
  }

  return minVal;
}

llvm::StringRef ofp::yaml::SetEnumError(
    void *ctxt, llvm::StringRef name,
    const std::vector<llvm::StringRef> &vals) {
  Encoder *encoder = detail::YamlContext::GetEncoder(ctxt);
  if (encoder) {
    llvm::yaml::IO *io = encoder->io();
    if (io) {
      io->setError("unknown value \"" + name + "\" Did you mean \"" +
                   closestValue(name, vals) + "\"?");
    }
  }
  return "";
}

void ofp::yaml::SetFlagError(llvm::yaml::IO &io, llvm::StringRef name,
                             const std::string &schema) {
  llvm::SmallVector<llvm::StringRef, 25> vals;
  llvm::StringRef{schema}.split(vals, ",");

  std::vector<llvm::StringRef> words;
  for (auto val : vals) {
    if (!val.ltrim().startswith("'0x"))
      words.push_back(val.trim());
  }

  io.setError("unknown value \"" + name + "\". Did you mean \"" +
              closestValue(name, words) + "\"?");
}
