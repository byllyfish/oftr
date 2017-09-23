// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YCONTROLLERMAXLEN_H_
#define OFP_YAML_YCONTROLLERMAXLEN_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/controllermaxlen.h"
#include "ofp/yaml/enumconverter.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::ControllerMaxLen> {
  static const ofp::yaml::EnumConverterSparse<ofp::OFPControllerMaxLen>
      converter;

  static void output(const ofp::ControllerMaxLen &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    auto maxLen = static_cast<ofp::OFPControllerMaxLen>(value);
    if (converter.convert(maxLen, &scalar)) {
      out << scalar;
    } else {
      // Output ControllerMaxLen in hexadecimal.
      ScalarTraits<Hex16>::output(maxLen, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::ControllerMaxLen &value) {
    if (!scalar.empty() && std::isalpha(scalar.front())) {
      ofp::OFPControllerMaxLen maxLen;
      if (!converter.convert(scalar, &maxLen)) {
        return "Invalid max_len value";
      }
      value = maxLen;
      return "";
    }

    uint16_t num;
    auto err = ScalarTraits<uint16_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static bool mustQuote(StringRef) { return false; }

  using json_type = ofp::ControllerMaxLen;
};

template <>
inline void primitive_to_json(ofp::ControllerMaxLen value,
                              llvm::raw_ostream &os) {
  llvm::StringRef scalar;
  auto maxLen = static_cast<ofp::OFPControllerMaxLen>(value);
  if (ScalarTraits<ofp::ControllerMaxLen>::converter.convert(maxLen, &scalar)) {
    os << '"' << scalar << '"';
  } else {
    os << maxLen;
  }
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YCONTROLLERMAXLEN_H_
