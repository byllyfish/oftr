//  ===== ---- ofp/yaml/ybytelist.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::ScalarTraits for ByteRange and ByteList.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YBYTELIST_H
#define OFP_YAML_YBYTELIST_H

#include "ofp/yaml/yllvm.h"
#include "ofp/bytelist.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct ScalarTraits<ofp::ByteRange> {

    static void output(const ofp::ByteRange &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << ofp::RawDataToHex(value.data(), value.size());
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::ByteRange &value)
    {
        return "Not supported.";
    }
};

template <>
struct ScalarTraits<ofp::ByteList> {

    static void output(const ofp::ByteList &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << ofp::RawDataToHex(value.data(), value.size());
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::ByteList &value)
    {
        value.resize(scalar.size() / 2 + 2);
        bool error = false;
        size_t actualSize = ofp::HexToRawData(scalar, value.mutableData(),
                                              value.size(), &error);
        value.resize(actualSize);
        if (error)
            return "Invalid hexadecimal text.";
        return "";
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YBYTELIST_H
