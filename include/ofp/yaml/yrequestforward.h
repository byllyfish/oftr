// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YREQUESTFORWARD_H_
#define OFP_YAML_YREQUESTFORWARD_H_

namespace llvm {
namespace yaml {

// The YAML mapping struct for OFPT_REQUESTFORWARD is just a message
// Encoder/Decoder.

const char *const kRequestForwardSchema = R"""({Message/RequestForward}
type: REQUESTFORWARD
msg: Message
)""";

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YREQUESTFORWARD_H_
