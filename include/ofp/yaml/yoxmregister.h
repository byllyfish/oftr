// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YOXMREGISTER_H_
#define OFP_YAML_YOXMREGISTER_H_

#include "ofp/oxmregister.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::OXMRegister> {
  static void output(const ofp::OXMRegister &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value;
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::OXMRegister &value) {
    if (!value.parse(scalar)) {
      return "Invalid OXM register.";
    }
    return "";
  }

  static bool mustQuote(StringRef) { return true; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YOXMREGISTER_H_
