#ifndef OFP_YAML_YPORTNUMBER_H_
#define OFP_YAML_YPORTNUMBER_H_

#include "ofp/portnumber.h"
#include "ofp/yaml/enumconverter.h"
#include "ofp/log.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::PortNumber> {
  static ofp::yaml::EnumConverterSparse<ofp::OFPPortNo> converter;

  static void output(const ofp::PortNumber &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    auto portNum = static_cast<ofp::OFPPortNo>(value);
    if (converter.convert(portNum, &scalar)) {
        out << scalar;
    } else {
      // Output PortNumber in hexadecimal.
      ScalarTraits<Hex32>::output(portNum, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::PortNumber &value) {
    if (!scalar.empty() && std::isalpha(scalar.front())) {
      ofp::OFPPortNo portNum; 
      if (!converter.convert(scalar, &portNum)) {
        return "Invalid port number value";
      }
      value = portNum;
      return "";
    }

    uint32_t num;
    auto err = ScalarTraits<uint32_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static bool mustQuote(StringRef) { return false; }

  using json_type = uint32_t;
};

}  // namespace yaml
}  // namespace llvm

#endif // OFP_YAML_YPORTNUMBER_H_
