// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCID_H_
#define OFP_RPC_RPCID_H_

#include "ofp/yaml/yllvm.h"

namespace ofp {
namespace rpc {

class RpcID {
 public:
  enum : UInt64 {
    MAX_VALUE = 0xfffffffffffffffdUL,
    NULL_VALUE = 0xfffffffffffffffeUL,
    MISSING = 0xffffffffffffffffUL
  };

  /* implicit NOLINT */ RpcID(UInt64 id = MISSING) : id_{id} {}

  bool is_null() const { return id_ == NULL_VALUE; }
  bool is_missing() const { return id_ == MISSING; }
  UInt64 value() const { return id_; }

  bool parse(llvm::StringRef value) {
    UInt64 id;
    if (value.getAsInteger(0, id)) {
      // value is not an unsigned integer...
      if (value == "null") {
        id = NULL_VALUE;
      } else if (value == "") {
        id = MISSING;
      } else {
        return false;
      }
    } else if (id > MAX_VALUE) {
      return false;
    }
    id_ = id;
    return true;
  }

 private:
  UInt64 id_;
};

}  // namespace rpc
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::rpc::RpcID> {
  static void output(const ofp::rpc::RpcID &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value.value();
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::rpc::RpcID &value) {
    if (!value.parse(scalar)) {
      return "Invalid RPC ID (must be unsigned int < 2^64-2)";
    }
    return "";
  }

  static bool mustQuote(StringRef) { return false; }

  using json_type = ofp::rpc::RpcID;
};

template <>
inline std::string primitive_to_json(ofp::rpc::RpcID value) {
  return value.is_null() ? "null" : value.is_missing()
                                        ? "\"\""
                                        : std::to_string(value.value());
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_RPC_RPCID_H_
