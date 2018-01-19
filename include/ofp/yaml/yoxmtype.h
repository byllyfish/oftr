// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YOXMTYPE_H_
#define OFP_YAML_YOXMTYPE_H_

#include "ofp/oxmtype.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::OXMType> {
  static void output(const ofp::OXMType &value, void *ctxt,
                     llvm::raw_ostream &out) {
    const ofp::OXMTypeInfo *info = (value.length() == 0)
                                       ? value.lookupInfo_IgnoreLength()
                                       : value.lookupInfo();
    if (info) {
      out << info->name;
    } else {
      out << llvm::format("0x%08X", value.oxmNative());
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::OXMType &value) {
    if (!value.parse(scalar)) {
      ofp::UInt32 num;
      if (ofp::yaml::ParseUnsignedInteger(scalar, &num)) {
        value.setOxmNative(num);
        return "";
      }

      return "Invalid OXM type.";
    }

    return "";
  }

  static bool mustQuote(StringRef) { return false; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YOXMTYPE_H_
