// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YBYTELIST_H_
#define OFP_YAML_YBYTELIST_H_

#include "ofp/bytelist.h"
#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

// Make a new type equivalent to ofp::ByteRange so we can control the strict
// JSON output format. `JsonByteRange` is used as ByteRange's json_type.
class JsonByteRange {
 public:
  /* implicit NOLINT */ JsonByteRange(const ofp::ByteRange &r) : value{r} {}
  /* implicit NOLINT */ JsonByteRange(const ofp::ByteList &l) : value{l} {}
  ofp::ByteRange value;
};

void primitive_to_json(JsonByteRange r, llvm::raw_ostream &os);

template <>
struct ScalarTraits<ofp::ByteRange> {
  static void output(const ofp::ByteRange &value, void *ctxt,
                     llvm::raw_ostream &out) {
    ofp::RawDataToHex(value.data(), value.size(), out);
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::ByteRange &value) {
    return "Not supported.";
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }

  using json_type = JsonByteRange;
};

template <>
struct ScalarTraits<ofp::ByteList> {
  static void output(const ofp::ByteList &value, void *ctxt,
                     llvm::raw_ostream &out) {
    ofp::RawDataToHex(value.data(), value.size(), out);
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::ByteList &value) {
    value.resize(scalar.size() / 2 + 2);
    bool error = false;
    size_t actualSize =
        ofp::HexToRawData(scalar, value.mutableData(), value.size(), &error);
    value.resize(actualSize);
    if (error)
      return "Invalid hexadecimal text.";
    return "";
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }

  using json_type = JsonByteRange;
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBYTELIST_H_
