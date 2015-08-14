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
    } else {
      out << llvm::format("0x%08X", value.oxmNative());
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::OXMFullType &value) {
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

#endif // OFP_YAML_YOXMFULLTYPE_H_
