// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
