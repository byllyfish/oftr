// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMETERNUMBER_H_
#define OFP_YAML_YMETERNUMBER_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/meternumber.h"
#include "ofp/yaml/enumconverter.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::MeterNumber> {
  static const ofp::yaml::EnumConverterSparse<ofp::OFPMeterNo> converter;

  static void output(const ofp::MeterNumber &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    auto meterNum = static_cast<ofp::OFPMeterNo>(value);
    if (converter.convert(meterNum, &scalar)) {
      out << scalar;
    } else {
      // Output MeterNumber in hexadecimal.
      ScalarTraits<Hex32>::output(meterNum, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::MeterNumber &value) {
    if (!scalar.empty() && std::isalpha(scalar.front())) {
      ofp::OFPMeterNo meterNum;
      if (!converter.convert(scalar, &meterNum)) {
        return "Invalid meter number value";
      }
      value = meterNum;
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

  using json_type = ofp::MeterNumber;
};

template <>
inline void primitive_to_json(ofp::MeterNumber value, llvm::raw_ostream &os) {
  llvm::StringRef scalar;
  auto meterNum = static_cast<ofp::OFPMeterNo>(value);
  if (ScalarTraits<ofp::MeterNumber>::converter.convert(meterNum, &scalar)) {
    os << '"' << scalar << '"';
  } else {
    os << meterNum;
  }
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMETERNUMBER_H_
