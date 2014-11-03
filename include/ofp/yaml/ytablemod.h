//  ===== ---- ofp/ytablemod.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for TableMod and
/// TableModBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YTABLEMOD_H_
#define OFP_YAML_YTABLEMOD_H_

#include "ofp/tablemod.h"

namespace llvm {
namespace yaml {

//---
// type: OFPT_TABLE_MOD
// msg:
//   table_id: <UInt8>    { Required }
//   config: <UInt32>     { Required }
//...

template <>
struct MappingTraits<ofp::TableMod> {
  static void mapping(IO &io, ofp::TableMod &msg) {
    io.mapRequired("table_id", msg.tableId_);
    io.mapRequired("config", msg.config_);
  }
};

template <>
struct MappingTraits<ofp::TableModBuilder> {
  static void mapping(IO &io, ofp::TableModBuilder &msg) {
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapRequired("config", msg.msg_.config_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YTABLEMOD_H_
