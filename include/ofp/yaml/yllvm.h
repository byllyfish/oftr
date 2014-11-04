// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YLLVM_H_
#define OFP_YAML_YLLVM_H_

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/Format.h"
#include "ofp/yaml/yconstants.h"
#include "ofp/header.h"

LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(ofp::UInt8);
LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(std::string);

namespace llvm {
namespace yaml {

// Since YAML/IO is based on the type of an object, we'll sometimes need to
// use a pseudo-type reference to implement behavior for a specific type. For
// example, we might want to encode a PropertyRange differently depending on
// whether we are handling Queue properties or TableFeature properties. Use
// the Ref_cast utility function to perform the cast of a reference.

template <class DestType, class SourceType>
inline DestType &Ref_cast(SourceType &obj) {
  return reinterpret_cast<DestType &>(obj);
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YLLVM_H_
