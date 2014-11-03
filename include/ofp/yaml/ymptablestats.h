//  ===== ---- ofp/yaml/ymptablestats.h --------------------*- C++ -*- =====  //
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
/// \brief Defines the MappingTraits for MPTableStats and MPTableStatsBuilder.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YMPTABLESTATS_H_
#define OFP_YAML_YMPTABLESTATS_H_

#include "ofp/mptablestats.h"

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::MPTableStats> {
  static void mapping(IO &io, ofp::MPTableStats &body) {
    io.mapRequired("table_id", body.tableId_);
    io.mapOptional("name", body.name_);
    io.mapOptional("wildcards", body.wildcards_);
    io.mapOptional("max_entries", body.maxEntries_);
    io.mapRequired("active_count", body.activeCount_);
    io.mapRequired("lookup_count", body.lookupCount_);
    io.mapRequired("matched_count", body.matchedCount_);
  }
};

template <>
struct MappingTraits<ofp::MPTableStatsBuilder> {
  static void mapping(IO &io, ofp::MPTableStatsBuilder &msg) {
    io.mapRequired("table_id", msg.msg_.tableId_);
    io.mapOptional("name", msg.msg_.name_);
    io.mapOptional("wildcards", msg.msg_.wildcards_, ofp::Big32{});
    io.mapOptional("max_entries", msg.msg_.maxEntries_, ofp::Big32{});
    io.mapRequired("active_count", msg.msg_.activeCount_);
    io.mapRequired("lookup_count", msg.msg_.lookupCount_);
    io.mapRequired("matched_count", msg.msg_.matchedCount_);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPTABLESTATS_H_
