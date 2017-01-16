// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YOXMFULLTYPE_H_
#define OFP_YAML_YOXMFULLTYPE_H_

#include "ofp/oxmfulltype.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::OXMFullType> {
  static void output(const ofp::OXMFullType &value, void *ctxt,
                     llvm::raw_ostream &out) {
    const ofp::OXMTypeInfo *info = value.lookupInfo();
    if (info) {
      out << info->name;
    } else if (value.experimenter() == 0) {
      // Format as:  `oxm`
      out << llvm::format("0x%08X", value.oxmNative());
    } else {
      // Format as:  `oxm.experimenter`
      out << llvm::format("0x%08X.0x%08X", value.oxmNative(),
                          value.experimenter());
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::OXMFullType &value) {
    if (value.parse(scalar)) {
      return "";
    }

    // Check for format `oxm.experimenter`
    auto pair = scalar.split('.');
    if (!pair.second.empty()) {
      ofp::UInt32 oxm;
      ofp::UInt32 experimenter;
      if (ofp::yaml::ParseUnsignedInteger(pair.first, &oxm) &&
          ofp::yaml::ParseUnsignedInteger(pair.second, &experimenter)) {
        value.setOxmNative(oxm, experimenter);
        return "";
      }
    }

    // Check for format `oxm` only.
    ofp::UInt32 num;
    if (ofp::yaml::ParseUnsignedInteger(scalar, &num)) {
      value.setOxmNative(num, 0);
      return "";
    }

    return "Invalid OXM type.";
  }

  static bool mustQuote(StringRef) { return false; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YOXMFULLTYPE_H_
