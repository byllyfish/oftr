#include "ofp/yaml/ybytelist.h"

namespace ofp {

// Set once at program startup to output MongoDB shell JSON syntax, e.g.:
//     Binary data: `HexData("aabbcc")`
bool GLOBAL_ARG_MongoDBCompatible = false;

}  // namespace ofp



namespace llvm {
namespace yaml {

std::string primitive_to_json(JsonByteRange r) {
  std::ostringstream oss;
  if (ofp::GLOBAL_ARG_MongoDBCompatible) {
    oss << "{\"$binary\":\"" << ofp::RawDataToBase64(r.value.data(), r.value.size()) << "\",\"$type\":\"00\"}";
  } else {
    // N.B. We are responsible for adding quotes.
    oss << '"' << ofp::RawDataToHex(r.value.data(), r.value.size()) << '"';
  }
  return oss.str();
}

}  // namespace yaml
}  // namespace llvm
