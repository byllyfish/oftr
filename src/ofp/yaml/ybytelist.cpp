// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/ybytelist.h"

namespace llvm {
namespace yaml {

void primitive_to_json(JsonByteRange r, llvm::raw_ostream &os) {
  // N.B. We are responsible for adding quotes.
  os << '"';
  ofp::RawDataToHex(r.value.data(), r.value.size(), os);
  os << '"';
}

}  // namespace yaml
}  // namespace llvm
