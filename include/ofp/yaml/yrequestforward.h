// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YREQUESTFORWARD_H_
#define OFP_YAML_YREQUESTFORWARD_H_

namespace llvm {
namespace yaml {

// The YAML mapping struct for OFPT_REQUESTFORWARD is just a message
// Encoder/Decoder.

const char *const kRequestForwardSchema = R"""({Message/RequestForward}
type: 'REQUESTFORWARD'
msg: {Message}
)""";

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YREQUESTFORWARD_H_
