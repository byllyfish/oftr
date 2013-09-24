//  ===== ---- ofp/yaml/yexperimenter.h --------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the Experimenter and 
/// ExperimenterBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YEXPERIMENTER_H
#define OFP_YAML_YEXPERIMENTER_H

#include "ofp/experimenter.h"
#include "ofp/yaml/ybytelist.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::Experimenter> {

    static void mapping(IO &io, ofp::Experimenter &msg)
    {
        ofp::UInt32 experimenter = msg.experimenter();
        ofp::UInt32 expType = msg.expType();
        ofp::ByteRange data = msg.expData();
        io.mapRequired("experimenter", experimenter);
        io.mapRequired("exp_type", expType);
        io.mapRequired("data", data);
    }
};


template <>
struct MappingTraits<ofp::ExperimenterBuilder> {

    static void mapping(IO &io, ofp::ExperimenterBuilder &msg)
    {
        ofp::UInt32 experimenter;
        ofp::UInt32 expType;
        ofp::ByteList data;
        io.mapRequired("experimenter", experimenter);
        io.mapRequired("exp_type", expType);
        io.mapRequired("data", data);
        msg.setExperimenter(experimenter);
        msg.setExpType(expType);
        msg.setExpData(data.data(), data.size());
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YEXPERIMENTER_H
