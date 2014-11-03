//  ===== ---- ofp/yaml/ydatapathid.h ----------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::ScalarTraits for DatapathID.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YDATAPATHID_H_
#define OFP_YAML_YDATAPATHID_H_

#include "ofp/datapathid.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::DatapathID> {
  static void output(const ofp::DatapathID &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value.toString();
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::DatapathID &value) {
    if (!value.parse(scalar)) {
      return "Invalid DatapathID.";
    }
    return "";
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YDATAPATHID_H_
