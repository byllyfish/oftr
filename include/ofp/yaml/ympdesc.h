//  ===== ---- ofp/ympdesc.h -------------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for the MPDesc class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YMPDESC_H_
#define OFP_YAML_YMPDESC_H_

#include "ofp/mpdesc.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPDesc> {

  static void mapping(IO &io, ofp::MPDesc &body) {
    io.mapRequired("mfr_desc", body.mfrDesc_);
    io.mapRequired("hw_desc", body.hwDesc_);
    io.mapRequired("sw_desc", body.swDesc_);
    io.mapRequired("serial_num", body.serialNum_);
    io.mapRequired("dp_desc", body.dpDesc_);
  }
};

template <>
struct MappingTraits<ofp::MPDescBuilder> {

  static void mapping(IO &io, ofp::MPDescBuilder &body) {
    io.mapRequired("mfr_desc", body.msg_.mfrDesc_);
    io.mapRequired("hw_desc", body.msg_.hwDesc_);
    io.mapRequired("sw_desc", body.msg_.swDesc_);
    io.mapRequired("serial_num", body.msg_.serialNum_);
    io.mapRequired("dp_desc", body.msg_.dpDesc_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPDESC_H_
