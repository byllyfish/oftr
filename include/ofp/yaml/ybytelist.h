// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YBYTELIST_H_
#define OFP_YAML_YBYTELIST_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/bytelist.h"

namespace ofp {

// Set once at program startup to output MongoDB shell JSON syntax, e.g.:
//     Binary data: `HexData("aabbcc")`
extern bool GLOBAL_ARG_MongoDBCompatible;

}  // namespace ofp

namespace llvm {
namespace yaml {

// Make a new type equivalent to ofp::ByteRange so we can control the strict 
// JSON output format. `JsonByteRange` is used as ByteRange's json_type.
class JsonByteRange {
public:
  JsonByteRange(const ofp::ByteRange &r) : value{r} {}
  ofp::ByteRange value;
};

std::string primitive_to_json(JsonByteRange r);

template <>
struct ScalarTraits<ofp::ByteRange> {
  static void output(const ofp::ByteRange &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << ofp::RawDataToHex(value.data(), value.size());
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::ByteRange &value) {
    return "Not supported.";
  }

  static bool mustQuote(StringRef) { return false; }

  using json_type = JsonByteRange;
};

template <>
struct ScalarTraits<ofp::ByteList> {
  static void output(const ofp::ByteList &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << ofp::RawDataToHex(value.data(), value.size());
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::ByteList &value) {
    scalar = clean(scalar);
    value.resize(scalar.size() / 2 + 2);
    bool error = false;
    size_t actualSize =
        ofp::HexToRawData(scalar, value.mutableData(), value.size(), &error);
    value.resize(actualSize);
    if (error)
      return "Invalid hexadecimal text.";
    return "";
  }

  static bool mustQuote(StringRef) { return false; }

private:
  static StringRef clean(StringRef s) {
    if (s.startswith_lower("hexdata(\"")) {
      s = s.drop_front(9);
      if (s.endswith("\")")) {
        s = s.drop_back(2);
      }
    }
    return s;
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBYTELIST_H_
