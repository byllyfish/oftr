//  ===== ---- ofp/ysmallcstring.h -------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::ScalarTraits for the SmallCString class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YSMALLCSTRING_H_
#define OFP_YAML_YSMALLCSTRING_H_

#include "ofp/smallcstring.h"

namespace llvm {
namespace yaml {

template <size_t Size>
struct ScalarTraits<ofp::SmallCString<Size>> {
    static void output(const ofp::SmallCString<Size> &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::SmallCString<Size> &value)
    {
    	if (scalar.size() > value.capacity()) {
    		return "Value is too long";
    	}
        value = scalar;

        return "";
    }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YSMALLCSTRING_H_
