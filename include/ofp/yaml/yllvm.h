//  ===== ---- ofp/yaml/yllvm.h ----------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Include LLVM YAML headers and define yaml traits for common types.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YLLVM_H_
#define OFP_YAML_YLLVM_H_

// The following two definitions are required by llvm/Support/DataTypes.h.
// These C macros have no effect when using C++11.
#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"
#include "ofp/yaml/yconstants.h"
#include "ofp/header.h"

LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(ofp::UInt8);

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
