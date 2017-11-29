// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_FILTERACTION_H_
#define OFP_RPC_FILTERACTION_H_

#include "ofp/byterange.h"
#include "ofp/portnumber.h"
#include "ofp/yaml/yllvm.h"

namespace ofp {

class Message;

namespace rpc {

class FilterAction {
 public:
  enum Type { NONE, GENERIC_REPLY };

  virtual ~FilterAction() {}

  virtual bool apply(ByteRange enetFrame, PortNumber inPort,
                     Message *message) = 0;
};

}  // namespace rpc
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct ScalarEnumerationTraits<ofp::rpc::FilterAction::Type> {
  static void enumeration(IO &io, ofp::rpc::FilterAction::Type &value) {
    io.enumCase(value, "NONE", ofp::rpc::FilterAction::NONE);
    io.enumCase(value, "GENERIC_REPLY", ofp::rpc::FilterAction::GENERIC_REPLY);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_RPC_FILTERACTION_H_
