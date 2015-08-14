#ifndef OFP_YAML_YLLDPVALUE_H_
#define OFP_YAML_YLLDPVALUE_H_

#include "ofp/lldpvalue.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::LLDPValue> {
  static void output(const ofp::LLDPValue &value, void *ctxt,
                     llvm::raw_ostream &out) {
    ofp::log::debug("LLDPValue:output", value.size(), RawDataToHex(&value, sizeof(value)));
    out << ofp::RawDataToHex(value.data(), value.size());
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::LLDPValue &value) {
    bool error = false;
    size_t actualSize =
        ofp::HexToRawData(scalar, value.mutableData(), value.maxSize(), &error);
    value.resize(actualSize);
    if (error)
      return "Invalid hexadecimal text.";
    return "";
  }

  static bool mustQuote(StringRef) { return false; }
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YLLDPVALUE_H_
