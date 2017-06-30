// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YVLANNUMBER_H_
#define OFP_YAML_YVLANNUMBER_H_

#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

#define VLAN_FORMAT_DISPLAY 0

template <>
struct ScalarTraits<ofp::VlanNumber> {
  static void output(const ofp::VlanNumber &value, void *ctxt,
                     llvm::raw_ostream &out) {
#if VLAN_FORMAT_DISPLAY
    ScalarTraits<ofp::SignedInt32>::output(value.displayCode(), ctxt, out);
#else
    ScalarTraits<Hex16>::output(value.value(), ctxt, out);
#endif  // VLAN_FORMAT_DISPLAY
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::VlanNumber &value) {
#if VLAN_FORMAT_DISPLAY
    ofp::SignedInt32 code;
    auto err = ScalarTraits<ofp::SignedInt32>::input(scalar, ctxt, code);
    if (err.empty()) {
      value.setDisplayCode(code);
    }
    return err;
#else
    ofp::UInt16 code;
    auto err = ScalarTraits<ofp::UInt16>::input(scalar, ctxt, code);
    if (err.empty()) {
      value = ofp::VlanNumber{code};
    }
    return err;
#endif  // VLAN_FORMAT_DISPLAY
  }

  static bool mustQuote(StringRef) { return false; }

  using json_type = ofp::VlanNumber;
};

template <>
inline std::string primitive_to_json(ofp::VlanNumber value) {
#if VLAN_FORMAT_DISPLAY
  return std::to_string(value.displayCode());
#else
  return std::to_string(value.value());
#endif
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YVLANNUMBER_H_
