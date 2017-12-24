// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YLLVM_H_
#define OFP_YAML_YLLVM_H_

#include "llvm/ADT/SmallString.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/YAMLTraits.h"
#include "ofp/header.h"
#include "ofp/yaml/yconstants.h"

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

namespace ofp {

class ByteList;
class Message;

namespace yaml {

// Forward declare these here rather than stick them in their own header.
void DecodeRecursively(llvm::yaml::IO &io, const char *key, const Message *msg);
void EncodeRecursively(llvm::yaml::IO &io, const char *key, ByteList &data);

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_YLLVM_H_
